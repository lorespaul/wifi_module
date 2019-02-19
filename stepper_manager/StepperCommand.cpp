//
//  StepperCommand.cpp
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <Arduino.h>
#include "StepperCommand.h"

using namespace stepper_motor;

StepperCommand::StepperCommand(){
    //Serial.println("New StepperCommand");
    this->inExecution = false;
    this->initTime = -1;
    this->lastStepTime = -1;
    this->halfStepInterval = -1;
}

StepperCommand::~StepperCommand(){
    //Serial.println("Destroy StepperCommand");
}

//bool StepperCommand::start(int stepsToExecute, int totalStepsTimeMillis){
//    if(!isCommandInExecution()){
//        this->inExecution = true;
//        this->stepsToExecute = stepsToExecute;
//        this->initTime = micros();
//        this->lastStepTime = this->initTime;
//        this->halfStepInterval = 500 * (totalStepsTimeMillis / (float)stepsToExecute);
//        return true;
//    }
//    return false;
//}

// 8mm : 360 = 0,04mm : 1.8
// 8mm : 360 = 10mm : 450 -> 1cm
// 8mm : 200 = millimeters : y
bool StepperCommand::start(int millimeters, int movementTimeMillis){
    if(!isCommandInExecution()){
        this->inExecution = true;
        this->stepsToExecute = REVOLUTION_STEPS * millimeters / MM_PER_REVOLUTION;
        this->initTime = micros();
        this->lastStepTime = this->initTime;
        this->halfStepInterval = 500 * (movementTimeMillis / (float)this->stepsToExecute);
        return true;
    }
    return false;
}

void StepperCommand::stop(){
    if(!stepsTerminated())
        return;
    
    //Serial.println("command end in " + to_string((micros() - this->initTime) / 1000) + " millis.");
    this->inExecution = false;
    this->initTime = -1;
    this->lastStepTime = -1;
    this->halfStepInterval = -1;
}

void StepperCommand::halfStepDone(unsigned long timestamp, int power){
    this->lastStepTime = timestamp;
    if(power == HIGH)
        this->stepsToExecute--;
}

bool StepperCommand::isCommandInExecution(){
    return this->inExecution;
}

bool StepperCommand::canDoHalfStep(unsigned long timestamp){
    return timestamp - this->lastStepTime >= this->halfStepInterval;
}

bool StepperCommand::stepsTerminated(){
    return this->stepsToExecute == 0;
}
