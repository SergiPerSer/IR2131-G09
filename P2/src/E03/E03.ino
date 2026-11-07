
// LIBRERÍAS
#include "Particle.h"              // Incluye la API principal de Particle.
#include "MQTT.h"                  // Incluye la librería de comunicación MQTT.

SYSTEM_THREAD(ENABLED);            // Permite atender la conexión en paralelo al programa.

// CONFIGURACIÓN GENERAL
const char* GROUP_ID = "G09-2627"; // Identificador del grupo.
const char* BROKER_HOST = "54.242.252.105"; // Dirección IP pública del broker MQTT (EC2).
const int MQTT_PORT = 1883;        // Puerto de comunicación MQTT.
const char* TOPIC = "IR2131/2627/G09/light"; // Tópico MQTT de publicación y suscripción.
const int LIGHT_PIN = A0;          // Pin analógico al que está conectado el sensor de luz.

// VARIABLES GLOBALES
int light_value = 0;               // Almacena el valor leído por el sensor de luz.
String clientId;                   // Identificador único del dispositivo Particle.
String mqttReply = "";             // Almacena el último mensaje recibido mediante MQTT.
unsigned long lastConnect = 0;     // Registra el instante del último intento de conexión.
unsigned long lastPublish = 0;     // Registra el instante de la última publicación.

// RECEPCIÓN DE MENSAJES MQTT
// Se ejecuta cuando llega un mensaje al tópico suscrito.
void onMessage(char*, byte* payload, unsigned int length) {
    mqttReply = "";                // Limpia el contenido del mensaje anterior.
    for (unsigned int i = 0; i < length; i++) { // Recorre todos los bytes recibidos.
        mqttReply += char(payload[i]); // Convierte cada byte en un carácter y lo almacena.
    }
}

// CLIENTE MQTT
MQTT client(BROKER_HOST, MQTT_PORT, onMessage); // Crea el cliente indicando broker, puerto y callback.

// CONFIGURACIÓN INICIAL (SETUP)
void setup() {
    clientId = System.deviceID();  // Obtiene un identificador único para cada dispositivo.
    Particle.variable("light_value", light_value); // Permite consultar el valor del sensor desde Particle Cloud.
    Particle.variable("mqttReply", mqttReply); // Permite consultar el último mensaje recibido.
    lastConnect = millis() - 5000; // Fuerza que el primer intento de conexión pueda realizarse inmediatamente.
}

// BUCLE PRINCIPAL (LOOP)
void loop() {
    if (!WiFi.ready()) return;     // Comprueba que existe conexión WiFi; si no, espera.
    if (!client.isConnected()) {   // Comprueba si el cliente MQTT está desconectado.
        if (millis() - lastConnect >= 5000) { // Espera cinco segundos entre intentos de conexión.
            lastConnect = millis(); // Actualiza el instante del último intento.
            if (client.connect(clientId.c_str())) { // Intenta conectar con el broker usando el Client ID.
                client.subscribe(TOPIC); // Se suscribe al tópico para recibir mensajes; debe repetirse tras reconectar.
                lastPublish = millis() - 5000; // Permite realizar la primera publicación inmediatamente.
            }
        }
        return;                    // Sale del loop mientras no exista conexión MQTT.
    }
    client.loop();                 // Atiende los mensajes MQTT entrantes y ejecuta onMessage().
    if (millis() - lastPublish >= 5000) { // Comprueba si han transcurrido cinco segundos desde la última publicación.
        lastPublish = millis();    // Actualiza el instante de publicación.
        light_value = analogRead(LIGHT_PIN); // Lee el valor analógico del sensor de luz.
        char buffer[128] = {};     // Reserva un búfer para construir el mensaje JSON.
        JSONBufferWriter json(buffer, sizeof(buffer) - 1); // Crea el escritor JSON dejando espacio para el carácter nulo.
        json.beginObject();        // Comienza la construcción del objeto JSON.
        json.name("group").value(GROUP_ID); // Añade el identificador del grupo al JSON.
        json.name("light").value(light_value); // Añade la lectura del sensor de luz al JSON.
        json.endObject();          // Finaliza la construcción del objeto JSON.
        client.publish(TOPIC, buffer); // Publica el JSON en el tópico MQTT (QoS 0, sin retención).
    }
}





