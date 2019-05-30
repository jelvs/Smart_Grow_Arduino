#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <SoftwareSerial.h>

#define DHTPIN 2 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11

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
int redLedPin = 6;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Arduino Started!"));

  // Set up pins for the LEDS
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin,OUTPUT);

  dht.begin();
  wifiSerial.begin(4800);
}

// Turn light on/off depending on the request
void turnOnOFFLight(String message){
  if(message.equals("ON")){
    digitalWrite(greenLedPin, HIGH);
  }else{
    digitalWrite(greenLedPin, LOW);
  }
}

// Turn water on/off depending on the request
void turnOnOFFWater(String message){
  if(message.equals("ON")){
    digitalWrite(redLedPin, HIGH);
  }else{
    digitalWrite(redLedPin, LOW);
  }
}

void loop() {
  // Wait a few seconds between measurements.
  delay(10000);

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  soilSensorValue = soilSensorValue + analogRead(SoilMoisturePin);
  lightValue = analogRead(LightPin);
  soilSensorValue = soilSensorValue/100.0; 
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(F("  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(F("  Soil: "));
  Serial.print(soilSensorValue);
  Serial.print(F("   Light: "));
  Serial.println(lightValue);

  
  Serial.println("Sending temperature to wifi module...");
  wifiSerial.print("temp:" + String(hic) + ";hum:" + String(h) + ";soil:" + String(soilSensorValue) + ";light:" + String(lightValue) + ";finish");
  delay(500);

  // Receiving requests from the wifi module to turn on/off light/water
  if (wifiSerial.available()) {
    String light_water_switch = wifiSerial.readString();
    int ind1 = light_water_switch.indexOf(':');
    String switchString = light_water_switch.substring(0, ind1); 
    if(switchString.equals("Light")){
      String onOff = light_water_switch.substring(ind1+1, light_water_switch.length()); 
      turnOnOFFLight(onOff);
    }else{
      String onOff = light_water_switch.substring(ind1+1, light_water_switch.length()); 
      turnOnOFFWater(onOff);
    }  
  }
}
