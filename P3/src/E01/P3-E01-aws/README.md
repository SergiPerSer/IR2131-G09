# P3-E01-aws

Proyecto de Particle Workbench para el Codigo 4 de la Practica 3 (IR2131):
cliente MQTT-TLS del Argon con AWS IoT Core.

1. Iniciad sesion en la extension: Ctrl+Mayus+P > "Particle: Login".
2. Descomprimid el proyecto en una ruta sin espacios (por ejemplo,
   C:\IR2131\P3-E01-aws) y abrid la carpeta (File > Open Folder).
3. Target > Configure for device: argon, Device OS 6.4.1 y vuestro Argon.
   La primera vez, instalad la toolchain cuando Workbench lo pida.
4. Copiad src/secrets.example.h como src/secrets.h y rellenad el endpoint,
   la CA raiz (AmazonRootCA1.pem), el certificado y la clave privada.
5. Cambiad el grupo en GROUP_ID, CLIENT_ID y los topicos.
6. Compile > Local compile.
7. Conectad el Argon por USB y Flash > Local flash.

La carpeta lib/MQTT-TLS contiene la libreria MQTT-TLS 0.2.27 con una linea
corregida para Device OS 4 o posterior (src/MQTT-TLS.cpp, funcion _gettimeofday).
No la instaleis con "Install library": se sustituiria por la version original,
que no compila.
No subais src/secrets.h ni la carpeta target/ al repositorio.
