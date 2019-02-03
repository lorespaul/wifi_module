#include <ESP8266WiFi.h>
#include "serial.h"

void serialClean(){
  while (Serial.available()) {
    byte a = Serial.read();
  }
}


int readFromSerial(char readBuffer[]){
  if(Serial.available()){
    int bytes = Serial.readBytesUntil(NL, readBuffer, BUFFER_LENGTH);
    readBuffer[bytes - 1] = ES;
    return bytes - 2; 
  }
  return 0;
}
