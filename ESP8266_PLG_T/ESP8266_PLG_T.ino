#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h> 
WiFiManager wifiManager;
int IntRst = 15; // Pin D8=15, 
int Outp = 13; // Pin D7=13, 

void setup() {
  delay(100);
  Serial.begin(115200);
  wifiManager.autoConnect("SCS_SMRATPLUG");
  pinMode(IntRst, INPUT);
  pinMode(Outp, OUTPUT);
  
}

void loop() {
  int stateBtn = digitalRead(IntRst);
  if (stateBtn == 1){
    Serial.println("Reset : " + stateBtn);
    wifiManager.resetSettings();
  }else{
    digitalWrite(Outp, LOW);
  }
}
