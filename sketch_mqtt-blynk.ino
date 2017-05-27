/**************************************************************
 * IoT Motion Detector with Blynk
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 * 
 * Developed by Marcelo Rovai - 30 November 2016
 * Modified by Joachim Hummel - 27 Mai 2017
 **************************************************************/

/* https://play.google.com/store/apps/details?id=cc.blynk */

/***********************   Includes **************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <BlynkSimpleEsp8266.h>

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "mqtt.unixweb.de"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

/* HC-SR501 / AS312 Motion Detector */
#define ledPin D7 // LED Output
#define pirPin D1 // Input for PIR AS312
#define DHTPIN D2 //  Spare for DHT Seonsors
int pirValue; // Place to store read PIR Value


/************************* WiFi Access Point *********************************/
/* WiFi credentials */
char ssid[] = ".......";
char pass[] = ".......";

/************************* BLYNK AuTH *********************************/
char auth[] = ".......";

/************ Global State MQTT ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/a1b2c3d4e5");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/

void MQTT_connect();

// Time to sleep (in seconds):
const int sleepTimeS = 10;
// Host
const char* host = "dweet.io";

 


void setup()
{
  Serial.begin(115200);
  delay(10);
  Blynk.begin(auth, ssid, pass);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  digitalWrite(ledPin, LOW);
  
}
 
void loop()
{
  MQTT_connect();
  getPirValue();
  delay(10);
  Blynk.run();
}

void getPirValue(void)
{
  pirValue = digitalRead(pirPin);
  if (pirValue) 
  { 
    Serial.println("==> Motion detected");
    (! photocell.publish("Motion_Detected"));
    Blynk.notify("T==> Motion detected");  
  }
  digitalWrite(ledPin, pirValue);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
