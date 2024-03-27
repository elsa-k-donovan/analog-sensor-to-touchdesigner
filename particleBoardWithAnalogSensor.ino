#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

#define ID_BASE 200
#define ID 1


/*ADD YOUR PASSWORD BELOW*/
const char *ssid = "WIFI_NAME"; 
const char *password = "WIFI_PASSWORD";

WiFiClient client;

// IPAddress staticIP(192, 168, 0, ID_BASE+ID);
IPAddress staticIP(192, 168, 0, 254); //192.168.0.254
IPAddress gateway(192, 168, 0, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 0, 254);


// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(255, 255, 255, 255); // Broadcast address

const unsigned int outPort = 9999;          // remote port (not needed for receive)
const unsigned int localPort = 8888;        // local port to listen for UDP packets (here's where we send the packets)

/*
* Connect your controller to WiFi
*/
void connectToWiFi() {
//Connect to WiFi Network
   Serial.println();
   Serial.println();
   Serial.print("Connecting to WiFi");
   Serial.println("...");

if (!WiFi.config(staticIP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure");
  }

WiFi.begin(ssid, password);

int retries = 0;

while ((WiFi.status() != WL_CONNECTED) && (retries < 15)) {
   retries++;
   delay(500);
   Serial.print(".");
}

if (retries > 14) {
    Serial.println(F("WiFi connection FAILED"));
}
if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Udp.begin(localPort);
}
    Serial.println(F("Setup ready"));
}

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the variable resistor is attached to
const int outPin = D0;  // Analog output pin that the LED is attached to

int sensorValue = 0;  // value read from the pot
int outputValue = 0;  // value output to the PWM (analog out)
int outputInversed = 0;

void setup() {
  pinMode(outPin, OUTPUT);  // Initialize the outPin as an output
  Serial.begin(9600);
  connectToWiFi();
}

void sendOSCMessage(OSCMessage &msg) {
  // Establish a connection to the OSC server
  Udp.beginPacket(outIp, outPort);

  // Serialize and send the OSC message
  msg.send(Udp);

  // End the packet
  Udp.endPacket();

  Serial.println("Sent UDP packet");
  msg.empty();
}

void loop() {

  // read the analog in value
  sensorValue = analogRead(analogInPin);

  // Create an OSC message
  OSCMessage msg("/sensor_pressure");
   
  // range of pressure sensor is (0 - 10)
  outputValue = map(sensorValue, 0, 10, 0, 255);

  outputInversed = outputValue+40;

  // if using an analog of digital PWM output uncomment below:
  // analogWrite(analogOutPin, outputInversed);

  msg.add(outputValue);

  // Send the OSC message
  sendOSCMessage(msg);

  if (outputInversed > 50){
    digitalWrite(outPin, LOW);
    Serial.println("LOW");
  }
  else{
    digitalWrite(outPin, HIGH);
    Serial.println("HIGH");
  }

  // print the results to the Serial Monitor:
  Serial.print("sensor = ");
  Serial.print(outputValue);
  Serial.print("\t output = ");
  Serial.println(outputInversed);

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(2);
}
