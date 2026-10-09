// Incluye la API de Particle.
#include "Particle.h"
// Permite atender la conexion en paralelo al programa.
SYSTEM_THREAD(ENABLED);
// Identifica al grupo; sustituid XX por vuestro numero.
const char* GROUP_ID = "G09-2627";
// Entrada analogica del sensor Grove.
const int LIGHT_PIN = A0;
// Es global para que siga existiendo despues de terminar setup.
int lightValue = 0;
// Guarda el instante de la ultima lectura.
unsigned long lastRead = 0;
int last_published = 0;
bool first_pub = true;
// Registra la variable y obtiene una primera lectura al arrancar.
void setup() {
    // El primer argumento es el nombre remoto; el segundo, la variable local.
    Particle.variable("lightValue", lightValue);
    // Guarda un valor inicial del sensor.
    lightValue = analogRead(LIGHT_PIN);
    // Inicia el intervalo para la siguiente lectura.
    lastRead = millis();
}

String makePayload( int result) {
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
    // Anade el resultado como numero, sin comillas.
    json.name("light").value(result);
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

// Actualiza la variable sin bloquear el programa durante un segundo.
void loop() {
    // Comprueba si ha transcurrido el intervalo de muestreo.
    if (millis() - lastRead >= 1000) {
        // Reinicia el temporizador de lectura.
        lastRead = millis();
        // Sustituye el valor anterior por la lectura actual de A0.
        lightValue = analogRead(LIGHT_PIN);
    }
    
    if((abs(last_published - lightValue) >= 300) || first_pub){
        String payload = makePayload(lightValue);
        bool published = Particle.publish(
                "IR2131-G09-E03",
                payload,
                PRIVATE
            );
        last_published = lightValue;
        first_pub = false;
    }
    
}