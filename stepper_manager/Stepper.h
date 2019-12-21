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
            int homePin;
            int direction;
            int step;
            void invertRotation();
            void manageDirection(int dir);
            
        public:
            Stepper(int directionPin, int stepPin, int homePin);
            ~Stepper();
            void begin();
            void makeStepAsync(StepperCommand &command);
            void reset();
        
    };
    
}

#endif /* Stepper_h */
