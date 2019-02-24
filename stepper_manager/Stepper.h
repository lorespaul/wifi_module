//
//  Stepper.h
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef Stepper_h
#define Stepper_h

#include "StepperCommand.h"

namespace stepper_motor {
    
    class Stepper {
        
        private:
            int directionPin;
            int stepPin;
            int direction;
            int step;
            void invertRotation();
            void manageDirection(int dir);
            
        public:
            Stepper(int directionPin, int stepPin);
            Stepper(int directionPin, int stepPin, bool bispositionStepper);
            ~Stepper();
            void begin();
            void makeStepAsync(StepperCommand &command);
        
    };
    
}

#endif /* Stepper_h */
