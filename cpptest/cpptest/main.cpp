//
//  main.cpp
//  cpptest
//
//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <iostream>
#include <cstdio>

using namespace std;

#define MAX_CLIENTS 10
#define MAX_SERVER 3
#define BUFFER_LENGTH 100
#define DEFAULT_SERVER_PORT 333

//static char AT[] = "AT";
//static char AT_RST[] = "AT+RST";
//static char AT_DIAG[] = "AT+DIAG";
//static char AT_GMR[] = "AT+GMR";
//static char AT_CWMODE[] = "AT+CWMODE";
//static char AT_CWJAP[] = "AT+CWJAP";
//static char AT_CIPSTA[] = "AT+CIPSTA?";
//static char AT_CIPMUX[] = "AT+CIPMUX";
//static char AT_CIPSERVER[] = "AT+CIPSERVER";
//static char AT_CIPSTO[] = "AT+CIPSTO";
//static char AT_CWHOSTNAME[] = "AT+CWHOSTNAME";
//static char AT_CIPSEND[] = "AT+CIPSEND";
//static char IPD[] = "+IPD,";
//
//static char OKY[] = "OK";
//static char ERR[] = "ERROR";
#define EQ '='
#define ASK '?'

#define NL '\n'
#define CR '\r'
#define ES '\0'

char readBuffer[BUFFER_LENGTH];
char writeBuffer[BUFFER_LENGTH];
#define MAX_SUPPORT 3
char support[MAX_SUPPORT][BUFFER_LENGTH];


char _charAt(char* string, int index){
    return *(string + index);
}

bool strstart(char str[], char check[]){
    int len = (int)strlen(check);
    bool ret = strlen(str) >= len && strncmp(str, check , len) == 0;
    return ret;
}

bool streq(char str[], char check[]){
    int len = (int)strlen(str);
    if(len != strlen(check))
        return false;
    for(int i=0; i<len; i++){
        if(str[i] != check[i])
            return false;
    }
    return true;
}

void _memcpy(char buff[], char* start, int len){
    memcpy(buff, start, len);
    buff[len] = ES;
}

void substr(char* string, char buff[], int start, int finish){
    int len = finish - start;
    _memcpy(buff, &string[start], len);
}

void split(char* data, char separator)
{
    int founded = -1;
    int strIndex[] = { 0, -1 };
    int maxIndex = (int)strlen(data) - 1;
    
    for (int i = 0; i <= maxIndex; i++) {
        if (_charAt(data, i) == separator || i == maxIndex) {
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
            founded++;
            substr(data, support[founded], strIndex[0], strIndex[1]);
        }
    }
}


bool isNumber(char* string){
    for(int i=0; i<strlen(string); i++){
        if(!isdigit(_charAt(string, i))){
            return false;
        }
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
            str[i] = '\0';
        }
    }
}


void strappend(char str[], char* append){
    int strLen = (int)strlen(str);
    int appendLen = (int)strlen(append);
    for(int i=strLen; i<strLen+appendLen; i++){
        int appendIndex = (i - strLen);
        str[i] = *(append + appendIndex);
    }
}


int main(int argc, const char * argv[]) {
    char abc[20] = "ciao";
    char test[] = "ne";
    if(&test[0] == test){
        printf("%s\n", "aia");
    }
    
    strappend(abc, test);
    _charAt(test, 9);
    substr((char*)"ciaonissimo", test, 2, 5);
    isNumber((char*)"345");
    split((char*)"ciao,bella,ciao", ',');
    streq((char*)"uno", (char*)"unooo");
    streq((char*)"uno", (char*)"uno");
    strstart((char*)"unoooo", (char*)"uno");
    strstart((char*)"uoooo", (char*)"uno");
    char test2[] = "\"ciao\"";
    charremove(test2, '"');
    return 0;
}
