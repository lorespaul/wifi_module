//
//  stepperCommon.cpp
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <stdio.h>

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "stepperCommon.h"


using namespace std;
using namespace std::chrono;

unsigned long micros(){
    return time_point_cast<microseconds>(system_clock::now()).time_since_epoch().count();
}

void delayMicroseconds(unsigned int micros){
    usleep(micros);
}

void digitalWrite(int pin, Power power){
    //cout << "digitalWrite on pin " + to_string(pin) + " power " + (power == HIGH ? "HIGH" : "LOW") << endl;
}
