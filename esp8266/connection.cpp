#include <ESP8266WiFi.h>
#include "connection.h"
#include "at_base.h"
#include "serial.h"


bool checkAtCwqap(const char readBuffer[], char writeBuffer[]){
  if(streq(readBuffer, AT_CWQAP)) {
    if(WiFi.status() == WL_CONNECTED){
      WiFi.disconnect(true);
    }
    _memcpy(writeBuffer, OKY, sizeof(OKY));
    return true;
  }
  return false;
}


bool checkAtCwjap(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error){
  if(strstart(readBuffer, AT_CWJAP)) {
    if(WiFi.getMode() == 1 || WiFi.getMode() == 3){
      split(readBuffer, EQ, support);
      _memcpy(writeBuffer, support[1], strlen(support[1]));
      split(writeBuffer, COMMA, support);
      charremove(support[0], '"');
      charremove(support[1], '"');
      char* ssid = support[0];
      char* password = support[1];
      if(WiFi.status() == WL_CONNECTED){
        WiFi.disconnect(true);
      }
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED)
      {
        if(WiFi.status() == WL_CONNECT_FAILED){
          *error = true;
          break;
        }
        if(!(*error)){
          delay(200);
        }
      }
      _memcpy(writeBuffer, OKY, sizeof(OKY));
      serialClean();
    } else {
      *error = true;
    }
    return true;
  }
  return false;
}


bool checkAtCipsta(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH]){
  if(streq(readBuffer, AT_CIPSTA)) {
    _memcpy(writeBuffer, "\r+CIPSTA:\"", 10);
    strcpy(support[0], WiFi.localIP().toString().c_str());
    strappend(writeBuffer, support[0]);
    writeBuffer[strlen(writeBuffer)] = '"';
    return true;
  }
  return false;
}


bool checkAtCwsap(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error){
  if(strstart(readBuffer, AT_CWSAP)) {
    if(WiFi.getMode() == 2 || WiFi.getMode() == 3){
      boolean printInfo = false;
      if(_charAt(readBuffer, strlen(readBuffer) - 1) == ASK){
        printInfo = true;
      } else {
        split(readBuffer, EQ, support);
        _memcpy(writeBuffer, support[1], strlen(support[1]));
        split(writeBuffer, COMMA, support);
        charremove(support[0], '"');
        charremove(support[1], '"');
        if(strlen(support[0]) > 0 && strlen(support[2]) > 0){
          _memcpy(ap_ssid, support[0], strlen(support[0]));
          _memcpy(ap_password, support[1], strlen(support[1]));
          ap_channel = atoi(support[2]);
          WiFi.softAP(ap_ssid, ap_password, ap_channel, false); 
          printInfo = true;
        } else {
          *error = true;
        }
      }
      if(printInfo){
        _memcpy(writeBuffer, "+CWSAP:", 7);
        writeBuffer[strlen(writeBuffer)] = '"';
        strappend(writeBuffer, ap_ssid);
        writeBuffer[strlen(writeBuffer)] = '"';
        writeBuffer[strlen(writeBuffer)] = ',';
        writeBuffer[strlen(writeBuffer)] = '"';
        strappend(writeBuffer, ap_password);
        writeBuffer[strlen(writeBuffer)] = '"';
        writeBuffer[strlen(writeBuffer)] = ',';
        itoa(ap_channel, support[0], 10);
        strappend(writeBuffer, support[0]);
      }
    } else {
      *error = true;
    }
    return true;
  }
  return false;
}
