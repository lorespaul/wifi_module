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

int CommandBuilder::computeStartEndPosDistanceLinear(int startPos, int endPos){
    if(endPos == INT_MIN) return 0;
    return abs(startPos - endPos);
}

int CommandBuilder::computeSpeedMillisLinear(float mmPerSec, int xEndPos, int yEndPos, int zEndPos){
    int mmX = this->computeStartEndPosDistanceLinear(xLastPos, xEndPos);
    int mmY = this->computeStartEndPosDistanceLinear(yLastPos, yEndPos);
    int mmZ = this->computeStartEndPosDistanceLinear(zLastPos, zEndPos);
    double result = sqrt(pow(mmX, 2) + pow(mmY, 2));
    result = sqrt(pow(result, 2) + pow(mmZ, 2));
    return 1000 * (result / mmPerSec);
}

void CommandBuilder::buildSingleLinear(StepperCommand &command, int *startPos, int endPos, int speedMillis){
    if(endPos > INT_MIN){
        int dir;
        if(endPos > *startPos) dir = GO_AHEAD;
        else dir = GO_BACK;
        int distance = this->computeStartEndPosDistanceLinear(*startPos, endPos);
        if(distance > 0){
            command.startLinear(distance, speedMillis, dir);
            *startPos = endPos;
        }
    }
}


double CommandBuilder::computeRadius(){
    return sqrt(pow(iOffset, 2) + pow(jOffset, 2));
}

int CommandBuilder::computeStartEndPosDistanceCircularProjection(int startPos, int endPos, int offset, bool clockwise, bool startUp, bool endUp){
    if(endPos == INT_MIN) return 0;
    int radius = this->computeRadius();
    int result;
    //Serial.println(startPos);
    //Serial.println(endPos);
    //Serial.println(startUp);
    //Serial.println(endUp);
    if((clockwise && startUp) || (!clockwise && !startUp)){
        if(endPos > startPos && ((clockwise && startUp && endUp) || (!clockwise && !startUp && !endUp))){
            //Serial.println("A");
            result = abs(endPos - startPos);
        } else {
            int mid = offset < 0 ? radius - abs(offset) : radius + abs(offset);
            //Serial.print("MidA=");
            //Serial.println(mid);
            result = mid + abs(startPos + mid - endPos);
            //Serial.println(result);
            if(startUp == endUp){
                mid = radius - ((startPos + offset) - endPos);
                //Serial.print("Mid2A=");
                //Serial.println(mid);
                result += (mid * 2);
            }
        }
    } else {
        if(endPos < startPos && ((clockwise && !startUp && !endUp) || (!clockwise && startUp && endUp))){
            //Serial.println("B");
            result = abs(startPos - endPos);
        } else {
            int mid = offset < 0 ? radius + abs(offset) : radius - abs(offset);
            //Serial.print("MidB=");
            //Serial.println(mid);
            result = mid + abs(endPos - (startPos - mid));
            //Serial.println(result);
            if(startUp == endUp){
                mid = radius - abs((startPos + offset) - endPos);
                //Serial.print("Mid2B=");
                //Serial.println(mid);
                result += (mid * 2);
            }
        }
    }
    Serial.print("Proj=");
    Serial.println(result);
    return result;
}


int CommandBuilder::computeSpeedMillisCircular(float mmPerSec, int xEndPos, int yEndPos, bool clockwise, bool yStartUp, bool yEndUp){
    int mmX = this->computeStartEndPosDistanceLinear(xLastPos, xEndPos);
    int mmY = this->computeStartEndPosDistanceLinear(yLastPos, yEndPos);
    
    double radius = this->computeRadius();
    int xCenterPos = xLastPos + iOffset;
    int yCenterPos = yLastPos + jOffset;
    int mmEndX = this->computeStartEndPosDistanceLinear(xCenterPos, xEndPos);
    int mmEndY = this->computeStartEndPosDistanceLinear(yCenterPos, yEndPos);
    if(sqrt(pow(mmEndX, 2) + pow(mmEndY, 2)) != radius)
        return -1;
    
    double rope = sqrt(pow(mmX, 2) + pow(mmY, 2));
    double angle = (asin(rope / (double)(2 * radius)) * 180 / PI) * 2;
    double girth = PI * 2 * radius;
    double arch = (girth * angle) / 360.00;

    int xMmProjection = computeStartEndPosDistanceCircularProjection(xLastPos, xEndPos, iOffset, clockwise, yStartUp, yEndUp);
    if(xMmProjection > (radius * 2))
        arch = girth - arch;

    Serial.print("Arch=");
    Serial.println(arch);
    return 1000 * (arch / mmPerSec);
}

