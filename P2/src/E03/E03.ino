// This #include statement was automatically added by the Particle IDE.
#include "Particle.h"
#include "MQTT.h"
SYSTEM_THREAD(ENABLED);
const char* GROUP_ID = "G09-2627";
const char* BROKER_HOST = "54.242.252.105"; // IPv4 publica de vuestra instancia EC2
const int MQTT_PORT = 1883;
const char* TOPIC = "IR2131/2627/G09/light";
const int LIGHT_PIN = A0;
int light_value = 0;
String clientId;
String mqttReply = "";
unsigned long lastConnect = 0;
unsigned long lastPublish = 0;
// El payload no termina en cero: se copia byte a byte.
void onMessage(char*, byte* payload, unsigned int length) {
    mqttReply = "";
    for (unsigned int i = 0; i < length; i++) {
        mqttReply += char(payload[i]);
    }
}
MQTT client(BROKER_HOST, MQTT_PORT, onMessage);
void setup() {
    
    clientId = System.deviceID(); // Un Client ID distinto en cada placa
    Particle.variable("light_value",light_value);
    Particle.variable("mqttReply", mqttReply);
    lastConnect = millis() - 5000; // Primer intento de conexion inmediato
}
void loop() {
    if (!WiFi.ready()) return;
    if (!client.isConnected()) {
        if (millis() - lastConnect >= 5000) {
            lastConnect = millis();
            if (client.connect(clientId.c_str())) {
                client.subscribe(TOPIC); // Tras reconectar hay que suscribirse otra vez
                lastPublish = millis() - 5000;
            }
        }
        return;
    }
    client.loop();
    if (millis() - lastPublish >= 5000) {
        lastPublish = millis();
        light_value = analogRead(LIGHT_PIN);
        char buffer[128] = {};
        JSONBufferWriter json(buffer, sizeof(buffer) - 1); // Deja sitio para el cero final
        json.beginObject();
        json.name("group").value(GROUP_ID);
        json.name("light").value(light_value);
        json.endObject();
        client.publish(TOPIC, buffer); // QoS 0, sin retencion
    }
}





