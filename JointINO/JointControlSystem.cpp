#include "./JointControlSystem.h"

JointControlSystem::JointControlSystem()
{
    joints[THUMB] = ThumbJoint(THUMB);
    joints[INDEX] = IndexJoint(INDEX);
    joints[OTHER] = OtherDigJoint(OTHER);
    joints[WRIST_FLEX] = WristFlexJoint(WRIST_FLEX);
    joints[WRIST_ROT] = WristRotJoint(WRIST_ROT);
}

void JointControlSystem::processSignals(float signal[]){

};

int JointControlSystem::getGrip()
{
    // get angles of each joint
    float thumbAngle = joints[THUMB].getAngle();
    float indexAngle = joints[INDEX].getAngle();
    float otherDigitsAngle = joints[OTHER].getAngle();
    float wristFlexAngle = joints[WRIST_FLEX].getAngle();
    float wristRotAngle = joints[WRIST_ROT].getAngle();

    // define angle thresholds
    auto isFlat = [](float angle) -> bool
    {
        return angle == 0;
    };
    auto isCurled = [](float angle) -> bool
    {
        return 35 <= angle <= 40;
    };
    auto isClosed = [](float angle) -> bool
    {
        return 75 <= angle <= 80;
    };

    // define thresholds for each grip
    if (isFlat(wristRotAngle))
    {
        if (isFlat(wristFlexAngle))
        {
            if (isFlat(thumbAngle) && isClosed(indexAngle) && isClosed(otherDigitsAngle))
            {
                return GRIP_POWER;
            }
            if (isClosed(thumbAngle) && isClosed(indexAngle) && isClosed(otherDigitsAngle))
            {
                return GRIP_HOOK;
            }
            if (isFlat(thumbAngle) && isFlat(indexAngle) && isClosed(otherDigitsAngle))
            {
                return GRIP_POINTER;
            }
            if (isCurled(thumbAngle) && isCurled(indexAngle) && isCurled(otherDigitsAngle))
            {
                return GRIP_TRIPOD;
            }
            if (isFlat(thumbAngle) && isFlat(indexAngle) && isFlat(otherDigitsAngle))
            {
                return GRIP_MOUSE;
            }
        }
        else if (isClosed(wristFlexAngle))
        {
            if (isFlat(thumbAngle) && isFlat(indexAngle) && isFlat(otherDigitsAngle))
            {
                return GRIP_FLAT;
            }
        }
    }
    return GRIP_NONE;
};
