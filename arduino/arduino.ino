#include <ArduinoJson.h>

// Analog pins connected to buttons
const int buttonPins[4] = {A5, A4, A3, A2};
bool buttonStates[4] = {false, false, false, false}; // Record button states
const int threshold = 100; // Set voltage threshold; below 100 is considered pressed

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

  // Send data only when button state changes
  if (updated) {
    sendData();
  }

  delay(10); // Prevent excessive data transmission
}