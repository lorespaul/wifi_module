//#include <Arduino.h>
//#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#define RX 10
#define TX 11

String AP = "TNCAPD493DF"; 
String PASS = "1nt3rn3tt0rn4t0";
//String AP = "Vodafone_my-internenzio"; 
//String PASS = "0lly3sc3m0";

int countTimeCommand;
int countTrueCommand;

const int BUFFER_LENGTH = 500;
const char END_CHAR = '\r';
const char ES = '\0';
char readBuffer[BUFFER_LENGTH];

//SoftwareSerial esp8266(RX, TX);
AltSoftSerial esp8266;

bool readNextAsSlave1ConnectionNumber = false;
String slave1ConnectionNumber;

void setup() {
  Serial.begin(9600);
  esp8266.begin(74880);
  
  pinMode(2, OUTPUT);
  
  //sendCommand("AT+RST", 5, "");
  sendCommand("AT", 5, "OK");
  if (!sendCommand("AT+CWMODE?", 5, "+CWMODE:3")) {
    Serial.println(". set CWMODE to 3.");
    sendCommand("AT+CWMODE=3", 2, "OK");
  }
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK");
  delay(100);
  //sendCommand("AT+CIFSR", 5, "+CIFSR:STAIP,");
  if(sendCommand("AT+CIPSTA?", 5, "+CIPSTA:")){
    Serial.print("IP=");
    Serial.println(espRead(NULL));
  }
  //sendCommand("AT+CIPMUX=1", 5, "OK");
  sendCommand("AT+CIPSERVER=1,333", 20, "OK"); //AT+CIPSERVER=1,333 -> porta di default
  sendCommand("ATE1", 5, "OK");
  //sendCommand("AT+CIPSTO=7200", 5, "OK");
  //sendCommand("AT+MDNS=1,\"espressif\",\"iot\",333", 5, "OK");
  sendCommand("AT+CWHOSTNAME=\"espressif\"", 5, "OK");
}

void loop() { 
  
  
  //String readed = espRead("+IPD,");
  String readed = espRead(NULL);
  Serial.println("DEBUG1: " + readed);
  Serial.println("DEBUG2: " + stringFind(readed, "+IPD,"));
  if (readed.length() > 0 && stringFind(readed, "+IPD,").length() > 0) {
    readed = stringFind(readed, "+IPD,");
    Serial.println("DEBUG3: " + readed);

    String connectionNumber = getValue(readed, ',', 0);
    readed = getValue(readed, ':', 1);

    Serial.print("----> ");
    Serial.println(readed);
    
    String response = "KO";

    if(readNextAsSlave1ConnectionNumber && readed == "Slave1"){
      Serial.println("Slave1 connected...");
      slave1ConnectionNumber = connectionNumber;
    }
    else if(readed == "ON"){
      response = "ON:OK";
      digitalWrite(2, HIGH);
      if(sendCommand("AT+CIPSEND=" + slave1ConnectionNumber + ",6", 1, NULL))
        sendCommand("HIGH", 1, NULL);
    } 
    else if(readed == "OFF") {
      response = "OFF:OK";
      digitalWrite(2, LOW);
      if(sendCommand("AT+CIPSEND=" + slave1ConnectionNumber + ",5", 1, NULL))
        sendCommand("LOW", 1, NULL);
    }

    if(readNextAsSlave1ConnectionNumber){
      Serial.println("Connection wasn't from a slave.");
      readNextAsSlave1ConnectionNumber = false;
    }
    /*if(sendCommand("AT+CIPSEND=" + connectionNumber + "," + (response.length()+2), 1, NULL)){
      sendCommand(response, 1, NULL);//"SEND OK"); 
    }*/
  } 
  else if(readed.endsWith(",CONNECTED")){
    Serial.println("Connection incoming...");
    readNextAsSlave1ConnectionNumber = true;
  }

}

String stringFind(String str, char check[]){
  int checkLen = (int)strlen(check);
  int checkLenConst = checkLen;
  int strCheckStartIndex = 0;
  String result = "";
  for(int i=0; i<str.length(); i++){
    for(int j=0; j<checkLenConst && i + j < str.length(); j++){
      if(str.charAt(i + j) != check[j]){
        checkLen = checkLenConst;
        break;
      }
      else checkLen--;
      if(checkLen == 0){
        strCheckStartIndex = i + checkLenConst;
        break; 
      }
    }
    if(checkLen == 0)
      break;
  }
  if(checkLen == 0){
    return str.substring(strCheckStartIndex);
  }
  return "";
}


boolean sendCommand(String command, int maxTime, char readReplay[]) 
{
  
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  boolean found = false;
  while (countTimeCommand < (maxTime * 1))
  {
    esp8266.println(command);//send command
    
    if (readReplay == NULL || esp8266.find(readReplay))
    {
      found = true;
      break;
    }

    countTimeCommand++;
  }

  if (found == true)
  {
    Serial.println("-> OYI");
    countTrueCommand++;
  }

  if (found == false)
  {
    Serial.println("-> Fail");
    countTrueCommand = 0;
  }

  countTimeCommand = 0;
  return found;
}


String espRead(char condition[])
{
  String content = "";
  if (condition == NULL || esp8266.find(condition))
  {
    memset(readBuffer, ES, BUFFER_LENGTH);
    int bytes = esp8266.readBytesUntil(END_CHAR, readBuffer, BUFFER_LENGTH);
    if(readBuffer[bytes-1] == '\n')
      readBuffer[bytes-1] = END_CHAR;
    readBuffer[bytes] = END_CHAR;
    for(int i=0; readBuffer[i] != END_CHAR && i < BUFFER_LENGTH; i++){
      content.concat(readBuffer[i]);
    }
  }
  return content;
}



String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
    String result = "";

    for (int i = 0; i <= maxIndex; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
            if(index == found)
            {
              result = data.substring(strIndex[0], strIndex[1]);
              break;
            }
            found++;
        }
    }
    return result;
}
