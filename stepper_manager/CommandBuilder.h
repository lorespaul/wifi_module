//
//  CommandBuilder.h
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef CommandBuilder_h
#define CommandBuilder_h

#include <Arduino.h>
#include "StepperCommand.h"

#define ZC '\0'
#define NL '\n'
#define CR '\r'

#define SPACE " "
#define COMMA ","

#define X 'X'
#define Y 'Y'
#define Z 'Z'

#define INTERVAL 'i'
#define STEPS 's'
#define DIRECTION 'd'

#define GO_AHEAD_CHAR '0'
#define GO_BACK_CHAR '1'
#define GO_AHEAD 0
#define GO_BACK 1

#define MOTORS 3
#define BUFFER 25


namespace stepper_motor {
    
    class CommandBuilder {

        private:
            char allPramatersCache[MOTORS][BUFFER];
            long motorsRatio[MOTORS];
            int positionMax;
            int positionMin;
            int positionMid;
            long max;
            long min;
            long mid;
            long buildOne(StepperCommand &command, char *oneCommandParameters);
            void computeRatios();
            void cleanMotorsRatio();
            
        public:
            CommandBuilder();
            ~CommandBuilder();
            void build(char allCommandParameters[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand);
            void resyncCommands(StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand);
        
    };
    
}

#endif /* CommandBuilder_h */
