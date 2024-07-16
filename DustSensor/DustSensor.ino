#include "M5Atom.h"

//WiFiとMQTTのライブラリ
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>



//WiFiの設定

const char ssid[] = "";
const char password[] = "";
const char* mqttHost = "";
const int mqttPort = 1883;

//ここまで
const char* topic = "/dust";
char* payload;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char* deviceID = "device01";


const int pinPPD42NS = 32; // PPD42NSのデジタル出力ピン
const unsigned long sampleTimeMillis = 600000; // サンプル時間（ミリ秒）
unsigned long startTimeMillis;
unsigned long duration;
unsigned long lowPulseOccupancy = 0;
float ratio;
float concentration;

void setup() {
  M5.begin();
  Serial.begin(115200);
  pinMode(pinPPD42NS, INPUT);
  startTimeMillis = millis();

  //WiFiとMQTTの設定
  connectWiFi();
  mqttClient.setServer(mqttHost, mqttPort);
  connectMqtt();
}

void loop() {
  M5.update();

  duration = pulseIn(pinPPD42NS, LOW);
  lowPulseOccupancy += duration;

  if ((millis() - startTimeMillis) > sampleTimeMillis) {

    Serial.println("test");
    ratio = lowPulseOccupancy / (sampleTimeMillis * 10.0); // Integer percentage 0=>100
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; 
    char json[200];
    const size_t capacity = JSON_OBJECT_SIZE(4);
    StaticJsonDocument<capacity> doc;
    doc["deviceID"]=deviceID;
    doc["lowPulseOccupancy"] = lowPulseOccupancy;
    doc["Ratio"] = ratio;
    doc["concentration"] = concentration;
    serializeJson(doc, json);
    mqttClient.publish(topic, json);
    delay(1000);
    if (WiFi.status() == WL_DISCONNECTED) {
        connectWiFi();
    }
    lowPulseOccupancy = 0;
    startTimeMillis = millis();
    
  }

  if (M5.Btn.wasPressed()) {
    Serial.println("test");
    ratio = lowPulseOccupancy / (sampleTimeMillis * 10.0); // Integer percentage 0=>100
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; 
    char json[200];
    const size_t capacity = JSON_OBJECT_SIZE(4);
    StaticJsonDocument<capacity> doc;
    doc["deviceID"]=deviceID;
    doc["lowPulseOccupancy"] = lowPulseOccupancy;
    doc["Ratio"] = ratio;
    doc["concentration"] = concentration;
    serializeJson(doc, json);
    mqttClient.publish(topic, json);
    delay(1000);
    if (WiFi.status() == WL_DISCONNECTED) {
        connectWiFi();
    }
    lowPulseOccupancy = 0;
  }

  // connectTDUWiFi();
  mqttloop();
  delay(1000);

}


void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.printf("connecting to %s\n", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.printf("\nWiFi connected\n");
}


void connectMqtt() {
  while (!mqttClient.connected()) {
    Serial.println("connecting to MQTT...");
    String clientID = "M5Atom-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientID.c_str())) {
      Serial.println("connected");
    } else {
      delay(1000);
      randomSeed(micros());
    }
  }
}

void mqttloop() {
  if (!mqttClient.connected()) {
    connectMqtt();
  }
  mqttClient.loop();
}
