#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Servo.h>

// Struct for Command
struct Command {
  char action;
  int value;
};

// Function prototypes
void executeCommand(char commandChar, int value);
void checkForObstacles();
void checkServoPosition(Servo& servo, int servoNumber);
void grab(int angle);
void releaseServo(int angle);
void HitForward(int duration);
void HitBackward(int duration);
void moveForward(int duration);
void moveBackward(int duration);
void turnLeft(int duration);
void turnRight(int duration);
void moveRight(int duration);
void moveLeft(int duration);
void rotateClockwise(int duration);
void rotateAntiClockwise(int duration);
void stopMotors();

#endif
