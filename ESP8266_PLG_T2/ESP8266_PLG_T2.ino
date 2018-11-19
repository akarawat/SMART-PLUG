#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h> 
WiFiManager wifiManager;
int IntRst = 15; // Pin D8=15, 
int Outp = 13; // Pin D7=13, 
WiFiClient client;

String ledID = "homeplug1";//5760561,9983908,3161443,4037176,
String LEDMsg = "";
String weatherString;
int updCnt = 0;
int dots = 0;
long dotTime = 0;
long clkTime = 0;
String date;
String YY, MM, DD;

void setup(void) {
  delay(100);
  Serial.begin(115200);
  wifiManager.autoConnect("SCS_SMRATPLUG");
  pinMode(IntRst, INPUT);
  pinMode(Outp, OUTPUT);
  
}

void loop(void) {
  delay(100);
  int stateBtn = digitalRead(IntRst);
  if (stateBtn == 1){
    Serial.println("Reset : " + stateBtn);
    wifiManager.resetSettings();
  }else{
    //digitalWrite(Outp, LOW);
    
    // Clock Start
    if (updCnt <= 0) { // ทุก 1 นาทีอัพเดทข้อมูล
      //updCnt = 10;
      updCnt = 1;
      Serial.println("Getting data ...");
      getWeatherData();
      if (weatherString == "ON"){
        digitalWrite(Outp, LOW);
      }
      if (weatherString == "OFF"){
        digitalWrite(Outp, HIGH);
      }
      //getDate();
      //getData();
      Serial.println("Data loaded");
      clkTime = millis();
    }
    
    if (millis() - clkTime > 1500 && dots) { //ทุก 15 วินาทีแสดงสภาพอากาศ
      String txtShow;      
      txtShow = weatherString;
      //ScrollText(txtShow);
      Serial.println("15 SEC : " + txtShow);
      updCnt--;
      clkTime = millis();
    }
    if (millis() - dotTime > 5000) { // ความถี่การอัพเดท 
      dotTime = millis();
      dots = !dots;
    }
    
  }
}

/// === ดึงข้อมูล === ///

//const char *weatherHost = "api.openweathermap.org";
const char *weatherHost = "www.scsthai.com";
//http://27.254.172.48/plesk-site-preview/led.scsthai.com/ledjson.php?ledno=9983908
//ต้องเข้าผ่านไอพีเท่านั้น เว็ย scs โฮสท์มันไม่ให้ใช้ IP
void getWeatherData()
{
  //led.scsthai.com/ledjson.php?ledno=3161443
  Serial.print("connecting to "); Serial.println(weatherHost);
  if (client.connect(weatherHost, 80)) {
    client.println(String("GET /plesk-site-preview/led.scsthai.com/ledjson.php?ledno=") + ledID + "\r\n" +
                   "Host: " + weatherHost + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                   "Connection: close\r\n\r\n");
  } else {
    Serial.println("connection failed");
    return;
  }
  String line;
  
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println("w.");
    repeatCounter++;
  }
  
  while (client.connected() && client.available()) {
    char c = client.read();
    if (c == '[' || c == ']') c = ' ';
    line += c;
  }
  client.stop();
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
  //weatherString = DD + " " + MM + " " + YY + " ";
  weatherString = LEDMsg;
  //Serial.println(weatherString);
  
  /*
    weatherDescription = root["weather"]["description"].as<String>();
    weatherDescription.toLowerCase();
    temp = root["main"]["temp"];
    humidity = root["main"]["humidity"];
    pressure = root["main"]["pressure"];
    tempMin = root["main"]["temp_min"];
    tempMax = root["main"]["temp_max"];
    windSpeed = root["wind"]["speed"];
    clouds = root["clouds"]["all"];
    String deg = String(char('~'+25));

    weatherString += "Temp:" + String(temp,1)+"C ";
    weatherString += weatherDescription;
    weatherString += " Humid:" + String(humidity) + "% ";
    weatherString += "Pressure:" + String(pressure/1.3332239) + " hpa ";
    weatherString += "Cloudiness:" + String(clouds) + "% ";
    weatherString += "Wind:" + String(windSpeed,1) + " m/s ";
  */
}

void getDate()
{
  WiFiClient client;
  if (!client.connect("www.google.com", 80)) {
    Serial.println("connection to google failed");

    return;
  }
  client.print(String("GET / HTTP/1.1\r\n") +
               String("Host: www.google.com\r\n") +
               String("Connection: close\r\n\r\n"));
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    //Serial.println(".");
    repeatCounter++;
  }

  String line;
  client.setNoDelay(false);
  while (client.connected() && client.available()) {
    line = client.readStringUntil('\n');
    line.toUpperCase();
    if (line.startsWith("DATE: ")) {
      date = "     " + line.substring(6, 22);
      DD = line.substring(11, 13);
      MM = line.substring(14, 17);
      YY = line.substring(18, 22);
    }
  }
  client.stop();
  weatherString = DD + " " + MM + " " + YY + " ";
}

