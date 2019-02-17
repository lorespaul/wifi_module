
const int stepsPerRevolution = 200;
int motors[2][3] = {{6,7,LOW},{4,5,LOW}};
// command in execution / last step command millis / remaining steps
int motorsCommand[2][3] = {{0,0,0},{0,0,0}};
float motorsStepsInterval[2];

int penMotor[2] = {2,3};

bool loopEven = true;

void setup() {
  // initialize the serial port:
  Serial.begin(9600);

  pinMode(motors[0][0], OUTPUT);
  pinMode(motors[0][1], OUTPUT);
  pinMode(motors[1][0], OUTPUT);
  pinMode(motors[1][1], OUTPUT);
  pinMode(penMotor[0], OUTPUT);
  pinMode(penMotor[1], OUTPUT);
}

void loop() {
  /*makeRevolution(7, 5);
  makeRevolution(5, 8);
  makeRevolution(3, 9);*/
  if(allMotorFinishACommand()){
    loopEven = !loopEven;
    if(loopEven){
      liftPen();
    } else {
      lowerPen();
    }
    
    createCommand(0, 200, 800);    
    createCommand(1, 200, 1000);
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

void createCommand(int motorPos, int steps, int totalStepsTime){
  if(!isCommandInExecution(motorPos)){
    motorsCommand[motorPos][0] = 1;
    motorsCommand[motorPos][1] = millis();
    motorsCommand[motorPos][2] = steps;
    motorsStepsInterval[motorPos] = totalStepsTime / (float)steps;
    Serial.println("Step interval " + String(motorsStepsInterval[motorPos]) + " motor " + motorPos);
  }
}

void makeSteps(int motorPos){
  if(isCommandInExecution(motorPos)){

    digitalWrite(motors[motorPos][1], HIGH);
    
    int timestamp = millis();
    if((float)(timestamp - motorsCommand[motorPos][1]) > motorsStepsInterval[motorPos]){
      motorsCommand[motorPos][1] = timestamp;
      
      digitalWrite(motors[motorPos][1], LOW);
      
      motorsCommand[motorPos][2]--;
      
      if(motorsCommand[motorPos][2] == 0){
        finishCommand(motorPos);
        //Serial.println("Finish for motor: " + String(motorPos) + " in " + String(timestamp - motorsCommand[motorPos][4]));
      }
    } 
  }
}


bool allMotorFinishACommand(){
  return !isCommandInExecution(0) && !isCommandInExecution(1);
}

void finishCommand(int motorPos){
  motorsCommand[motorPos][0] = 0;
  //Serial.println("Command finish for motor " + String(motorPos));
}

bool isCommandInExecution(int motorPos){
  return motorsCommand[motorPos][0] == 1;
}

void liftPen(){
  digitalWrite(penMotor[0], LOW);
  makeRevolution(penMotor[1], 1);
}

void lowerPen(){
  digitalWrite(penMotor[0], HIGH);
  makeRevolution(penMotor[1], 1);
}

void makeRevolution(int stepPin, int delays){
  for(int i=0;i<stepsPerRevolution;i++){
     digitalWrite(stepPin, HIGH);
     delay(delays);
     digitalWrite(stepPin, LOW);
  }
}
