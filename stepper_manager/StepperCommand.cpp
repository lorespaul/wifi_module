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
    this->inExecution = this->infinite = false;
    this->lastStepTime = this->halfStepInterval = -1;
}

StepperCommand::~StepperCommand(){
}

int StepperCommand::getDirection(){
    return this->direction;
}


bool StepperCommand::startLinear(unsigned long interval, long steps, int dir){
    if(!isInExecution() && (interval > MIN_INTERVAL || interval == 0) && steps > 0 && (dir == 0 || dir == 1)){
        this->inExecution = true;
        this->infinite = interval == 0 ? true : false;
        this->halfStepInterval = interval == 0 ? GO_HOME_INTERVAL : interval;
        this->stepsToExecute = this->initialSteps = steps;
        this->direction = interval == 0 ? !dir : dir;
        this->lastStepTime = micros();
        return true;
    }
    return false;
}


bool StepperCommand::forceStop(){
    if(!this->inExecution)
        return false;
    this->inExecution = this->infinite = false;
    this->lastStepTime = this->halfStepInterval = -1;
    return true;
}

void StepperCommand::stop(unsigned long timestamp){
    if(!stepsTerminated())
        return;
    forceStop();
}


void StepperCommand::halfStepDone(unsigned long timestamp, int power){
    this->lastStepTime = timestamp;
    if(power == HIGH){
        if(this->infinite && this->stepsToExecute == this->initialSteps - QUARTER_REVOLUTION_STEPS){
            this->direction = !this->direction;
        }
        this->stepsToExecute--;
    }
}

bool StepperCommand::isInExecution(){
    return this->inExecution;
}

bool StepperCommand::canDoHalfStep(unsigned long timestamp){
    return timestamp - this->lastStepTime >= this->halfStepInterval;
}

bool StepperCommand::stepsTerminated(){
    /*if(this->infinite && this->stepsToExecute == 0){
        this->stepsToExecute = this->initialSteps - QUARTER_REVOLUTION_STEPS - 1;
    }*/
    return this->stepsToExecute == 0;
}
