//
//  CommandBuilder.cpp
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <float.h>
#include "CommandBuilder.h"

using namespace stepper_motor;

CommandBuilder::CommandBuilder(){
    fMode = 0;  
    xLastPos = 0;
    yLastPos = 0;
    zLastPos = 0;
    xPos = -DBL_MAX;
    yPos = -DBL_MAX;
    zPos = -DBL_MAX;
    iOffset = -DBL_MAX;
    jOffset = -DBL_MAX;
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
            case GG:
                strcpy(gMode, &token[0]);
                //Serial.println(gMode);
                break;
            case MM:
                strcpy(mMode, &token[0]);
                //Serial.println(mMode);
                break;
            case FF:
                fMode = atoi(&token[1]);
                //Serial.println(fMode);
                break;
            case XX:
                xPos = atof(&token[1]);
                coordinatesClean[0] = true;
                //Serial.println(xPos);
                break;
            case YY:
                yPos = atof(&token[1]);
                coordinatesClean[1] = true;
                //Serial.println(yPos);
                break;
            case ZZ:
                zPos = atof(&token[1]);
                coordinatesClean[2] = true;
                //Serial.println(zPos);
                break;
            case II:
                iOffset = atof(&token[1]);
                coordinatesClean[3] = true;
                //Serial.println(iOffset);
                break;
            case JJ:
                jOffset = atof(&token[1]);
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
                    xPos = -DBL_MAX;
                    //Serial.println("Clean x");
                    break;
                case 1:
                    yPos = -DBL_MAX;
                    //Serial.println("Clean y");
                    break;
                case 2:
                    zPos = -DBL_MAX;
                    //Serial.println("Clean z");
                    break;
                case 3:
                    iOffset = -DBL_MAX;
                    //Serial.println("Clean i");
                    break;
                case 4:
                    jOffset = -DBL_MAX;
                    //Serial.println("Clean j");
                    break; 
            }
        }
    }
}


bool CommandBuilder::modeEq(const char mode[], const char comparison[]){
    for(int i=0; i<MODE_CHARS; i++){
        if(mode[i] != comparison[i])
            return false;
    }
    return true;
}

int CommandBuilder::computeSpeedMillis(float mmPerSec, double millimeters){
    return 1000 * (millimeters / mmPerSec);
}

double CommandBuilder::computeStartEndPosDistanceLinear(double startPos, double endPos){
    if(endPos == -DBL_MAX) return 0;
    return fabs(startPos - endPos);
}

int CommandBuilder::computeSpeedMillisLinear(float mmPerSec, double xEndPos, double yEndPos, double zEndPos){
    double mmX = this->computeStartEndPosDistanceLinear(xLastPos, xEndPos);
    double mmY = this->computeStartEndPosDistanceLinear(yLastPos, yEndPos);
    double mmZ = this->computeStartEndPosDistanceLinear(zLastPos, zEndPos);
    double result = sqrt(pow(mmX, 2) + pow(mmY, 2));
    result = sqrt(pow(result, 2) + pow(mmZ, 2));
    //return 1000 * (result / mmPerSec);
    return computeSpeedMillis(mmPerSec, result);
}

void CommandBuilder::buildSingleLinear(StepperCommand &command, double *startPos, double endPos, int speedMillis, int percentageErrorMargin, bool isExceeding){
    if(endPos > -DBL_MAX){
        int dir;
        if(endPos > *startPos) dir = GO_AHEAD;
        else dir = GO_BACK;
        double distance = this->computeStartEndPosDistanceLinear(*startPos, endPos);
        if(distance >= MIN_DISTANCE){
            command.startLinear(distance, speedMillis, dir, percentageErrorMargin, isExceeding);
            *startPos = endPos;
        }
    }
}


double CommandBuilder::computeRadius(){
    return sqrt(pow(iOffset, 2) + pow(jOffset, 2));
}

double CommandBuilder::computeStartEndPosDistanceCircularProjection(double startPos, double endPos, double offset, bool clockwise, bool startUp, bool endUp){
    if(endPos == -DBL_MAX) return 0;
    double radius = this->computeRadius();
    double result;
    //Serial.println(startPos);
    //Serial.println(endPos);
    //Serial.println(startUp);
    //Serial.println(endUp);
    if((clockwise && startUp) || (!clockwise && !startUp)){
        if(endPos > startPos && ((clockwise && startUp && endUp) || (!clockwise && !startUp && !endUp))){
            //Serial.println("A");
            result = fabs(endPos - startPos);
        } else {
            double mid = offset < 0 ? radius - fabs(offset) : radius + fabs(offset);
            //Serial.print("MidA=");
            //Serial.println(mid);
            result = mid + fabs(startPos + mid - endPos);
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
            result = fabs(startPos - endPos);
        } else {
            double mid = offset < 0 ? radius + fabs(offset) : radius - fabs(offset);
            //Serial.print("MidB=");
            //Serial.println(mid);
            result = mid + fabs(endPos - (startPos - mid));
            //Serial.println(result);
            if(startUp == endUp){
                mid = radius - fabs((startPos + offset) - endPos);
                //Serial.print("Mid2B=");
                //Serial.println(mid);
                result += (mid * 2);
            }
        }
    }
    //Serial.print("Proj=");
    //Serial.println(result);
    return result;
}


