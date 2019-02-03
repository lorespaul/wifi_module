//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef at_base_h
#define at_base_h

extern "C" {
#include "common.h"
}

static const char ATE[] = "ATE";
static const char AT[] = "AT";
static const char AT_RST[] = "AT+RST";
static const char AT_DIAG[] = "AT+DIAG";
static const char AT_GMR[] = "AT+GMR";
static const char AT_CWMODE[] = "AT+CWMODE";
static const char AT_CWJAP[] = "AT+CWJAP";
static const char AT_CWQAP[] = "AT+CWQAP";
static const char AT_CWSAP[] = "AT+CWSAP";
static const char AT_CIPSTA[] = "AT+CIPSTA?";
static const char AT_CIPMUX[] = "AT+CIPMUX";
static const char AT_CIPSERVER[] = "AT+CIPSERVER";
static const char AT_CIPSTO[] = "AT+CIPSTO";
static const char AT_CWHOSTNAME[] = "AT+CWHOSTNAME";
static const char AT_CIPSEND[] = "AT+CIPSEND";
static const char IPD[] = "+IPD,";

static const char OKY[] = "OK";
static const char ERR[] = "ERROR";

static const char UNSETTED[] = "unsetted";
static const char LOCALE[] = ".local";


bool checkAt(const char readBuffer[], char writeBuffer[]);
bool checkAtRst(const char readBuffer[], char writeBuffer[]);
bool checkAtGmr(const char readBuffer[], char writeBuffer[]);
bool checkAtDiag(const char readBuffer[], char writeBuffer[], Print& serial);
bool checkAtCwmode(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error);


#endif /* at_base_h */
