//
//  StepperCommand.cpp
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <Arduino.h>
#include <limits.h>
#include "StepperCommand.h"

using namespace stepper_motor;

StepperCommand::StepperCommand(){
    //Serial.println("New StepperCommand");
    this->inExecution = false;
    this->initTime = this->lastStepTime = this->halfStepInterval = -1;
}

StepperCommand::~StepperCommand(){
    //Serial.println("Destroy StepperCommand");
}

int StepperCommand::getDirection(){
    return this->direction;
}


long StepperCommand::getInitialSteps(){
    return this->initialSteps;
}

long StepperCommand::getStepsToExecute(){
    return this->stepsToExecute;
}


bool StepperCommand::startLinear(unsigned long interval, long steps, int dir){
    if(!isInExecution() && (interval > MIN_INTERVAL || interval == 0) && steps > 0 && (dir == 0 || dir == 1)){
        this->inExecution = true;
        this->infinite = interval == 0 ? true : false;
        this->halfStepInterval = interval == 0 ? GO_HOME_INTERVAL : interval;
        this->stepsToExecute = this->initialSteps = steps;
        this->direction = interval == 0 ? !dir : dir;
        this->initTime = micros();
        this->lastStepTime = this->initTime;
        this->isInPause = false;
        /*Serial.println("||||||||||||");
        Serial.print("Millimeters="); 
        Serial.println(millimeters);
        Serial.print("In millis="); 
        Serial.println(movementTimeMillis);
        Serial.print("Half speed micros="); 
        Serial.println(this->halfStepInterval);
        Serial.print("Total steps="); 
        Serial.println(this->stepsToExecute);
        Serial.println("+++++++++++");*/
        return true;
    }
    return false;
}


void StepperCommand::forceStop(){
    this->inExecution = this->isInPause = false;
    this->initTime = this->lastStepTime = this->halfStepInterval = -1;
}

void StepperCommand::stop(unsigned long timestamp){
    if(!stepsTerminated())
        return;
    
    //Serial.print("Command end in ");
    //Serial.print((timestamp - this->initTime) / 1000);
    //Serial.println(" millis.");
    forceStop();
}


void StepperCommand::halfStepDone(unsigned long timestamp, int power){
    this->lastStepTime = timestamp;
    if(power == HIGH){
        if(this->infinite && this->stepsToExecute == this->initialSteps - REVOLUTION_STEPS){
            this->direction = !this->direction;
        }
        this->stepsToExecute--;
    }
}

bool StepperCommand::isInExecution(){
    return this->inExecution;
}

bool StepperCommand::canDoHalfStep(unsigned long timestamp){
    return !this->isInPause && timestamp - this->lastStepTime >= this->halfStepInterval;
}

bool StepperCommand::stepsTerminated(){
    if(this->infinite && this->stepsToExecute == 0){
        this->stepsToExecute = INT_MAX - REVOLUTION_STEPS - 1;
    }
    return this->stepsToExecute == 0;
}


void StepperCommand::pause(){
    this->isInPause = true;
}

void StepperCommand::release(){
    this->isInPause = false;
}
