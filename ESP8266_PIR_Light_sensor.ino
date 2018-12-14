/*
	This example uses a ESP8266 board (Wemos d1 mini), HomeyDuino, a PIR, one led and a BH1750 lightsensor.
	Using this example you will be able to detect motion and the amount of light in the room and report to Homey. 
*/
#include <Wire.h>
#include <BH1750.h>
#include <ESP8266WiFi.h>
#include <Homey.h>

#define ledPin D7 // Green LED
#define pirPin D1 // Input for HC-SR501

BH1750 lightMeter(0x23);
float previousL = 0;
int state = LOW; // by default, no motion detected
int pirPinValue; // variable to store read PIR Value

unsigned long previousMillis = 0;
const unsigned long ldrInterval = 2 * 60 * 1000; //Interval in milliseconds (every 2 minutes)

const char* ssid     = "ssid";
const char* password = "password";

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  digitalWrite(ledPin, LOW); // led off

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin(D4, D3);
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);

      /*
    BH1750 has six different measurement modes. They are divided in two groups;
    continuous and one-time measurements. In continuous mode, sensor continuously
    measures lightness value. In one-time mode the sensor makes only one
    measurement and then goes into Power Down mode.
    Each mode, has three different precisions:
      - Low Resolution Mode - (4 lx precision, 16ms measurement time)
      - High Resolution Mode - (1 lx precision, 120ms measurement time)
      - High Resolution Mode 2 - (0.5 lx precision, 120ms measurement time)
    By default, the library uses Continuous High Resolution Mode, but you can
    set any other mode, by passing it to BH1750.begin() or BH1750.configure()
    functions.
    [!] Remember, if you use One-Time mode, your sensor will go to Power Down
    mode each time, when it completes a measurement and you've read it.
    Full mode list:
      BH1750_CONTINUOUS_LOW_RES_MODE
      BH1750_CONTINUOUS_HIGH_RES_MODE (default)
      BH1750_CONTINUOUS_HIGH_RES_MODE_2
      BH1750_ONE_TIME_LOW_RES_MODE
      BH1750_ONE_TIME_HIGH_RES_MODE
      BH1750_ONE_TIME_HIGH_RES_MODE_2
  */

  // begin returns a boolean that can be used to detect setup problems.
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else {
    Serial.println(F("Error initialising BH1750"));
  }
  
  previousMillis = ldrInterval;
  registerSensorWithHomey();
}

void loop()
{
  lightDetection();
  motionDetection();
}

void lightDetection()
{
  if (readLight()) // use interval to read
  {
    float lux = lightMeter.readLightLevel();
    //int lum = analogRead( LDRsensor);
    Serial.print("Luminosity ");Serial.println(lux);

    homeyConnect();
    Homey.setCapabilityValue("measure_luminance", lux);
    previousL = lux;
  }
}

bool readLight()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > ldrInterval)
  {
    previousMillis = currentMillis;
    return true;
  }
  else
  {
    return false;
  }
}

void homeyConnect()
{
  wifiConnect();
  Homey.loop();
}

void motionDetection()
{
  // motion detection sends motion_alarm immediately to Homey, no intervals
  pirPinValue = digitalRead(pirPin);
  if (pirPinValue == HIGH)
  {
    digitalWrite(ledPin, HIGH); // turn LED ON
    delay(100);                 // delay 100 milliseconds
    if (state == LOW)
    {
      Serial.println("Motion detected!");
      state = HIGH;
      setMotionAlarm(true);
    }
  }
  else
  {
    digitalWrite(ledPin, LOW); // turn LED OFF
    delay(200);                // delay 200 milliseconds

    if (state == HIGH)
    {
      Serial.println("Motion stopped!");
      state = LOW;
      setMotionAlarm(false);
    }
  }
}

void setMotionAlarm(bool motionDetected) {
  homeyConnect();
  Homey.setCapabilityValue("alarm_motion", (motionDetected));
}
void registerSensorWithHomey()
{
  wifiConnect();

  Serial.println("Register sensor with Homey");
  Homey.begin("PIR with lightsensor");
  Homey.setClass("sensor");
  Homey.addCapability("alarm_motion");
  Homey.addCapability("measure_luminance");
}

void wifiConnect()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Not connected to wifi, connecting now");
    WiFi.mode(WIFI_STA);
    //Connect to network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    //Print IP address
    Serial.print("Connected to IP address: ");
    Serial.print("Ip: "); Serial.print(WiFi.localIP());
    Serial.print("\tMAC: ");  Serial.println(WiFi.macAddress());
  }
}