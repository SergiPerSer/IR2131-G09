#include "Particle.h" // Incluye la API de Particle para un archivo .cpp.
#include "simple-coap.h" // Incluye la libreria coap de hirotakaster.
#include <stdio.h>
SYSTEM_THREAD(ENABLED); // Atiende la conexion en paralelo al programa.

const char* GROUP_ID = "G09-2627"; // Identifica al grupo.
IPAddress serverIP(203, 0, 113, 10); // EJEMPLO: sustituid por la IPv4 publica de EC2.
const int COAP_PORT = 5683; // Puerto UDP del servidor.
char resourcePath[] = "basic/G07-2627"; // Cambiad el grupo; la API recibe char*.
Coap coap; // Crea el cliente CoAP.
bool started = false; // Indica si se ha abierto el socket UDP.
bool sendPut = true; // La primera peticion sera PUT.
bool sendTrue = true; // El primer valor enviado sera true.
unsigned long lastRequest = 0; // Temporizador entre peticiones.
String coapReply = ""; // Conserva el texto de la ultima respuesta.
int coapCode = 0; // Conserva el codigo numerico de la ultima respuesta.
int pub_index = 0;
struct Publicaciones {
    char nombre[];        // Una cadena (vector de char)
};

// Recibe la respuesta; los dos ultimos argumentos no se utilizan aqui.
void onResponse(CoapPacket& packet, IPAddress, int) {
    coapCode = packet.code; // Guarda el codigo CoAP recibido.
    coapReply = ""; // Vacia el texto de la respuesta anterior.
    // La libreria solo rellena payload si la respuesta trae opciones o datos.
    // La respuesta a un PUT llega vacia: solo se leen las respuestas 2.05.
    if (packet.code != 69) return; // 69 es el valor numerico de 2.05 Content.
    for (unsigned int i = 0; i < packet.payloadlen; i++) { // Recorre los bytes recibidos.
        coapReply += char(packet.payload[i]); // Copia cada caracter sin suponer un cero final.
    }
}

// Registra la respuesta y las variables consultables desde Console.
void setup() {
    coap.response(onResponse); // Asocia las respuestas con la funcion anterior.
    Particle.variable("coapReply", coapReply); // Permite consultar el texto recibido.
    Particle.variable("coapCode", coapCode); // Permite consultar el codigo de respuesta.
    struct Publicaciones pub[] = {
        {"true"},
        {"false"},
        {"test"}
    };
}

// Procesa la comunicacion continuamente y alterna PUT y GET cada cinco segundos.
void loop() {
    if (!WiFi.ready()) { // Espera a disponer de conexion IP.
        started = false; // Prepara el socket para cuando vuelva la red.
        return; // Continua en otra vuelta sin usar delay.
    }
    if (!started) { // Abre el socket cuando la red ya esta disponible.
        coap.start(); // Inicializa el cliente UDP de la libreria.
        started = true; // Evita repetir la inicializacion en cada vuelta.
        lastRequest = millis(); // Empieza el intervalo de la primera peticion.
        return; // Procesara peticiones en las siguientes vueltas.
    }
    coap.loop(); // Atiende los datagramas y ejecuta onResponse cuando llega una respuesta.
    if (millis() - lastRequest < 5000) return; // Espera sin detener la atencion de CoAP.
    lastRequest = millis(); // Reinicia el intervalo.
    if (sendPut) { // Envia el siguiente valor al recurso.
        coap.put(serverIP, COAP_PORT, resourcePath, pub[pub_index]); // Envia PUT.
        pub_index = (pub_index == 2) ? 0 : pub_index + 1;
    } else { // Consulta el valor que conserva el servidor.
        coap.get(serverIP, COAP_PORT, resourcePath); // Envia GET al mismo recurso.
    }
    sendPut = !sendPut; // Alterna PUT y GET.
}