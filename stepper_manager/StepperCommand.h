//
//  StepperCommand.h
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef StepperCommand_h
#define StepperCommand_h

#define REVOLUTION_STEPS 800
#define QUARTER_REVOLUTION_STEPS REVOLUTION_STEPS/4

#define GO_HOME_INTERVAL 100

namespace stepper_motor {
    
    class StepperCommand {
        
        private:
            bool inExecution;
            bool infinite;
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
            bool forceStop();
            void stop(unsigned long timestamp);
            void halfStepDone(unsigned long timestamp, int power);
            bool isInExecution();
            bool canDoHalfStep(unsigned long timestamp);
            bool stepsTerminated();
        
    };
    
}

#endif /* StepperCommand_h */
