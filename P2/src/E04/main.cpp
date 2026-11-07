
// LIBRERÍAS

#include "Particle.h"
#include "MQTT.h"
#include <Grove_4Digit_Display.h>
// CONFIGURACIÓN GENERAL
SYSTEM_THREAD(ENABLED);

// Identificador del grupo
const char* GROUP_ID = "G09-2627";
// Dirección IP pública del broker MQTT (instancia EC2)
const char* BROKER_HOST = "54.242.252.105";
// Puerto de comunicación MQTT
const int MQTT_PORT = 1883;
// Tópicos MQTT
const char* TOPIC = "IR2131/2627/G09/light";
const char* TOPIC_request = "IR2131/2627/G09/request";
// Pin analógico del sensor de luz
const int LIGHT_PIN = A0;

// VARIABLES GLOBALES

// Valor leído por el sensor de luz
int light_value = 0;
// Identificador único del dispositivo Particle
String clientId;
// Variable para almacenar respuestas MQTT
String mqttReply = "";
// Copia del último mensaje MQTT recibido
String lastMessage = "";
// Indica si hay un mensaje pendiente de procesar
bool messagePending = false;
// Indica si se ha recibido una petición READ válida
bool peticion = false;
// Control de los intervalos de conexión y publicación
unsigned long lastConnect = 0;
unsigned long lastPublish = 0;


// CONFIGURACIÓN DE LA PANTALLA
// Pantalla conectada a los pines D4 (reloj) y D5 (datos)
TM1637 display(D4, D5);
// RECEPCIÓN DE MENSAJES MQTT
// MQTT ejecuta esta función desde client.loop()
// cuando recibe un mensaje en alguno de los tópicos suscritos.

void onMessage(char* topic, byte* payload, unsigned int length) {
    // Vaciamos el contenido del mensaje anterior
    lastMessage = "";
    // Recorremos los bytes del mensaje recibido
    for (unsigned int i = 0; i < length; i++) {
        // Convertimos cada byte en un carácter
        lastMessage += char(payload[i]);
    }
    // Indicamos que existe un mensaje pendiente de procesar
    messagePending = true;
}
// VALIDACIÓN DE PETICIONES READ
// Devuelve TRUE si el mensaje recibido es una petición
// READ y pertenece al grupo indicado en GROUP_ID.

bool isReadRequest(const String& message) {
    // Analizamos el contenido del mensaje como JSON
    JSONValue root = JSONValue::parseCopy(message.c_str());
    // Comprobamos que el JSON sea válido y un objeto
    if (!root.isValid() || !root.isObject()) {
        return false;
    }
    // Variables para almacenar los campos del JSON
    String source = "";
    String request = "";
    // Iterador para recorrer los campos del objeto JSON
    JSONObjectIterator field(root);
    while (field.next()) {
        // Obtenemos el identificador del grupo
        if (field.name() == "group" && field.value().isString()) {
            source = field.value().toString().data();
        }
        // Obtenemos el tipo de petición
        if (field.name() == "request" && field.value().isString()) {
            request = field.value().toString().data();
        }
    }
    // Comprobamos que el grupo y la petición sean correctos
    return source == GROUP_ID && request == "READ";
}

// VISUALIZACIÓN EN LA PANTALLA
// Muestra un número entero de 0 a 9999
// utilizando las cuatro posiciones de la pantalla.

void showNumber(int value) {
    // Recorremos las posiciones de derecha a izquierda
    for (int i = 3; i >= 0; i--) {
        // Mostramos la última cifra del número
        display.display(i, value % 10);
        // Eliminamos la cifra que ya hemos mostrado
        value /= 10;
    }
}

// CLIENTE MQTT

// Creamos el cliente MQTT indicando:
// - Dirección del broker.
// - Puerto de conexión.
// - Función callback para recibir mensajes.

MQTT client(BROKER_HOST, MQTT_PORT, onMessage);

// CONFIGURACIÓN INICIAL (SETUP)
void setup() {

    // Obtenemos un identificador único para el dispositivo
    clientId = System.deviceID();
    // Registramos variables accesibles desde Particle Cloud
    Particle.variable("light_value", light_value);
    Particle.variable("mqttReply", mqttReply);
    // Permitimos realizar el primer intento de conexión
    lastConnect = millis() - 5000;
    // INICIALIZACIÓN DEL DISPLAY
    display.init();
    // Configuramos el brillo de la pantalla
    display.set(BRIGHT_TYPICAL);
    // Desactivamos los puntos centrales
    display.point(POINT_OFF);
    // Inicializamos los cuatro dígitos a cero
    int8_t digits[] = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++) {
        display.display(i, digits[i]);
    }
    // FIN DE LA INICIALIZACIÓN
}
// BUCLE PRINCIPAL (LOOP)

void loop() {

    // COMPROBACIÓN DE CONEXIÓN WIFI
    // Si el WiFi no está disponible, esperamos
    if (!WiFi.ready()) {
        return;
    }
    // CONEXIÓN CON EL BROKER MQTT
    // Comprobamos si el cliente está desconectado
    if (!client.isConnected()) {
        // Intentamos reconectar cada 5 segundos
        if (millis() - lastConnect >= 5000) {
            lastConnect = millis();
            // Intentamos establecer la conexión MQTT
            if (client.connect(clientId.c_str())) {
                // Nos suscribimos al tópico de iluminación
                client.subscribe(TOPIC);
                // Nos suscribimos al tópico de peticiones
                client.subscribe(TOPIC_request);
                // Permitimos publicar inmediatamente
                // cuando se reciba una petición válida
                lastPublish = millis() - 5000;
            }
        }
        // Salimos del loop mientras no haya conexión
        return;
    }
    // RECEPCIÓN Y PROCESAMIENTO DE MENSAJES
    // Gestionamos los mensajes entrantes del broker MQTT
    client.loop();
    // Comprobamos si hay un mensaje pendiente
    if (messagePending) {
        // Marcamos el mensaje como procesado
        messagePending = false;
        // Comprobamos si el mensaje es una petición READ válida
        peticion = isReadRequest(lastMessage);
    }
    // LECTURA Y PUBLICACIÓN DEL SENSOR
    // Comprobamos que exista una petición válida
    // y que hayan transcurrido al menos 5 segundos
    // desde la última publicación.

    if ((millis() - lastPublish >= 5000) && peticion) {

        // Actualizamos el instante de publicación
        lastPublish = millis();
        // Leemos el valor analógico del sensor de luz
        light_value = analogRead(LIGHT_PIN);
        // GENERACIÓN DEL MENSAJE JSON
        // Reservamos memoria para construir el mensaje
        char buffer[128] = {};
        // Creamos el escritor JSON
        // Dejamos espacio para el carácter nulo final
        JSONBufferWriter json(buffer, sizeof(buffer) - 1);
        // Comenzamos el objeto JSON
        json.beginObject();
        // Añadimos el identificador del grupo
        json.name("group").value(GROUP_ID);
        // Añadimos el valor medido por el sensor
        json.name("light").value(light_value);
        // Cerramos el objeto JSON
        json.endObject();
        // PUBLICACIÓN MQTT
        // Publicamos el mensaje en el tópico de iluminación
        // QoS 0, sin retención
        client.publish(TOPIC, buffer);
        // ACTUALIZACIÓN DEL DISPLAY
        // Mostramos el valor de luz obtenido
        showNumber(light_value);
        // FINALIZACIÓN DE LA PETICIÓN
        // Desactivamos la petición para evitar
        // publicaciones repetidas sin una nueva orden
        peticion = false;
    }
}
