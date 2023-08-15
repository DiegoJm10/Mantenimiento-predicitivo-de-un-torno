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
