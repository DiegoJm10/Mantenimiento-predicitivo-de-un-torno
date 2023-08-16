# Mantenimiento predicitivo de un torno

## Introducción
En el TM7 de TAMULBA de FCQEI los tornos llevan un mantenimiento semestral, lo cual, no es suficiente cuando existen fallas del equipo, en dado caso se les realiza un mantenimiento correctivo y se procesa a pedir material o en todo caso la reparación con una empresa externa, todo eso es un proceso muy tardado, llega a tardar alrededor de 6 meses y a veces los alumnos no pueden ocupar el equipo.

En este proyecto se busca solucionar que se pueda prevenir alguna falla del motor (220v y 60 HZ) o sistema eléctrico y poder tener control de sí mismo de su estatus de su mantenimiento. 

## Lista de Materiales
### Software
- [Arduino IDE 1.8.19](https://www.arduino.cc/en/software)
- [XAMPP 8.2.4](https://www.apachefriends.org/)
- [Node.js](https://nodejs.org/en)
- [Node-red](https://nodered.org/docs/getting-started/windows)

### Hardware
- [Esp32](http://esp32.net/)

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/esp32.jpg?raw=true)

- [Sensor sct 013](https://pdf1.alldatasheet.com/datasheet-pdf/view/1159366/YHDC/SCT013.html)

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/sensor-de-corriente-ac-30a.jpg?raw=true)

- [Sensor termopar tipo K + Modulo Max6675](https://pdf1.alldatasheet.com/datasheet-pdf/view/73692/MAXIM/MAX6675.html)

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/Sensor%20termopar.jpg?raw=true)

## Desarrollo
 1. Se instalaron los drivers para esp32 en Arduino IDE.
      - Empezamos abriendo Arduino IDE
      - Abrimos la pestaña de Archivo
      - Seleccionamos Preferencias.
      - En la parte de Gestor de URLs Adicionales de Tarjetas colocamos lo siguiente: 
      ```
      https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
      ```
      - Abrimos pestaña de Herramientas.
      - Seleccionamos Placa y ahi mismo gestor de tarjetas.
      - Buscamos esp32 y seleccionamos lo que muestra en la imagen
      ![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/libreria%20arduino.png?raw=true)

 2. Se seleccionó la tarjeta adecuada la cual se llama **ESP32 Dev Module**.

 3. Se realizaron los códigos por separado para poder obtener un mejor resultado.

 4. Empezamos haciendo el código del termopar tipo K.

```
//Realizado por Diego Jasso Miranda

#include<max6675.h> // Se instalo la librería
int CSK=13; // Son las conexiones del sensor
int CS=12;
int SO=14;
MAX6675 termopar(CSK,CS,SO);
void setup() {
Serial.begin(9600);
}


void loop() {
 int temperatura;
 temperatura=termopar.readCelsius(); // Hace la lectura del sensor Analogico
 Serial.print(temperatura);// Muestra el resultado en el Serial.
 Serial.println(" °C");
 delay(1000);

}
```
5. Se realizo el codigo de el sensor sct 013.

```
// Instalacion de Librerias
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "EmonLib.h"
#include <SPI.h>
 
EnergyMonitor emon1;
 

 
//Tensión da red elétrica
int rede = 127;
 
//Pin del sensor SCT
int pino_sct = 34;
 
void setup()
{
  Serial.begin(9600);
 
  
  emon1.current(pino_sct, 20);
  
}
 
void loop()
{
  //Calcula a corrente
  double Irms = emon1.calcIrms(1480);
  Serial.print("Corrente : ");
  Serial.print(Irms-0.30); // Irms
  Serial.println();
 delay (1000);

}
```
6. Se juntaron todos los codigos con el codigo de conexion a internet 

```
// Librerias que se deben incluir en el progra,a
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define BUILTIN_LED 2
#include<max6675.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "EmonLib.h"
#include <SPI.h>

//Se declaran las variables globales tipo entera y los pines que se van a utilizar para conectar los sensores
int CSK=13;
int CS=12;
int SO=14;
MAX6675 termopar(CSK,CS,SO);
EnergyMonitor emon1;
int rede = 127;
int pino_sct = 34;

//Este apartado se usa para realizar la conexioon de la tarjeta con una red WI-FI
const char* ssid = "Eventos_FCQeI";
const char* password = "2023FCQEI";

//  El siguiente bloque realiza la configuración con el servidor mqtt
const char* mqtt_server = "52.28.62.138";
String username_mqtt="DiegoJm";
String password_mqtt="12345678";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // En este bloque podremos ver en monitor serial si la tarjeta logro conectarse a una red WI-FI
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  //poro medio del puerto serial podemos visualizar si la conexion con una red WI-FI fue exitosa
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   
  
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  
    
  }

}

//con la siguiente función  podemos ver mediante el monitor seria si la conexión con mqtt fue exitosa
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), username_mqtt.c_str() , password_mqtt.c_str())) {
      Serial.println("connected");
      client.publish("outTopic", "hello world");
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  emon1.current(pino_sct, 20);
  pinMode(BUILTIN_LED, OUTPUT);     
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
 
}

void loop() {
// Se obtiene el dato del sensor de temperatura para leerlo en celcius.
int temperatura;
 temperatura=termopar.readCelsius();
delay(1000);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
double Irms = emon1.calcIrms(1480);
  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
  
// Se realiza un documento json el cual se manda a node-red
    StaticJsonDocument<128> doc;

    doc["DEVICE"] = "ESP32";
    //doc["Anho"] = 2022;
    //doc["Empresa"] = "Tamulba";
    doc["TEMPERATURA"] = temperatura;
    doc["AMPERAJE"] = (Irms-0.30);
   
// Muestra los datos en el Serial.
    String output;
    
    serializeJson(doc, output);
    Serial.print(Irms-0.30);
    Serial.print("Publish message: ");
    Serial.println(output);
    client.publish("Tamulbaout", output.c_str());
  }
}

```

7. Mostramos la conexión de Node-red.

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/node-red%20con%20telegram.png?raw=true)

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/Dashboard.png?raw=true)

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/Filtrer.png?raw=true)

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/UI%20CONTROL.png?raw=true)

