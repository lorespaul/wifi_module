#include "Stepper.h"
#include "CommandBuilder.h";

#define ZC '\0'
#define NL '\n'
#define CR '\r'
#define BUFFER_LENGTH 100

#define SAFE_PIN 11

#define SERIAL_BAUD 230400

char readBuffer[BUFFER_LENGTH];

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
}

void loop() {

    if(digitalRead(SAFE_PIN) == HIGH){
        Serial.end();
        xCommand.forceStop();
        yCommand.forceStop();
        zCommand.forceStop();
        Serial.begin(SERIAL_BAUD);
    }
    
    if(allMotorFinishACommand()){
        if(readCommandFromSerial() > 0){
            int commandTime = commandBuilder.build(readBuffer, xCommand, yCommand, zCommand);
            Serial.print("CommandTime=");
            Serial.println(commandTime);
            Serial.println("------------");
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
