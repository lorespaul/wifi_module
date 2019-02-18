//
//  Stepper.cpp
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <Arduino.h>
#include "Stepper.h"
#include "StepperCommand.h"

using namespace stepper_motor;

Stepper::Stepper(int directionPin, int stepPin, bool bispositionStepper){
    this->bispositionStepper = bispositionStepper;
    this->directionPin = directionPin;
    this->stepPin = stepPin;
    this->direction = LOW;
    this->step = LOW;
    digitalWrite(this->directionPin, this->direction);
    //Serial.println("New Stepper");
}

Stepper::Stepper(int directionPin, int stepPin) : Stepper(directionPin, stepPin, false){
}

Stepper::~Stepper(){
    //Serial.println("Destroy Stepper");
}

void Stepper::begin(){
  pinMode(this->directionPin, OUTPUT); 
  pinMode(this->stepPin, OUTPUT); 
}

void Stepper::makeStepAsync(StepperCommand &command){
    if(this->bispositionStepper){
        //Serial.println("Can't make step async in biposition stepper.");
        return;
    }
    
    unsigned long timestamp = micros();
    if(command.isCommandInExecution() && command.canDoHalfStep(timestamp)){
        
        if(step == LOW){
            this->step = HIGH;
            digitalWrite(this->stepPin, HIGH);
        } else if(step == HIGH){
            this->step = LOW;
            digitalWrite(this->stepPin, LOW);
        }
        
        command.halfStepDone(timestamp, this->step);
        
        if(command.stepsTerminated()){
            command.finish();
        }
        
    }
}

void Stepper::makeSteps(int steps, int delayBetweenStepMillis){
    float halfDelay = delayBetweenStepMillis / 2.00;
    //Serial::println(to_string(halfDelay));
    for(int i=0;i<steps;i++){
        digitalWrite(this->stepPin, HIGH);
        delayMicroseconds(halfDelay);
        digitalWrite(this->stepPin, LOW);
        delayMicroseconds(halfDelay);
    }
}

void Stepper::makeRevolution(){
    //Serial.println("Revolution begin");
    makeSteps(REVOLUTION_STEPS, 1000);
    //Serial.println("Revolution done");
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
