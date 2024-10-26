#include <Servo.h>
#include "functions.h"

#define servo1Pin 6
#define servo2Pin 9
#define leftIR 7
#define rightIR 8
#define ledPin 2

Servo servo1;
Servo servo2;

// Define command array
Command commands[] = {
  { 'F', 2472 },
  { 'L', 150 },
  { 'F', 1724 },
  { 'R', 186 },
  { 'F', 434 },
  { 'G', 120 },
  { 'B', 928 },
  { 'L', 612 },
  { 'F', 674 },
  { 'L', 360 },
  { 'F', 1308 },
  { 'R', 809 },
  { 'F', 2461 },
  { 'R', 722 },
  { 'F', 1619 },
  { 'S', 0 },
};

void setup() {
  Serial.begin(9600);
  
  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(leftIR, LOW);
  digitalWrite(rightIR, LOW);
  
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  checkServoPosition(servo1, 1);
  checkServoPosition(servo2, 2);

  digitalWrite(ledPin, LOW);
}

void loop() {
  checkForObstacles();
  delay(3000);
}
