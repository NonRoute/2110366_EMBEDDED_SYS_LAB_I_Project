#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "proj"
#define WIFI_PASSWORD "esp proj"

#define API_KEY "AIzaSyChA3IxstEVevYQpvPV6ddn-sGkBHZC5eg"

#define USER_EMAIL "guest@email.com"
#define USER_PASSWORD "password"

#define DATABASE_URL "https://psa-esp-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid;
String databasePath;
String dataPath = "/data";
String timePath = "/timestamp";

FirebaseJson json;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void toggle() digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    toggle();
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

unsigned long getTime() {
  timeClient.update();
  return timeClient.getEpochTime();
}
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  initWiFi();
  timeClient.begin();
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    toggle();
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  databasePath = "/UsersData/" + uid + "/readings";
}

char buff [69] = "";
int idx = 0;
int UARTend() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (idx < sizeof buff) {
      if (c == '\n') {
        idx = 0;
        return getTime();
      }else buff [idx++] = c;
    }
  }
  return false;
}
int timestamp;
void loop() {
  if (timestamp = UARTend()) {
    digitalWrite(BUILTIN_LED, LOW);
    Serial.print("Received : ");
    Serial.println(buff);
    json.set(dataPath.c_str(), buff);
    json.set(timePath, String(timestamp));
    Firebase.RTDB.setJSON(&fbdo, databasePath, &json);
    Serial.println("Transmitted");
    memset(buff, 0, sizeof(buff));
  }
  digitalWrite(BUILTIN_LED, HIGH);

}
