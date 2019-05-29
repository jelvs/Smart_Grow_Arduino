






// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <SoftwareSerial.h>


#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Analog for Soil Moisture Sensor
#define SoilMoisturePin A0 
//Analog for Light Sensor
#define LightPin A2  

// Initialize DHT sensor.

DHT dht(DHTPIN, DHTTYPE);

// Initialize serial communication
SoftwareSerial wifiSerial(10, 11); // RX, TX

//Soil Sensor Value
float soilSensorValue = 0;
//Light Sensor Value
float lightValue = 0;

int greenLedPin = 3;
int yellowLedPin = 4;
int redLedPin = 5;

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHT11 test!"));
  //pinMode(greenLedPin, OUTPUT);
  //pinMode(yellowLedPin,OUTPUT);
  //pinMode(redLedPin,OUTPUT);
  dht.begin();
  wifiSerial.begin(4800);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(10000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  soilSensorValue = soilSensorValue + analogRead(SoilMoisturePin);
  lightValue = analogRead(LightPin);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  soilSensorValue = soilSensorValue/100.0; 
  Serial.println("Soil Reading: " + String(soilSensorValue)); 
  Serial.println("Light Reading: " + String(lightValue)); 
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  
  Serial.println("Sending temperature to wifi module...");
  wifiSerial.print("temp:" + String(hic) + ";hum:" + String(h) + ";soil:" + String(soilSensorValue) + ";light:" + String(lightValue));
  delay(5000);
}
