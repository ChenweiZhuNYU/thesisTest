#include <ArduinoJson.h>

// 按钮连接的模拟引脚
const int buttonPins[4] = {A5, A4, A3, A2};
bool buttonStates[4] = {false, false, false, false}; // 记录按钮状态
const int threshold = 100; // 设定电压判断阈值，低于 500 视为按下

void sendData() {
  StaticJsonDocument<200> resJson;
  JsonObject data = resJson.createNestedObject("data");

  // 发送按钮状态
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

  // 初始化模拟引脚
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
  }
}

void loop() {
  bool updated = false; // 记录是否有按钮状态发生变化

  for (int i = 0; i < 4; i++) {
    int analogValue = analogRead(buttonPins[i]); // 读取电压
    bool newState = analogValue > threshold; // 低于阈值视为按下
    if (newState != buttonStates[i]) {
      buttonStates[i] = newState;
      updated = true;
    }
  }

  // 仅当按钮状态变化时发送数据
  if (updated) {
    sendData();
  }

  delay(10); // 防止数据过于频繁传输
}