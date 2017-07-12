
// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// PECMAC125A
// This code is designed to work with the PECMAC125A_DLCT03C20 I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Current?sku=PECMAC125A_DLCT03C20#tabs-0-product_tabset-2
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

#define Addr 0x2A
///// enter your wifi login info///////////
const char* ssid = "your ssid network";
const char* password = "your password";
unsigned int data[36];
int typeOfSensor = 0;
int maxCurrent = 0;
int noOfChannel = 0;
ESP8266WebServer server(80);

void handleroot()
{
  server.sendContent
  ("<html><head><meta http-equiv='refresh' content='5'</meta>"
   "<h1 style=text-align:center;font-size:300%;color:blue;font-family:britannic bold;>CONTROL EVERYTHING</h1>"
   "<h3 style=text-align:center;font-family:courier new;><a href=http://www.controleverything.com/ target=_blank>www.controleverything.com</a></h3><hr>"
   "<h2 style=text-align:center;font-family:tahoma;><a href=https://www.controleverything.com/content/Current?sku=PECMAC125A_DLCT03C20#tabs-0-product_tabset-2 \n"
   "target=_blank>Daniel Energy Monitor Controller</a></h2>");
   
  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Command header byte-1
  Wire.write(0x92);
  // Command header byte-2
  Wire.write(0x6A);
  // Command 2 is used to read no of sensor type, Max current, No. of channel
  Wire.write(0x02);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // Reserved
  Wire.write(0x00);
  // CheckSum
  Wire.write(0xFE);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 6 bytes of data
  Wire.requestFrom(Addr, 6);

  // Read 6 bytes of data
  if (Wire.available() == 6)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
  }

  typeOfSensor = data[0];
  maxCurrent = data[1];
  noOfChannel = data[2];
 // Output data to serial monitor
  Serial.print("Type Of Sensor : ");
  Serial.println(typeOfSensor);
  Serial.print("Max Current : ");
  Serial.print(maxCurrent);
  Serial.println(" Amp");
  Serial.print("No. Of Channel : ");
  Serial.println(noOfChannel);

 for (int j = 1; j < noOfChannel + 1; j++)
  {
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Command header byte-1
    Wire.write(0x92);
    // Command header byte-2
    Wire.write(0x6A);
    // Command 1
    Wire.write(0x01);
    // Start Channel No.
    Wire.write(j);
    // End Channel No.
    Wire.write(j);
    // Reserved
    Wire.write(0x00);
    // Reserved
    Wire.write(0x00);
    // CheckSum
    Wire.write((0x92 + 0x6A + 0x01 + j + j + 0x00 + 0x00) & 0xFF);
    // Stop I2C Transmission
    Wire.endTransmission();
    delay(500);

    // Request 3 bytes of data
    Wire.requestFrom(Addr, 3);

    // Read 3 bytes of data
    // msb1, msb, lsb
    int msb1 = Wire.read();
    int msb = Wire.read();
    int lsb = Wire.read();
    float current = (msb1 * 65536) + (msb * 256) + lsb;

    // Convert the data to ampere
    current = current / 1000;

    // Output to the serial monitor
    Serial.print("Channel : ");
    Serial.println(j);
    Serial.print("Current Value : ");
    Serial.println(current);
    delay(1000);
 

  // Output data to web server
    server.sendContent
  ("<h3 style=text-align:center;font-family:tahoma;>Channel Number = " + String(j) + " ");
  server.sendContent
  ("<h3 style=text-align:center;font-family:tahoma;>Current Value = " + String(current) + " Amp");
  
  
  }
}

void setup()
{
  // Initialise I2C communication as MASTER
  Wire.begin(12, 14);
  // Initialise serial communication, set baud rate = 115200
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  // Get the IP address of ESP8266
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleroot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}
