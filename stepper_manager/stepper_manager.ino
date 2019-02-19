#include "Stepper.h"

#define CR '\r'
#define BUFFER_LENGTH 100

char readBuffer[BUFFER_LENGTH];

using namespace stepper_motor;

Stepper x(6, 7);
Stepper y(4, 5);
Stepper z(2, 3, true);

StepperCommand xCommand;
StepperCommand yCommand;

void setup() {
  Serial.begin(115200);

  x.begin();
  y.begin();
  z.begin();
}

void loop() {
  
  if(allMotorFinishACommand()){
    z.makeRevolution();
    z.invertRotation();
    
    xCommand.start(24, 1000);    
    yCommand.start(50, 1000);

    x.invertRotation();
    y.invertRotation();
  }

  x.makeStepAsync(xCommand);
  y.makeStepAsync(yCommand);

}


bool allMotorFinishACommand(){
  return !xCommand.isCommandInExecution() && !yCommand.isCommandInExecution();
}

void readCommandFromSerial(){
  if (Serial.available()){
    int bytes = Serial.readBytesUntil(CR, readBuffer, BUFFER_LENGTH);
    if(readBuffer[bytes-1] == '\n')
      readBuffer[bytes-1] = CR;
    readBuffer[bytes] = CR;
  }
}
