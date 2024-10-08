#include <ESP32Servo.h>

// Define motor pins
#define motorA_forward 23
#define motorA_backward 22
#define motorB_forward 19
#define motorB_backward 21
#define LED 2

// Define servo motor pins
#define servo1Pin 5   // Pin for Servo 1
#define servo2Pin 18  // Pin for Servo 2

// Define a struct to hold command and value
struct Command {
  char action;
  int value;  // Can be duration or angle for servos
};

// Array of commands with grab/release actions for servos and motor commands
Command commands[] = {
  { 'F', 1515 },  // Move Forward
  { 'L', 677 },   // Turn Left
  { 'F', 1301 },  // Move Forward
  { 'R', 727 },   // Turn Right
  { 'F', 995 },   // Move Forward
  { 'R', 740 },   // Turn Right
  { 'R', 740 },   // Turn Right
  { 'F', 1093 },  // Move Forward
  { 'R', 727 },   // Turn Right
  { 'F', 2117 },  // Move Forward
  { 'R', 727 },   // Turn Right
  { 'F', 1298 },  // Move Forward
  { 'C', 1492 },  // Rotate Clockwise
  { 'F', 2582 },  // Move Forward
  { 'C', 1492 },  // Rotate Clockwise
  { 'F', 1500 },  // Move Forward
  { 'R', 727 },   // Turn Right
  { 'F', 3233 },  // Move Forward
  // { 'L', 720 },   // Turn Left
  { 'L', 707 },   // Turn Left
  { 'F', 2461 }   // Move Forward
  // { 'H', 1000 },  // Hit Forward (doubled time)
  // { 'J', 1000 },
  // { 'G', 180 },   // Grab (servo angle = 180)
  // { 'S', 0 },     // Release (servo angle = 0)
  // { 'G', 180 },   // Grab (servo angle = 180)
  // { 'S', 0 },     // Release (servo angle = 0)

};

// Create Servo objects
Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(115200);  // Start serial communication

  // Set up motor pins
  pinMode(motorA_forward, OUTPUT);
  pinMode(motorA_backward, OUTPUT);
  pinMode(motorB_forward, OUTPUT);
  pinMode(motorB_backward, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED ,LOW);
  // Attach servos
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  // Check if servos are at zero position on start
  checkServoPosition(servo1, 1);
  checkServoPosition(servo2, 2);

  Serial.println("Both servos are at zero position.");
}

void loop() {
  // Execute each command in the array
  delay(1000);
  for (const Command& cmd : commands) {
    executeCommand(cmd.action, cmd.value);
  }

  // After executing all commands, stop the program
  while (true) {
    stopMotors();
    delay(1000);
  }
}

void executeCommand(char commandChar, int value) {
  switch (commandChar) {
    case 'G':
      grab(value);
      delay(5000);
      break;
    case 'S':
      releaseServo(value);
      delay(1000);
      break;
    case 'F':
      moveForward(value);
      delay(1500);
      break;
    case 'L':
      turnLeft(value);
      delay(1500);
      break;
    case 'R':
      turnRight(value);
      delay(1500);
      break;
    case 'C':
      rotateClockwise(value);
      delay(1500);
      break;
    case 'A':
      rotateAntiClockwise(value);
      delay(1500);
      break;
    case 'H':
      HitForward(value);
      delay(1500);
      break;
    case 'J':
      HitBackward(value);
      delay(1500);
      break;
    default:
      stopMotors();
      delay(1500);
      break;
  }
}

void checkServoPosition(Servo& servo, int servoNumber) {
  int currentPos = servo.read();  // Get current position of servo
  if (servoNumber == 1) {
    if (currentPos != 0) {
      Serial.println("Servo 1 is not at zero position, resetting to zero...");
      servo.write(0);  // Move to zero position if not already there
      delay(1000);     // Wait for servo to move
    } else {
      Serial.println("Servo 1 is already at zero position.");
    }
  } else if (servoNumber == 2) {
    if (currentPos != 180) {
      Serial.println("Servo 2 is not at zero position, resetting to zero...");
      servo.write(180);  // Move to inverted zero position if not already there
      delay(1000);       // Wait for servo to move
    } else {
      Serial.println("Servo 2 is already at zero position.");
    }
  }
}

void grab(int angle) {
  Serial.println("Grabbing...");
  servo1.write(angle);        // Grab with servo 1
  servo2.write(180 - angle);  // Inverted grab with servo 2
  delay(1000);                // Wait for the servos to move
  Serial.println("Grab complete.");
}

void releaseServo(int angle) {
  Serial.println("Releasing...");
  servo1.write(0);    // Release servo 1 to zero position
  servo2.write(180);  // Release servo 2 to its max (inverted zero)
  delay(1000);        // Wait for the servos to move
  Serial.println("Release complete.");
}

void HitForward(int duration) {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  delay(duration);  // Move forward for the specified duration
  stopMotors();     // Stop after moving forward
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  delay(duration);  // Move backward for the specified duration
  stopMotors();
}

void HitBackward(int duration) {
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  delay(duration);  // Move backward for the specified duration
  stopMotors();
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  delay(duration);  // Move forward for the specified duration
  stopMotors();     // Stop after moving forward
}

void moveForward(int duration) {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  delay(duration);  // Move forward for the specified duration
  stopMotors();     // Stop after moving forward
}

void turnLeft(int duration) {
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  delay(duration);  // Turn left for the specified duration
  stopMotors();     // Stop after turning left
}

void turnRight(int duration) {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  delay(duration);  // Turn right for the specified duration
  stopMotors();     // Stop after turning right
}

void rotateClockwise(int duration) {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  delay(duration);  // Rotate clockwise for the specified duration
  stopMotors();     // Stop after rotating clockwise
}

void rotateAntiClockwise(int duration) {
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  delay(duration);  // Rotate anti-clockwise for the specified duration
  stopMotors();     // Stop after rotating anti-clockwise
}

void stopMotors() {
  digitalWrite(motorA_forward, LOW);
  digitalWrite(motorA_backward, LOW);
  digitalWrite(motorB_forward, LOW);
  digitalWrite(motorB_backward, LOW);
}
