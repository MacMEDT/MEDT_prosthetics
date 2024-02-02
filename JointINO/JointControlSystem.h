#include "Joint.h"

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
    Joint joints[3]; // array of defined joints

    JointControlSystem();

    void processSignals(float signal[]);
    int getGrip();
};