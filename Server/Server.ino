#include <SPI.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <ESP8266HTTPClient.h>
int flag = 0;

String thingSpeakAddress = "http://api.thingspeak.com/update?";
String writeAPIKey;
String tsfield1Name;
String request_string;


char ssid[] = "<SSID>";           // SSID of your home WiFi
char pass[] = "<PWD>";            // password of your home WiFi

IPAddress ip(192, 168, 1, 80);      // server should have fixed/known ip
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

HTTPClient http;
WiFiServer server(12345);
WiFiClient client;

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
  WiFi.mode(WIFI_STA);               // ignoring this may show AP active if previously enabled
  WiFi.begin(ssid, pass);             // connects to the WiFi router
  Serial.println("\nServer - Started");
  Serial.print("Connecting to router - ");
  Serial.println(ssid);

  while ((!(WiFi.status() == WL_CONNECTED))) {
    delay(300);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Could not connect to "); Serial.println(ssid);
  } else {
    Serial.print("Connected to "); Serial.println(ssid);
  }

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  //Serial.println(client);
  unsigned int temp = 0;
  float maxpoint = 0;
  int i = 0;
  int wifiStatus = WiFi.status();
  if (wifiStatus == WL_CONNECTED && flag == 0) {
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    flag = 1;
  }
  for (i = 0; i < 500; i++) {
    if (temp = analogRead(analogchannel), temp > maxpoint) {
      maxpoint = temp;
    }
  }
  adcvalue = maxpoint;
  Voltage = (adcvalue / 1024.0) * 5000;
  ecurrent = ((Voltage - offsetvoltage) / sensitivity);
  ecurrent = (ecurrent) / (sqrt(2));
  ecurrent -= 0.392;
  //Serial.println(ecurrent);

  if (client) {
    if (client.connected()) {
      Serial.println(".");
      String request = client.readStringUntil('\r');    // receives the message from the client
      //Serial.print("From client: "); Serial.println(request);
      client.flush();
      client.println(ecurrent); // sends the answer to the client
      Serial.print("Server Current: ");
      Serial.print(ecurrent);
      Serial.println(" sent to Client!");



      delay(6000);
      WiFiClient client;
      if (client.connect("api.thingspeak.com", 80)) {
        writeAPIKey = "key=T53N10X4SHO2107P";
        tsfield1Name = "&field1=10";
        request_string = thingSpeakAddress;
        request_string += "key=";
        request_string += "NLKPJMVJL5OJEC2G";
        request_string += "&";
        request_string += "field1";
        request_string += "=";
        request_string += ecurrent;
        request_string += "&";
        request_string += "field2";
        request_string += "=";
        request_string += "123";
        http.begin(request_string);
        http.GET();
        http.end();
        Serial.println("Data uploaded to ThingsSpeak");
      }




    }
    //client.stop();                // tarminates the connection with the client
  }
  delay(1000);

}
