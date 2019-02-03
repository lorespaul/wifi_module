//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef serial_h
#define serial_h

extern "C" {
#include "common.h"
}

void serialClean();
int readFromSerial(char readBuffer[]);

#endif /* serial_h */
