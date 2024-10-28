#define LED_PIN 4       // GPIO 4 is the onboard LED for ESP32-CAM
#define INPUT_PIN 13    // GPIO 13 as input pin for receiving signal from Arduino

void setup() {
  pinMode(LED_PIN, OUTPUT);      // Set LED pin as output
  pinMode(INPUT_PIN, INPUT);     // Set input pin for Arduino signal
}

void loop() {
  // Read the state of the input pin
  int state = digitalRead(INPUT_PIN);

  if (state == HIGH) {
    digitalWrite(LED_PIN, HIGH);  // Turn on the LED if input is HIGH
  } else {
    digitalWrite(LED_PIN, LOW);   // Turn off the LED if input is LOW
  }
}
