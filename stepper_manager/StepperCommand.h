//
//  StepperCommand.h
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef StepperCommand_h
#define StepperCommand_h

#define REVOLUTION_STEPS 200
#define MM_PER_REVOLUTION 8.00

namespace stepper_motor {
    
    class StepperCommand {
        
    private:
        bool inExecution;
        int stepsToExecute;
        unsigned long initTime;
        unsigned long lastStepTime;
        unsigned long halfStepInterval;
        
        
    public:
        StepperCommand();
        ~StepperCommand();
        
        //bool begin(int stepsToExecute, int totalStepsTimeMillis);
        bool start(int millimeters, int movementTimeMillis);
        void stop();
        void halfStepDone(unsigned long timestamp, int power);
        bool isCommandInExecution();
        bool canDoHalfStep(unsigned long timestamp);
        bool stepsTerminated();
        
    };
    
}

#endif /* StepperCommand_h */
