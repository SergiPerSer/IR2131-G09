**Grupo:** G09
**Identificador:** G09-2627

## E01 - LED Grove simple con dos patrones automáticos

**Estado:** Completado

### Artefactos
- [Codigo](src/E01/)

### Evidencias
- [Video](https://drive.google.com/drive/u/1/folders/1WP251OqCh4MTeqonMLc-gk7lxf5ROKAD)

### Validacion
#### Tabla de Tiempos y Modos de Operación del LED (Pin D4)

| Ventana de Tiempo | Modo Activo | Valor de `parpadeo` | Comportamiento del LED |
| :--- | :--- | :--- | :--- |
| **`0s – 5s`** (Inicio) | Arranque / Transitorio | `0 ms` | Parpadeo ultra-rápido (máxima velocidad del ciclo de la placa) |
| **`5s – 10s`** | Normal | `500 ms` | Conmuta cada 500 ms (~250 ms Encendido / ~250 ms Apagado) |
| **`10s – 15s`** | Alerta | `50 ms` | Parpadeo rápido de alerta (~25 ms Encendido / ~25 ms Apagado) |
| **`15s – 20s`** | Normal | `500 ms` | Retorna al modo normal (`500 ms`) |
| **`20s – 25s`** | Alerta | `50 ms` | Retorna al modo alerta (`50 ms`) |
| **`> 25s`** | *Ciclo Continuo* | *Alterna 500ms / 50ms* | El bucle se repite indefinidamente cambiando de modo cada 5 segundos |

Se nota un cambio en el parpadeo porque pasa menos tiempo entre encender y apagar. Primero pusimos un tiempo de alerta mayor que el que hay ahora, pero lo cambiamos por exagerar los resultados. Sin embargo, no lo hemos disminuido mucho para que no se vuelva homogéneo.
## E02 - Control remoto del LED con Particle.function()

**Estado:** Completado

### Artefactos
- [Codigo](src/E02/)

### Evidencias
- [Video](https://drive.google.com/drive/u/1/folders/1k93aIkPy_gPYVdx0ndnb8Gdw87-CSVo6)
- [Evidencias](evidencias)

### Validacion
No hemos elegido ningún estado inicial en específico pero el led está apagado, por lo que podemos decir que está en OFF. En cuanto a BLINK parpadea cada 100 ms y para que parpadee hay un if que cada 100 ms enciende o apaga el led si el comando equivale a "BLINK".

El comando inválido escogido es TEST y su valor es -1. Mediante variables intermedias mantenemos el verdadero valor del estado sin contar los comandos inválidos para mantener el estado actual pero enviamos por el paquete JSON los datos del último comando, ya sea válido o inválido.
## E03 - Sensor de luz, variable remota y telemetría

**Estado:** Completado

### Artefactos
- [Codigo](src/E03/)

### Evidencias
- [Evidencias](evidencias)

### Validacion
Descubierto: 1452
Cubierto: 187
El valor de la variable disminuye cuando cubres el sensor.
Comparamos con last_published para guardar el valor anterior y restarlo al actual para encontrar una diferencia de 300, de esta manera solo publicamos si hay una diferencia de iluminación de 300
## E04 - Detección de cambios de estado

**Estado:** Completado

### Artefactos
- [Codigo](src/E04/)

### Evidencias
- [Video](https://drive.google.com/drive/u/1/folders/1ezIGdwPDIwPQZHnQGqcW8wQZ7gitVYph)
- [Evidencias](evidencias)

### Validacion
En las pruebas que hemos hecho en las condiciones ambientales iniciales nuestro light_ref es de 1000 y nuestro dark_ref es de 50 y nuestro threshold es de 525 que es la media de estas dos medidas. Comparamos los valores de iluminación con el threshold, si el valor es superior o igual habrá LIGHT, de lo contrario habrá DARK. Si los valores de iluminación oscilan por el umbral tenemos cambios rápidos de estado. Para solucionar este problema debemos introducir un algoritmo de histéresis

## FINAL - [ALARMA]

**Estado:** Completado
### Propósito
Este código detecta subidas bruscas de iluminación y avisa al usuario mediante un buzzer. Además podemos ver en tiempo real la iluminación en un display de 4 digitos, el cual puede encenderse y apagarse desde una función Particle.
### Artefactos
- [Codigo](src/E05/)

### Evidencias
- [Video](ENLACE_DRIVE)
- [Evidencias](evidencias)
### Validacion
| stateCode | Estado | Condicion de entrada | Respuesta fisica |
|---|---|---|---|
| 0 | BRIGHT | `lightValue >= 1800` (mayor o igual que `BRIGHT_THRESHOLD`) | LED encadenable en color **blanco** (`RGB: 255, 255, 255`), display de 4 dígitos mostrando el valor de luz actual y activación temporal del zumbador si se transiciona desde *NIGHT*. |
| 1 | TWILIGHT | `400 <= lightValue < 1800` (entre `TWILIGHT_THRESHOLD` y `BRIGHT_THRESHOLD`) | LED encadenable en color **naranja** (`RGB: 255, 165, 0`) y display de 4 dígitos mostrando el valor de luz actual. |
| 2 | NIGHT | `lightValue < 400` (menor que `TWILIGHT_THRESHOLD`) | LED encadenable en color **rojo** (`RGB: 255, 0, 0`) y display de 4 dígitos mostrando el valor de luz actual. |

| Comando | Accion | Retorno |
|---|---|---|
| `DISPLAY` | Enciende el display, restaura el brillo típico (`BRIGHT_TYPICAL`) y reanuda la actualización de los valores | `1` |
| `DISPLAYN'T` | Limpia y apaga el display de 4 dígitos (`clearDisplay`) | `1` |
| Comando inválido (cualquier otro texto) | Sin cambio en el funcionamiento del display | `-1` |

Durante el proceso de diseño hemos decidido reducir los umbrales de NIGHT para que las transiciones entre NIGHT y TWILIGHT sean más naturales. Se decidió añadir un display para simular una pequeña terminal a la que poder acceder a los valores de iluminación.

Declaramos que hemos realizado, ejecutado y comprobado las tareas y las evidencias de este cuaderno de campo. Hemos declarado en IA.md cualquier uso de IA generativa. Todos los integrantes comprendemos el funcionamiento de la solución que entregamos.
