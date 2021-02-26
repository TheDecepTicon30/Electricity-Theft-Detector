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

HTTPClient http;

const int analogchannel = 0; //Connect current sensor with A0 of Arduino
int sensitivity = 185; // use 100 for 20A Module and 66 for 30A Module
float adcvalue = 0;
int offsetvoltage = 2500;
double Voltage = 0; //voltage measuring
double ecurrent = 0;// Current measuring

byte ledPin = 2;
unsigned long askTimer = 0;

IPAddress server(192, 168, 1, 80);    // the fix IP address of the server
WiFiClient client;

void setup() {
  Serial.begin(115200);               // only for debug
  WiFi.mode(WIFI_STA);               // ignoring this may show AP active if previously enabled
  WiFi.begin(ssid, pass);             // connects to the WiFi router
  Serial.println("\nClient - Started");
  Serial.print("Connecting to router - ");
  Serial.println(ssid);
    
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Could not connect to "); Serial.println(ssid);
  } else {
    Serial.print("Connected to "); Serial.println(ssid);
  }
    
  pinMode(ledPin, OUTPUT);
}

void loop () {
  unsigned int temp = 0;
  float maxpoint = 0;
  int i = 0;
  int wifiStatus = WiFi.status();
  if (wifiStatus == WL_CONNECTED && flag == 0) {
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    flag = 1;
  }
  for (i = 0; i < 500; i++)
  {
    if (temp = analogRead(analogchannel), temp > maxpoint)
    {
      maxpoint = temp;
    }
  }
  adcvalue = maxpoint;
  Voltage = (adcvalue / 1024.0) * 5000;
  ecurrent = ((Voltage - offsetvoltage) / sensitivity);
  ecurrent = ( ecurrent ) / ( sqrt(2) );
  ecurrent -= 0.392;
  //  time_t t = time.now(); // Store the current time in time
  client.connect(server, 12345);
  Serial.println(".");
  client.println("Hello server! Are you sleeping?\r");  // sends the message to the server
  
  String answer = client.readStringUntil('\r');   // receives the answer from the sever
  Serial.println("Server Current: " + answer);
  Serial.print("Client Current = ");
  Serial.println(roundf(ecurrent * 100) / 100, 3);
    
  delay(6000);
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
    request_string += answer;
    http.begin(request_string);
    http.GET();
    http.end();
    Serial.println("Data uploaded to ThingsSpeak");
  }

  client.flush();
  delay(2000);                  // client will trigger the communication after two seconds
}
