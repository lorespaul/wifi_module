//
//  Stepper.cpp
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <Arduino.h>
#include "Stepper.h"

using namespace stepper_motor;

Stepper::Stepper(int directionPin, int stepPin){
    this->directionPin = directionPin;
    this->stepPin = stepPin;
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
    digitalWrite(this->directionPin, this->direction);
    digitalWrite(this->stepPin, this->step);
}


void Stepper::makeStepAsync(StepperCommand &command){
    
    unsigned long timestamp = micros();
    if(command.isInExecution() && command.canDoHalfStep(timestamp)){
        this->manageDirection(command.getDirection());
        
        this->step = (this->step + 1) % 2;
        digitalWrite(this->stepPin, this->step);
        
        command.halfStepDone(timestamp, this->step);
        
        if(command.stepsTerminated()){
            digitalWrite(this->stepPin, LOW);
            command.stop(timestamp);
        }
        
    }
}

void Stepper::invertRotation(){
    if(this->direction == LOW){
        this->direction = HIGH;
        digitalWrite(this->directionPin, HIGH);
    } else if(this->direction == HIGH){
        this->direction = LOW;
        digitalWrite(this->directionPin, LOW);
    }
}


void Stepper::manageDirection(int dir){
    if(this->direction != dir){
        this->direction = dir;
        digitalWrite(this->directionPin, dir);
    }
}
