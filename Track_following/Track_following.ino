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
  // { 'F', 1655 },  // Move Forward
  // { 'L', 777 },   // Turn Left
  // { 'F', 1331 },  // Move Forward
  // { 'R', 800 },   // Turn Right
  // { 'F', 1195 },   // Move Forward
  // { 'H', 200 },
  // { 'R', 800 },   // Turn Right
  // { 'H', 200 },
  // { 'R', 800 },   // Turn Right
  // { 'F', 1600 },  // Move Forward // long
  // { 'R', 780 },   // Turn Right
  // { 'F', 2517 },  // Move Forward
  // { 'R', 750 },   // Turn Right
  // { 'F', 1400 },  // Move Forward
  // { 'R', 750 },   // Turn Right
  // { 'H', 200 },
  // { 'R', 750 },   // Turn Right
  // { 'R', 750 },   // Turn Right
  // { 'F', 1400 },  // Move Forward
  // { 'L', 750 },   // Turn Right
  // { 'C', 1552 },  // Rotate Clockwise
  // { 'F', 3100 },  // Move Forward
  // { 'H', 200 },
  // { 'C', 1552 },  // Rotate Clockwise
  // { 'F', 1550 },  // Move Forward
  // { 'R', 750 },   // Turn Rightn
  // { 'F', 3933 },  // Move Forward
  //{ 'L', 750 },   // Turn Left
  //{ 'F', 2461 },   // Move Forward
  
  /// Second PArt 
  { 'F', 752 },
  { 'R', 50 },
  { 'F', 1724 },
  { 'R', 186 },
  { 'F', 434 },
  { 'G', 80 },
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
  // Execute each command in the array
  delay(5000);
  // for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
  //   executeCommand(commands[i].action, commands[i].value);
  //   delay(1000);
  //   checkForObstacles();
  //   delay(1000);
  // }

  // // After executing all commands, stop the program
  // while (true) {
  //   stopMotors();
  //   delay(1000);
  // }

  checkForObstacles();
  delay(1000);
}
