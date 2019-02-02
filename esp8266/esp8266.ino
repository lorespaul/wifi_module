#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#define MAX_CLIENTS 10
#define MAX_SERVER 3
#define BUFFER_LENGTH 100
#define DEFAULT_SERVER_PORT 333

static char AT[] = "AT";
static char AT_RST[] = "AT+RST";
static char AT_DIAG[] = "AT+DIAG";
static char AT_GMR[] = "AT+GMR";
static char AT_CWMODE[] = "AT+CWMODE";
static char AT_CWJAP[] = "AT+CWJAP";
static char AT_CIPSTA[] = "AT+CIPSTA?";
static char AT_CIPMUX[] = "AT+CIPMUX";
static char AT_CIPSERVER[] = "AT+CIPSERVER";
static char AT_CIPSTO[] = "AT+CIPSTO";
static char AT_CWHOSTNAME[] = "AT+CWHOSTNAME";
static char AT_CIPSEND[] = "AT+CIPSEND";
static char IPD[] = "+IPD,";

static char UNSETTED[] = "unsetted";
static char LOCALE[] = ".local";

static char OKY[] = "OK";
static char ERR[] = "ERROR";

static char COMMA = ',';
static char POINTS = ':';

#define EQ '='
#define ASK '?'

#define NL '\n'
#define CR '\r'
#define ES '\0'

char readBuffer[BUFFER_LENGTH];
char writeBuffer[BUFFER_LENGTH];
#define MAX_SUPPORT 3
char support[MAX_SUPPORT][BUFFER_LENGTH];

WiFiServer tcpServer[MAX_SERVER];
int usedPorts[MAX_SERVER];
WiFiClient tcpClients[MAX_CLIENTS];
bool clientConnected[MAX_CLIENTS];

char lastHostname[50];

char ssid[50];
char password[50];


void setup() {
  Serial.begin(9600);
  
  WiFi.mode(WIFI_AP);
  for(int i=0; i<MAX_SERVER; i++){
    usedPorts[i] = -1;
  }
  for(int i=0; i<MAX_CLIENTS; i++){
    clientConnected[i] = false;
  }
}


void loop() {

  boolean error = false;
  boolean skip = false;

  memset(writeBuffer, ES, BUFFER_LENGTH);
  memset(readBuffer, ES, BUFFER_LENGTH);
  cleanSupport();

  for(int i=0; i<MAX_SERVER; i++){
    WiFiClient newClient = tcpServer[i].available();
    if (newClient) {
      for (int j=0 ; j<MAX_CLIENTS; ++j) {
        if (!clientConnected[j]) {
          tcpClients[j] = newClient;
          clientConnected[j] = true;
          Serial.print(j);
          Serial.println(",CONNECTED");
          break;
        }
      }
    }
  }

  boolean goToPrint = false;
  for (int i=0 ; i<MAX_CLIENTS ; ++i) {
    if (clientConnected[i] && tcpClients[i].available()) {
      _memcpy(writeBuffer, IPD, sizeof(IPD));
      itoa(i, support[0], 10);
      strappend(writeBuffer, support[0]);
      writeBuffer[strlen(writeBuffer)] = COMMA;
      int j; char newChar;
      for(j=0; (newChar = tcpClients[i].read()) != NL && j<BUFFER_LENGTH; j++){
        if(newChar == CR)
          break;
        support[1][strlen(support[1])] = newChar;
      }
      support[1][j] = ES;
      itoa(j, support[2], 10);
      strappend(writeBuffer, support[2]);
      writeBuffer[strlen(writeBuffer)] = POINTS;
      strappend(writeBuffer, support[1]);
      tcpClients[i].flush();
      goToPrint = true;
    } else if(clientConnected[i] && !tcpClients[i].connected()){
      clientConnected[i] = false;
      Serial.print(i);
      Serial.println(",DISCONNECTED");
    }
  }

  if(!goToPrint){

    int maxIndex = readFromSerial();
    
    if(streq(readBuffer, AT)) {
      _memcpy(writeBuffer, OKY, sizeof(OKY));
    }
  
    if(streq(readBuffer, AT_RST)) {
      _memcpy(writeBuffer, OKY, sizeof(OKY));
      ESP.restart();
    }
  
    if(streq(readBuffer, AT_GMR)) {
      Serial.println(ESP.getCoreVersion());
      skip = true;
    }
  
    if(streq(readBuffer, AT_DIAG)) {
      WiFi.printDiag(Serial);
      skip = true;
    }

    if(strstart(readBuffer, AT_CWMODE)){
      if(_charAt(readBuffer, maxIndex) == ASK){
        Serial.print("\r+CWMODE:");
        Serial.println(WiFi.getMode());
      } else {
        split(readBuffer, EQ);
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
          error = true;
        }
      }
    }

    if(strstart(readBuffer, AT_CWJAP)) {
      split(readBuffer, EQ);
      _memcpy(readBuffer, support[1], strlen(support[1]));
      split(readBuffer, ',');
      charremove(support[0], '"');
      charremove(support[1], '"');
      char* ssid = support[0];
      char* password = support[1];
      if(WiFi.status() == WL_CONNECTED){
        WiFi.disconnect(true);
      }
      WiFi.begin(ssid, password);
      Serial.print("SSID: ");
      Serial.println(ssid);
      Serial.print("Password: ");
      Serial.println(password);
      while (WiFi.status() != WL_CONNECTED)
      {
        if(WiFi.status() == WL_CONNECT_FAILED){
          error = true;
          break;
        }
        if(!error){
          delay(200);
          Serial.print("."); 
        }
      }
      Serial.println(); 
      _memcpy(writeBuffer, OKY, sizeof(OKY));
      serialClean();
    }

    if(streq(readBuffer, AT_CIPSTA)) {
      Serial.print("\r+CIPSTA:\"");
      Serial.print(WiFi.localIP());
      Serial.println("\"");
      skip = true;
    }

    if(strstart(readBuffer, AT_CIPSERVER)) {
      split(readBuffer, EQ);
      _memcpy(readBuffer, support[1], strlen(support[1]));
      split(readBuffer, ',');
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
      Serial.print("port: ");
      Serial.println(port);
      Serial.print("index: ");
      Serial.println(index);
      if(index < MAX_SERVER){
        if(usedPorts[index] != -1){
          usedPorts[index] = -1;
          tcpServer[index].stop();
        }
        int indexPortUser;
        if((indexPortUser = isServerPortUsed(port)) != -1){
          Serial.println("Server not created!");
          error = true;
        } else {
          Serial.println("Server creation...");
          tcpServer[index] = WiFiServer(port);
          tcpServer[index].begin();
          _memcpy(writeBuffer, OKY, sizeof(OKY));
        }
      } else {
        error = true;
      }
    }

    if(strstart(readBuffer, AT_CWHOSTNAME)) {
      if(_charAt(readBuffer, maxIndex) == ASK){
        char* toPrint = strlen(lastHostname) > 0 ? lastHostname : UNSETTED;
        _memcpy(writeBuffer, toPrint, strlen(toPrint));
      } else {
        split(readBuffer, EQ);
        charremove(support[1], '"');
        char* hostname = support[1];
        if(MDNS.begin(hostname)){
          _memcpy(lastHostname, hostname, strlen(hostname));
          strappend(lastHostname, LOCALE);
          Serial.print("Host: ");
          Serial.println(lastHostname);
          _memcpy(writeBuffer, OKY, sizeof(OKY));
        } else {
          error = true;
        }
      }
    }

    if(strstart(readBuffer, AT_CIPSEND)){
      split(readBuffer, EQ);
      _memcpy(readBuffer, support[1], strlen(support[1]));
      split(readBuffer, ',');
      char* sClientIndex = support[0];
      char* sResponseLength = support[1];
      if(isNumber(sClientIndex) && isNumber(sResponseLength)){
        int clientIndex = atoi(sClientIndex);
        if(clientIndex < MAX_CLIENTS && clientConnected[clientIndex]){
          int responseLength = atoi(sResponseLength);
          Serial.println(OKY);
          Serial.print("\n> ");
          int start = millis();
          while(readFromSerial() + 3 < responseLength){
            if(millis() - start > 20000) 
              break;
          }
          tcpClients[clientIndex].println(readBuffer);
          tcpClients[clientIndex].flush();
          Serial.println(readBuffer);
          Serial.println("SEND OK");
          serialClean();
          skip = true; 
        } else {
          error = true;
        }
      } else {
        error = true;
      }
    }

  }
  
  if(error) {
    _memcpy(writeBuffer, ERR, sizeof(ERR));
  }

  if(!skip && strlen(writeBuffer) > 0){
    Serial.println(writeBuffer);
  }
}


