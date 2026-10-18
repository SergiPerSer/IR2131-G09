// This #include statement was automatically added by the Particle IDE.
#include <Grove_4Digit_Display.h>

// This #include statement was automatically added by the Particle IDE.
#include <Grove-Ultrasonic-Ranger.h>

// Incluye las funciones y los tipos de Particle.
#include "Particle.h"
// Incluye la libreria del sensor ultrasonico Grove.
#include "Grove-Ultrasonic-Ranger.h"
#include <Grove_4Digit_Display.h>
// Mantiene las tareas de conexion separadas del programa.
SYSTEM_THREAD(ENABLED);
// Identifica al grupo; sustituid XX por vuestro numero.
const char* GROUP_ID = "G09-2627";

//Xrear una variable estado que sea cadena y corregir
const int SAFE = 20;
const int WARNING = 15;
const int DANGER = 5;
// Crea el sensor cuya senal esta conectada a D2.
Ultrasonic ultrasonic(D2);
TM1637 display(D4, D5);
const int BUZZER_PIN = A2;

// Conserva la distancia entera, en centimetros, para consultarla desde la nube.
int distance = 0;
// Guarda el instante de la ultima medida.
unsigned long lastRead = 0;

// Registra la variable y prepara el primer muestreo.
void setup() {
    // Publica el nombre de la variable que se consultara en Console.
    // Registrar una variable no envia un evento.
    Particle.variable("distance", distance);
    // Permite medir en la primera pasada por loop.
    lastRead = millis() - 2000;
        // Inicializa la pantalla antes de configurar y escribir sus digitos.
    display.init();
    // Selecciona el nivel de brillo habitual definido por la libreria.
    display.set(BRIGHT_TYPICAL);
    // Desactiva el separador de dos puntos de la pantalla.
    display.point(POINT_OFF);
    
    pinMode(BUZZER_PIN, OUTPUT);
}


// Actualiza el dato cada 250 ms.
void loop() {
    if (millis() - lastRead >= 500) {
        lastRead = millis();
        distance = int(ultrasonic.MeasureInCentimeters());
        
        // Muestra la distancia en el display de 4 dígitos
        display.display(0, (distance / 1000) % 10);
        display.display(1, (distance / 100) % 10);
        display.display(2, (distance / 10) % 10);
        display.display(3, distance % 10);
    }

    if (distance >= SAFE) {
        digitalWrite(BUZZER_PIN, LOW);
    } 
    else if (distance > DANGER && distance <= WARNING) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500); 
        digitalWrite(BUZZER_PIN, LOW);
        delay(500);
    } 
    else if (distance <= DANGER) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100); 
        digitalWrite(BUZZER_PIN, LOW);
        delay(100);
    }
}
