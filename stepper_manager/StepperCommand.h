//
//  StepperCommand.h
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef StepperCommand_h
#define StepperCommand_h

#define MIN_INTERVAL 310
#define REVOLUTION_STEPS 400

namespace stepper_motor {
    
    class StepperCommand {
        
        private:
            bool inExecution;
            bool infinite;
            unsigned long initTime;
            unsigned long lastStepTime;
            unsigned long halfStepInterval;
            long initialSteps;
            long stepsToExecute;
            int direction;
        
        
        public:
            StepperCommand();
            ~StepperCommand();
            int getDirection();
            bool startLinear(unsigned long interval, long steps, int dir);
            void forceStop();
            void stop(unsigned long timestamp);
            void halfStepDone(unsigned long timestamp, int power);
            bool isInExecution();
            bool canDoHalfStep(unsigned long timestamp);
            bool stepsTerminated();
        
    };
    
}

#endif /* StepperCommand_h */
