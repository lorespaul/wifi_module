#include <WiFiClient.h>
#include "tcp_client.h"
#include "tcp_server.h"
#include "common.h"
#include "at_base.h"


static bool clientEchoActive = false;

bool checkAte(const char readBuffer[], char writeBuffer[], bool* error){
  if(strstart(readBuffer, ATE)) { 
    if(readBuffer[3] == '0'){
      clientEchoActive = false;
      _memcpy(writeBuffer, OKY, sizeof(OKY));
    } else if(readBuffer[3] == '1'){
      clientEchoActive = true;
      _memcpy(writeBuffer, OKY, sizeof(OKY));
    } else {
      *error = true;
    }
    return true;
  }
  return false;
}


bool checkClientConnection(char writeBuffer[], char support[][BUFFER_LENGTH]){
  for(int i=0; i<MAX_SERVER; i++){
    WiFiClient newClient = tcpServer[i].available();
    if (newClient) {
      for (int j=0 ; j<MAX_CLIENTS; ++j) {
        if (!clientConnected[j]) {
          tcpClients[j] = newClient;
          clientConnected[j] = true;
          itoa(j, support[0], 10);
          writeBuffer[0] = NL;
          strappend(writeBuffer, support[0]);
          strappend(writeBuffer, ",CONNECTED");
          return true;
        }
      }
    }
  }
  return false;
}
  

bool checkClientRequest(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], void (*sendClientRequest)(char* request)){
  for (int i=0 ; i<MAX_CLIENTS ; ++i) {
    if (clientConnected[i] && tcpClients[i].available()) {
      writeBuffer[0] = NL;
      strappend(writeBuffer, IPD);
      itoa(i, support[0], 10);
      strappend(writeBuffer, support[0]);
      writeBuffer[strlen(writeBuffer)] = COMMA;
      int j; char newChar; bool breakNext = false;
      for(j=0; (newChar = tcpClients[i].read()) != NL && j<BUFFER_LENGTH; j++){
        if(newChar == CR || newChar == ES)
          break;
        support[1][strlen(support[1])] = newChar;
        if(breakNext)
          break;
        if(!tcpClients[i].available())
          breakNext = true;
      }
      if(j == 0){
        memset(writeBuffer, ES, BUFFER_LENGTH);
        return false;
      }
      support[1][j] = ES;
      itoa(j, support[2], 10);
      strappend(writeBuffer, support[2]);
      writeBuffer[strlen(writeBuffer)] = POINTS;
      strappend(writeBuffer, support[1]);
      tcpClients[i].flush();
      if(clientEchoActive){
        tcpClients[i].print(support[1]);
        tcpClients[i].print(POINTS);
        tcpClients[i].println(OKY);
        tcpClients[i].flush();
      }
      if(strstart(support[1], AT_CWJAP)){
        memset(writeBuffer, ES, BUFFER_LENGTH);
        (*sendClientRequest)(support[1]);
        return false;
      } else {
        return true;
      }
    } else if(clientConnected[i] && !tcpClients[i].connected()){
      clientConnected[i] = false;
      itoa(i, support[0], 10);
      writeBuffer[0] = NL;
      strappend(writeBuffer, support[0]);
      strappend(writeBuffer, ",CLOSED");
      return true;
    }
  }
  return false;
}
