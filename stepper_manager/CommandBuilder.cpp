//
//  CommandBuilder.cpp
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <Arduino.h>
#include "CommandBuilder.h"
#include "StepperCommand.h"


using namespace stepper_motor;

CommandBuilder::CommandBuilder(){
}

CommandBuilder::~CommandBuilder(){
}

void CommandBuilder::prepareContext(char stringCommand[]){
  char *token = strtok(stringCommand, SPACE);
  while(token != NULL){
    Serial.print(token[0]);
    Serial.print(" -> ");
    switch(token[0]){
      case G:
        strcpy(gMode, &token[1]);
        Serial.println(gMode);
        break;
      case F:
        fMode = atoi(&token[1]);
        Serial.println(fMode);
        break;
      case X:
        xPos = atoi(&token[1]);
        Serial.println(xPos);
        break;
      case Y:
        yPos = atoi(&token[1]);
        Serial.println(yPos);
        break;
      case Z:
        zPos = atoi(&token[1]);
        Serial.println(zPos);
        break;
      case I:
        iOffset = atoi(&token[1]);
        Serial.println(iOffset);
        break;
      case J:
        jOffset = atoi(&token[1]);
        Serial.println(jOffset);
        break;
    }
    token = strtok(NULL, SPACE);
  }
}

void CommandBuilder::build(char stringCommand[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand){
  this->prepareContext(stringCommand);
  Serial.println("----");
}
