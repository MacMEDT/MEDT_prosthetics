#include "./Joint.h"

Joint::Joint(int servoPin)
{
    _servoPin = servoPin;
}

float Joint::getAngle() { return _angle; }

// Move this joint to the angle specified
void Joint::setAngle(float angle)
{
    _angle = angle;

    // TODO: add servo logic to control how this joint is moved
}

// Calculate the angle that this joint should move to, based on the EMG signals
float Joint::calculateAngle(float inputSignals[])
{
    // TODO: make this abstract? and in the child classes add calculations
    // on how much the joint should move based on the input signals
    return 0;
}
