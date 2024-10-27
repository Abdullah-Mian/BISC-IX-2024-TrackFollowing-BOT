#include "functions.h"
#include <Arduino.h>

// Define motor pins, servo pins, IR pins, and other constants here, or use extern for variables from main
#define motorA_forward 12
#define motorA_backward 13
#define motorB_forward 10
#define motorB_backward 11
#define leftIR 7
#define rightIR 8
#define ledPin 2

#define StandardDealy 1000

extern Servo servo1;
extern Servo servo2;


// External variables initialized
char lastCommand = '\0';
unsigned long commandStartTime = 0;
unsigned long lastDuration = 0;

void executeCommand(char commandChar) {
  stopMotors();  // Ensure motors are stopped before issuing a new command
  lastCommand = commandChar;
  commandStartTime = millis();  // Start timing the command

  switch (commandChar) {
    case 'G':
      grab();
      break;
    case 'T':
      releaseServo();
      break;
    case 'F':
      Serial.println("Moving Forward");
      moveForward();
      break;
    case 'H':
      Serial.println("Forward Hit");
      HitForward();
      break;
    case 'J':
      Serial.println("Forward Hit");
      HitBackward();
      break;
    case 'B':
      Serial.println("Moving Backward");
      moveBackward();
      break;
    case 'C':
      Serial.println("Rotating Clockwise");
      rotateClockwise();
      break;
    case 'A':
      Serial.println("Rotating Anti-Clockwise");
      rotateAntiClockwise();
      break;
    case 'L':
      Serial.println("Turning Left");
      turnLeft();
      break;
    case 'R':
      Serial.println("Turning Right");
      turnRight();
      break;
  }
}




void checkForObstacles() {
  int leftSensorValue = digitalRead(leftIR);
  int rightSensorValue = digitalRead(rightIR);
  Serial.print("Left IR Value: ");
  Serial.print(leftSensorValue);
  Serial.print(" | Right IR Value: ");
  Serial.println(rightSensorValue);

  if (leftSensorValue != HIGH) {
    Serial.println("Obstacle detected on the left!");
    moveRight(100);
    stopMotors();
  } else if (rightSensorValue != HIGH) {
    Serial.println("Obstacle detected on the right!");
    moveLeft(100);
    stopMotors();
  }
}

void checkServoPosition(Servo& servo, int servoNumber) {
  int currentPos = servo.read();
  if (servoNumber == 1) {
    if (currentPos != 0) {
      Serial.println("Servo 1 is not at zero position, resetting to zero...");
      servo.write(0);
      delay(StandardDealy);
    } else {
      Serial.println("Servo 1 is already at zero position.");
    }
  } else if (servoNumber == 2) {
    if (currentPos != 180) {
      Serial.println("Servo 2 is not at zero position, resetting to zero...");
      servo.write(180);
      delay(StandardDealy);
    } else {
      Serial.println("Servo 2 is already at zero position.");
    }
  }
}
void grab() {
  Serial.println("Grabbing...");
  servo1.write(120);        // Grab with servo 1
  servo2.write(180 - 120);  // Inverted grab with servo 2
  Serial.println("Grab complete.");
  delay(2000);
}

void releaseServo() {
  Serial.println("Releasing...");
  servo1.write(0);    // Release servo 1 to zero position
  servo2.write(180);  // Release servo 2 to its max (inverted zero)
  Serial.println("Release complete.");
  delay(2000);
}

void HitForward() {
  // if (!checkBlackDetection()) {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  stopMotors();
  delay(500);
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  stopMotors();
  // } else {
  Serial.println("Hit Forward aborted: Black detected.");
  // }
}

void HitBackward() {
  // if (!checkBlackDetection()) {
  analogWrite(motorA_backward, 170);
  analogWrite(motorB_backward, 170);
  stopMotors();
  delay(500);
  analogWrite(motorA_forward, 170);
  analogWrite(motorB_forward, 170);
  stopMotors();  // Stop after moving forward
                 // } else {
  Serial.println("Hit Backward aborted: Black detected.");
  // }
}

void moveForward() {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  digitalWrite(motorA_backward, LOW);
  digitalWrite(motorB_backward, LOW);
}

void moveBackward() {
  digitalWrite(motorB_backward, HIGH);
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorA_forward, LOW);
  digitalWrite(motorB_forward, LOW);
}

void rotateClockwise() {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  digitalWrite(motorA_backward, LOW);
  digitalWrite(motorB_forward, LOW);
}

void rotateAntiClockwise() {
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  digitalWrite(motorA_forward, LOW);
  digitalWrite(motorB_backward, LOW);
}

void turnLeft() {
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  digitalWrite(motorA_forward, LOW);
  digitalWrite(motorB_backward, LOW);
}

void turnRight() {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  digitalWrite(motorA_backward, LOW);
  digitalWrite(motorB_forward, LOW);
}


void moveRight(int duration) {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  delay(duration);
  stopMotors();
}

void moveLeft(int duration) {
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  delay(duration);
  stopMotors();
}

void stopMotors() {
  digitalWrite(motorA_forward, LOW);
  digitalWrite(motorA_backward, LOW);
  digitalWrite(motorB_forward, LOW);
  digitalWrite(motorB_backward, LOW);
}
