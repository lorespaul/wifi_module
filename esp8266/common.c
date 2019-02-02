//  Created by Lorenzo Daneo on 02/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "common.h"

void strappend(char str[], char* append){
    int strLen = (int)strlen(str);
    int appendLen = (int)strlen(append);
    for(int i=strLen; i<strLen+appendLen; i++){
        int appendIndex = (i - strLen);
        str[i] = *(append + appendIndex);
    }
}


void split(char* data, char separator, char support[][BUFFER_LENGTH]){
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


bool strstart(char str[], char check[]){  
  int len = strlen(check);
  return strlen(str) >= len && strncmp(str, check , len) == 0;
}



bool streq(char str[], char check[]){
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



void _memcpy(char buff[], char* start, int len){
  memcpy(buff, start, len);
  buff[len] = ES;
}


char _charAt(char* string, int index){
  return *(string + index);
}


bool isNumber(char* string){
  for(int i=0; i<strlen(string); i++){
    if(!isdigit(_charAt(string, i))){
      return false;
    }
  }
  return true;
}


void cleanBidimensionalCharArray(char support[][BUFFER_LENGTH]){
  for(int i=0; i<sizeof(support); i++){
    memset(support[i], ES, sizeof(support[i]));
  }
}
