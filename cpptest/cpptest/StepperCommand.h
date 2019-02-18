//
//  StepperCommand.h
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef StepperCommand_h
#define StepperCommand_h

#include "stepperCommon.h"

namespace stp_motor {
    
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
        
        // 8mm : 360 = 0,04mm : 1.8
        // 8mm : 360 = 10mm : 450 -> 1cm
        
        //bool begin(int stepsToExecute, int totalStepsTimeMillis);
        bool begin(int millimeters, int movementTimeMillis);
        void end();
        void halfStepDone(unsigned long timestamp, int power);
        bool isCommandInExecution();
        bool canDoHalfStep(unsigned long timestamp);
        bool stepsTerminated();
        
    };
    
}

#endif /* StepperCommand_h */
