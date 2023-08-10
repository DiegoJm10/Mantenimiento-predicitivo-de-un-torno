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
- [Esp32](chrome-extension://efaidnbmnnnibpcajpcglclefindmkaj/https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/AR0453-ESP32-V2.jpg?raw=true)

- [Sensor sct 013](https://pdf1.alldatasheet.com/datasheet-pdf/view/1159366/YHDC/SCT013.html)

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/sensor-de-corriente-ac-30a.jpg?raw=true)

- [Sensor termopar tipo K + Modulo Max6675](https://pdf1.alldatasheet.com/datasheet-pdf/view/73692/MAXIM/MAX6675.html)

![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/61n5RQZDeoL._AC_UF1000,1000_QL80_.jpg?raw=true)

## Desarrollo
 1. Se instalaron los drivers para esp32 en Arduino IDE.
      - Empezamos abriendo Arduino IDE
      - Abrimos la pestaña de Archivo
      - Seleccionamos Preferencias.
      - En la parte de Gestor de URLs Adicionales de Trajetas colocamos lo siguente: 
      ```
      https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
      ```
      - Abrimos pestaña de Herramientas.
      - Seleccionamos Placa y ahi mismo gestor de tarjetas.
      - Buscamos esp32 y seleccionamos lo que muestra en la imagen
      ![](https://github.com/DiegoJm10/Mantenimiento-predicitivo-de-un-torno/blob/main/Pruebas_SENSOR_DE_TEMOPAR%20Arduino%201.8.19%2010_08_2023%2010_49_33%20a.%20m..png?raw=true)

 2. Se seleccionó la tarejeta adecuada la cual se llama **ESP32 Dev Module**.

 3. Se realizaron los codigos por separado para poder obtener un mejor resultado.

 4. Empezamos haciendo el codigo del termopar tipo K.

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
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "EmonLib.h"
#include <SPI.h>
int CSK=13;
int CS=12;
int SO=14;
MAX6675 termopar(CSK,CS,SO);
EnergyMonitor emon1;
int rede = 127;
int pino_sct = 34;


const char* ssid = "Totalplay-B4A4";
const char* password = "B4A45BF2bs5x8Mb4";
const char* mqtt_server = "3.64.154.127";
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
  // We start by connecting to a WiFi network
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
  

    StaticJsonDocument<128> doc;

    doc["DEVICE"] = "ESP32";
    //doc["Anho"] = 2022;
    //doc["Empresa"] = "Tamulba";
    doc["TEMPERATURA"] = temperatura;
    doc["AMPERAJE"] = (Irms-0.30);
   

    String output;
    
    serializeJson(doc, output);
    Serial.print(Irms-0.30);
    Serial.print("Publish message: ");
    Serial.println(output);
    client.publish("Tamulbaout", output.c_str());
  }
}
```
