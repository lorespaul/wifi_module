//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef connection_h
#define connection_h

extern "C" {
#include "common.h"
}

static char lastHostname[50];

static char ssid[50];
static char password[50];

static char ap_ssid[50] = "AI-THINKER_A0C76D";
static char ap_password[50] = "";
static int ap_channel = 11;

bool checkAtCwqap(const char readBuffer[], char writeBuffer[]);
bool checkAtCwjap(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error);
bool checkAtCipsta(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH]);
bool checkAtCwsap(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error);

#endif /* connection_h */
