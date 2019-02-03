#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266mDNS.h>
#include "tcp_server.h"
#include "at_base.h"
#include "connection.h"
#include "serial.h"
#include "tcp_client.h"


int isServerPortUsed(int port){
  for(int i=0; i<MAX_SERVER; i++){
    if(usedPorts[i] == port)
      return i;
  }
  return -1;
}


bool checkAtCwhostname(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error){
  if(strstart(readBuffer, AT_CWHOSTNAME)) {
    if(_charAt(readBuffer, strlen(readBuffer) - 1) == ASK){
      const char* toPrint = strlen(lastHostname) > 0 ? lastHostname : UNSETTED;
      _memcpy(writeBuffer, toPrint, strlen(toPrint));
    } else {
      split(readBuffer, EQ, support);
      charremove(support[1], '"');
      char* hostname = support[1];
      if(MDNS.begin(hostname)){
        _memcpy(lastHostname, hostname, strlen(hostname));
        strappend(lastHostname, LOCALE);
        _memcpy(writeBuffer, OKY, sizeof(OKY));
      } else {
        *error = true;
      }
    }
    return true;
  }
  return false;
}


bool checkAtCipserver(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error){
  if(strstart(readBuffer, AT_CIPSERVER)) {
    split(readBuffer, EQ, support);
    _memcpy(writeBuffer, support[1], strlen(support[1]));
    split(writeBuffer, ',', support);
    char* sIndex = support[0];
    char* sPort = support[1];
    int index = -1, port = -1;
    if(isNumber(sPort)){
      port = atoi(sPort);
    } else {
      port = DEFAULT_SERVER_PORT;
    }
    if(isNumber(sIndex)){
      index = atoi(sIndex);
    } else {
      index = 0;
    }
    if(index < MAX_SERVER){
      if(usedPorts[index] != -1){
        usedPorts[index] = -1;
        tcpServer[index].stop();
      }
      int indexPortUser;
      if((indexPortUser = isServerPortUsed(port)) != -1){
        *error = true;
      } else {
        tcpServer[index] = WiFiServer(port);
        tcpServer[index].begin();
        _memcpy(writeBuffer, OKY, sizeof(OKY));
      }
    } else {
      *error = true;
    }
    return true;
  }
  return false;
}


bool checkAtCipsend(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], char* (*readFromSerial)(int readLength), bool* error){
  if(strstart(readBuffer, AT_CIPSEND)){
    split(readBuffer, EQ, support);
    _memcpy(writeBuffer, support[1], strlen(support[1]));
    split(writeBuffer, ',', support);
    char* sClientIndex = support[0];
    char* sResponseLength = support[1];
    if(isNumber(sClientIndex) && isNumber(sResponseLength)){
      int clientIndex = atoi(sClientIndex);
      if(clientIndex < MAX_CLIENTS && clientConnected[clientIndex]){
        int responseLength = atoi(sResponseLength);
        char* response = (*readFromSerial)(responseLength);
        tcpClients[clientIndex].println(response);
        tcpClients[clientIndex].flush();
        _memcpy(writeBuffer, response, strlen(response));
        strappend(writeBuffer, "\n\nSEND OK");
        serialClean();
      } else {
        *error = true;
      }
    } else {
      *error = true;
    }
    return true;
  }
  return false;
}
