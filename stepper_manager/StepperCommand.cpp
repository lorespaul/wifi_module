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
    this->initTime = -1;
    this->lastStepTime = -1;
    this->halfStepInterval = -1;
}

StepperCommand::~StepperCommand(){
    //Serial.println("Destroy StepperCommand");
}

int StepperCommand::getDirection(){
    return this->direction;
}


bool StepperCommand::startInfiniteLinear(int speedMmSec, int dir){
    if(!isInExecution()){
        this->inExecution = true;
        this->circular = false;
        this->infinite = true;
        this->stepsToExecute = INT_MAX;
        this->direction = !dir;
        this->initTime = micros();
        this->lastStepTime = this->initTime;
        this->halfStepInterval = MILLIS_TO_MICROS_MID_MULTIPLIER * (1 / (double)((REVOLUTION_STEPS / MM_PER_REVOLUTION * speedMmSec)) * 1000);
        return true;
    }
    return false;
}


// 8mm : 360 = 0,04mm : 1.8
// 8mm : 360 = 10mm : 450 -> 1cm
// 8mm : 200 = millimeters : y
bool StepperCommand::startLinear(double millimeters, int movementTimeMillis, int dir, double percentageErrorMargin, bool isExceeding){
    if(!isInExecution()){
        this->inExecution = true;
        this->circular = false;
        this->infinite = false;
        this->stepsToExecute = REVOLUTION_STEPS / MM_PER_REVOLUTION * millimeters;
        
        //if(this->stepsToExecute < 0.04 && this->stepsToExecute > 0){ // TODO: works for minimum distance of motor?
        //    this->stepsToExecute = 0.04;
        //}
        
        this->direction = dir;
        this->initTime = micros();
        this->lastStepTime = this->initTime;
        this->halfStepInterval = MILLIS_TO_MICROS_MID_MULTIPLIER * (movementTimeMillis / (float)this->stepsToExecute);
        if(percentageErrorMargin > 0 && !isExceeding){
            this->halfStepInterval = this->halfStepInterval * (1 + (percentageErrorMargin / 1000.00));
        } else if(percentageErrorMargin > 0) {
            this->halfStepInterval = this->halfStepInterval * (1 - (percentageErrorMargin / 1000.00));
        }
        if(this->halfStepInterval < MIN_INTERVAL){
            this->halfStepInterval = MIN_INTERVAL;
        } 
        if(this->stepsToExecute == 0){
            forceStop();
            //Serial.println("No steps to do.");
            return false;
        }
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

bool StepperCommand::startCircular(double mmFromProjection, double mmRadius, int movementTimeMillis, int startSpeedPercentual, int initialDir, bool initialIncrising){
    if(!isInExecution()){
        this->inExecution = true;
        this->circular = true;
        this->infinite = false;
        this->stepsToExecute = REVOLUTION_STEPS / MM_PER_REVOLUTION * mmFromProjection;
        if(this->stepsToExecute == 0){
            forceStop();
            Serial.println("No steps to do.");
            return false;
        }
        this->direction = initialDir;
        this->initTime = micros();
        this->lastStepTime = this->initTime;
        double linearHalfStepInterval = MILLIS_TO_MICROS_MID_MULTIPLIER * (movementTimeMillis / (float)this->stepsToExecute);
        if(linearHalfStepInterval < MIN_INTERVAL){
            linearHalfStepInterval = MIN_INTERVAL * 2;
        }
        this->circularMinHalfStepInterval = CIRCULAR_EXTREME_RANGE;
        this->circularMaxHalfStepInterval = (linearHalfStepInterval * 2) - CIRCULAR_EXTREME_RANGE;
        this->halfStepInterval = ((this->circularMaxHalfStepInterval - this->circularMinHalfStepInterval) / 100.00 * startSpeedPercentual) + this->circularMinHalfStepInterval;
        this->cicularIncrement = initialIncrising;
        this->cicularDeIncrementInterval = (this->circularMaxHalfStepInterval - this->circularMinHalfStepInterval) / (REVOLUTION_STEPS / MM_PER_REVOLUTION * mmRadius);
        /*Serial.println("||||||||||||");
        Serial.print("Total steps="); 
        Serial.println(this->stepsToExecute);
        Serial.print("In millis="); 
        Serial.println(movementTimeMillis);
        Serial.print("Init incr="); 
        Serial.println(this->cicularIncrement);
        Serial.print("Half speed micros="); 
        Serial.println(this->halfStepInterval);
        Serial.print("Max half speed micros="); 
        Serial.println(this->circularMaxHalfStepInterval);
        Serial.print("Min half speed micros="); 
        Serial.println(this->circularMinHalfStepInterval);
        Serial.print("Linear half speed micros="); 
        Serial.println(linearHalfStepInterval);
        Serial.print("Start perc="); 
        Serial.println(startSpeedPercentual);
        Serial.print("Incr interval="); 
        Serial.println(this->cicularDeIncrementInterval);
        Serial.println("+++++++++++");*/
        return true;
    }
    return false;
}

void StepperCommand::forceStop(){
    this->inExecution = false;
    this->initTime = -1;
    this->lastStepTime = -1;
    this->halfStepInterval = -1;
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
        if(this->circular){
            // la velocità è inversamente proporzionale alla grandezza dell'intervallo
            if(this->cicularIncrement){
                this->halfStepInterval -= this->cicularDeIncrementInterval;
            } else {
                this->halfStepInterval += this->cicularDeIncrementInterval;
            }
            if(this->halfStepInterval <= this->circularMinHalfStepInterval){
                this->cicularIncrement = !this->cicularIncrement;
                this->halfStepInterval = this->circularMinHalfStepInterval + this->cicularDeIncrementInterval;
            } else if(this->halfStepInterval >= this->circularMaxHalfStepInterval){
                this->cicularIncrement = !this->cicularIncrement;
                this->halfStepInterval = this->circularMaxHalfStepInterval - this->cicularDeIncrementInterval;
                this->direction = (this->direction + 1) % 2;
            }
        }
        if(this->infinite && this->stepsToExecute == INT_MAX - REVOLUTION_STEPS){
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
    if(this->infinite && this->stepsToExecute == 0){
        this->stepsToExecute = INT_MAX - REVOLUTION_STEPS - 1;
    }
    return this->stepsToExecute == 0;
}
