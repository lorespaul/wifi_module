//
//  StepperCommand.cpp
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

int StepperCommand::getDirection(){
    return this->direction;
}

// 8mm : 360 = 0,04mm : 1.8
// 8mm : 360 = 10mm : 450 -> 1cm
// 8mm : 200 = millimeters : y
bool StepperCommand::startLinear(int millimeters, int movementTimeMillis, int dir){
    if(!isInExecution()){
        this->inExecution = true;
        this->circular = false;
        this->stepsToExecute = REVOLUTION_STEPS / MM_PER_REVOLUTION * millimeters;
        this->direction = dir;
        this->initTime = micros();
        this->lastStepTime = this->initTime;
        this->halfStepInterval = MILLIS_TO_MICROS_MID_MULTIPLIER * (movementTimeMillis / (float)this->stepsToExecute);
        /*Serial.print("Millimeters="); 
        Serial.println(millimeters);
        Serial.print("In millis="); 
        Serial.println(movementTimeMillis);
        Serial.print("Half speed micros="); 
        Serial.println(this->halfStepInterval);
        Serial.print("Total steps="); 
        Serial.println(this->stepsToExecute);*/
        return true;
    }
    return false;
}

bool StepperCommand::startCircular(int mmFromProjection, int mmRadius, int movementTimeMillis, int startSpeedPercentual, int initialDir, bool initialIncrising){
    if(!isInExecution()){
        this->inExecution = true;
        this->circular = true;
        this->stepsToExecute = REVOLUTION_STEPS / MM_PER_REVOLUTION * mmFromProjection;
        this->direction = initialDir;
        this->initTime = micros();
        this->lastStepTime = this->initTime;
        int linearHalfStepInterval = MILLIS_TO_MICROS_MID_MULTIPLIER * (movementTimeMillis / (float)this->stepsToExecute);
        this->circularMinHalfStepInterval = CIRCULAR_EXTREME_RANGE;
        this->circularMaxHalfStepInterval = (linearHalfStepInterval * 2) - CIRCULAR_EXTREME_RANGE;
        this->halfStepInterval = ((this->circularMaxHalfStepInterval - this->circularMinHalfStepInterval) / 100.00 * startSpeedPercentual) + this->circularMinHalfStepInterval;
        this->cicularIncrement = initialIncrising;
        this->cicularDeIncrementInterval = (this->circularMaxHalfStepInterval - this->circularMinHalfStepInterval) / (REVOLUTION_STEPS / MM_PER_REVOLUTION * mmRadius);
        /*Serial.print("Total steps="); 
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

void StepperCommand::stop(unsigned long timestamp){
    if(!stepsTerminated())
        return;
    
    Serial.print("Command end in ");
    Serial.print((timestamp - this->initTime) / 1000);
    Serial.println(" millis.");
    this->inExecution = false;
    this->initTime = -1;
    this->lastStepTime = -1;
    this->halfStepInterval = -1;
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
                //Serial.print("Change step=");
                //Serial.println(this->stepsToExecute);
            }
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
    return this->stepsToExecute == 0;
}
