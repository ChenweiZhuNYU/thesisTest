#include <ArduinoJson.h>

// Analog pins connected to buttons and light sensor
const int buttonPins[4] = {A5, A4, A3, A2};
bool buttonStates[4] = {false, false, false, false}; // Record button states
const int buttonThreshold = 100; // Button voltage threshold; below 100 is considered pressed
const int lightSensorThreshold = 400; // Light sensor threshold; above 1000 is considered pressed
const int ledPin = 13; // Define LED pin

unsigned long eventStartTime = 0; // Record the time when button is pressed
const unsigned long waitDuration = 8000; // Wait for 8 seconds before turning on LED
const unsigned long ledDuration = 5000; // LED stays on for 5 seconds
bool isWaiting = false; // Whether we are in the waiting period
bool isLedOn = false; // Whether the LED is currently on

void sendData() {
  StaticJsonDocument<200> resJson;
  JsonObject data = resJson.createNestedObject("data");

  // Send button states
  for (int i = 0; i < 4; i++) {
    data["button" + String(i + 1)] = buttonStates[i]; 
  }

  String resTxt;
  serializeJson(resJson, resTxt);
  Serial.println(resTxt);  // Ensure newline for p5.js compatibility
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  // Initialize analog pins
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
  }

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

void loop() {
  bool updated = false;

  for (int i = 0; i < 4; i++) {
    int analogValue = analogRead(buttonPins[i]);
    bool newState;
    if (i == 2) { // A3 is now a light sensor
      newState = analogValue > lightSensorThreshold;
    } else {
      newState = analogValue > buttonThreshold;
    }
    
    if (newState != buttonStates[i]) {
      buttonStates[i] = newState;
      updated = true;
    }
  }

  if (buttonStates[3] && !isWaiting && !isLedOn) {
    eventStartTime = millis();
    isWaiting = true;
  }

  if (isWaiting && (millis() - eventStartTime >= waitDuration)) {
    digitalWrite(ledPin, LOW);
    isLedOn = true;
    isWaiting = false;
    eventStartTime = millis();
  }

  if (isLedOn && (millis() - eventStartTime >= ledDuration)) {
    digitalWrite(ledPin, HIGH);
    isLedOn = false;
  }

  if (updated) {
    sendData();
  }

  delay(10);
}

