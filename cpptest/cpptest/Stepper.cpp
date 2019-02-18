//
//  Stepper.cpp
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <iostream>
#include "Stepper.h"
#include "StepperCommand.h"
#include "stepperCommon.h"
#include "Serial.h"

using namespace std;
using namespace stp_motor;
using namespace serial;

Stepper::Stepper(int directionPin, int stepPin, bool bispositionStepper){
    this->bispositionStepper = bispositionStepper;
    this->directionPin = directionPin;
    this->stepPin = stepPin;
    this->direction = LOW;
    this->step = LOW;
    digitalWrite(directionPin, (Power)this->direction);
    Serial::println("New Stepper");
}

Stepper::Stepper(int directionPin, int stepPin) : Stepper(directionPin, stepPin, false){
}

Stepper::~Stepper(){
    Serial::println("Destroy Stepper");
}

void Stepper::makeStepAsync(StepperCommand &command){
    if(this->bispositionStepper){
        cout << "Can't make step async in biposition stepper." << endl;
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
        
        command.halfStepDone(timestamp, (Power)this->step);
        
        if(command.stepsTerminated()){
            command.end();
        }
        
    }
}

void Stepper::makeSteps(int steps, int delayBetweenStepMillis){
    float halfDelay = delayBetweenStepMillis / 2.00;
    //Serial::println(to_string(halfDelay));
    for(int i=0;i<steps;i++){
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(halfDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(halfDelay);
    }
}

void Stepper::makeRevolution(){
    Serial::println("Revolution begin");
    makeSteps(REVOLUTION_STEPS, 1000);
    Serial::println("Revolution done");
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
