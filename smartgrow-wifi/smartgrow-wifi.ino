#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "Pixel_7800";
const char* password = "fb0e2ec94666";

const char fingerprint[] PROGMEM = "AB D6 83 53 83 E3 5A EF 40 16 2C 26 70 56 30 11 54 BA 28 DB";

//Link to read data from https://jsonplaceholder.typicode.com/comments?postId=7
//Web/Server address to read/write from 
const char *host = "api.smartgrow.space";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

SoftwareSerial wifiSerial(12, 14); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("ESP Started!");

      // set the data rate for the SoftwareSerial port
  wifiSerial.begin(4800);

  //mySerial.println("I am esp");

  WiFi.mode(WIFI_OFF); //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA); 
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.println(WiFi.localIP());// Print the IP address
}

void sendRequest (String newReading) {
  Serial.println ("Sending new request");
            
        httpsClient.println("POST /temperature HTTP/1.1");
        httpsClient.println ("Host: api.smartgrow.space");
        httpsClient.println ("Connection: keep-alive");
        httpsClient.println ("User-Agent: Arduino");
        httpsClient.println ("Content-Type: application/json");
        httpsClient.println ("content-length: 21");
        httpsClient.println ("");    
        httpsClient.print ("{\n\t\"reading\": ");
        httpsClient.print (newReading);
        httpsClient.print ("\n}");

        Serial.println("request sent");
}

void loop() { // run over and over
  String newReading;
  
  if (wifiSerial.available()) {
    newReading = wifiSerial.readString();
    Serial.println("New reading: " + newReading);

    if (httpsClient.connected()){
      Serial.println ("Using old connection");

      if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
        sendRequest (newReading);
      } else {
        Serial.println ("Wifi NOT connected");
      }  
    } else {
      Serial.println("Https new Connection");
      httpsClient.setFingerprint(fingerprint);
      httpsClient.setTimeout(15000);
      delay(1000);
    
      int r=0; //retry counter
      while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
        delay(100);
        Serial.print(".");
        r++;
      }

      sendRequest (newReading);
    } 
  } else {
    delay(5000);
  }
}
