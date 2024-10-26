#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Servo.h>

// Struct for Command
struct Command {
  char action;
  int value;
};

// External variable declarations
extern char lastCommand;
extern unsigned long commandStartTime;
extern unsigned long lastDuration;

// Function prototypes
void executeCommand(char commandChar);
void checkForObstacles();
void checkServoPosition(Servo& servo, int servoNumber);
void grab();
void releaseServo();
void HitForward();
void HitBackward();
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void rotateClockwise();
void rotateAntiClockwise();
void moveRight(int duration);
void moveLeft(int duration);
void stopMotors();

#endif
