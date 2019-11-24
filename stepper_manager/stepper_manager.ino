#include "Stepper.h"
#include "CommandBuilder.h"

#define ZC '\0'
#define NL '\n'
#define CR '\r'
#define BUFFER_LENGTH 20
#define PRELOADED 50
#define GET_NEXT "GET_NEXT"
#define EXIT "EXIT"
#define SINGLE -1

#define SAFE_PIN 11

#define SERIAL_BAUD 230400

char readBuffer[BUFFER_LENGTH];
char preloadedCommands[PRELOADED][BUFFER_LENGTH];

int commandsMissingUntilNextPreload = 0;
int currentPreloaded = 0;

using namespace stepper_motor;

Stepper x(6, 7, 8);
Stepper y(4, 5, 9);
Stepper z(2, 3, 10);

StepperCommand xCommand;
StepperCommand yCommand;
StepperCommand zCommand;

CommandBuilder commandBuilder;

void setup() {
    Serial.begin(SERIAL_BAUD);

    pinMode(SAFE_PIN, INPUT);
    
    x.begin();
    y.begin();
    z.begin();

    Serial.println(GET_NEXT);
}

void loop() {

    if(digitalRead(SAFE_PIN) == HIGH){
        Serial.end();
        xCommand.forceStop();
        yCommand.forceStop();
        zCommand.forceStop();
        Serial.begin(SERIAL_BAUD);
    }
    
    if(allMotorsFinishACommand()){
        if(commandsMissingUntilNextPreload == 0){
            commandsMissingUntilNextPreload = preload();
        }

        if(commandsMissingUntilNextPreload == SINGLE){
            //Serial.print(readBuffer);
            //Serial.println("");
            commandBuilder.build(readBuffer, xCommand, yCommand, zCommand);
            commandsMissingUntilNextPreload = 0;
        }
        
        if(commandsMissingUntilNextPreload > 0){

            //Serial.print(preloadedCommands[currentPreloaded]);
            //Serial.println("");
            
            commandBuilder.build(preloadedCommands[currentPreloaded], xCommand, yCommand, zCommand);

            if(currentPreloaded == commandsMissingUntilNextPreload){
                commandsMissingUntilNextPreload = 0;
                currentPreloaded = 0;
            } else {
                currentPreloaded++;   
            }
        }
    }

    x.makeStepAsync(xCommand);
    y.makeStepAsync(yCommand);
    z.makeStepAsync(zCommand);

}


bool allMotorsFinishACommand(){
    return !xCommand.isInExecution() && !yCommand.isInExecution() && !zCommand.isInExecution();
}

int preload(){
    int counter = 0;
    while(counter < PRELOADED){
        if(readCommandFromSerial() > 0){
            Serial.println(GET_NEXT);
            if(readBuffer[0] == '-'){
                readBuffer[0] = ' ';
                return SINGLE;
            } else if(strncmp(readBuffer, EXIT, 4) == 0) {
                memset(readBuffer, ZC, BUFFER_LENGTH);
                return counter;
            }
            
            memset(preloadedCommands[counter], ZC, BUFFER_LENGTH);
            strncpy(preloadedCommands[counter], readBuffer, BUFFER_LENGTH);
            counter++;
        }
    }
    return counter;
}



int readCommandFromSerial(){
    if (Serial.available()){
        memset(readBuffer, ZC, BUFFER_LENGTH);
        int bytes = Serial.readBytesUntil(NL, readBuffer, BUFFER_LENGTH);
        if(bytes == 1 && (readBuffer[0] == NL || readBuffer[0] == CR))
            return 0;
        //G01 X563 Y7 Z34 F567
        //Serial.println(readBuffer);
        return bytes;
    }
    return 0;
}
