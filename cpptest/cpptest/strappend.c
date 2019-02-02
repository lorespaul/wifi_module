//
//  strappend.c
//  cpptest
//
//  Created by Lorenzo Daneo on 02/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "interface.h"

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
