#include <SoftwareSerial.h>

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
}

void loop() { // run over and over
  if (wifiSerial.available()) {
    Serial.write(wifiSerial.read());
  }
}
