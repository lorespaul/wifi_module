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
        //Serial.print("axisToken: ");
        //Serial.println(axisToken);
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

    int i = 0;
    memset(allPramatersCache, ZC, BUFFER * MOTORS);
    
    while(axisArrayToken != NULL && i < MOTORS){
        
        strncpy(allPramatersCache[i], axisArrayToken, BUFFER - 1);
        axisArrayToken = strtok(NULL, SPACE);
        i++;
    }
    free(axisArrayToken);

    for(i = 0; i < MOTORS; i++){
        //Serial.print("axisArrayToken: ");
        //Serial.println(allPramatersCache[i]);
        switch(allPramatersCache[i][0]){
            case X:
                this->buildOne(xCommand, &allPramatersCache[i][1]);
                break;
            case Y:
                this->buildOne(yCommand, &allPramatersCache[i][1]);
                break;
            case Z:
                this->buildOne(zCommand, &allPramatersCache[i][1]);
                break;
        }
    }
    
}
