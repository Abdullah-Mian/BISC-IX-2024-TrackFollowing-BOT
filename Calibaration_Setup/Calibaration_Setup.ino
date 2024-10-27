#include <Servo.h>
#include "functions.h"

#define servo1Pin 6
#define servo2Pin 9
#define leftIR 7
#define rightIR 8
#define ledPin 2

Servo servo1;
Servo servo2;

// Declare and Initialize command array
Command commands[] = {
  { 'F', 3533 },  // Move Forward
  // { 'L', 700 },   // Turn Left
  // { 'G', 80 },  // Grab (servo angle = 190)
  // { 'S', 0 },    // Release (servo angle = 0)
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
  Serial.println("Both servos are at zero position.");

  // Colour Sensor LED
  digitalWrite(ledPin, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    char commandChar = Serial.read();
    commandChar = toupper(commandChar);  // Normalize input to uppercase

    if (commandChar == 'S') {
      // Stop motors and print last command and duration
      stopMotors();
      unsigned long runTime = millis() - commandStartTime;
      Serial.print("Last command: ");
      Serial.print(lastCommand);
      Serial.print(", Run time: ");
      Serial.print(runTime);
      Serial.println(" ms");

      // Reset command tracking
      lastCommand = '\0';
      commandStartTime = 0;
      lastDuration = runTime;
    } else if (commandChar == 'F' || commandChar == 'B' || commandChar == 'L' || commandChar == 'R' || commandChar == 'C' || commandChar == 'A' || commandChar == 'G' || commandChar == 'T' || commandChar == 'H' || commandChar == 'J') {
      // Execute command
      executeCommand(commandChar);
    }
  }

  // checkForObstacles();
  // delay(1000);
}