void CommandBuilder::buildSingleCircular(StepperCommand &command, int *startPos, int endPos, int offset, bool clockwise, bool startUp, bool endUp, int speedMillis){
    if(endPos > INT_MIN){
        int dir;
        if(endPos > *startPos || (endPos == *startPos && clockwise)) dir = GO_AHEAD;
        else dir = GO_BACK;
        int distance = this->computeStartEndPosDistanceCircularProjection(*startPos, endPos, offset, clockwise, startUp, endUp);
        if(distance > 0){
            //(int mmFromProjection, int mmRadius, int movementTimeMillis, int startSpeedPercentual, int initialDir, bool initialIncrising)
            int radius = this->computeRadius();
            command.startCircular(distance, radius, speedMillis, abs((100 * offset) / radius), dir, (offset > 0 && clockwise && startUp) || 
                                                                                                    (offset > 0 && !clockwise && !startUp) || 
                                                                                                    (offset < 0 && clockwise && !startUp) || 
                                                                                                    (offset < 0 && !clockwise && startUp));
            *startPos = endPos;
        }
    }
}

int CommandBuilder::build(char stringCommand[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand){
    this->prepareContext(stringCommand);
    int speedMillis = 0;
    if(this->gModeEq(G00)){
        Serial.println("Mode G00");
        speedMillis = this->computeSpeedMillisLinear(STD_F_FAST, xPos, yPos, zPos);
        this->buildSingleLinear(xCommand, &xLastPos, xPos, speedMillis);
        this->buildSingleLinear(yCommand, &yLastPos, yPos, speedMillis);
        this->buildSingleLinear(zCommand, &zLastPos, zPos, speedMillis);
    } else if(this->gModeEq(G01)){
        Serial.println("Mode G01");
        speedMillis = this->fMode > 0 ? this->computeSpeedMillisLinear(this->fMode, xPos, yPos, zPos) : this->computeSpeedMillisLinear(STD_F_SLOW, xPos, yPos, zPos);
        this->buildSingleLinear(xCommand, &xLastPos, xPos, speedMillis);
        this->buildSingleLinear(yCommand, &yLastPos, yPos, speedMillis);
        this->buildSingleLinear(zCommand, &zLastPos, zPos, speedMillis);
    } else if(this->gModeEq(G02)){
        Serial.println("Mode G02");
        speedMillis = this->fMode > 0 ? this->computeSpeedMillisCircular(this->fMode, xPos, yPos, true, (iOffset > 0 && jOffset <= 0) || (iOffset <= 0 && jOffset < 0), yPos > (yLastPos + jOffset)) 
                                      : this->computeSpeedMillisCircular(STD_F_SLOW, xPos, yPos, true, (iOffset > 0 && jOffset <= 0) || (iOffset <= 0 && jOffset < 0), yPos > (yLastPos + jOffset));
        if(speedMillis > -1){
            int hereXLastPos = xLastPos;
            this->buildSingleCircular(xCommand, &xLastPos, xPos, iOffset, true, (iOffset > 0 && jOffset <= 0) || (iOffset <= 0 && jOffset < 0), yPos >= (yLastPos + jOffset), speedMillis);
            this->buildSingleCircular(yCommand, &yLastPos, yPos, jOffset, true, (jOffset > 0 && iOffset >= 0) || (jOffset <= 0 && iOffset > 0), xPos <= (hereXLastPos + iOffset), speedMillis);
        } else {
            Serial.println("Command not valid");
        }
    } else if(this->gModeEq(G03)){
        Serial.println("Mode G03");
        speedMillis = this->fMode > 0 ? this->computeSpeedMillisCircular(this->fMode, xPos, yPos, false, (iOffset >= 0 && jOffset < 0) || (iOffset < 0 && jOffset <= 0), yPos >= (yLastPos + jOffset)) 
                                      : this->computeSpeedMillisCircular(STD_F_SLOW, xPos, yPos, false, (iOffset >= 0 && jOffset < 0) || (iOffset < 0 && jOffset <= 0), yPos >= (yLastPos + jOffset));
        if(speedMillis > -1){
            int hereXLastPos = xLastPos;
            this->buildSingleCircular(xCommand, &xLastPos, xPos, iOffset, false, (iOffset >= 0 && jOffset < 0) || (iOffset < 0 && jOffset <= 0), yPos >= (yLastPos + jOffset), speedMillis);
            this->buildSingleCircular(yCommand, &yLastPos, yPos, jOffset, false, (jOffset >= 0 && iOffset > 0) || (jOffset < 0 && iOffset >= 0), xPos <= (hereXLastPos + iOffset), speedMillis);
        } else {
            Serial.println("Command not valid");
        }
    } else if(this->gModeEq(G28)){
        Serial.println("Mode G28");
        speedMillis = this->computeSpeedMillisLinear(STD_F_FAST, 0, 0, 0);
        this->buildSingleLinear(xCommand, &xLastPos, 0, speedMillis);
        this->buildSingleLinear(yCommand, &yLastPos, 0, speedMillis);
        this->buildSingleLinear(zCommand, &zLastPos, 0, speedMillis);
    }
    Serial.print("Speed=");
    Serial.println(speedMillis);
    Serial.print("X=");
    Serial.println(xLastPos);
    Serial.print("Y=");
    Serial.println(yLastPos);
    Serial.print("Z=");
    Serial.println(zLastPos);
    Serial.print("F=");
    Serial.println(fMode);
    
    //G03 X0 Y0 I60 J0
    
    Serial.println("----");
    return speedMillis;
}
