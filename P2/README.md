**Grupo:** G09
**Identificador:** G09-2627

## E01 - Peticiones PUT y GET desde el Argon y lectura de la respuesta

**Estado:** Completado

### Artefactos
- [Codigo](src/E01/P2-e01.ino)

### Evidencias
- [Evidencias](Evidencias)

### Validacion
Tras un PUT inválido el GET nos muestra que el valor se actualiza y nos muestra un invalid. Será nuestra tarea en el código gestionar estas señales para obviar invalid, en caso de que queramos utilizarlos.

## E02 - Control de un actuador a partir del estado guardado en el servidor.

**Estado:** Completado

### Artefactos
- [Codigo](src/E02/E02.ino)

### Evidencias
- [Video](https://drive.google.com/file/d/1_Og65k__lUSp-O3MJLNJ4nFstdTAZf2b/view?usp=drive_link)
- [Evidencias](Evidencias)

### Validacion
El periodo de get es de un segundo en nuestro código. Esto hace que primero tiene que publicarse el put, luego el get tras un segundo y, finalmente, actualizarse el valor del led al segundo siguiente. Por lo tanto, estamos ante un proceso de encender y apagar el led de dos segundos (En nuestro caso). Distinguimos a los clientes por su IP (La del pulsador es la IP del Argon y la del otro cliente es la IP local 127.0.0.1). El antirrebote no se ha actualizado porque no ha dado problemas en las pruebas.
## E03 - Publicación periódica de lecturas y comprobación con MQTTX

**Estado:** Completado

### Artefactos
- [Codigo](src/E03/E03.ino)

### Evidencias
- [Video](https://drive.google.com/file/d/1BmnOXav9HqqbiRQDX9ikJhV4W7pK5uDT/view?usp=drive_link)
- [Evidencias](Evidencias)

### Validacion
Topic: IR2131/2627/G09/light QoS: 0
{"group":"G09-2627","light":582}

Intervalo de publicación: 5 segundos

El intervalo de 5 segundos nos parece cómodo para observar los cambios en la iluminación sin saturar a mensajes el mqttx.

El mqttx tiene como broker la ip de la EC2 por lo tanto no necesita comunicarse con el argon para leer los mensajes publicados en el topic "light". Esto se debe a que el argon ya conoce la IP del broker de EC2 y allí es donde envía las publicaciones. De esta forma EC2 hace de intermediario o "gestor" de los topics para que cualquier cliente pueda leerlos.

## E04 - Suscripción, interpretación de un JSON recibido y respuesta a peticiones

**Estado:** Completado

### Artefactos
- [Codigo](src/E04/E04.ino)

### Evidencias
- [Video](https://drive.google.com/file/d/1yYIup5fQJ94ykWHiYwHVu32mQ9WXqUMj/view?usp=drive_link)
- [Evidencias](Evidencias)

### Validacion
IR2131/2627/G09/light: A este topic se suscribe el cliente de MQTTX para leer los datos que publica el argon. Y el argon es el cliente que publica los datos cuando el request se lo pide.

IR2131/2627/G09/request: A este topic se suscribe el argon y mediante la lógica interna del programa extrae los datos y comprueba que se cumplan los requisitos para permitir la publicación del valor de luz. Por otro lado, el cliente de MQTTX es suscriptor y publisher ya que es desde el MQTTX por donde enviamos la señal de activación enviada por el topic /request.

NOTA: el cliente MQTTX es suscriptor de ambos topics ya que se suscribe a IR2131/2627/G09/#

1. Prueba1: El sensor se encuentra en luz ambiente. El cliente MQTTX publica con el identificador del grupo y el request "READ". Argon, que está suscrito, lee este mensaje y da luz verde para mandar una lectura.
2. Prueba2: El sensor se encuentra cubierto. El cliente MQTTX publica con el identificador del grupo y el request "READ". Argon, que está suscrito, lee este mensaje y da luz verde para mandar una lectura.
3. Prueba3: El sensor se encuentra en luz ambiente. El cliente MQTTX publica con el identificador del grupo y el request "WRITE". Argon, que está suscrito, lee este mensaje y nota que es incorrecto. Por lo tanto, no manda la lectura de luz y el display no se actualiza.

El callback no manda el mensaje porque su propósito es el de recibir y gestionar un mensaje entrante. El loop es el encargado de publicar y realizar las acciones. De esta forma el callback solo se dedica a recibir mensajes y no se satura. Si MQTTX envía una petición y el argon está apagado o desconectado, el broker recibe el mensaje publicado al topic pero no lo manda al argon por lo que no recibimos respuesta aunque la petición sea correcta.
## E05 - Niveles de QoS y mensajes retenidos con los clientes de Mosquitto

**Estado:** Completado
### Propósito
Este código detecta subidas bruscas de iluminación y avisa al usuario mediante un buzzer. Además podemos ver en tiempo real la iluminación en un display de 4 digitos, el cual puede encenderse y apagarse desde una función Particle.
### Artefactos

- Parte A/Terminal 1

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_sub -h localhost -t "IR2131/2627/G09/qos" -q 2 -v -d

Client (null) sending CONNECT

Client (null) received CONNACK (0)

Client (null) sending SUBSCRIBE (Mid: 1, Topic: IR2131/2627/G09/qos, QoS: 2, Options: 0x00)

Client (null) received SUBACK

Subscribed (mid: 1): 2

Client (null) received PUBLISH (d0, q0, r0, m0, 'IR2131/2627/G09/qos', ... (11 bytes))

IR2131/2627/G09/qos G09-2627-Q0

Client (null) received PUBLISH (d0, q1, r0, m1, 'IR2131/2627/G09/qos', ... (11 bytes))

Client (null) sending PUBACK (m1, rc0)

IR2131/2627/G09/qos G09-2627-Q0

Client (null) received PUBLISH (d0, q2, r0, m2, 'IR2131/2627/G09/qos', ... (11 bytes))

Client (null) sending PUBREC (m2, rc0)

Client (null) received PUBREL (Mid: 2)

Client (null) sending PUBCOMP (m2)

IR2131/2627/G09/qos G09-2627-Q0

^CClient (null) sending DISCONNECT

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_sub -h localhost -t "IR2131/2627/G09/qos" -q 0 -v -d

Client (null) sending CONNECT

Client (null) received CONNACK (0)

Client (null) sending SUBSCRIBE (Mid: 1, Topic: IR2131/2627/G09/qos, QoS: 0, Options: 0x00)

Client (null) received SUBACK

Subscribed (mid: 1): 0

Client (null) received PUBLISH (d0, q0, r0, m0, 'IR2131/2627/G09/qos', ... (11 bytes))

IR2131/2627/G09/qos G09-2627-Q0

Client (null) sending PINGREQ

Client (null) received PINGRESP

Client (null) sending PINGREQ

Client (null) received PINGRESP

Client (null) sending PINGREQ

Client (null) received PINGRESP

Client (null) sending PINGREQ

Client (null) received PINGRESP

^CClient (null) sending DISCONNECT

-Parte A/Terminal 2

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_pub -h localhost -t "IR2131/2627/G09/qos" -q 0 -m "G09-2627-Q0" -d

Client (null) sending CONNECT

Client (null) received CONNACK (0)

Client (null) sending PUBLISH (d0, q0, r0, m1, 'IR2131/2627/G09/qos', ... (11 bytes))

Client (null) sending DISCONNECT

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_pub -h localhost -t "IR2131/2627/G09/qos" -q 1 -m "G09-2627-Q0" -d

Client (null) sending CONNECT

Client (null) received CONNACK (0)

Client (null) sending PUBLISH (d0, q1, r0, m1, 'IR2131/2627/G09/qos', ... (11 bytes))

Client (null) received PUBACK (Mid: 1, RC:0)

Client (null) sending DISCONNECT

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_pub -h localhost -t "IR2131/2627/G09/qos" -q 2 -m "G09-2627-Q0" -d

Client (null) sending CONNECT

Client (null) received CONNACK (0)

Client (null) sending PUBLISH (d0, q2, r0, m1, 'IR2131/2627/G09/qos', ... (11 bytes))

Client (null) received PUBREC (Mid: 1)

Client (null) sending PUBREL (m1)

Client (null) received PUBCOMP (Mid: 1, RC:0)

Client (null) sending DISCONNECT

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_pub -h localhost -t "IR2131/2627/G09/qos" -q 2 -m "G09-2627-Q0" -d
Client (null) sending CONNECT

Client (null) received CONNACK (0)

Client (null) sending PUBLISH (d0, q2, r0, m1, 'IR2131/2627/G09/qos', ... (11 bytes))

Client (null) received PUBREC (Mid: 1)

Client (null) sending PUBREL (m1)

Client (null) received PUBCOMP (Mid: 1, RC:0)

Client (null) sending DISCONNECT

Parte B/Terminal 1

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_sub -h localhost -t "IR2131/2627/G09/retain" -v -d

Client (null) sending CONNECT

Client (null) received CONNACK (0)

Client (null) sending SUBSCRIBE (Mid: 1, Topic: IR2131/2627/G09/retain, QoS: 0, Options: 0x00)

Client (null) received SUBACK

Subscribed (mid: 1): 0

Client (null) received PUBLISH (d0, q0, r0, m0, 'IR2131/2627/G09/retain', ... (17 bytes))

IR2131/2627/G09/retain G09-2627 retenido

Client (null) sending PINGREQ

Client (null) received PINGRESP

^CClient (null) sending DISCONNECT

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_sub -h localhost -t "IR2131/2627/G09/retain" -v -d

Client (null) sending CONNECT

Client (null) received CONNACK (0)

Client (null) sending SUBSCRIBE (Mid: 1, Topic: IR2131/2627/G09/retain, QoS: 0, Options: 0x00)

Client (null) received SUBACK

Subscribed (mid: 1): 0

Client (null) received PUBLISH (d0, q0, r1, m0, 'IR2131/2627/G09/retain', ... (17 bytes))

IR2131/2627/G09/retain G09-2627 retenido

Client (null) sending PINGREQ

Client (null) received PINGRESP

^CClient (null) sending DISCONNECT

Parte B/Terminal 2

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_pub -h localhost -t "IR2131/2627/G09/retain" -r -n

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_pub -h localhost -t "IR2131/2627/G09/retain" -q 1 -m "G09-2627 normal"

**luis@luis-HP-Pavilion-Gaming-Laptop-16-a0xxx:~$** mosquitto_pub -h localhost -t "IR2131/2627/G09/retain" -r -q 1 -m "G09-2627 retenido"



### Evidencias
- [Evidencias](Evidencias)
### Validacion

**Completad una tabla con los paquetes que aparecen con QoS 0, 1 y 2 en cada tramo: publicador y bróker, y bróker y suscripto**

| Nivel de QoS | Tramo 1: Publicador Bróker | Tramo 2: Bróker Suscriptor |
| :--- | :--- | :--- |
| **QoS 0** | `PUBLISH` | `PUBLISH` |
| **QoS 1** | `PUBLISH`, `PUBACK` | `PUBLISH`, `PUBACK` |
| **QoS 2** | `PUBLISH`, `PUBREC`, `PUBREL`, `PUBCOMP` | `PUBLISH`, `PUBREC`, `PUBREL`, `PUBCOMP` |

 **¿Qué garantía da cada nivel**

* **QoS 0 (Como máximo una vez):** Entrega tipo *best-effort*. No hay acuses de recibo ni reintentos; el mensaje puede perderse, pero nunca se duplica.
* **QoS 1 (Al menos una vez):** Asegura que el mensaje llega al receptor mediante reintentos con `PUBACK`. Puede haber duplicados.
* **QoS 2 (Exactamente una vez):** El nivel más seguro (intercambio de 4 pasos). Garantiza que no hay pérdidas ni duplicados.

 **¿Con qué QoS llega G09-2627-Q2 al suscriptor 0que pide QoS 0, y por qué?**
 
 El mensaje llega con qos 0 porque el qos del mensaje no puede ser mayor al qos ni del subscriptor ni del publicador y como el del subscriptor es 0 el del mensaje es 0

**¿Por qué el suscriptor recibe un mensaje en el paso 5 de la parte B y ninguno en el paso 3?**

Porque el mensaje del paso 3 es uno normal y el del paso 5 está retenido.
 
 **¿Qué diferencia hay entre conservar el último mensaje mediante retain y guardar un historial de publicaciones?**
 
 Que el retain guarda solo el último mensaje que se ha enviado con estas características y uh historial de publicaciones (además de necesitar medios externos) te permite almacenar todos los mensajes que se quiera guardar a lo largo de un tiempo determinado

 
