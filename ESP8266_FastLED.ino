/*
	This example uses a ESP8266 board, HomeyDuino and FastLED.
	Using this example you will be able to control the leds using Homey. 
	If you are powering a lot of leds take a look at this site https://www.eerkmans.nl/powering-lots-of-leds-from-arduino/
*/
#include <ESP8266WiFi.h>
#include <Homey.h>
#include "FastLED.h"
#define NUM_LEDS 167
#define PIN 3 //A0 on Wemos D1 Mini

CRGB leds[NUM_LEDS];

const char* ssid     = "ssid";
const char* password = "password";

bool ledOnOff = false;
float ledHue = 0;
float ledSaturation = 0;
float ledBrightness = 0;

void setup()
{
  // Enable serial
  Serial.begin(115200);
  
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.clear(true);
  
  registerSensorWithHomey();
}

void loop() { 
  wifiConnect();
  //Handle incoming connections
  Homey.loop();
}

void SendColorToLeds(){
  Serial.print("Hue:\t");Serial.print(ledHue);Serial.print("\tSaturation:\t");Serial.print(ledSaturation);Serial.print("\tBrightness:\t");Serial.println(ledBrightness);
  Serial.print("Convert to 255 -> Hue:\t");Serial.print(round(255 * ledHue));Serial.print("\tSaturation:\t");Serial.print(round(255 * ledSaturation));Serial.print("\tBrightness:\t");Serial.println(round(255 * ledBrightness));
  //Convert color from float 0..1 to byte 0..255
  CHSV hsvval(round(255 * ledHue),round(255 * ledSaturation),round(255 * ledBrightness));
  
  if(ledOnOff){
    fill_solid(leds, NUM_LEDS,  hsvval);// set color
    FastLED.show();
  }else{
    FastLED.clear(true); //leds off
  } 
}

void lightsOnoff( void ) {
  ledOnOff = Homey.value.toInt(); //boolean
  Serial.print("Light OnOff\t");Serial.println(ledOnOff);
  SendColorToLeds();
}

void setHue( void ) {
  Serial.print("Light Hue\t");
  ledHue = Homey.value.toFloat(); //number 0.00 - 1.00
  Serial.println(ledHue);
  SendColorToLeds();
}
void dimLights( void ) {
  Serial.print("Light Dim\t");
  ledBrightness = Homey.value.toFloat(); //number 0.00 - 1.00
  Serial.println(ledBrightness);
  SendColorToLeds();
}
void setSaturation( void ) {
  Serial.print("Light Saturation\t");
  ledSaturation = Homey.value.toFloat(); //number 0.00 - 1.00
  Serial.println(ledSaturation);
  SendColorToLeds();
}
void registerSensorWithHomey() {
  Serial.println("Register sensor with Homey");
  wifiConnect(); // connect to wifi
  
  Homey.begin("HomeyDuino Ledstrip");

  Homey.setClass("light");
  Homey.addCapability("onoff", lightsOnoff);                 //boolean 
  Homey.addCapability("dim", dimLights);                     //number 0.00 - 1.00
  Homey.addCapability("light_hue", setHue);                  //number 0.00 - 1.00
  Homey.addCapability("light_saturation", setSaturation);    //number 0.00 - 1.00

  Homey.setCapabilityValue("onoff", ledOnOff); //Set initial value
  Homey.setCapabilityValue("dim", ledBrightness); //Set initial value
  Homey.setCapabilityValue("light_hue", ledHue); //Set initial value
  Homey.setCapabilityValue("light_saturation", ledSaturation); //Set initial value
}

void wifiConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to wifi, connecting now");
    //WiFi.mode(WIFI_STA);
    //Connect to network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { 
      delay(500); Serial.print("."); 
    }
  
    //Print IP address
    Serial.print("Connected to IP address: ");
    Serial.print("Ip: "); Serial.print(WiFi.localIP());
    Serial.print("\tMAC: "); Serial.println(WiFi.macAddress());
  }
}
