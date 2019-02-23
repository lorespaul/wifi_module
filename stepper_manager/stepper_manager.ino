#include "Stepper.h"
#include "CommandBuilder.h";

#define ZC '\0'
#define NL '\n'
#define CR '\r'
#define BUFFER_LENGTH 100

char readBuffer[BUFFER_LENGTH];

using namespace stepper_motor;

Stepper x(6, 7);
Stepper y(4, 5);
Stepper z(2, 3);

StepperCommand xCommand;
StepperCommand yCommand;
StepperCommand zCommand;

CommandBuilder commandBuilder;

void setup() {
    Serial.begin(115200);
    
    x.begin();
    y.begin();
    z.begin();
}

void loop() {
    if(allMotorFinishACommand()){
        if(readCommandFromSerial() > 0){
          commandBuilder.build(readBuffer, xCommand, yCommand, zCommand);
        }
    }

    x.makeStepAsync(xCommand);
    y.makeStepAsync(yCommand);
    z.makeStepAsync(zCommand);

}


bool allMotorFinishACommand(){
    return !xCommand.isInExecution() && !yCommand.isInExecution() && !zCommand.isInExecution();
}

int readCommandFromSerial(){
    if (Serial.available()){
        memset(readBuffer, ZC, BUFFER_LENGTH);
        int bytes = Serial.readBytesUntil(NL, readBuffer, BUFFER_LENGTH);
        if(bytes == 1 && (readBuffer[0] == NL || readBuffer[0] == CR))
            return 0;
        //G01 X563 Y7 Z34 F567
        Serial.println(readBuffer);
        return bytes;
    }
    return 0;
}
