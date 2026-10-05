// Incluye los pines y las funciones de Particle.
#include "Particle.h"
// Mantiene las tareas de conexion separadas del programa del usuario.
SYSTEM_THREAD(ENABLED);
// Identifica al grupo; sustituid XX por vuestro numero.
const char* GROUP_ID = "G09-2627";
// D4 es el pin de senal del LED Grove simple.
const int LED_PIN = D4;
// Recuerda si el LED esta encendido; empieza apagado.
bool ledOn = false;
// Guarda el instante del ultimo cambio de salida.
unsigned long lastChange = 0;
unsigned long lastMode = 0;
int parpadeo = 0;
int normal = 500;
int alerta = 50;

// Se ejecuta una vez al arrancar.
void setup() {
    // Configura el pin como salida.
    pinMode(LED_PIN, OUTPUT);
    // Arranca con el LED apagado.
    digitalWrite(LED_PIN, LOW);
}

// Se repite continuamente, sin esperar con delay.
void loop() {
    if (millis()-lastMode >= 5000){
        if (parpadeo == normal){
            parpadeo = alerta;
            
        }else{
            parpadeo = normal;
        }
        lastMode = millis();
    }
    // Comprueba si han pasado 500 ms desde el ultimo cambio.
    if (millis() - lastChange >= parpadeo) {
        // Guarda el instante desde el que se contara otro intervalo.
        lastChange = millis();
        // Invierte el estado: encendido pasa a apagado y viceversa.
        ledOn = !ledOn;
        // HIGH enciende el LED y LOW lo apaga.
        digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
    }
}