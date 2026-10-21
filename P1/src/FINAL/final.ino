// This #include statement was automatically added by the Particle IDE.
// This #include statement was automatically added by the Particle IDE.

#include "Particle.h"
#include <Grove_4Digit_Display.h>
#include <Grove_ChainableLED.h>

SYSTEM_THREAD(ENABLED);

//CONFIGURACIÓN DE HARDWARE Y CONSTANTES
const char* GROUP_ID = "G09-2627";
const int LIGHT_PIN = A0;
const int BUZZER_PIN = A2;

const int BRIGHT_THRESHOLD = 1800;
const int TWILIGHT_THRESHOLD = 400;

TM1637 display(D2, D3);
ChainableLED leds(D4, D5, 1);

//LECTURAS Y ESTADOS DE SENSORES
int lightValue = 0;
int stateCode= 0;
const char* state = "BOOT";
const char* last_state = "BOOT";
const char* last_difstate = "BOOT";

const char* alarm_state = "DESACTIVADA";

//CONTROL DE TIEMPOS Y FLAGS
unsigned long last_Read = 0;
unsigned long last_buzzed = 0;
bool first_pub = false;
bool buzzOn = false;
bool display_off = false;
//COMANDOS Y RESULTADOS
String lastCommand = "";
int lastResult = 0;

int displayControl(String command){
    int result = -1;
    if (command == "DISPLAY"){
        display.set(BRIGHT_TYPICAL);
        display_off = false;
        result = 1;
    }
    if (command == "DISPLAYN'T"){
        display.clearDisplay();
        display_off = true;
        result = 1;
    }
    lastCommand = command;
    lastResult = result;
    return result;
}

void setup() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    Particle.function("displayControl", displayControl);
    Particle.variable("lightValue", lightValue);
    Particle.variable("stateCode", stateCode);
    lightValue = analogRead(LIGHT_PIN);
    last_Read = millis();
    //INICIALIZACIÓN DEL DISPLAY
    display.init();
    display.set(BRIGHT_TYPICAL);
    display.point(POINT_OFF);
    int8_t digits[]={0,0,0,0};
    for(int i=0;i<4;++i){
    display.display(i,digits[i]);
    }
    //FIN DE LA INICIALIZACIÓN
    //INICIALIZACIÓN DE LOS LEDS
    leds.init();
    leds.setColorRGB(0,0, 0, 0);
    //FIN DE LA INICIALIZACIÓN
}

String makePayload(const String& estado, int result, const String& alarm_state,int com_value,const String& command) {
    // Reserva espacio para los comandos breves de esta practica.
    // Los ceros iniciales dejan un terminador al final del texto.
    char buffer[256] = {};
    // Deja un byte libre para el terminador de la cadena.
    JSONBufferWriter json(buffer, sizeof(buffer) - 1);
    // Abre el objeto JSON.
    json.beginObject();
    // Anade el identificador como texto.
    json.name("group").value(GROUP_ID);
    // Anade el comando recibido como texto, con sus escapes.
    json.name("estado").value(estado);
    // Anade el resultado como numero, sin comillas.
    json.name("result").value(result);
    // En el pkg añadimos el estado de la alarma
    json.name("alarm").value(alarm_state);
    json.name("command_value").value(com_value);
    json.name("command").value(command);
    // Cierra el objeto JSON.
    json.endObject();
    // Comprueba si el JSON completo cabe en el espacio reservado.
    if (json.dataSize() > json.bufferSize()) {
    // Devuelve un texto vacio para indicar que no se ha podido construir.
    return String();
    }
    // Copia el JSON antes de que desaparezca el buffer local.
    return String(buffer);
}

void loop(){
    //BLOQUE DE LOGICA DE LOS ESTADOS
    if (first_pub && ((millis() - last_Read) >= 1000)) {
            last_Read = millis();
            lightValue = analogRead(LIGHT_PIN);
            if (lightValue >= BRIGHT_THRESHOLD){
                state = "BRIGHT";
                stateCode=0;
                leds.setColorRGB(0,255,255,255);
            }else if(lightValue >= TWILIGHT_THRESHOLD){
                state = "TWILIGHT";
                stateCode=1;
                leds.setColorRGB(0,255, 165, 0);
            }else{
                state = "NIGHT";
                stateCode=2;
                leds.setColorRGB(0,255, 0, 0);
            }
    }
    //FIN DEL BLOQUE DE LOGICA DE LOS ESTADOS
    
    //BLOQUE DE PUBLICACIONES
    if(state != last_state || !first_pub){
    String payload = makePayload(state,lightValue,alarm_state,lastResult, lastCommand);
        bool published = Particle.publish(
                "IR2131-G09-FINAL",
                payload,
                PRIVATE
            );
        if (published){
            first_pub = true;
        }
        last_difstate = last_state;
        last_state = state;
    }
    //FIN DE BLOQUE DE PUBLICACIONES
    
    //BLOQUE DE FUNCIONAMIENTO DE LA ALARMA
    if (last_difstate == "NIGHT" && state == "BRIGHT"){
        last_buzzed = millis();
        alarm_state = "ACTIVADA";
        buzzOn = !buzzOn;
        digitalWrite(BUZZER_PIN, buzzOn);
        delay(1000);
    }else{
        alarm_state = "DESACTIVADA";
        digitalWrite(BUZZER_PIN, LOW);
    
    }
    //FIN DE FUNCIONAMIENTO DE LA ALARMA
    
    //BLOQUE DEL DISPLAY
    if(!display_off){
        int8_t digits[]={0,0,0,0};
        digits[0] = lightValue / 1000;      
        digits[1] = (lightValue / 100) % 10;   
        digits[2] = (lightValue / 10) % 10;    
        digits[3] = lightValue % 10;
        for(int i=0;i<4;++i){
            display.display(i,digits[i]);
        }
    }

    //FIN DEL BLOQUE DE DISPLAY    

}
