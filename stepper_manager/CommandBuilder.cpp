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


long CommandBuilder::buildOne(StepperCommand &command, char *oneCommandParameters){

    char *axisToken = strtok(oneCommandParameters, COMMA);
    unsigned long interval;
    long steps = 0;
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
    return steps;
}



void CommandBuilder::build(char allCommandParameters[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand){

    //this->cleanMotorsRatio();
    
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
                motorsRatio[0] = this->buildOne(xCommand, &allPramatersCache[i][1]);
                break;
            case Y:
                motorsRatio[1] = this->buildOne(yCommand, &allPramatersCache[i][1]);
                break;
            case Z:
                motorsRatio[2] = this->buildOne(zCommand, &allPramatersCache[i][1]);
                break;
        }
    }

    /*positionMax = positionMin = positionMid = -1;
    max = min = mid = 0;
    this->computeRatios();
    
    if(min != 0){
        int realMidRatio = 0,
            realMaxRatio = max / min;
        if(mid > 0) {
            realMidRatio = max / mid;
        }
    
        if(positionMax > -1)
            motorsRatio[positionMax] = realMaxRatio;
        if(positionMin > -1)
            motorsRatio[positionMin] = 1;
        if(positionMid > -1)
            motorsRatio[positionMid] = realMidRatio;   
    } else {
        positionMax = positionMin = positionMid = -1;
        max = min = mid = 0;
        this->cleanMotorsRatio();
    }*/
    
}


void CommandBuilder::cleanMotorsRatio(){
    int i;
    for(i = 0; i < MOTORS; i++)
        motorsRatio[i] = 0;
}


void CommandBuilder::computeRatios(){
    int i; 
    for(i = 0; i < MOTORS; i++){
        if(max < motorsRatio[i] && motorsRatio[i] > 0){
            max = motorsRatio[i];
            positionMax = i;
        }
        if(min > motorsRatio[i] && motorsRatio[i] > 0){
            min = motorsRatio[i];
            positionMin = i;
        }
    }
}



void CommandBuilder::resyncCommands(StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand){
    /*if(positionMax == -1)
        return;
    
    StepperCommand commands[3] = { xCommand, yCommand, zCommand };
    
    (commands[positionMax].getInitialSteps() - commands[positionMax].getStepsToExecute()) / motorsRation[positionMax]
    

    free(commands);*/
}
