//#include <WiFi101.h>
#include <rgb_lcd.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "rgb_lcd.h"
int status = WL_IDLE_STATUS;     // the WiFi radio's status

rgb_lcd lcd;

#define SENSORPIN 4
#define LEDPIN 13

//COUNT INTS
int count = 0;
int sensorState = 0;
int prestate = 0;         // variable for reading the pushbutton status

//INDUCTIVE SENSOR
int Aluminum = 0;
int InduPin = 1;
int AluminumPrestate = 0;

//LED INTS
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

//HTTPPushingBox
const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "v699139878AAC62C"; //device ID on Pushingbox for our Scenario

const char* ssid = "eduroam";
const char* user =  "f005s5t";
const char* pass =  "Pumpkin12!";


// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)


void setup() {
  lcd.begin(16, 2);

  lcd.setRGB(colorR, colorG, colorB);

  delay(1000);
 
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);      
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);    
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
//  while (!Serial) 
//  {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  // check for the presence of the shield:
//  if (WiFi.status() == WL_NO_SHIELD) {
//    Serial.println("WiFi shield not present");
//    // don't continue:
//    while (true);
//  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to SSID: ");
    lcd.print ("Attempting SSID: ");
    Serial.println(ssid);
    lcd.println(ssid);
    //Connect to WPA/WPA2 network.Change this line if using open/WEP network
    status = WiFi.beginEnterprise(ssid, user, pass);

    // wait 10 seconds for connection:
    delay(10000);

  }
  
  Serial.println("Connected to wifi");
  lcd.setCursor(0,0);
  lcd.print("Connected");
  delay(1000);
  lcd.clear();


  printWifiStatus();
  
}

void loop() {

     //delay(10000);



  sensorState = digitalRead(SENSORPIN);
  Aluminum = digitalRead(InduPin);

  if (Aluminum == 1)
  {
    AluminumPrestate = 1;
  }

  if (sensorState == LOW && prestate == 0)
  {
    count = (count + 1);
    Serial.println(count + "cans recycled"); 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Cans Recycled:"); 
    lcd.setCursor(0,2);
    lcd.print(count); 
    digitalWrite(LEDPIN, HIGH);
    prestate = 1;


    //Sending Data To sheet
     int canCount = count;
    Serial.println("\nSending Data to Server..."); 
    WiFiClient client;  //Instantiate WiFi object, can scope from here or Globally

    //API service using WiFi Client through PushingBox then relayed to Google
    if (client.connect(WEBSITE, 80))
        { 
         client.print("GET /pushingbox?devid=" + devid
       + "&canCount=" + (String) canCount
       + "&celData="      + (String) AluminumPrestate
         );

              // HTTP 1.1 provides a persistent connection, allowing batched requests
             // or pipelined to an output buffer
             client.println(" HTTP/1.1"); 
             client.print("Host: ");
             client.println(WEBSITE);
             client.println("User-Agent: MKR1000/1.0");
             //for MKR1000, unlike esp8266, do not close connection
             client.println();
             Serial.println("\nData Sent"); 
             delay(1000);
             client.stop();
             AluminumPrestate = 0;


         }

     //END
  }
  else if (sensorState == HIGH){
    prestate = 0;

  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