8. En la siguiente lista se mostrara los códigos de cada fuction:
   - Fuction 1: 
    ```
    msg.topic = "SELECT * FROM tamulba7 ORDER BY FECHA DESC";
    msg.leer = true;
    return msg;
    ```
   - Fuction 2:
   ```
   var query = "INSERT INTO `tamulba7`(`ID`, `FECHA`, `DEVICE`, `TEMPERATURA`, `AMPERAJE`) VALUES (NULL, current_timestamp(), '";
   query = query+msg.payload.DEVICE + "','";
   query = query+msg.payload.TEMPERATURA + "','";
   query = query+msg.payload.AMPERAJE + "');'";
   msg.topic=query;
   return msg;
   ```

   - Fuction 3:

   ```
   msg.topic = "SELECT AMPERAJE FROM tamulba7 ORDER by ID DESC LIMIT 1;"; 
   msg.leer = true;
   return msg;
   ```

   - Fuction 4:

   ```
   msg.payload = msg.payload.AMPERAJE;
   msg.topic = "AMPERAJE";
   return msg;  
   ```

   - Fuction 5:

   ```
   msg.payload = msg.payload.TEMPERATURA;
   msg.topic = "TEMPERATURA";
   return msg;
   ```

   - Fuction 6:
   
   ```
   msg.topic = "TRUNCATE TABLE tamulba7"
   return msg;
   ```
 
   - Fuction 8:

   ```
   msg.topic = "SELECT * FROM tamulba7 ORDER BY FECHA DESC";
   msg.leer = true;
   return msg;
   ```

   - Fuction clearData:

   ```
   msg.payload = {
    "command" : "clearData",
    arguments: []
    }
    
    flow.set("id", 0)
    return msg;
   ```

   - Fuction deleteRow:

   ```
   const row = flow.get("selectedRow")
   
   msg.payload = {
    command : "deleteRow",
    arguments : [row.id]
    }
    return msg;
   ```

   - Fuction Construct Filter:

   ```
   const filter = flow.get("filter");
   
   msg.payload = {
    command: "setFilter",
    arguments: [
        filter.field,
        filter.type,
        filter.value
        }
        return msg;

   
Una vez configurado el flow en Node red para la obtienencion de los datos,  anexamos los nodos telegram para implementar un sistema de notificaciones que se detonen cuando supere el limite establecido de amperaje y corriente de peracion adecuada para el motor en este caso espeficico,utilizando un bot de telegram para este apartado del sistema, como se muestra a continuacion en el flow correspondiente:

![](https://github.com/Yurikolugo/flow1_NodeRed/blob/main/WhatsApp%20Image%202023-08-15%20at%2012.09.33%20AM.jpeg?raw=true)


El nodo switch nos manda un 1 cuando el valor del msg.payload es igual o mayor a 1 para el caso del amperaje, para el caso de la  temperatura se dejo en = o mayor a 34 °C:
![](https://github.com/Yurikolugo/flow1_NodeRed/blob/main/WhatsApp%20Image%202023-08-15%20at%2012.12.35%20AM.jpg?raw=true)

posteriorment se creo una funcion donde si es 1 manda el mensaje al chat con el bot de telegram 
a continuación la funcion:
var tgalert,l1;

if (msg.payload == 1){

    tgalert = { "chadId": 5460520497,"type": "message","content": "Alerta"}
 msg.payload = tgalert;
   
}

return msg;

Finalmente se configura los nodos notificacion telegram del siguiente manera :

 ![](https://github.com/Yurikolugo/flow1_NodeRed/blob/main/WhatsApp%20Image%202023-08-15%20at%2012.20.54%20AM.jpeg?raw=true)

   ```

