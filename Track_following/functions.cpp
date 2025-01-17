#include "functions.h"
#include <Arduino.h>

// Define motor pins, servo pins, IR pins, and other constants here, or use extern for variables from main
#define motorA_forward 12
#define motorA_backward 13
#define motorB_forward 10
#define motorB_backward 11
#define leftIR 7
#define rightIR 8
#define lineFollower 4
#define ledPin 2

#define StandardDealy 1000

extern Servo servo1;
extern Servo servo2;

void executeCommand(char commandChar, int value) {
  switch (commandChar) {
    case 'G':
      grab(value);
      delay(StandardDealy);
      break;
    case 'S':
      releaseServo(value);
      delay(StandardDealy);
      break;
    case 'F':
      moveForward(value);
      delay(StandardDealy);
      break;
    case 'B':
      moveBackward(value);
      delay(StandardDealy);
      break;

    case 'L':
      turnLeft(value);
      delay(StandardDealy);
      break;
    case 'R':
      turnRight(value);
      delay(StandardDealy);
      break;
    case 'C':
      rotateClockwise(value);
      delay(StandardDealy);
      break;
    case 'A':
      rotateAntiClockwise(value);
      delay(StandardDealy);
      break;
    case 'H':
      HitForward(value);
      delay(StandardDealy);
      break;
    case 'J':
      HitBackward(value);
      delay(StandardDealy);
      break;
    default:
      stopMotors();
      delay(StandardDealy);
      break;
  }
}

void followLine(char CurrentExecutingFunction) {
  Serial.println("Searching for the line...");
  int stepSize = 100;
  if (CurrentExecutingFunction == 'F') {
    // Try a small right adjustment, then move forward, then a small left adjustment
    moveRight(stepSize);  // Move a little to the right
    delay(100);
    if (digitalRead(lineFollower) == HIGH) {
      Serial.println("Line found after adjusting right.");
      return;
    }

    moveForward(stepSize);  // Move a little forward
    delay(100);
    if (digitalRead(lineFollower) == HIGH) {
      Serial.println("Line found after moving forward.");
      return;
    }

    moveLeft(stepSize);  // Move a little to the left
    delay(100);
    if (digitalRead(lineFollower) == HIGH) {
      Serial.println("Line found after adjusting left.");
      return;
    }

  } else if (CurrentExecutingFunction == 'B') {
    // Try a small left adjustment, then move backward, then a small right adjustment
    moveLeft(stepSize);  // Move a little to the left
    delay(100);
    if (digitalRead(lineFollower) == HIGH) {
      Serial.println("Line found after adjusting left.");
      return;
    }

    moveBackward(stepSize);  // Move a little backward
    delay(100);
    if (digitalRead(lineFollower) == HIGH) {
      Serial.println("Line found after moving backward.");
      return;
    }

    moveRight(stepSize);  // Move a little to the right
    delay(100);
    if (digitalRead(lineFollower) == HIGH) {
      Serial.println("Line found after adjusting right.");
      return;
    }

  } else {
    // Default search pattern if the function is not specifically 'F' or 'B'
    
    int moveCount = 2;

    for (int i = 0; i < moveCount; i++) {
      moveLeft(stepSize);
      delay(200);
      if (digitalRead(lineFollower) == HIGH) {
        Serial.println("Line found while moving left.");
        return;
      }
    }
    delay(500);
    moveRight(stepSize * moveCount);
    delay(500);
    if (digitalRead(lineFollower) == HIGH) {
      Serial.println("Line found while moving left.");
      return;
    }
    for (int i = 0; i < moveCount; i++) {
      moveRight(stepSize);
      delay(200);
      if (digitalRead(lineFollower) == HIGH) {
        Serial.println("Line found while moving right.");
        return;
      }
    }
    delay(500);
    moveLeft(stepSize * moveCount);
    if (digitalRead(lineFollower) == HIGH) {
      Serial.println("Line found while moving left.");
      return;
    }
    Serial.println("Line not found after searching.");
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

void grab(int angle) {
  Serial.println("Grabbing...");
  servo1.write(angle);
  servo2.write(180 - angle);
  delay(StandardDealy);
  Serial.println("Grab complete.");
  delay(2000);
}

void releaseServo(int angle) {
  Serial.println("Releasing...");
  servo1.write(0);
  servo2.write(180);
  delay(StandardDealy);
  Serial.println("Release complete.");
  delay(2000);
}

void HitForward(int duration) {
  // if (!checkBlackDetection()) {
  digitalWrite(ledPin, HIGH);
  delay(StandardDealy);
  digitalWrite(ledPin, LOW);
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  delay(duration);  // Move forward for the specified duration
  stopMotors();     // Stop after moving forward
  delay(StandardDealy - 500);
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  delay(duration);  // Move backward for the specified duration
  stopMotors();
  // } else {
  Serial.println("Hit Forward aborted: Black detected.");
  // }
}

void HitBackward(int duration) {
  // if (!checkBlackDetection()) {
  analogWrite(motorA_backward, 170);
  analogWrite(motorB_backward, 170);
  delay(duration);  // Move backward for the specified duration
  stopMotors();
  delay(500);
  analogWrite(motorA_forward, 170);
  analogWrite(motorB_forward, 170);
  delay(duration);  // Move forward for the specified duration
  stopMotors();     // Stop after moving forward
                    // } else {
  Serial.println("Hit Backward aborted: Black detected.");
  // }
}

void moveForward(int duration) {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  delay(duration);
  stopMotors();
}

void moveBackward(int duration) {
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  delay(duration);
  stopMotors();
}

void turnLeft(int duration) {
  digitalWrite(motorA_backward, HIGH);
  digitalWrite(motorB_forward, HIGH);
  delay(duration);
  stopMotors();
}

void turnRight(int duration) {
  digitalWrite(motorA_forward, HIGH);
  digitalWrite(motorB_backward, HIGH);
  delay(duration);
  stopMotors();
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
