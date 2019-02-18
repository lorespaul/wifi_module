//
//  Stepper.h
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef Stepper_h
#define Stepper_h

#include "StepperCommand.h"

namespace stp_motor {
    
    class Stepper {
        
    private:
        bool bispositionStepper;
        int directionPin;
        int stepPin;
        int direction;
        int step;
        
    public:
        Stepper(int directionPin, int stepPin);
        Stepper(int directionPin, int stepPin, bool bispositionStepper);
        ~Stepper();
        void begin();
        virtual void makeStepAsync(StepperCommand &command);
        void makeSteps(int steps, int delayBetweenStepMillis);
        void makeRevolution();
        void invertRotation();
        
    };
    
}

#endif /* Stepper_h */
