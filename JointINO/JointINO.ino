#include "JointControlSystem.h"
#include "Joint.h"

JointControlSystem jcs;
float emgData[1];

int TEST_PIN = 9;
float received;

void setup() {
  Serial.begin(9600);
  jcs.joints[0].setPin(TEST_PIN);
}

void loop() {
  emgData[0] = 0.2;
  jcs.processSignals(emgData);
  emgData[0] = 0.4;
  jcs.processSignals(emgData);
}
