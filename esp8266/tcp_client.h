//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef tcp_client_h
#define tcp_client_h

#include <WiFiClient.h>

extern "C" {
#include "common.h"
}

#define MAX_CLIENTS 10

extern WiFiClient tcpClients[MAX_CLIENTS];
extern bool clientConnected[MAX_CLIENTS];

bool checkAte(const char readBuffer[], char writeBuffer[], bool* error);
bool checkClientConnection(char writeBuffer[], char support[][BUFFER_LENGTH]);
bool checkClientRequest(char writeBuffer[], char support[][BUFFER_LENGTH], void (*sendClientRequest)(char* request));

#endif /* tcp_client_h */
