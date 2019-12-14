//
//  Stepper.cpp
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <Arduino.h>
#include "Stepper.h"

using namespace stepper_motor;

Stepper::Stepper(int directionPin, int stepPin, int homePin){
    this->directionPin = directionPin;
    this->stepPin = stepPin;
    this->homePin = homePin;
    this->direction = LOW;
    this->step = LOW;
    //Serial.println("New Stepper");
}

Stepper::~Stepper(){
    //Serial.println("Destroy Stepper");
}

void Stepper::begin(){
    pinMode(this->directionPin, OUTPUT); 
    pinMode(this->stepPin, OUTPUT);
    pinMode(this->homePin, INPUT);
    digitalWrite(this->directionPin, this->direction);
    digitalWrite(this->stepPin, this->step);
}


void Stepper::makeStepAsync(StepperCommand &command){

    if(digitalRead(this->homePin) == HIGH){
        //delay(3);
        if(command.forceStop()){
            this->direction = LOW;
            this->step = LOW;
            digitalWrite(this->directionPin, this->direction);
            digitalWrite(this->stepPin, this->step);
        }
        return;
    }
    
    unsigned long timestamp = micros();
    if(command.isInExecution() && command.canDoHalfStep(timestamp)){
        this->manageDirection(command.getDirection());
        
        this->step = !this->step;
        digitalWrite(this->stepPin, this->step);
        
        command.halfStepDone(timestamp, this->step);
        
        if(command.stepsTerminated()){
            //digitalWrite(this->stepPin, LOW);
            command.stop(timestamp);
        }
        
    }
}

void Stepper::invertRotation(){
    this->direction = !this->direction;
    digitalWrite(this->directionPin, this->direction);
}


void Stepper::manageDirection(int dir){
    if(this->direction != dir){
        this->direction = dir;
        digitalWrite(this->directionPin, dir);
    }
}
