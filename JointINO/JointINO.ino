#include "JointControlSystem.h"
#include "Joint.h"

//JointControlSystem jcs;
Joint j;

void setup() {
  Serial.begin(9600);
  j.setPin(9);

}

void loop() {
  j.setAngle(90);
  j.setAngle(0);
}
