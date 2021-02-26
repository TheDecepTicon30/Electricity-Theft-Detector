#include <ESP8266WiFi.h>
#include <time.h>
#include <SPI.h>
#include <ESP8266HTTPClient.h>
int flag = 0;
WiFiClient client;

IPAddress ip(192, 168, 0, 80);           
IPAddress gateway(192,168,0,1);          
IPAddress subnet(255,255,255,0);

String thingSpeakAddress = "http://api.thingspeak.com/update?";
String writeAPIKey;
String tsfield1Name;
String request_string;

HTTPClient http;
WiFiServer server(80);

const int analogchannel = 0;
int sensitivity = 185;
float adcvalue = 0;
int offsetvoltage = 2500;
double Voltage = 0;
double ecurrent = 0;
int count = 0;
void setup() {
  Serial.begin(115200);
  WiFi.config(ip, gateway, subnet);  
  WiFi.begin("Optilink", "Aksharva");
  while ((!(WiFi.status() == WL_CONNECTED))) {
    delay(300);
    Serial.print(".");
  }
   server.begin(); 
}

void loop(){
  delay(1000);
  WiFiClient client = server.available();
  Serial.println(client);
  unsigned int temp = 0;
  float maxpoint = 0;
  int i = 0;
  int wifiStatus = WiFi.status();
  if (wifiStatus == WL_CONNECTED && flag == 0){
    Serial.println("Connected to IP:");
    Serial.println(WiFi.localIP());
    flag = 1;
  }
  for (i = 0; i < 500; i++){
    if (temp = analogRead(analogchannel), temp > maxpoint){
      maxpoint = temp;
    }
  }
  adcvalue = maxpoint;
  Voltage = (adcvalue/1024.0) * 5000;
  ecurrent = ((Voltage - offsetvoltage)/sensitivity);
  ecurrent = (ecurrent)/(sqrt(2));
  ecurrent -= 0.392;
  if(ecurrent < 0)
    ecurrent = 0;
  Serial.println("Server current:" + (String)ecurrent);
  
  if (client) {
    if (client.connected()) {
      Serial.println(".");
      String request = client.readStringUntil('\r');    // receives the message from the client
      Serial.print("From client: "); Serial.println(request);
      client.flush();
      Serial.println(ecurrent);
      client.println(ecurrent); // sends the answer to the client
    }
    //client.stop();                // tarminates the connection with the client
  }
  delay(1000);

}
