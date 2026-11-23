#include "Particle.h"
#include "MQTT-TLS.h"
#include "secrets.h"  // Endpoint, certificados y clave; no se sube a GitHub
SYSTEM_THREAD(ENABLED);

const char* GROUP_ID = "G09-2627";
const char* CLIENT_ID = "IR2131-G09-2627";  // Igual que el nombre del Thing
// Fragmento para E03: anadidlo despues de la declaracion de client.
const char* SHADOW_UPDATE = "$aws/things/IR2131-G09-2627/shadow/update";
const char* SHADOW_ACCEPTED = "$aws/things/IR2131-G09-2627/shadow/update/accepted";
const char* SHADOW_REJECTED = "$aws/things/IR2131-G09-2627/shadow/update/rejected";
// Fragmento para E02: anadidlo antes de setup().
const int LIGHT_PIN = A0; // Sensor de luz Grove
const int LED_PIN = D4; // LED Grove simple, salida PWM
int light = 0; // Lectura relativa, de 0 a 100
int desiredLight = 50; // Consigna, de 0 a 100
int brightness = 0; // Brillo del LED, de 0 a 255
int tiempo_consigna = 0; // Variable para controlar el tiempo de actualizacion de la consigna
// Publica en reported la lectura, el brillo y la consigna aplicada.

const int MQTT_PACKET_SIZE = 2048;  // Las respuestas de la sombra superan 255 bytes
unsigned long sequence = 0;
unsigned long lastConnect = 0;
unsigned long lastPublish = 0;
String mqttReply = "";
int awsConnected = 0;
String awsStatus = "iniciando";  // Paso en el que esta la conexion

// El payload no termina en cero: se copia byte a byte.
const char* SHADOW_DELTA = "$aws/things/IR2131-G09-2627/shadow/update/delta";
String lastTopic = "";
String lastMessage = "";
bool messagePending = false;
int setpoint = 0;

// Se ejecuta dentro de client.loop(): solo copia el mensaje y lo marca.
void onMessage(char* topic, byte* payload, unsigned int length) {
    lastTopic = topic;
    lastMessage = "";
    for (unsigned int i = 0; i < length; i++) {
        lastMessage += char(payload[i]);
    }
    messagePending = true;
}

// Devuelve el valor de una clave de un objeto JSON, o un valor no valido.
JSONValue getField(const JSONValue& object, const char* key) {
    JSONObjectIterator field(object);
    while (field.next()) {
        if (field.name() == key) return field.value();
    }
    return JSONValue();
}

// Busca state.desiredLight en un delta. Devuelve true y deja el valor en
// setpoint si es un entero de 0 a 100.
bool readDesiredLight(const String& message, int& setpoint) {
    JSONValue root = JSONValue::parseCopy(message.c_str());
    JSONValue value = getField(getField(root, "state"), "desiredLight");
    if (!value.isNumber()) return false;
    double number = value.toDouble();
    if (number < 0 || number > 100 || number != floor(number)) return false;
    setpoint = (int)number;
    return true;
}


MQTT client(AWS_ENDPOINT, 8883, onMessage, MQTT_PACKET_SIZE);

void subscribeTopics() {
    client.subscribe(SHADOW_ACCEPTED);
    client.subscribe(SHADOW_REJECTED);
    client.subscribe(SHADOW_DELTA);
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


// Lee la luz, la compara con la consigna y corrige el brillo.
void updateLight() {
    light = constrain(map(analogRead(LIGHT_PIN), 0, 4095, 0, 100), 0, 100);
    brightness = constrain(brightness + (desiredLight - light), 0, 255);
    analogWrite(LED_PIN, brightness); // 0 apaga el LED y 255 da el maximo
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Particle.variable("light",light);
  Particle.variable("desiredLight",desiredLight);
  Particle.variable("brightness",brightness);
  Particle.variable("mqttReply", mqttReply);
  Particle.variable("awsConnected", awsConnected);
  Particle.variable("awsStatus", awsStatus);
  lastConnect = millis() - 5000;  // Primer intento de conexion inmediato
}
bool publishReported() {
    char buffer[256] = {};
    JSONBufferWriter json(buffer, sizeof(buffer) - 1);
    json.beginObject();
    json.name("state").beginObject();
    json.name("reported").beginObject();
    json.name("group").value(GROUP_ID);
    json.name("light").value(light);
    json.name("brightness").value(brightness);
    json.name("desiredLight").value(desiredLight);
    json.endObject(); // Cierra reported
    json.endObject(); // Cierra state
    json.endObject();
    return client.publish(SHADOW_UPDATE, buffer);
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

    if (messagePending){
        messagePending = false;
        setpoint = 0;
        if (lastTopic == SHADOW_DELTA && readDesiredLight(lastMessage, setpoint)){
            desiredLight = setpoint;
        }
    }


    if (millis() - lastPublish >= 5000) {
        lastPublish = millis();
        sequence++;
        publishReported();
    }
    if (millis() - tiempo_consigna >= 200){
      updateLight();
      tiempo_consigna = millis();
    }
}
