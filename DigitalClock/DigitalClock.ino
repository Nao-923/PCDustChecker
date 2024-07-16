#include <M5Core2.h>
#include <WiFi.h>
#include <time.h>
#include <PubSubClient.h>

// Wi-Fi設定
const char* ssid = "";
const char* password = "";

// MQTT設定
const char* mqtt_server = "";
const int mqtt_port = 1883;
const char* mqtt_topic = "/screen";

// NTP設定
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 32400;  // 日本時間 (UTC+9)
const int   daylightOffset_sec = 0;

WiFiClient espClient;
PubSubClient client(espClient);

float ratio = 0.0;

void setup() {
  M5.begin();
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

  // Wi-Fi接続
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.println("\nWi-Fi connected");

  // 時刻設定
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // MQTT設定
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  displayTime();
  displayRatio();
  setBrightness();

  delay(1000);
}

void displayTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    M5.Lcd.println("Failed to obtain time");
    return;
  }
  
  char timeString[6];
  strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo);
  
  M5.Lcd.setTextSize(6);
  M5.Lcd.setCursor(60, 90);
  M5.Lcd.print(timeString);
}

void displayRatio() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 220);
  M5.Lcd.printf("Ratio: %.6f", ratio);
}

void setBrightness() {
  int brightness = map(ratio * 100, 0, 50, 100, 0);
  brightness = constrain(brightness, 0, 100);
  M5.Lcd.setBrightness(brightness);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  ratio = message.toFloat();
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("M5StackClient")) {
      client.subscribe(mqtt_topic);
    } else {
      delay(5000);
    }
  }
}