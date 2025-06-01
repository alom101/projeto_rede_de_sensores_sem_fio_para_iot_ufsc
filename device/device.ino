
// Sensor
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


#define DHT_PIN D5
#define DHT_TYPE DHT11
DHT_Unified dht(DHT_PIN, DHT_TYPE);
//#define SAMPLE_RATE 60*1000 //ms

// Device
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "credentials.hpp"
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* mqtt_server = BROKER_IP;

#define SEND_TIME 10*1000 //ms


WiFiClient espClient;
PubSubClient * client;
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  randomSeed(micros());
}

// OTA
#include <ArduinoOTA.h>


void OTA_init(){
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setHostname(OTA_NAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
//  ArduinoOTA.setPasswordHash(OTA_PASSWORD_HASH);
  ArduinoOTA.begin();
}



void blink(){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
}

void  callback(char* topic, byte* payload, unsigned int length) {
  if(String(topic)=="led"){
    if((char)payload[0] == '1') digitalWrite(LED_BUILTIN, LOW);
    if((char)payload[0] == '0') digitalWrite(LED_BUILTIN, HIGH);
    if((char)payload[0] == 'b'){
      int state = digitalRead(LED_BUILTIN);
      blink();
      blink();
      blink();
      digitalWrite(LED_BUILTIN, state);
    }
  }
}


void reconnect_mqtt() {
  while (!client->connected()) {
    char* clientId = "ESP8266Client - MyClient";
    if (client->connect(clientId)) {
      client->subscribe("led");
    } else {
      delay(5000);
    }
  }
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  blink();
  blink();
  blink();

  dht.begin();

  setup_wifi();

  client = new PubSubClient(espClient);

  client->setServer(mqtt_server, 1883);
  client->setCallback(callback);
  client->subscribe("led");

  OTA_init();
}

void send_data(){
  unsigned long now = millis();
  if (now - lastMsg > SEND_TIME) {
    sensors_event_t event;
    lastMsg = now;

    dht.temperature().getEvent(&event);
    if(!isnan(event.temperature)){
      snprintf(msg, MSG_BUFFER_SIZE, "%f", event.temperature);
      client->publish("temperature", msg);
    }
    dht.humidity().getEvent(&event);
    if(!isnan(event.relative_humidity)){
      snprintf(msg, MSG_BUFFER_SIZE, "%f", event.relative_humidity);
      client->publish("relative_humidity", msg);
    }
  }
}
void update_client(){
  if (!client->connected()) {
    reconnect_mqtt();
  }
  client->loop();
}

void loop() {
  ArduinoOTA.handle();
  update_client();
  send_data();
}
