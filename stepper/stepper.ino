
const int stepsPerRevolution = 200;
// dir pin / step pin / direction / step
int motors[2][4] = {{6,7,LOW,LOW},{4,5,LOW,LOW}};
// command in execution / remaining steps
int motorsCommand[2][2] = {{0,0},{0,0}};
unsigned long motorsLastStep[2];
unsigned long motorsStepsInterval[2];

int penMotor[2] = {2,3};

bool loopEven = true;

void setup() {
  // initialize the serial port:
  Serial.begin(230400);

  pinMode(motors[0][0], OUTPUT);
  pinMode(motors[0][1], OUTPUT);
  pinMode(motors[1][0], OUTPUT);
  pinMode(motors[1][1], OUTPUT);
  pinMode(penMotor[0], OUTPUT);
  pinMode(penMotor[1], OUTPUT);

  digitalWrite(motors[0][1], LOW);
  digitalWrite(motors[1][1], LOW);
}

void loop() {
  
  if(allMotorFinishACommand()){
    loopEven = !loopEven;
    if(loopEven){
      liftPen();
    } else {
      lowerPen();
    }
    
    createCommand(0, 200, 930);    
    createCommand(1, 200, 300);

    invertRotation(0);
    invertRotation(1);
  }

  makeSteps(0);
  makeSteps(1);

}


void invertRotation(int motorPos){
  if(motors[motorPos][2] == LOW){
    motors[motorPos][2] = HIGH;
    digitalWrite(motors[motorPos][0], HIGH);
  } else if(motors[motorPos][2] == HIGH){
    motors[motorPos][2] = LOW;
    digitalWrite(motors[motorPos][0], LOW);
  }
}

void createCommand(int motorPos, int steps, int totalStepsTimeMillis){
  if(!isCommandInExecution(motorPos)){
    motorsCommand[motorPos][0] = 1;
    motorsCommand[motorPos][1] = steps;
    motorsLastStep[motorPos] = micros();
    motorsStepsInterval[motorPos] = 1000 * (totalStepsTimeMillis / (float)steps);
    //Serial.println("Step interval " + String(1000 * (totalStepsTimeMillis / (float)steps)) + " motor " + String(motorPos));
  }
}

void makeSteps(int motorPos){
  if(isCommandInExecution(motorPos)){
    
    unsigned long timestamp = micros();
    if(timestamp - motorsLastStep[motorPos] >= motorsStepsInterval[motorPos]){
      motorsLastStep[motorPos] = timestamp;
      
      if(motors[motorPos][3] == LOW){
        motors[motorPos][3] = HIGH;
        digitalWrite(motors[motorPos][1], HIGH);
        motorsCommand[motorPos][1]--; // step done
      } else if(motors[motorPos][3] == HIGH){
        motors[motorPos][3] = LOW;
        digitalWrite(motors[motorPos][1], LOW);
      }
      
      if(motorsCommand[motorPos][1] == 0){
        terminateCommand(motorPos);
        //Serial.println("Finish for motor: " + String(motorPos) + " in " + String(timestamp - motorsCommand[motorPos][4]));
      }
    } 
  }
}


bool allMotorFinishACommand(){
  return !isCommandInExecution(0) && !isCommandInExecution(1);
}

void terminateCommand(int motorPos){
  digitalWrite(motors[motorPos][1], LOW);
  motorsCommand[motorPos][0] = 0;
}

bool isCommandInExecution(int motorPos){
  return motorsCommand[motorPos][0] == 1;
}

void liftPen(){
  digitalWrite(penMotor[0], LOW);
  makeRevolution(penMotor[1]);
}

void lowerPen(){
  digitalWrite(penMotor[0], HIGH);
  makeRevolution(penMotor[1]);
}

void makeRevolution(int stepPin){
  for(int i=0;i<stepsPerRevolution;i++){
     digitalWrite(stepPin, HIGH);
     delayMicroseconds(500);
     digitalWrite(stepPin, LOW);
     delayMicroseconds(500);
  }
}
