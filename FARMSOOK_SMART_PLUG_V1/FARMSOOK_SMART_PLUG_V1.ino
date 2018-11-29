/*
  - แก้ไขจากเวอร์ชั่นใช้ที่บ้าน มีเครื่องหมาย [] มาจาก php แก้ที่ php file ไม่ให้มี []
  - Relay active LOW ต้องระวังตรงเงื่อนไข if else
  - host ใช้เป็น led.scsthai.com ได้เลย
  - url แก้ตามช้งานจริง
*/
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
WiFiManager wifiManager;
int IntRst = 15; // Pin D8=15,
int Outp = 13; // Pin D7=13,
WiFiClient client;

String ledID = "farmsook1";//5760561,9983908,3161443,4037176,
String LEDMsg = "";
String weatherString;
int updCnt = 0;
int dots = 0;
long dotTime = 0;
long clkTime = 0;
String date;
String YY, MM, DD;

#define SECONDS_DS(seconds) ((seconds)*1000000UL)
void setup(void) {
  delay(100);
  Serial.begin(115200);
  wifiManager.autoConnect("SCS_SMARTPLUG");
  pinMode(IntRst, INPUT);
  pinMode(Outp, OUTPUT);
}

const char* host = "led.scsthai.com";

void loop(void) {
  delay(100);
  int stateBtn = digitalRead(IntRst);
  if (stateBtn == 1) {
    Serial.println("Reset : " + stateBtn);
    wifiManager.resetSettings();
  } else {
    //digitalWrite(Outp, LOW);

    // Clock Start
    if (updCnt <= 0) { // ทุก 1 นาทีอัพเดทข้อมูล
      //updCnt = 10;
      updCnt = 1;
      Serial.println("Getting data ...");

      //getWeatherData();

      Serial.print("connecting to ");
      Serial.println(host);

      // Use WiFiClient class to create TCP connections
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }

      String url = "/ledjson_farmsook.php?ledno=" + ledID + ""; // แก้ไขใหม่ ไม่ให้ php ส่ง [] ไปใน json
      
      Serial.print("Requesting URL: ");
      Serial.println(url);

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
      }
      
      String line;
      while (client.available()) {
        line = client.readStringUntil('\r');
        Serial.print(line);
      }

      Serial.println();
      //line = "{\"ledno\":\"farmsook1\",\"msg\":\"ON\"}";
      Serial.print("Line:");
      Serial.println(line);

      DynamicJsonBuffer jsonBuf;
      JsonObject &root = jsonBuf.parseObject(line);
      if (!root.success())
      {
        Serial.println("parseObject() failed");
        return;
      }
      Serial.println(root["msg"].as<String>());
      LEDMsg = root["msg"].as<String>();
      
      weatherString = LEDMsg;
      Serial.println(weatherString);
      Serial.println("closing connection");
      
      if (weatherString == "ON") { //On เป็น LOW เพราะ ใช้ Solid State Active Low
        digitalWrite(Outp, LOW);
      }
      if (weatherString == "OFF") {
        digitalWrite(Outp, HIGH);
      }
      Serial.println("Data loaded");
    }

    if (millis() - dotTime > 5000) { // ความถี่การอัพเดท
      dotTime = millis();
      dots = !dots;
      updCnt--;
    }

  }
}

