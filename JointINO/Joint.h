#ifndef JOINT_H
#define JOINT_H

class Joint
{
public:
    Joint(int servoPin);

    // to read/write _angle
    float getAngle();
    void setAngle(float newAngle);
    float calculateAngle(float inputSignals[]);

protected:
    int _servoPin;
    float _angle;
};

// ---------------------
// Joint implementations
// ---------------------

class ThumbJoint : public Joint
{
public:
    ThumbJoint(int servoPin) : Joint(servoPin){};
};

class IndexJoint : public Joint
{
public:
    IndexJoint(int servoPin) : Joint(servoPin){};
};

class OtherDigJoint : public Joint
{
public:
    OtherDigJoint(int servoPin) : Joint(servoPin){};
};

class WristFlexJoint : public Joint
{
public:
    WristFlexJoint(int servoPin) : Joint(servoPin){};
};

class WristRotJoint : public Joint
{
public:
    WristRotJoint(int servoPin) : Joint(servoPin){};
};

#endif
