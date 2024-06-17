#include "Joint.h"

#define NUM_JOINTS 1

class JointControlSystem
{

public:
    enum gripType
    {
        GRIP_NONE,
        GRIP_FLAT,
        GRIP_FIST,
    };

    enum jointIndex
    {
        INDEX,
        THUMB,
        OTHER,
    };

    int current_grip;
    Joint joints[NUM_JOINTS]; // array of defined joints

    JointControlSystem();

    void processSignals(float signal[]);
    void moveJoints();
    int getGrip();
};