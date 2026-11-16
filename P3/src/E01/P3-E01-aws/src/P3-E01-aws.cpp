#include "Particle.h"
#include "MQTT-TLS.h"
#include "secrets.h"  // Endpoint, certificados y clave; no se sube a GitHub
SYSTEM_THREAD(ENABLED);

const char* GROUP_ID = "G09-2627";
const char* CLIENT_ID = "IR2131-G09-2627";  // Igual que el nombre del Thing
const char* TELEMETRY_TOPIC = "IR2131/2627/G09/P3/aws/telemetry";
const char* COMMAND_TOPIC = "IR2131/2627/G09/P3/aws/command";
const int MQTT_PACKET_SIZE = 2048;  // Las respuestas de la sombra superan 255 bytes
unsigned long sequence = 0;
unsigned long lastConnect = 0;
unsigned long lastPublish = 0;
String mqttReply = "";
int awsConnected = 0;
String awsStatus = "iniciando";  // Paso en el que esta la conexion

// El payload no termina en cero: se copia byte a byte.
void onMessage(char*, byte* payload, unsigned int length) {
    mqttReply = "";
    for (unsigned int i = 0; i < length; i++) {
        mqttReply += char(payload[i]);
    }
}

MQTT client(AWS_ENDPOINT, 8883, onMessage, MQTT_PACKET_SIZE);

void subscribeTopics() {
    client.subscribe(COMMAND_TOPIC);
}

// MQTT-TLS borra los certificados tras cada intento y desactiva TLS al
// desconectar: hay que volver a cargarlos antes de cada conexion.
bool connectAws() {
    client.disconnect();  // Libera el estado del intento anterior
    int tlsError = client.enableTls(ROOT_CA, sizeof(ROOT_CA), DEVICE_CERT,
                                    sizeof(DEVICE_CERT), DEVICE_KEY, sizeof(DEVICE_KEY));
    if (tlsError != 0) {
        awsStatus = String::format("certificados no validos (%d)", tlsError);
        return false;
    }
    if (!client.connect(CLIENT_ID)) {
        awsStatus = "conexion rechazada; reintento";
        return false;
    }
    subscribeTopics();  // Tras reconectar hay que suscribirse otra vez
    awsStatus = "conectado";
    return true;
}

void setup() {
    Particle.variable("mqttReply", mqttReply);
    Particle.variable("awsConnected", awsConnected);
    Particle.variable("awsStatus", awsStatus);
    lastConnect = millis() - 5000;  // Primer intento de conexion inmediato
}

void loop() {
    // TLS necesita red y la hora correcta para validar el certificado.
    if (!WiFi.ready()) {
        awsStatus = "sin Wi-Fi";
        return;
    }
    if (!Time.isValid()) {
        awsStatus = "esperando la hora";
        return;
    }
    if (!client.isConnected()) {
        awsConnected = 0;
        if (millis() - lastConnect >= 5000) {
            lastConnect = millis();
            awsStatus = "conectando";
            awsConnected = connectAws() ? 1 : 0;
        }
        return;
    }
    client.loop();
    if (millis() - lastPublish >= 5000) {
        lastPublish = millis();
        sequence++;
        char buffer[256] = {};
        JSONBufferWriter json(buffer, sizeof(buffer) - 1);
        json.beginObject();
        json.name("group").value(GROUP_ID);
        json.name("seq").value(sequence);
        json.name("source").value("simulated");
        json.name("temperature").value(random(2000, 3000) / 100.0, 2);
        json.name("humidity").value(random(4000, 6000) / 100.0, 2);
        json.name("pressure").value(random(95000, 105000) / 100.0, 2);
        json.endObject();
        client.publish(TELEMETRY_TOPIC, buffer);
    }
}
