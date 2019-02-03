#include <ESP8266WiFi.h>
extern "C" {
#include "common.h"
}
#include "at_base.h"
#include "connection.h"
#include "serial.h"
#include "tcp_server.h"
#include "tcp_client.h"


char readBuffer[BUFFER_LENGTH];
char writeBuffer[BUFFER_LENGTH];
#define MAX_SUPPORT 3
char support[MAX_SUPPORT][BUFFER_LENGTH];

bool clientSendAtRequest = false;

WiFiClient tcpClients[MAX_CLIENTS];
bool clientConnected[MAX_CLIENTS];

WiFiServer tcpServer[MAX_SERVER];
int usedPorts[MAX_SERVER];


void setup() {
  Serial.begin(9600);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password, ap_channel, false);
  
  for(int i=0; i<MAX_SERVER; i++){
    usedPorts[i] = -1;
  }
  for(int i=0; i<MAX_CLIENTS; i++){
    clientConnected[i] = false;
  }
}



void loop() {

  bool error = false;

  memset(writeBuffer, ES, BUFFER_LENGTH);
  memset(readBuffer, ES, BUFFER_LENGTH);
  cleanBidimensionalCharArray(support);


  if(checkClientConnection(writeBuffer, support)){
    Serial.println(writeBuffer);
    return;
  }
  
  if(checkClientRequest(readBuffer, writeBuffer, support, &getClientRequest)){
    Serial.println(writeBuffer);
    return;
  }
  

  if(!clientSendAtRequest){
    int maxIndex = readFromSerial(readBuffer);
    if(maxIndex == 0)
      return;
  } else {
    clientSendAtRequest = false;
  }
    
  Serial.println();
  Serial.println(readBuffer);
  
  checkAt(readBuffer, writeBuffer);
  checkAtRst(readBuffer, writeBuffer);
  checkAtGmr(readBuffer, writeBuffer);
  if(checkAtDiag(readBuffer, writeBuffer, Serial)){
    return;
  }
  checkAtCwmode(readBuffer, writeBuffer, support, &error);

  checkAtCwqap(readBuffer, writeBuffer);
  checkAtCwjap(readBuffer, writeBuffer, support, &error);
  checkAtCipsta(readBuffer, writeBuffer, support);
  checkAtCwsap(readBuffer, writeBuffer, support, &error);

  checkAtCwhostname(readBuffer, writeBuffer, support, &error);
  checkAtCipserver(readBuffer, writeBuffer, support, &error);
  checkAtCipsend(readBuffer, writeBuffer, support, &handleServerReadFromSerial, &error);
  
  if(error) {
    _memcpy(writeBuffer, ERR, sizeof(ERR));
  }

  if(strlen(writeBuffer) > 0){
    Serial.println(writeBuffer);
  }
}


char* handleServerReadFromSerial(int readLength){
  Serial.println(OKY);
  Serial.print("\n> ");
  int start = millis();
  while(readFromSerial(readBuffer) + 3 < readLength){
    if(millis() - start > 20000) {
      break; 
    }
  }
  return readBuffer;
}


void getClientRequest(char* request){
  Serial.print("\nNext AT is from client...");
  clientSendAtRequest = true;
  _memcpy(readBuffer, request, strlen(request));
}
