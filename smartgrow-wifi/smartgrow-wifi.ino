#include <Arduino_JSON.h>

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "DD-WRT_CAMELOT";
const char* password = "CountachLP5000S";

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
    Serial.println("Connecting..");
  }

  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address

  Serial.println(host); 
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000);
  delay(1000);

  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
    delay(100);
    Serial.print(".");
    r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
  }


 

  
}

void loop() { // run over and over
  StaticJsonBuffer<200> temp;
  if (wifiSerial.available()) {
    Serial.write(wifiSerial.read());
    temp["reading"] = wifiSerial.read();
    //serializeJson(temp, Serial);
     
  }

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    Serial.println ("Sending new request");
    String getData, Link;
    Link = "/temperature";

    httpsClient.print(String("POST ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: keep-alive\r\n" + 
               "Content-Type: application/json;charset=UTF-8\r\n\r\n" + 
                serializeJson(temp, Serial) +"\r\n");

    Serial.println("request sent");

    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }

    Serial.println("reply was:");
    Serial.println("==========");
    String line;
    while(httpsClient.available()){        
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response
    }
    Serial.println("==========");
    Serial.println("closing connection");
    
    
  } else {
    Serial.println ("Wifi NOT connected");
  }
  delay(3000);    //Send a request every 30 seconds
}
