//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef common_h
#define common_h

#include <stdbool.h>

#define BUFFER_LENGTH 100
#define MAX_SUPPORT 4

#define EQ '='
#define ASK '?'
#define NL '\n'
#define CR '\r'
#define ES '\0'
#define COMMA ','
#define POINTS ':'

void strappend(char str[], const char* append, const int strSize);
void split(const char* data, const char separator, char support[][BUFFER_LENGTH]);
bool strstart(const char str[], const char check[]);
bool streq(const char str[], const char check[]);
void charremove(char str[], const char rem);
void substr(const char* string, char buff[], int start, int finish);
void _memcpy(char buff[], const char* start, int len);
char _charAt(const char* string, int index);
bool isNumber(char* string);


#endif /* common_h */