9. Los datos obtenidos se mandaron a una base de datos de mysql en la plataforma **phpmyadmin** como se muestra en la siguiente imagen.

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/phpmyadmin.png?raw=true)

10. Para configurar esta base datos se va hacer de la siguiente manera:

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/Datos%20de%20tabla.png?raw=true)

En la parte de electronica tuvimos que hacer unos arreglos de resistencias y capacitores para poder leer el sensor sct013.

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/RESISTENCIAS.png?raw=true)

También mostramos como conectar los sensores a la esp32

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/Circuito.png?raw=true)
## Resultados
![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/Dashboard1.png?raw=true)
Gráficas con indicador

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/Dashboard2.png?raw=true)
![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/Dashboard3.png?raw=true)
Tabla de datos con filtro.

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/EVIDENCIA%20DE%20IMAGENES/Dashboard4.png?raw=true)
Tabla de datos completa.
## Evidencia
- [Youtube](https://youtu.be/oZgP4gCtuEk)

- [Códigos de Arduino](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/Codigos%20arduino%20y%20node-red/Prueba_todo_junto05.06.ino)

- [Códigos de Node-red](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/Codigos%20arduino%20y%20node-red/flows%20(3).json)

- [Ejemplo de Datos recopilados](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/DOCUMENTOS/EJEMPLO%20DE%20DATOS%20RECOPILADOS.csv)

- [Pagina Web en Codigo IoT]()

- [Formato Plan de acción](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/DOCUMENTOS/Plan%20de%20Acci%C3%B3n%20-%20Mantenimiento%20predictivo%20de%20un%20torno%20.docx)
## Créditos

### Desarrollado por
 
- Diego Jasso Miranda
  - [GitHub](https://github.com/DiegoJm10)
  - [Facebook](https://www.facebook.com/jasso.diego.5/)
- Omar Mora Gomez
  - [GitHub](https://github.com/OmarMoraGomez)
  - [Facebook]()
- Yuriko Lugo Velazquez
  - [GitHub](https://github.com/Yurikolugo)
  - [Facebook]()

De igual manera un agradecimiento a nuestros profesores **Hugo Vargas** y **Diego Trujillo**.
