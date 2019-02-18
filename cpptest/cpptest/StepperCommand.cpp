//
//  StepperCommand.cpp
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//


#include <iostream>
#include "StepperCommand.h"
#include "stepperCommon.h"
#include "Serial.h"

using namespace std;
using namespace stp_motor;
using namespace serial;

StepperCommand::StepperCommand(){
    Serial::println("New StepperCommand");
}

StepperCommand::~StepperCommand(){
    Serial::println("Destroy StepperCommand");
}

// 8mm : 360 = 0,04mm : 1.8
// 8mm : 360 = 10mm : 450 -> 1cm

bool StepperCommand::begin(int stepsToExecute, int totalStepsTimeMillis){
    if(!isCommandInExecution()){
        this->inExecution = true;
        this->stepsToExecute = stepsToExecute;
        this->initTime = micros();
        this->lastStepTime = this->initTime;
        this->stepInterval = 500 * (totalStepsTimeMillis / (float)stepsToExecute);
        return true;
    }
    return false;
}

void StepperCommand::end(){
    if(!stepsTerminated())
        return;
    
    Serial::println("command end in " + to_string((micros() - this->initTime) / 1000) + " millis.");
    this->inExecution = false;
    this->lastStepTime = 0;
    this->stepInterval = 0;
}

void StepperCommand::halfStepDone(unsigned long timestamp, Power power){
    this->lastStepTime = timestamp;
    if(power == HIGH)
        this->stepsToExecute--;
}

bool StepperCommand::isCommandInExecution(){
    return this->inExecution;
}

bool StepperCommand::canDoHalfStep(unsigned long timestamp){
    return timestamp - this->lastStepTime >= this->stepInterval;
}

bool StepperCommand::stepsTerminated(){
    return this->stepsToExecute == 0;
}
