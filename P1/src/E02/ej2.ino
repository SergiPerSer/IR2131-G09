// Incluye las funciones, los pines y el tipo String de Particle.
#include "Particle.h"
// Permite que la conexion se atienda en un hilo del sistema.
SYSTEM_THREAD(ENABLED);
// Identifica al grupo de este programa.
const char* GROUP_ID = "G09-2627";
// Selecciona el pin del LED Grove simple.
const int LED_PIN = D4;

// Particle llama a esta funcion cuando recibe una orden remota.
int ledControl(String command) {
    // Compara el argumento con ON, respetando las mayusculas.
    if (command == "ON") {
        // Enciende el LED.
        digitalWrite(LED_PIN, HIGH);
        // Devuelve 1 para indicar que la orden se ha aceptado.
        return 1;
    }
    // Comprueba si la orden pide apagar el LED.
    if (command == "OFF") {
        // Apaga el LED.
        digitalWrite(LED_PIN, LOW);
        // Confirma que la orden se ha aceptado.
        return 1;
    }
    // Rechaza cualquier otro texto sin cambiar la salida.
    return -1;
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
    // ! significa "no" y && exige que se cumplan las dos condiciones.
    // Entra solo si falta publicar y existe conexion con Particle Cloud.
    if (!testPublished && Particle.connected()) {
        // Construye el JSON de un comando ON con resultado 1.
        String payload = makePayload("ON", 1);
        // Comprueba que makePayload haya devuelto un texto no vacio.
        if (payload.length() > 0) {
            // Publica el evento; cambiad XX por vuestro numero de grupo.
            Particle.publish("IR2131-G09-E02", payload, PRIVATE);
            // Recuerda la llamada para no repetirla en la siguiente vuelta.
            testPublished = true;
        }
    }
}