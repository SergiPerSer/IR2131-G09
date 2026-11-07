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

## E04 - Suscripción, interpretación de un JSON recibido y respuesta a peticiones

**Estado:** Completado

### Artefactos
- [Codigo](src/E04/E04.ino)

### Evidencias
- [Video](https://drive.google.com/file/d/1yYIup5fQJ94ykWHiYwHVu32mQ9WXqUMj/view?usp=drive_link)
- [Evidencias](Evidencias)

### Validacion

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
