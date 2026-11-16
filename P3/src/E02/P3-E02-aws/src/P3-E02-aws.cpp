/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Fragmento para E02: anadidlo antes de setup().
const int LIGHT_PIN = A0; // Sensor de luz Grove
const int LED_PIN = D4; // LED Grove simple, salida PWM
int light = 0; // Lectura relativa, de 0 a 100
int desiredLight = 50; // Consigna, de 0 a 100
int brightness = 0; // Brillo del LED, de 0 a 255
int tiempo_consigna = 0; // Variable para controlar el tiempo de actualizacion de la consigna
// Lee la luz, la compara con la consigna y corrige el brillo.
void updateLight() {
    light = constrain(map(analogRead(LIGHT_PIN), 0, 4095, 0, 100), 0, 100);
    brightness = constrain(brightness + (desiredLight - light), 0, 255);
    analogWrite(LED_PIN, brightness); // 0 apaga el LED y 255 da el maximo
}

void setup() {
  // Put initialization like pinMode and begin functions here
  pinMode(LED_PIN, OUTPUT);
  Particle.variable("light",light);
  Particle.variable("desiredLight",desiredLight);
  Particle.variable("brightness",brightness);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  
  if (millis() - tiempo_consigna >= 200){
    updateLight();
    tiempo_consigna = millis();
  }

  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}
