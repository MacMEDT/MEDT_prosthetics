#ifndef JOINT_H
#define JOINT_H
#include <Servo.h>

class Joint
{
public:
    Joint();

    void setPin(int pin);
    float getAngle();
    void setAngle(float newAngle);
    float calculateAngle(float inputSignals[]);

protected:
    int _servoPin;
    float _angle;
    Servo servo;
};

// ---------------------
// Joint implementations
// ---------------------

class ThumbJoint : public Joint
{
public:
    ThumbJoint() : Joint(){};
};

class IndexJoint : public Joint
{
public:
    IndexJoint() : Joint(){};
};

class OtherDigJoint : public Joint
{
public:
    OtherDigJoint() : Joint(){};
};

#endif
