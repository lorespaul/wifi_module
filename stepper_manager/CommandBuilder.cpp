//
//  CommandBuilder.cpp
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <limits.h>
#include "CommandBuilder.h"

using namespace stepper_motor;

CommandBuilder::CommandBuilder(){
    fMode = 0;  
    xLastPos = 0;
    yLastPos = 0;
    zLastPos = 0;
    xPos = INT_MIN;
    yPos = INT_MIN;
    zPos = INT_MIN;
    iOffset = INT_MIN;
    jOffset = INT_MIN;
}

CommandBuilder::~CommandBuilder(){
}

void CommandBuilder::prepareContext(char stringCommand[]){
    char *token = strtok(stringCommand, SPACE);
    bool coordinatesClean[5] = { false, false, false, false, false };
    while(token != NULL){
        //Serial.print(token[0]);
        //Serial.print(" -> ");
        switch(token[0]){
            case G:
                strcpy(gMode, &token[1]);
                //Serial.println(gMode);
                break;
            case F:
                fMode = atoi(&token[1]);
                //Serial.println(fMode);
                break;
            case X:
                xPos = atoi(&token[1]);
                coordinatesClean[0] = true;
                //Serial.println(xPos);
                break;
            case Y:
                yPos = atoi(&token[1]);
                coordinatesClean[1] = true;
                //Serial.println(yPos);
                break;
            case Z:
                zPos = atoi(&token[1]);
                coordinatesClean[2] = true;
                //Serial.println(zPos);
                break;
            case I:
                iOffset = atoi(&token[1]);
                coordinatesClean[3] = true;
                //Serial.println(iOffset);
                break;
            case J:
                jOffset = atoi(&token[1]);
                coordinatesClean[4] = true;
                //Serial.println(jOffset);
                break;
        }
        token = strtok(NULL, SPACE);
    }
  
    for(int i=0; i<5; i++){
        if(coordinatesClean[i] == false){
            switch(i){
                case 0:
                    xPos = INT_MIN;
                    //Serial.println("Clean x");
                    break;
                case 1:
                    yPos = INT_MIN;
                    //Serial.println("Clean y");
                    break;
                case 2:
                    zPos = INT_MIN;
                    //Serial.println("Clean z");
                    break;
                case 3:
                    iOffset = INT_MIN;
                    //Serial.println("Clean i");
                    break;
                case 4:
                    jOffset = INT_MIN;
                    //Serial.println("Clean j");
                    break; 
            }
        }
    }
}


bool CommandBuilder::gModeEq(char comparison[]){
    for(int i=0; i<G_MODE_CHARS; i++){
        if(gMode[i] != comparison[i])
            return false;
    }
    return true;
}

int CommandBuilder::computeStartEndPosDistance(int startPos, int endPos){
    if(endPos == INT_MIN) return 0;
    return abs(startPos - endPos);
}

int CommandBuilder::computeSpeedMillis(float mmPerSec, int xEndPos, int yEndPos, int zEndPos){
    int mmX = this->computeStartEndPosDistance(xLastPos, xEndPos);
    int mmY = this->computeStartEndPosDistance(yLastPos, yEndPos);
    int mmZ = this->computeStartEndPosDistance(zLastPos, zEndPos);
    int result = sqrt(pow(mmX, 2) + pow(mmY, 2));
    result = sqrt(pow(result, 2) + pow(mmZ, 2));
    return 1000 * (result / mmPerSec);
}

//void CommandBuilder::buildSingleCircular(){
//}

void CommandBuilder::buildSingleLinear(StepperCommand &command, int *startPos, int endPos, int speedMillis){
    if(endPos > INT_MIN){
        int dir;
        if(endPos > *startPos) dir = GO_AHEAD;
        else dir = GO_BACK;
        int distance = this->computeStartEndPosDistance(*startPos, endPos);
        if(distance > 0){
            command.start(distance, speedMillis, dir);
            *startPos = endPos;
        }
    }
}

void CommandBuilder::build(char stringCommand[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand){
    this->prepareContext(stringCommand);
    int speedMillis;
    if(this->gModeEq(G00)){
        //Serial.println("Mode G00");
        speedMillis = this->computeSpeedMillis(STD_F_FAST, xPos, yPos, zPos);
        this->buildSingleLinear(xCommand, &xLastPos, xPos, speedMillis);
        this->buildSingleLinear(yCommand, &yLastPos, yPos, speedMillis);
        this->buildSingleLinear(zCommand, &zLastPos, zPos, speedMillis);
    } else if(this->gModeEq(G01)){
        //Serial.println("Mode G01");
        speedMillis = this->fMode > 0 ? this->fMode : this->computeSpeedMillis(STD_F_FAST, xPos, yPos, zPos);
        this->buildSingleLinear(xCommand, &xLastPos, xPos, speedMillis);
        this->buildSingleLinear(yCommand, &yLastPos, yPos, speedMillis);
        this->buildSingleLinear(zCommand, &zLastPos, zPos, speedMillis);
    } else if(this->gModeEq(G02)){
        //Serial.println("Mode G02");
        
    } else if(this->gModeEq(G03)){
        //Serial.println("Mode G03");
        
    } else if(this->gModeEq(G28)){
        //Serial.println("Mode G28");
        speedMillis = this->computeSpeedMillis(STD_F_FAST, 0, 0, 0);
        this->buildSingleLinear(xCommand, &xLastPos, 0, speedMillis);
        this->buildSingleLinear(yCommand, &yLastPos, 0, speedMillis);
        this->buildSingleLinear(zCommand, &zLastPos, 0, speedMillis);
    }
    Serial.print("X=");
    Serial.println(xLastPos);
    Serial.print("Y=");
    Serial.println(yLastPos);
    Serial.print("Z=");
    Serial.println(zLastPos);
    //G00 X10 Y13 Z7
    Serial.println("----");
}
