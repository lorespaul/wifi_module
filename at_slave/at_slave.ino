#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define PIN_GPIO_0 0
#define BUFFER_LENGTH 100
#define ES '\0'
#define POINTS ':'
#define NL '\n'
#define CR '\r'

static char readBuffer[BUFFER_LENGTH];

static const char ERR[] = "Error on initialization... Restart!";
static const char OKY[] = "OK";

static const char ssid[] = "AI-THINKER_A0C76D";
static const char password[] = "";

static const char MASTER_IP[] = "192.168.4.1";

WiFiClient tcpClient;

bool canLoop = false;

void setup() {
  Serial.begin(74880);
  
  pinMode(PIN_GPIO_0, OUTPUT);
}

void loop() {
  if(!tcpClient.connected()){
    canLoop = false;
    slaveConnection();
  } else if(canLoop && tcpClient.available()){

    memset(readBuffer, ES, BUFFER_LENGTH);

    int j; char newChar; bool breakNext = false;
    for(j=0; (newChar = tcpClient.read()) != NL && j<BUFFER_LENGTH; j++){
      if(newChar == CR || newChar == ES)
        break;
      readBuffer[strlen(readBuffer)] = newChar;
      if(breakNext)
        break;
      if(!tcpClient.available())
        breakNext = true;
    }
    if(j == 0){
      return;
    }
    
    readBuffer[j] = ES;
    tcpClient.flush();
    Serial.print("DEBUG1: ");
    Serial.println(readBuffer);
    /*if(!strend(readBuffer, OKY)){
      Serial.print("DEBUG2: ");
      Serial.println(readBuffer);
      tcpClient.print(readBuffer);
      tcpClient.print(POINTS);
      tcpClient.println(OKY);
      tcpClient.flush();
    }*/

    if(streq(readBuffer, "HIGH")){
      digitalWrite(PIN_GPIO_0, HIGH);
    } else {
      digitalWrite(PIN_GPIO_0, LOW);
    }
  }
}



void slaveConnection(){
  while(!canLoop){
    canLoop = true;
    
    if(WiFi.status() == WL_CONNECTED){
      WiFi.disconnect(true);
    }
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      if(WiFi.status() == WL_CONNECT_FAILED){
        Serial.println(ERR);
        canLoop = false;
        break;
      }
      delay(200);
      Serial.print('.');
    }
    if(!canLoop)
      continue;
    Serial.println("WiFi connected!");
  
    if(!tcpClient.connect(MASTER_IP, 333)){
      Serial.println(ERR);
      canLoop = false;
      continue;
    }
  }

  Serial.println("Client connected to master!");
  tcpClient.println("Slave1");
  tcpClient.flush();
}



bool streq(const char str[], const char check[]){
  int len = strlen(str);
  if(len != strlen(check))
    return false;
  for(int i=0; i<len; i++){
    if(str[i] != check[i])
      return false;
  }
  return true;
}


bool strend(const char str[], const char check[]){
    int strLen = (int)strlen(str);
    int checkLen = (int)strlen(check);
    int i, j;
    for(i=strLen-1, j=checkLen-1; i>=0 && j>=0; i--, j--){
        if(str[i] != check[j])
            return false;
    }
    return true;
}
