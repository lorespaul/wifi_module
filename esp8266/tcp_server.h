//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef tcp_server_h
#define tcp_server_h

#include <WiFiServer.h>

extern "C" {
#include "common.h"
}


#define MAX_SERVER 3
#define DEFAULT_SERVER_PORT 333

extern WiFiServer tcpServer[MAX_SERVER];
extern int usedPorts[MAX_SERVER];

bool checkAtCwhostname(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error);
bool checkAtCipserver(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], bool* error);
bool checkAtCipsend(const char readBuffer[], char writeBuffer[], char support[][BUFFER_LENGTH], char* (*readFromSerial)(int readLength), bool* error);

#endif /* tcp_server_h */