int CommandBuilder::computeSpeedMillisCircular(float mmPerSec, double xEndPos, double yEndPos, bool clockwise, bool yStartUp, bool yEndUp){
    double mmX = this->computeStartEndPosDistanceLinear(xLastPos, xEndPos);
    double mmY = this->computeStartEndPosDistanceLinear(yLastPos, yEndPos);
    
    double radius = this->computeRadius();
    /*double xCenterPos = xLastPos + iOffset;
    double yCenterPos = yLastPos + jOffset;
    double mmEndX = this->computeStartEndPosDistanceLinear(xCenterPos, xEndPos);
    double mmEndY = this->computeStartEndPosDistanceLinear(yCenterPos, yEndPos);
    if(sqrt(pow(mmEndX, 2) + pow(mmEndY, 2)) != radius)
        return -1;*/
    
    double rope = sqrt(pow(mmX, 2) + pow(mmY, 2));
    double angle = (asin(rope / (double)(2 * radius)) * 180 / PI) * 2;
    double girth = PI * 2 * radius;
    double arch = (girth * angle) / 360.00;

    double xMmProjection = computeStartEndPosDistanceCircularProjection(xLastPos, xEndPos, iOffset, clockwise, yStartUp, yEndUp);
    if(xMmProjection > (radius * 2))
        arch = girth - arch;

    //Serial.print("Arch=");
    //Serial.println(arch);
    //return 1000 * (arch / mmPerSec);
    return computeSpeedMillis(mmPerSec, arch);
}

void CommandBuilder::buildSingleCircular(StepperCommand &command, double *startPos, double endPos, double offset, bool clockwise, bool startUp, bool endUp, int speedMillis){
    if(endPos > -DBL_MAX){
        int dir;
        if(endPos > *startPos || (endPos == *startPos && clockwise)) dir = GO_AHEAD;
        else dir = GO_BACK;
        double distance = this->computeStartEndPosDistanceCircularProjection(*startPos, endPos, offset, clockwise, startUp, endUp);
        if(distance > 0){
            //(int mmFromProjection, int mmRadius, int movementTimeMillis, int startSpeedPercentual, int initialDir, bool initialIncrising)
            double radius = this->computeRadius();
            command.startCircular(distance, radius, speedMillis, fabs((100 * offset) / radius), dir, (offset > 0 && clockwise && startUp) || 
                                                                                                    (offset > 0 && !clockwise && !startUp) || 
                                                                                                    (offset < 0 && clockwise && !startUp) || 
                                                                                                    (offset < 0 && !clockwise && startUp));
            *startPos = endPos;
        }
    }
}


double CommandBuilder::computeXYPercentageErrorMargin(){
    double xDistance = this->computeStartEndPosDistanceLinear(xLastPos, xPos);
    double yDistance = this->computeStartEndPosDistanceLinear(yLastPos, yPos);
    /*Serial.print("xDistance: ");
    Serial.println(xDistance);
    Serial.print("yDistance: ");
    Serial.println(yDistance);*/
    double mid = fabs(xDistance - yDistance) / 2.00;
    if(xDistance > yDistance && xDistance > MIN_DISTANCE && yDistance > MIN_DISTANCE){
        mid += yDistance;
        // mid : 100 = yDistance : x
        // result = 100 - x
        return 100.00 - (((100.00 * yDistance) / mid) * 1.55);
    } else if(xDistance < yDistance && xDistance > MIN_DISTANCE && yDistance > MIN_DISTANCE){
        mid += xDistance;
        return 100.00 - (((100.00 * xDistance) / mid) * 1.55);
    }
    return 0;
}

bool CommandBuilder::computeIsXExceeding(){
    double xDistance = this->computeStartEndPosDistanceLinear(xLastPos, xPos);
    double yDistance = this->computeStartEndPosDistanceLinear(yLastPos, yPos);
    return xDistance > yDistance;
}


