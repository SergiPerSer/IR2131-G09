**Grupo:** G09
**Identificador:** G09-2627

## E01 - Peticiones PUT y GET desde el Argon y lectura de la respuesta

**Estado:** Completado

### Artefactos
- [Codigo](src/E01/P2-e01.ino)

### Evidencias
- [Evidencias](Evidencias)

### Validacion

## E02 - Control de un actuador a partir del estado guardado en el servidor.

**Estado:** Completado

### Artefactos
- [Codigo](src/E02/p2-e02.ino)

### Evidencias
- [Video](https://drive.google.com/file/d/1_Og65k__lUSp-O3MJLNJ4nFstdTAZf2b/view?usp=drive_link)
- [Evidencias](Evidencias)

### Validacion

## E03 - Publicación periódica de lecturas y comprobación con MQTTX

**Estado:** Completado

### Artefactos
- [Codigo](src/E03/p2-e03.ino)

### Evidencias
- [Video](https://drive.google.com/file/d/1BmnOXav9HqqbiRQDX9ikJhV4W7pK5uDT/view?usp=drive_link)
- [Evidencias](Evidencias)

### Validacion

## E04 - Detección de cambios de estado

**Estado:** Completado

### Artefactos
- [Codigo](src/E04/ej4.ino)

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
- [Codigo](src/FINAL/final.ino)
- [Grafo](src/FINAL/GrafoMermaidFinal.png)
### Evidencias
- [Video](https://drive.google.com/drive/u/1/folders/1SK37XSgON459MLOut6ispp0Df9vmE1oH)
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
Además se diseñó un sistema para que las lecturas de estado se sincronicen y no haya errores en las lecturas. Por ejemplo que las variables se actualicen antes que el estado de la alarma.

Declaramos que hemos realizado, ejecutado y comprobado las tareas y las evidencias de este cuaderno de campo. Hemos declarado en IA.md cualquier uso de IA generativa. Todos los integrantes comprendemos el funcionamiento de la solución que entregamos.

