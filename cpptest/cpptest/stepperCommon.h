//
//  stepperCommon.cpp
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef stepperCommon_h
#define stepperCommon_h

#define REVOLUTION_STEPS 200
#define MM_PER_REVOLUTION 8.00
enum Power { LOW = 0, HIGH = 1 };

unsigned long micros();
void delayMicroseconds(unsigned int micros);
void digitalWrite(int pin, int power);

#endif