int CommandBuilder::build(char stringCommand[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand){
    this->prepareContext(stringCommand);
    int speedMillis = 0;

    if(this->modeEq(this->mMode, M09)){
        
        this->buildSingleLinear(zCommand, &zLastPos, 0, this->computeSpeedMillis(STD_F_FAST, 10), 0, false);
    
    } else if(this->modeEq(this->mMode, M10)){
        
        this->buildSingleLinear(zCommand, &zLastPos, 10, this->computeSpeedMillis(STD_F_FAST, 10), 0, false);
    
    } else if(this->modeEq(this->gMode, G00)){
        
        //Serial.println("Mode G00");
        speedMillis = this->computeSpeedMillisLinear(STD_F_FAST, xPos, yPos, zPos);
        double percentageMarginError = this->computeXYPercentageErrorMargin();
        bool isXExceeding = this->computeIsXExceeding();
        this->buildSingleLinear(xCommand, &xLastPos, xPos, speedMillis, percentageMarginError, isXExceeding);
        this->buildSingleLinear(yCommand, &yLastPos, yPos, speedMillis, percentageMarginError, !isXExceeding);
        this->buildSingleLinear(zCommand, &zLastPos, zPos, speedMillis, 0, false);
    
    } else if(this->modeEq(this->gMode, G01)){
        
        //Serial.println("Mode G01");
        speedMillis = this->fMode > 0 ? this->computeSpeedMillisLinear(this->fMode, xPos, yPos, zPos) : this->computeSpeedMillisLinear(STD_F_SLOW, xPos, yPos, zPos);
        double percentageMarginError = this->computeXYPercentageErrorMargin();
        bool isXExceeding = this->computeIsXExceeding();
        //Serial.print("error: ");
        //Serial.println(percentageMarginError);
        this->buildSingleLinear(xCommand, &xLastPos, xPos, speedMillis, percentageMarginError, isXExceeding);
        this->buildSingleLinear(yCommand, &yLastPos, yPos, speedMillis, percentageMarginError, !isXExceeding);
        this->buildSingleLinear(zCommand, &zLastPos, zPos, speedMillis, 0, false);
    
    } else if(this->modeEq(this->gMode, G02)){
        //Serial.println("Mode G02");
        speedMillis = this->fMode > 0 ? this->computeSpeedMillisCircular(this->fMode, xPos, yPos, true, (iOffset > 0 && jOffset <= 0) || (iOffset <= 0 && jOffset < 0), yPos > (yLastPos + jOffset)) 
                                      : this->computeSpeedMillisCircular(STD_F_SLOW, xPos, yPos, true, (iOffset > 0 && jOffset <= 0) || (iOffset <= 0 && jOffset < 0), yPos > (yLastPos + jOffset));
        if(speedMillis > -1){
            double hereXLastPos = xLastPos;
            this->buildSingleCircular(xCommand, &xLastPos, xPos, iOffset, true, (iOffset > 0 && jOffset <= 0) || (iOffset <= 0 && jOffset < 0), yPos >= (yLastPos + jOffset), speedMillis);
            this->buildSingleCircular(yCommand, &yLastPos, yPos, jOffset, true, (jOffset > 0 && iOffset >= 0) || (jOffset <= 0 && iOffset > 0), xPos <= (hereXLastPos + iOffset), speedMillis);
        } else {
            Serial.println("Command not valid");
        }
    } else if(this->modeEq(this->gMode, G03)){
        Serial.println("Mode G03");
        speedMillis = this->fMode > 0 ? this->computeSpeedMillisCircular(this->fMode, xPos, yPos, false, (iOffset >= 0 && jOffset < 0) || (iOffset < 0 && jOffset <= 0), yPos >= (yLastPos + jOffset)) 
                                      : this->computeSpeedMillisCircular(STD_F_SLOW, xPos, yPos, false, (iOffset >= 0 && jOffset < 0) || (iOffset < 0 && jOffset <= 0), yPos >= (yLastPos + jOffset));
        if(speedMillis > -1){
            double hereXLastPos = xLastPos;
            this->buildSingleCircular(xCommand, &xLastPos, xPos, iOffset, false, (iOffset >= 0 && jOffset < 0) || (iOffset < 0 && jOffset <= 0), yPos >= (yLastPos + jOffset), speedMillis);
            this->buildSingleCircular(yCommand, &yLastPos, yPos, jOffset, false, (jOffset >= 0 && iOffset > 0) || (jOffset < 0 && iOffset >= 0), xPos <= (hereXLastPos + iOffset), speedMillis);
        } else {
            Serial.println("Command not valid");
        }
    } else if(this->modeEq(this->gMode, G28)){
        Serial.println("Mode G28");
        xCommand.startInfiniteLinear(STD_F_HOME, GO_BACK);
        yCommand.startInfiniteLinear(STD_F_HOME, GO_BACK);
        zCommand.startInfiniteLinear(STD_F_HOME, GO_BACK);
        xLastPos = yLastPos = zLastPos = 0;
    }
    /*Serial.print("Speed=");
    Serial.println(speedMillis);
    Serial.print("X=");
    Serial.println(xLastPos);
    Serial.print("Y=");
    Serial.println(yLastPos);
    Serial.print("Z=");
    Serial.println(zLastPos);
    Serial.print("F=");
    Serial.println(fMode);*/
    return speedMillis;
}
