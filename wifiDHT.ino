/**
   wifiDHT.ino

    Created on: 2021.11.1

*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>


#define DHTTYPE DHT11
static const int DHT_SENSOR_PIN = 5;
DHT dht(DHT_SENSOR_PIN, DHTTYPE); 
float t = 0.0;
float h = 0.0;

#define USE_SERIAL Serial

#define SERVER_IP "192.168.0.13:3000" //server to send data to

#ifndef STASSID
#define STASSID "tmargesson1" // networkId
#define STAPSK  "ecitsuj53?" // password
#endif

void setup() {
  //pinMode(LED_BUILTIN, OUTPUT);
  dht.begin();

  USE_SERIAL.begin(9600);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USE_SERIAL.println("waiting...");
  }
  USE_SERIAL.println("");
  USE_SERIAL.print("Connected! IP address: ");
  USE_SERIAL.println(WiFi.localIP());

}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/"); //HTTP
    // http.addHeader("Content-Type", "text/plain");
    http.addHeader("Content-Type", "application/json");

    USE_SERIAL.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body

    float newT = dht.readTemperature(true);
    if (isnan(newT)) {
      // failed
    }
    else {
      t = newT;
    }
    float newH = dht.readHumidity();
    if (isnan(newH)) {
      // failed
    }
    else {
      h = newH;
    }
      
      int httpCode = http.POST("{\"temp\":\"" + String(t) + "\", \"humidity\":" + String(h) + "}");

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          const String& payload = http.getString();
          USE_SERIAL.println("received payload:\n<<");
          USE_SERIAL.println(payload);
          USE_SERIAL.println(">>");
        }
      } else {
        USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
      digitalWrite(LED_BUILTIN, LOW);


  }
  delay(10000);
}
