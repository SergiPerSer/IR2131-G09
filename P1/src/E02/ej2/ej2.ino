// Incluye las funciones, los pines y el tipo String de Particle.
#include "Particle.h"
// Permite que la conexion se atienda en un hilo del sistema.
SYSTEM_THREAD(ENABLED);
// Identifica al grupo de este programa.
const char* GROUP_ID = "G09-2627";
// Selecciona el pin del LED Grove simple.
const int LED_PIN = D4;
String lastCommand = "";
int lastResult = 0;
bool pendingPublish = false;
unsigned long last_pub;
unsigned long last_parpadeo;
bool ledOn = false;


// Particle llama a esta funcion cuando recibe una orden remota.
int ledControl(String command) {
    int result = -1;

    if (command == "ON") {
        digitalWrite(LED_PIN, HIGH);
        result = 1;
    }
    else if (command == "OFF") {
        digitalWrite(LED_PIN, LOW);
        result = 1;
    }
    else if (command == "BLINK") {
        result = 1;
    }
    
    if (result != -1){
        lastCommand = command;
        lastResult = result;    
    }

    return result;
}
// Configura la salida y registra la funcion al arrancar.
void setup() {
    // Configura D4 como salida digital.
    pinMode(LED_PIN, OUTPUT);
    // Mantiene el LED apagado inicialmente.
    digitalWrite(LED_PIN, LOW);
    // "ledControl" es el nombre remoto; ledControl es la funcion local.
    Particle.function("ledControl", ledControl);
}

// Fragmento para anadir al programa anterior; no es otro programa completo.
// Recibe el comando y el resultado de su ejecucion.
String makePayload(const String& command, int result) {
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
    json.name("cmd").value(command);
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

// Este ejemplo solo responde a las llamadas remotas.
// Variable global: recuerda si ya se ha hecho la publicacion de prueba.
bool testPublished = false;

// Sustituye al loop vacio del primer ejemplo de E02.
void loop() {
    
    if (pendingPublish && Particle.connected()) {
        String payload = makePayload(lastCommand, lastResult);

        if (payload.length() > 0) {
            bool published = Particle.publish(
                "IR2131-G09-E02",
                payload,
                PRIVATE
            );

            if (published) {
                pendingPublish = false;
                last_pub = millis();
            }
        }
    }
    if (lastCommand == "BLINK" && millis()-last_parpadeo >= 100){
        last_parpadeo = millis();
        ledOn = !ledOn;
        digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
    }
    if (millis()-last_pub >= 1000){
        pendingPublish=true;
    }
}