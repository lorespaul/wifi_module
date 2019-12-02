//
//  CommandBuilder.cpp
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include "CommandBuilder.h"

using namespace stepper_motor;

CommandBuilder::CommandBuilder(){
}

CommandBuilder::~CommandBuilder(){
}


void CommandBuilder::buildOne(StepperCommand &command, char *oneCommandParameters){

    char *axisToken = strtok(oneCommandParameters, COMMA);
    unsigned long interval;
    long steps;
    int direction;
    
    while(axisToken != NULL){

        switch(axisToken[0]){
            case INTERVAL:
                interval = strtoul(&axisToken[1], NULL, 10);
                break;
            case STEPS:
                steps = atol(&axisToken[1]);
                break;
            case DIRECTION:
                direction = axisToken[1] == GO_AHEAD_CHAR ? GO_AHEAD : axisToken[1] == GO_BACK_CHAR ? GO_BACK : -1; 
                break;
        }

        axisToken = strtok(NULL, COMMA);
    }

    free(axisToken);
    
    command.startLinear(interval, steps, direction);
}



void CommandBuilder::build(char allCommandParameters[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand){

    char *axisArrayToken = strtok(allCommandParameters, SPACE);

    while(axisArrayToken != NULL){
        
        switch(axisArrayToken[0]){
            case X:
                this->buildOne(xCommand, &axisArrayToken[1]);
                break;
            case Y:
                this->buildOne(yCommand, &axisArrayToken[1]);
                break;
            case Z:
                this->buildOne(zCommand, &axisArrayToken[1]);
                break;
        }

        axisArrayToken = strtok(NULL, SPACE);
    }
    free(axisArrayToken);
}
