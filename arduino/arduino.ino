#include <ArduinoJson.h>

// Analog pins connected to buttons
const int buttonPins[4] = {A5, A4, A3, A2};
bool buttonStates[4] = {false, false, false, false}; // Record button states
const int threshold = 100; // Set voltage threshold; below 100 is considered pressed
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

  String resTxt = "";
  serializeJson(resJson, resTxt);
  Serial.println(resTxt);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  // Initialize analog pins
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
  }

  pinMode(ledPin, OUTPUT); // Initialize LED pin
  digitalWrite(ledPin, HIGH); // Ensure LED is off at the start (HIGH = OFF for low-level trigger)
}

void loop() {
  bool updated = false; // Record whether any button state has changed

  for (int i = 0; i < 4; i++) {
    int analogValue = analogRead(buttonPins[i]); // Read voltage
    bool newState = analogValue > threshold; // Below threshold is considered pressed
    if (newState != buttonStates[i]) {
      buttonStates[i] = newState;
      updated = true;
    }
  }

  // If A2 button is pressed and we are not already waiting
  if (buttonStates[3] && !isWaiting && !isLedOn) { 
    eventStartTime = millis(); // Record the time when button was pressed
    isWaiting = true; // Start waiting period
  }

  // After 8 seconds, turn on the LED
  if (isWaiting && (millis() - eventStartTime >= waitDuration)) {
    digitalWrite(ledPin, LOW); // Turn on LED (LOW = ON for low-level trigger)
    isLedOn = true;
    isWaiting = false; // Stop waiting period
    eventStartTime = millis(); // Reset time for LED countdown
  }

  // After 5 seconds of LED being on, turn it off
  if (isLedOn && (millis() - eventStartTime >= ledDuration)) {
    digitalWrite(ledPin, HIGH); // Turn off LED (HIGH = OFF for low-level trigger)
    isLedOn = false;
  }

  // Send data only when button state changes
  if (updated) {
    sendData();
  }

  delay(10); // Prevent excessive data transmission
}

