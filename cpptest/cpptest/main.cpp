//
//  main.cpp
//  cpptest
//
//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <string>
#include "cpplink.h"
#include "interface.h"

using namespace std;

#define MAX_CLIENTS 10
#define MAX_SERVER 3
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

int convertIntToChar(char convert[]){
    return atoi(convert);
}

char testStatic[10];


string stringFind(string str, char check[]){
    int checkLen = (int)strlen(check);
    int checkLenConst = checkLen;
    int strCheckStartIndex = 0;
    string result = "";
    for(int i=0; i<str.length(); i++){
        for(int j=0; j<checkLenConst; j++){
            if(str.at(i + j) != check[j]){
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
        return str.substr(strCheckStartIndex);
    }
    return "";
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


int main(int argc, const char * argv[]) {
    //char abc[20] = "ciao";
    //char test[] = "ne";
//    if(&test[0] == test){
//        printf("%s\n", "aia");
//    }
    char find[] = "djalshclaksd+ID,";
    char find2[] = "+IPD,";
    strend(find, find2);
    string founded = stringFind("\n\n+IPD,kdjhf ijsfn, ndfnkaudfh", find);
    test();
    //char* a = testStatic;
//    referenceTest(a);
    check(&convertIntToChar);
    //strappend(AT, test);
    //_charAt(test, 9);
    //substr((char*)"ciaonissimo", test, 2, 5);
    isNumber((char*)"345");
    split((char*)"ciao,bella,ciao", ',', support);
    streq((char*)"uno", (char*)"unooo");
    streq((char*)"uno", (char*)"uno");
    strstart((char*)"unoooo", (char*)"uno");
    strstart((char*)"uoooo", (char*)"uno");
    char test2[] = "\"ciao\"";
    charremove(test2, '"');
    return 0;
}
