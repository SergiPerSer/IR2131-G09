// This #include statement was automatically added by the Particle IDE.
#include <Grove_4Digit_Display.h>

// This #include statement was automatically added by the Particle IDE.
#include <Grove-Ultrasonic-Ranger.h>

// Incluye las funciones y los tipos de Particle.
#include "Particle.h"
// Incluye la libreria del sensor ultrasonico Grove.
#include "Grove-Ultrasonic-Ranger.h"
#include <Grove_4Digit_Display.h>
// Mantiene las tareas de conexion separadas del programa.
SYSTEM_THREAD(ENABLED);
// Identifica al grupo; sustituid XX por vuestro numero.
const char* GROUP_ID = "G09-2627";

const char* state = "";
// Crea el sensor cuya senal esta conectada a D2.
Ultrasonic ultrasonic(D2);
TM1637 display(D4, D5);
const int BUZZER_PIN = A2;

// Conserva la distancia entera, en centimetros, para consultarla desde la nube.
int distance = 0;
// Guarda el instante de la ultima medida.
unsigned long lastRead = 0;
unsigned long lastSent = 0;
bool first_pub = true;
// Registra la variable y prepara el primer muestreo.
void setup() {
    // Publica el nombre de la variable que se consultara en Console.
    // Registrar una variable no envia un evento.
    Particle.variable("distance", distance);
    // Permite medir en la primera pasada por loop.
    lastRead = millis() - 2000;
        // Inicializa la pantalla antes de configurar y escribir sus digitos.
    display.init();
    // Selecciona el nivel de brillo habitual definido por la libreria.
    display.set(BRIGHT_TYPICAL);
    // Desactiva el separador de dos puntos de la pantalla.
    display.point(POINT_OFF);
    
    pinMode(BUZZER_PIN, OUTPUT);
}

void sonido(){
    
    if (state == "SAFE"){
        digitalWrite(BUZZER_PIN, LOW);
    }
    
    if (state == "WARNING"){
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500); 
        digitalWrite(BUZZER_PIN, LOW);
        delay(500);
    }
    
    if (state == "DANGER"){
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100); 
        digitalWrite(BUZZER_PIN, LOW);
        delay(100);
    }
}

String makePayload(const String& estado, int result) {
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


// Actualiza el dato cada 250 ms.
void loop() {
    if (millis() - lastRead >= 500) {
        lastRead = millis();
        distance = int(ultrasonic.MeasureInCentimeters());
        
        // Muestra la distancia en el display de 4 dígitos
        display.display(0, (distance / 1000) % 10);
        display.display(1, (distance / 100) % 10);
        display.display(2, (distance / 10) % 10);
        display.display(3, distance % 10);
    }

    if (distance > 20) {
        state = "SAFE";
    } 
    else if (distance >= 10 && distance <= 20) {
        state = "WARNING";
    } 
    else if (distance < 10) {
        state = "DANGER";
    }
    
    if ((millis() - lastSent < 5000) || first_pub){
        bool published = Particle.publish(
                "IR2131-G09-E04",
                state,
                distance
            );
        first_pub = false;
    }
}
