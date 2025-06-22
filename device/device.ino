
// Sensor
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


#define DHT_PIN D8
#define DHT_TYPE DHT11
DHT_Unified dht(DHT_PIN, DHT_TYPE);

#define ANALOG_SENSOR_PIN A0

#define BJT_OUTPUT D5
//#define LED_BUILTIN D6

// Device
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "credentials.hpp"
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* mqtt_server = BROKER_IP;

bool log_sent = false;

#define SAMPLE_RATE 5*1000 //ms
#define LOG_RATE    60*1000 //ms


WiFiClient espClient;
PubSubClient * client;
unsigned long last_sensor_msg = -SAMPLE_RATE;
unsigned long last_log_msg = -LOG_RATE;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];

float max_temp      = 30.0;
float max_humidity  = 95.0;

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
  if(String(topic)=="estufa/status/commands"){
    if((char)payload[0] == '1') digitalWrite(LED_BUILTIN, LOW);
    if((char)payload[0] == '0') digitalWrite(LED_BUILTIN, HIGH);
    if((char)payload[0] == '1') digitalWrite(BJT_OUTPUT, HIGH);
    if((char)payload[0] == '0') digitalWrite(BJT_OUTPUT, LOW);
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
      client->subscribe("estufa/status/commands");
    } else {
      delay(5000);
    }
  }
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BJT_OUTPUT, OUTPUT);
  blink();
  blink();
  blink();

  dht.begin();

  setup_wifi();

  client = new PubSubClient(espClient);

  client->setServer(mqtt_server, 1883);
  client->setCallback(callback);
  client->subscribe("estufa/status/commands");

  OTA_init();
}

void send_data(){
  unsigned long now = millis();
  if (now - last_sensor_msg > SAMPLE_RATE) {
    sensors_event_t event;
    last_sensor_msg = now;

    dht.temperature().getEvent(&event);
    if(!isnan(event.temperature)){
      snprintf(msg, MSG_BUFFER_SIZE, "%f", event.temperature);
      client->publish("estufa/temperatura/valor_atual", msg);
    }

    snprintf(msg, MSG_BUFFER_SIZE, "%f", max_temp);
    client->publish("estufa/temperatura/limiar", msg);

    dht.humidity().getEvent(&event);
    if(!isnan(event.relative_humidity)){
      snprintf(msg, MSG_BUFFER_SIZE, "%f", event.relative_humidity);
      client->publish("estufa/umidade_do_ar/valor_atual", msg);
    }

    snprintf(msg, MSG_BUFFER_SIZE, "%f", max_humidity);
    client->publish("estufa/umidade_do_ar/limiar", msg);

    float analog_sensor_value = analogRead(ANALOG_SENSOR_PIN);
    snprintf(msg, MSG_BUFFER_SIZE, "%f", analog_sensor_value);
    client->publish("estufa/umidade_do_solo/valor_atual", msg);

    int bjt_output_state = digitalRead(BJT_OUTPUT);
    snprintf(msg, MSG_BUFFER_SIZE, "%d", bjt_output_state);
    client->publish("estufa/ventilacao/valor_atual", msg);
  }

  if(now - last_log_msg > LOG_RATE){
    last_log_msg = now;

    snprintf(msg, MSG_BUFFER_SIZE, "%s", __TIMESTAMP__);
    client->publish("estufa/status/versao_firmware", msg);

    snprintf(msg, MSG_BUFFER_SIZE, "%ld", now);
    client->publish("estufa/status/timestap", msg);
  }
}

void update_actuators(){
    sensors_event_t event;
    int bjt_output_state = digitalRead(BJT_OUTPUT);

    dht.temperature().getEvent(&event);
    if(!isnan(event.temperature)){
      if(event.temperature > max_temp){
        bjt_output_state = HIGH;
      }
    }

    dht.humidity().getEvent(&event);
    if(!isnan(event.relative_humidity)){
      if(event.relative_humidity > max_humidity){
        bjt_output_state = HIGH;
      }
    }

    digitalWrite(BJT_OUTPUT, bjt_output_state);
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
  update_actuators();
  send_data();
}