int readFromSerial()
{
  if(Serial.available()){
    int bytes = Serial.readBytesUntil(NL, readBuffer, BUFFER_LENGTH);
    readBuffer[bytes - 1] = ES;
    return bytes - 2; 
  }
  return 0;
}


int isServerPortUsed(int port){
  for(int i=0; i<MAX_SERVER; i++){
    if(usedPorts[i] == port)
      return i;
  }
  return -1;
}


void split(char* data, char separator)
{
    int founded = -1;
    int strIndex[] = { 0, -1 };
    int maxIndex = strlen(data) - 1;

    for (int i = 0; i <= maxIndex; i++) {
        if (_charAt(data, i) == separator || i == maxIndex) {
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
            founded++;
            substr(data, support[founded], strIndex[0], strIndex[1]);
        }
    }
}

boolean strstart(char str[], char check[]){  
  int len = strlen(check);
  return strlen(str) >= len && strncmp(str, check , len) == 0;
}

boolean streq(char str[], char check[]){
  int len = strlen(str);
  if(len != strlen(check))
    return false;
  for(int i=0; i<len; i++){
    if(str[i] != check[i])
      return false;
  }
  return true;
}

void charremove(char str[], char rem){
    int replaced = 0;
    int len = (int)strlen(str);
    bool zeroAtTheEnd = false;
    for(int i=0; i<len; i++){
        if(!zeroAtTheEnd){
            if(str[i] == rem){
                replaced++;
            }
            if(i + replaced < len - 1){
                str[i] = str[i + replaced];
            } else {
                zeroAtTheEnd = true;
                i--;
            }
        } else {
            str[i] = ES;
        }
    }
}

void substr(char* string, char buff[], int start, int finish){
  int len = finish - start;
  _memcpy(buff, &string[start], len);
}

void strappend(char str[], char* append){
    int strLen = (int)strlen(str);
    int appendLen = (int)strlen(append);
    for(int i=strLen; i<strLen+appendLen; i++){
        int appendIndex = (i - strLen);
        str[i] = *(append + appendIndex);
    }
}

void _memcpy(char buff[], char* start, int len){
  memcpy(buff, start, len);
  buff[len] = ES;
}

char _charAt(char* string, int index){
  return *(string + index);
}

boolean isNumber(char* string){
  for(int i=0; i<strlen(string); i++){
    if(!isDigit(_charAt(string, i))){
      return false;
    }
  }
  return true;
}

void cleanSupport(){
  for(int i=0; i<MAX_SUPPORT; i++){
    memset(support[i], ES, BUFFER_LENGTH);
  }
}

void serialClean(){
  while (Serial.available()) {
    byte a = Serial.read();
  }
}
