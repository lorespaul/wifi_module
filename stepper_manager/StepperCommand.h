//
//  StepperCommand.h
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef StepperCommand_h
#define StepperCommand_h

#define REVOLUTION_STEPS 200
#define MM_PER_REVOLUTION 8.00

#define MILLIS_TO_MICROS_MID_MULTIPLIER 500
#define CIRCULAR_EXTREME_RANGE 300

namespace stepper_motor {
    
    class StepperCommand {
        
        private:
            bool inExecution;
            int stepsToExecute;
            int direction;
            bool circular;
            unsigned long initTime;
            unsigned long lastStepTime;
            double halfStepInterval;
            int circularMaxHalfStepInterval;
            int circularMinHalfStepInterval;
            bool cicularIncrement;
            double cicularDeIncrementInterval;
        
        
        public:
            StepperCommand();
            ~StepperCommand();
            int getDirection();
            bool startLinear(int millimeters, int movementTimeMillis, int dir);
            bool startCircular(int mmFromProjection, int mmRadius, int movementTimeMillis, int startSpeedPercentual, int initialDir, bool initialIncrising);
            void stop(unsigned long timestamp);
            void halfStepDone(unsigned long timestamp, int power);
            bool isInExecution();
            bool canDoHalfStep(unsigned long timestamp);
            bool stepsTerminated();
        
    };
    
}

#endif /* StepperCommand_h */
