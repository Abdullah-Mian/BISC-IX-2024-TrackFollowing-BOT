#include <Servo.h>
#include "functions.h"

#define servo1Pin 6
#define servo2Pin 9
#define leftIR 7
#define rightIR 8
#define lineFollower 4
#define ledPin 2

Servo servo1;
Servo servo2;
char CurrentExecutingFunction;

Command commands[] = {
  { 'F', 1655 },  // Move Forward
  { 'L', 777 },   // Turn Left
  { 'F', 1331 },  // Move Forward
  { 'R', 800 },   // Turn Right
  { 'F', 1195 },   // Move Forward
  { 'H', 200 },
  { 'R', 800 },   // Turn Right
  { 'H', 200 },
  { 'R', 800 },   // Turn Right
  { 'F', 1600 },  // Move Forward // long
  { 'R', 780 },   // Turn Right
  { 'F', 2517 },  // Move Forward
  { 'R', 750 },   // Turn Right
  { 'F', 1400 },  // Move Forward
  { 'R', 750 },   // Turn Right
  { 'H', 200 },
  { 'R', 750 },   // Turn Right
  { 'R', 750 },   // Turn Right
  { 'F', 1400 },  // Move Forward
  { 'L', 750 },   // Turn Right
  { 'C', 1552 },  // Rotate Clockwise
  { 'F', 3100 },  // Move Forward
  { 'H', 200 },
  { 'C', 1552 },  // Rotate Clockwise
  { 'F', 1550 },  // Move Forward
  { 'R', 750 },   // Turn Rightn
  { 'F', 3933 },  // Move Forward
  { 'L', 750 },   // Turn Left
  { 'F', 2461 },   // Move Forward

  // // Second Part
  { 'D', 000 },  // To make it skip obsticle scheck fro next step
  { 'F', 600 },
  { 'D', 000 },  // To make it skip obsticle scheck fro next step
  { 'R', 150 },
  { 'D', 000 },  // To make it skip obsticle scheck fro next step
  { 'G', 90 },
  { 'D', 000 },  // To make it skip obsticle scheck fro next step
  { 'B', 568 },
  //
  { 'L', 854 },
  { 'F', 1887 },
  
  { 'R', 716 },
  { 'F', 2238 },
  { 'R', 740 },
  { 'F', 1705 },
  { 'S', 0 },
  { 'R', 740 },

  { 'B', 200 },
  { 'R', 805 },
  { 'F', 400 },
  { 'G', 90 },
  { 'B', 950 },
  { 'L', 805 },
  { 'F', 230 },
  { 'S', 0 },
  { 'B', 790 },

  { 'B', 1100 },
  { 'R', 771 },
  { 'F', 1882 },
  { 'L', 741 },
  { 'F', 420 },
  { 'G', 90 },
  { 'B', 420 },
  { 'L', 767 },
  { 'F', 1522 },
  { 'R', 684 },
  { 'F', 1764 },
  { 'S', 0 },
  { 'B', 1764 },

  { 'R', 771 },
  { 'F', 1582 },
  { 'L', 771 },
  { 'F', 940 },
  { 'G', 90 },
  { 'B', 950 },
  { 'L', 767 },
  { 'F', 1522 },
  { 'R', 684 },
  { 'F', 1864 },
  { 'S', 0 },
  { 'B', 2500 },

  /// Last Section
  { 'B', 750 },
  { 'R', 800 },
  { 'F', 1546, 'A' },
  //
  { 'R', 367 },
  { 'G', 120 },
  { 'L', 400 },
  { 'B', 3000, 'A' },
  { 'R', 800 },
  { 'B', 2000, 'A' },
  { 'L', 800 },
  { 'B', 750 },
  { 'S', 0 },
  { 'F', 750 },
  { 'R', 800 },
  { 'F', 2000, 'A' },

  
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
  delay(5000);
  bool skipNextObstacleCheck = false;

  for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    CurrentExecutingFunction = commands[i].action;

    // If detectLine is not present, execute as usual
    if (commands[i].detectLine == NULL) {
      executeCommand(commands[i].action, commands[i].value);
    }
    // Handle commands with detectLine
    else if (commands[i].detectLine != NULL) {
      switch (commands[i].detectLine) {
        case 'S':
          {
            // Monitor lineFollower and execute action until it goes LOW
            unsigned long startTime = millis();
            while ((millis() - startTime < commands[i].value) && digitalRead(lineFollower) == LOW) {
              Serial.println("Executing command...");
              unsigned long commandStartTime = millis();
              executeCommand(commands[i].action, 100);  // Execute in 10 ms chunks
              commands[i].value += 1000;
              Serial.print("Command executed in: ");
              Serial.println(millis() - commandStartTime);

              delay(10);
            }
            if (digitalRead(lineFollower) == HIGH) {
              Serial.println("Line detected, stopping execution.");
            } else {
              Serial.println("Completed execution without interruption.");
            }
            break;
          }

        case 'A':
          {
            unsigned long startTime = millis();

            while (millis() - startTime < commands[i].value) {
              Serial.print("Executing command: ");
              Serial.print(commands[i].action);
              Serial.print(" with value: ");
              Serial.println(commands[i].value);
              unsigned long commandStartTime = millis();
              executeCommand(commands[i].action, 400);  // Execute in 10 ms chunks
              commands[i].value += 1000;
              if (digitalRead(lineFollower) == LOW) {
                followLine(CurrentExecutingFunction);  // Call the line-following function
                commands[i].value += 1400;
              }
              delay(10);
            }
            break;
          }
        default:
          Serial.println("Unknown detectLine character");
          break;
      }
    }

    delay(1000);

    if (CurrentExecutingFunction == 'D') {
      skipNextObstacleCheck = true;
      continue;
    }
    if (!skipNextObstacleCheck) {
      checkForObstacles();
    }
    skipNextObstacleCheck = false;
    delay(1000);
  }

  while (true) {
    stopMotors();
    delay(10000);
  }
}
