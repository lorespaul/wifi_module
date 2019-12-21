#include "Stepper.h"
#include "CommandBuilder.h"

#define BUFFER_LENGTH 50
#define PRELOADED 20
#define GET_NEXT "GET_NEXT"
#define EXIT "M02"
//#define SINGLE -1

#define SAFE_PIN 11

#define SERIAL_BAUD 230400

using namespace stepper_motor;

char readBuffer[BUFFER_LENGTH];
char preloadedCommands[PRELOADED][BUFFER_LENGTH];

int commandsMissingUntilNextPreload = 0;
int currentPreloaded = 0;

Stepper y(6, 7, 8);
Stepper x(4, 5, 9);
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
        if(xCommand.forceStop())
            x.reset();
        if(yCommand.forceStop())
            y.reset();
        if(zCommand.forceStop())
            z.reset();
        Serial.begin(SERIAL_BAUD);
    }
    
    if(allMotorsFinishACommand()){
        if(commandsMissingUntilNextPreload == 0){
            commandsMissingUntilNextPreload = preload();
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
    memset(preloadedCommands, ZC, BUFFER_LENGTH * PRELOADED);
    while(counter < PRELOADED){
        if(readCommandFromSerial() > 0){
            Serial.println(GET_NEXT);
            
            if(strncmp(readBuffer, EXIT, 3) == 0) {
                memset(readBuffer, ZC, BUFFER_LENGTH);
                return counter;
            }
            
            strncpy(preloadedCommands[counter], readBuffer, BUFFER_LENGTH - 1);
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
        //Serial.println(readBuffer);
        return bytes;
    }
    return 0;
}
