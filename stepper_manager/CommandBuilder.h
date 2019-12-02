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

namespace stepper_motor {
    
    class CommandBuilder {

        private:
            void buildOne(StepperCommand &command, char *oneCommandParameters);
            
        public:
            CommandBuilder();
            ~CommandBuilder();
            void build(char allCommandParameters[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand);
        
    };
    
}

#endif /* CommandBuilder_h */
