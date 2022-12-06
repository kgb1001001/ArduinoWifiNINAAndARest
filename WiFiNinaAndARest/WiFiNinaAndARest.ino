/*
  Example using the aREST library with the WifiNINA library (for the Arduino Uno Wifi Rev 2)
  This example has been tested on an Arduino Uno Wifi Rev 2 upgraded to the 1.4.8 firmware. 

  Contributed by Kyle Brown
  Written and tested in December 2021
  
 */

#include <SPI.h>
#include <WiFiNINA.h>
#include <aREST.h>


#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Create aREST instance
aREST rest = aREST();

// Initialize the WiFi server library
WiFiServer server(80);

// Variables to be exposed to the API
int temperature;
int humidity;

// Declare functions to be exposed to the API
int ledControl(String command);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  // Init variables and expose them to REST API
  temperature = 24;
  humidity = 40;
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);

  rest.function("led",ledControl);

  // Give name and ID to device (ID should be 6 characters long)
  rest.set_id("008");
  rest.set_name("dapper_drake");

  // Function to be exposed
  rest.function("led",ledControl);

  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {

  // listen for incoming clients
  WiFiClient client = server.available();
  rest.handle(client);

}

// Custom function accessible by the API
// The Arduino Uno Wifi (Rev 2) has a builtin LED so there is no need for additional hardware
int ledControl(String command) {

  // Get state from command
  int state = command.toInt();
  digitalWrite(LED_BUILTIN,state);
  return 1;
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
