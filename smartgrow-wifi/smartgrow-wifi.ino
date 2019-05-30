#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
 
ESP8266WebServer server(80);

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
  server.on("/light", handleLight); //Associate the handler function to the path
  server.on("/water", handleWater); //Associate the handler function to the path
  server.begin(); //Start the server
  Serial.println("Server listening");
}

void sendRequest (String newReading, String path) {
  Serial.println ("Sending new request");
            
        httpsClient.print("POST ");
        httpsClient.print(path);
        httpsClient.println(" HTTP/1.1");
        httpsClient.println ("Host: api.smartgrow.space");
        httpsClient.println ("Connection: keep-alive");
        httpsClient.println ("User-Agent: Arduino");
        httpsClient.println ("Content-Type: application/json");
        httpsClient.println ("content-length: 21");
        httpsClient.println ("");    
        httpsClient.print ("{\n\t\"reading\": ");
        httpsClient.print (newReading);
        httpsClient.print ("\n}");
       

        //Serial.println("request sent");
        //delay(5000);
        
        
}



void splitTempHumString (String newReading) {
  int ind1 = newReading.indexOf(';');  //finds location of first ,
  
  //temperature
  String tempString = newReading.substring(0, ind1);   //captures first data String
  
  int readingTempIndex = tempString.indexOf(':');
  String temp = tempString.substring(readingTempIndex+1, tempString.length());
  
  //Serial.println (temp);
  sendRequest (temp, String("/temperature"));
  //
  
  int ind2 = newReading.indexOf(';', ind1+1 );
  
  //humidity
  String humString = newReading.substring(ind1+1, ind2);
  
  int readingHumIndex = humString.indexOf(':');
  String hum = humString.substring(readingHumIndex+1, humString.length());
  //Serial.println(hum);
  
  sendRequest(hum, String("/humidity"));
  //
  
  int ind3 = newReading.indexOf(';', ind2+1 );
  
  //soil
  String soilString = newReading.substring(ind2+1, ind3);
  //Serial.println("soilString:" + soilString);
  
  int readingSoilIndex = soilString.indexOf(':');
  String soil = soilString.substring(readingSoilIndex+1, soilString.length());
  //Serial.println("soil: " + soil);

  sendRequest(soil, String("/soil"));
  //
  
  int ind4 = newReading.indexOf(';', ind3+1);
  
  //light
  String lightString = newReading.substring(ind3+1, ind4);
 
  int readingLightIndex = lightString.indexOf(':');
  String light = lightString.substring(readingLightIndex+1, lightString.length()-1);
  sendRequest(light, String("/light"));
  //
}

void handleLight() { //Handler for the body path
 
      if (server.hasArg("plain")== false){ //Check if body received
 
            server.send(200, "text/plain", "Body not received");
            return;
 
      }
 
      String message = "Light:";
             message += server.arg("plain");
             
 
      server.send(200, "text/plain", message);
      //Serial.println(message);
      wifiSerial.print(message);
}

void handleWater() { //Handler for the body path
 
      if (server.hasArg("plain")== false){ //Check if body received
 
            server.send(200, "text/plain", "Body not received");
            return;
 
      }
 
      String message = "Water:";
             message += server.arg("plain");
 
      server.send(200, "text/plain", message);
      wifiSerial.print(message);
 
}

void loop() { // run over and over
  String newReading;
  server.handleClient();
  
  if (wifiSerial.available()) {
    newReading = wifiSerial.readString();
    Serial.println("New reading: " + newReading);
    splitTempHumString (newReading);

    if (httpsClient.connected()){
      Serial.println ("Using old connection");

      if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
        //sendRequest (newReading);
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

      //sendRequest (newReading);
    }
  } else {
    delay(5000);
  }
}
