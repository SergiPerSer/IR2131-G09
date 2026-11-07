
// LIBRERÍAS
#include "Particle.h"              // Incluye la API principal de Particle.
#include "simple-coap.h"            // Incluye la librería CoAP de hirotakaster.
#include <stdio.h>                  // Incluye funciones estándar de entrada y salida.

SYSTEM_THREAD(ENABLED);             // Atiende la conexión en paralelo al programa.

// CONFIGURACIÓN GENERAL
const char* GROUP_ID = "G09-2627"; // Identificador del grupo.
IPAddress serverIP(203, 0, 113, 10); // Dirección IP pública del servidor EC2 (ejemplo).
const int COAP_PORT = 5683;         // Puerto UDP utilizado por CoAP.
char resourcePath[] = "basic/G07-2627"; // Ruta del recurso CoAP del grupo.
Coap coap;                          // Crea el cliente CoAP.

// VARIABLES DE COMUNICACIÓN
bool started = false;               // Indica si se ha inicializado el socket UDP.
bool sendPut = true;                // Determina si la siguiente petición será PUT o GET.
bool sendTrue = true;               // Variable original para controlar el envío de valores booleanos (actualmente no utilizada).
unsigned long lastRequest = 0;       // Registra el instante de la última petición.
String coapReply = "";              // Almacena el contenido de la última respuesta recibida.
int coapCode = 0;                   // Almacena el código numérico de respuesta CoAP.
int pub_index = 0;                  // Índice que permite recorrer las publicaciones.

// ESTRUCTURA DE PUBLICACIONES
struct Publicaciones {
    const char* nombre;             // Almacena el texto que se enviará mediante PUT.
};

// ARRAY DE PUBLICACIONES
Publicaciones pub[] = {             // Define los valores que se enviarán secuencialmente.
    {"true"},                       // Primera publicación.
    {"false"},                      // Segunda publicación.
    {"test"}                        // Tercera publicación.
};

const int NUM_PUBLICACIONES = sizeof(pub) / sizeof(pub[0]); // Calcula el número de elementos del array.

// RECEPCIÓN DE RESPUESTAS COAP
// Se ejecuta cuando llega una respuesta del servidor CoAP.
void onResponse(CoapPacket& packet, IPAddress, int) {
    coapCode = packet.code;         // Guarda el código CoAP recibido.
    coapReply = "";                 // Limpia el contenido de la respuesta anterior.
    if (packet.code != 69) return;  // Solo procesa respuestas 2.05 Content (código 69).
    for (unsigned int i = 0; i < packet.payloadlen; i++) { // Recorre los bytes del contenido recibido.
        coapReply += char(packet.payload[i]); // Convierte cada byte en un carácter y lo almacena.
    }
}

// CONFIGURACIÓN INICIAL (SETUP)
void setup() {
    coap.response(onResponse);      // Asocia las respuestas CoAP con la función onResponse.
    Particle.variable("coapReply", coapReply); // Permite consultar el contenido de la última respuesta.
    Particle.variable("coapCode", coapCode);   // Permite consultar el código de respuesta recibido.
}

// BUCLE PRINCIPAL (LOOP)
void loop() {
    if (!WiFi.ready()) {            // Comprueba si existe conexión WiFi.
        started = false;            // Permite reinicializar el cliente al recuperar la conexión.
        return;                     // Sale temporalmente del loop.
    }
    if (!started) {                 // Comprueba si el cliente CoAP ya está inicializado.
        coap.start();               // Inicializa el socket UDP.
        started = true;             // Marca el cliente como inicializado.
        lastRequest = millis();     // Guarda el instante de inicio del temporizador.
        return;                     // Continúa en la siguiente iteración.
    }
    coap.loop();                    // Atiende los datagramas entrantes y ejecuta onResponse.
    if (millis() - lastRequest < 5000) return; // Espera cinco segundos entre peticiones.
    lastRequest = millis();         // Actualiza el instante de la última petición.
    if (sendPut) {                  // Comprueba si corresponde enviar una petición PUT.
        coap.put(serverIP, COAP_PORT, resourcePath, pub[pub_index].nombre); // Envía el valor actual del array al servidor.
        pub_index = (pub_index == NUM_PUBLICACIONES - 1) ? 0 : pub_index + 1; // Avanza al siguiente elemento y vuelve al inicio al terminar.
    } else {                        // Si no corresponde PUT, realiza una petición GET.
        coap.get(serverIP, COAP_PORT, resourcePath); // Solicita el valor almacenado en el recurso del servidor.
    }
    sendPut = !sendPut;             // Alterna entre PUT y GET para la siguiente petición.
}
