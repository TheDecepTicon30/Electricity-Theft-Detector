#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "<SSID>";                  -------- WiFi SSID   
const char* pass = "<PWD>";                   -------- WiFi Password
const char* host = "192.168.2.108";

void setup() {
  delay(500);
  Serial.begin(38400);
  Serial.println();
  Serial.print("conecting to: ");
  Serial.println(ssid);
  Serial.println("Try to connect to server: ");
  Serial.println(host);

  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("My IP: ");
  Serial.println(WiFi.localIP());
  long rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void loop() {

  WiFiClient client;

  if (!client.connect(host, 3000)) { //The problem start here
    Serial.println(".");
    return;
  }

  Serial.println();
  Serial.print("Conected to IP: ");
  Serial.println(host);

  Serial.println("Sending string to server: ");
  client.println("OK");
  delay(100);

  String line = client.readStringUntil('\n');
  Serial.print(line);

  Serial.println();
  Serial.println("Closing connection");
  client.flush();
  client.stop();
  Serial.println("Connection Closed");

}
