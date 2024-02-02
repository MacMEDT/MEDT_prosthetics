#include <Arduino.h>
#include <Servo.h>

#include "Joint.h"

Joint::Joint()
{
    _angle = 0;
}

void Joint::setPin(int pin)
{
    servo.attach(pin);
    _servoPin = pin;
}

float Joint::getAngle() { return _angle; }

// Move this joint to the angle specified
void Joint::setAngle(float angle)
{
    int direction = (angle > _angle) ? 1 : -1;

    for (int pos = _angle; pos != angle; pos += direction)
    {
        servo.write(pos);
        delay(25);
    }

    _angle = angle;
}

// Calculate the angle that this joint should move to, based on the EMG signals
float Joint::calculateAngle(float inputSignals[])
{
    // TODO: make this abstract? and in the child classes add calculations
    // on how much the joint should move based on the input signals
    return 0;
}
