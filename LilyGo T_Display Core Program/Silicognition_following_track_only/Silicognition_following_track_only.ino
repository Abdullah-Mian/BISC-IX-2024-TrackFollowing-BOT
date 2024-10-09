#include <Servo.h>

// Define TCS3200 pins
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 3
#define ledPin 2  // Pin to control the LED on the sensor

// Variables to store color readings
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// Thresholds for detecting black color
const int blackThreshold = 700;  // Set an appropriate threshold

// Define motor pins
#define motorA_forward 13
#define motorA_backward 12
#define motorB_forward 10
#define motorB_backward 11

// Define servo motor pins
#define servo1Pin 8  // Pin for Servo 1
#define servo2Pin 9  // Pin for Servo 2

// Define a struct to hold command and value
struct Command {
  char action;
  int value;  // Can be duration or angle for servos
};

// Array of commands with grab/release actions for servos and motor commands
Command commands[] = {
  // { 'J', 1000 },

  { 'F', 1655 },  // Move Forward
  { 'L', 777 },   // Turn Left
  { 'F', 1231 },  // Move Forward
  { 'R', 800 },   // Turn Right
  { 'F', 1195 },   // Move Forward
  { 'H', 500 },
  { 'R', 800 },   // Turn Right
  { 'H', 500 },
  { 'R', 800 },   // Turn Right
  { 'F', 1250 },  // Move Forward // long
  { 'R', 820 },   // Turn Right
  { 'F', 2217 },  // Move Forward
  { 'R', 650 },   // Turn Right
  { 'F', 900 },  // Move Forward
  { 'H', 500 },
  { 'C', 1502 },  // Rotate Clockwise
  { 'F', 2600 },  // Move Forward
  { 'H', 500 },
  { 'C', 1502 },  // Rotate Clockwise
  { 'F', 1250 },  // Move Forward
  { 'R', 700 },   // Turn Right
  { 'F', 3533 },  // Move Forward
  { 'L', 700 },   // Turn Left
  { 'F', 2461 },   // Move Forward
  { 'G', 190 },  // Grab (servo angle = 190)
  { 'S', 0 },    // Release (servo angle = 0)

};

// Create Servo objects
Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(9600);  // Start serial communication

  // Set up motor pins
  pinMode(motorA_forward, OUTPUT);
  pinMode(motorA_backward, OUTPUT);
  pinMode(motorB_forward, OUTPUT);
  pinMode(motorB_backward, OUTPUT);

  // Attach servos
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  // Check if servos are at zero position on start
  checkServoPosition(servo1, 1);
  checkServoPosition(servo2, 2);

  Serial.println("Both servos are at zero position.");

  // Set pin modes for TCS3200
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(ledPin, OUTPUT);  // Set LED pin as OUTPUT

  // Turn on the sensor LED
  digitalWrite(ledPin, LOW);

  // Set frequency scaling to 20% (S0 HIGH, S1 LOW)
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

void loop() {
  // Execute each command in the array
  delay(5000);
  for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    executeCommand(commands[i].action, commands[i].value);
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
  delay(2000);
}

void releaseServo(int angle) {
  Serial.println("Releasing...");
  servo1.write(0);    // Release servo 1 to zero position
  servo2.write(180);  // Release servo 2 to its max (inverted zero)
  delay(1000);        // Wait for the servos to move
  Serial.println("Release complete.");
  delay(2000);
}

void HitForward(int duration) {
  if (!checkBlackDetection()) {
    digitalWrite(motorA_forward, HIGH);
    digitalWrite(motorB_forward, HIGH);
    delay(duration);  // Move forward for the specified duration
    stopMotors();  // Stop after moving forward
    delay(500);
    digitalWrite(motorA_backward, HIGH);
    digitalWrite(motorB_backward, HIGH);
    delay(duration);  // Move backward for the specified duration
    stopMotors();
  } else {
    Serial.println("Hit Forward aborted: Black detected.");
  }
}

void HitBackward(int duration) {
  if (!checkBlackDetection()) {
    analogWrite(motorA_backward, 170);
    analogWrite(motorB_backward, 170);
    delay(duration);  // Move backward for the specified duration
    stopMotors();
    delay(500);
    analogWrite(motorA_forward, 170);
    analogWrite(motorB_forward, 170);
    delay(duration);  // Move forward for the specified duration
    stopMotors();     // Stop after moving forward
  } else {
    Serial.println("Hit Backward aborted: Black detected.");
  }
}


bool checkBlackDetection() {
  digitalWrite(ledPin, HIGH);  // Turn on LED for better detection
  int blackCount = 0;

  for (int i = 0; i < 3; i++) {
    readColors();  // Read the colors
    if (!(redFrequency < blackThreshold && greenFrequency < blackThreshold && blueFrequency < blackThreshold)) {
      blackCount++;
    }
    delay(500);  // Delay between checks
  }

  digitalWrite(ledPin, LOW);  // Turn off LED after detection
  if (blackCount >= 2) {
    Serial.print("black detected");
  } else {
    Serial.print("not detected");
  }

  return (blackCount >= 2);  // If black is detected in at least 2 out of 3 checks
}


// Function to read color components
void readColors() {
  // Read Red component
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redFrequency = pulseIn(sensorOut, LOW);

  // Read Blue component
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueFrequency = pulseIn(sensorOut, LOW);

  // Read Green component
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenFrequency = pulseIn(sensorOut, LOW);

  // Print color values for debugging
  Serial.print("R = ");
  Serial.print(redFrequency);
  Serial.print(" | G = ");
  Serial.print(greenFrequency);
  Serial.print(" | B = ");
  Serial.println(blueFrequency);
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