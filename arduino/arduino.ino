#include <ArduinoJson.h>

// Analog pins connected to buttons and sensors
const int buttonPins[4] = {A5, A4, A3, A2};
bool buttonStates[4] = {false, false, false, false}; // Record button states
const int buttonThreshold = 100; // Button voltage threshold; below 100 is considered pressed
const int lightSensorThreshold = 400; // Light sensor threshold; above 400 is considered pressed
const int tiltSensorThreshold = 30; // Tilt sensor threshold; above 3500 is considered pressed
const int pressureSensorThreshold = 300; // Pressure sensor threshold; above 3700 is considered pressed
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
      //Serial.print("Light sensor value (A3): ");
      //Serial.println(analogValue);
      newState = analogValue > lightSensorThreshold;
    } else if (i == 1) { // A4 is now a tilt sensor
      //Serial.print("Tilt sensor value (A4): ");
      //Serial.println(analogValue);
      newState = analogValue > tiltSensorThreshold;
    } else if (i == 0) { // A5 is now a pressure sensor
      //Serial.print("Pressure sensor value (A5): ");
      //Serial.println(analogValue);
      newState = analogValue > pressureSensorThreshold;
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

