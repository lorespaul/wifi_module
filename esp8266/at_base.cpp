extern "C" {
#include <stdio.h>
#include <string.h>
}
#include <ESP8266WiFi.h>
#include "at_base.h"

bool checkAt(const char readBuffer[], char writeBuffer[]){
  if(streq(readBuffer, AT)) {
    _memcpy(writeBuffer, OKY, sizeof(OKY));
    return true;
  } 
  return false;
}


bool checkAtRst(const char readBuffer[], char writeBuffer[]){
  if(streq(readBuffer, AT_RST)) {
    _memcpy(writeBuffer, OKY, sizeof(OKY));
    ESP.restart();
    return true;
  }
  return false;
}


bool checkAtGmr(const char readBuffer[], char writeBuffer[]){
  if(streq(readBuffer, AT_GMR)) {
    strcpy(writeBuffer, ESP.getCoreVersion().c_str());
    return true;
  }
  return false;
}

bool checkAtDiag(const char readBuffer[], char writeBuffer[], Print& serial){
  if(streq(readBuffer, AT_DIAG)) {
    WiFi.printDiag(serial);
    return true;
  }
  return false;
}


bool checkAtCwmode(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error){
  if(strstart(readBuffer, AT_CWMODE)){
    if(_charAt(readBuffer, strlen(readBuffer) - 1) == ASK){
      _memcpy(writeBuffer, "\r+CWMODE:", 9);
      itoa((int)WiFi.getMode(), support[0], 10);
      strappend(writeBuffer, support[0]);
    } else {
      split(readBuffer, EQ, support);
      if(strlen(support[1]) == 1 && isDigit(support[1][0])) {
        char cwmode = support[1][0];
        char temp = cwmode;
        switch(atoi(&cwmode)){
          case 1:
            WiFi.mode(WIFI_STA);
            break;
          case 2:
            WiFi.mode(WIFI_AP);
            break;
          case 3:
          default:
            WiFi.mode(WIFI_AP_STA);
            cwmode = 3;
            break;
        }
        _memcpy(writeBuffer, OKY, sizeof(OKY));
      } else {
        *error = true;
      }
    }
    return true;
  }
  return false;
}
