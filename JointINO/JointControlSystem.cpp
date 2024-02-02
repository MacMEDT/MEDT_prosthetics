#include "JointControlSystem.h"
#include <Arduino.h>

JointControlSystem::JointControlSystem() {}

void JointControlSystem::processSignals(float signal[])
{
    // TODO: only using signal[0] for now
    float emg = signal[0];

    // values from emg_real.txt generally 0.2-0.5
    if (emg > 0 && emg < 0.35)
    {
        current_grip = GRIP_FLAT;
        joints[INDEX].setAngle(0);
    }
    else if (emg >= 0.35)
    {
        current_grip = GRIP_FIST;
        joints[INDEX].setAngle(90);
    }
    else
    {
        current_grip = GRIP_NONE;
    }
};

int JointControlSystem::getGrip()
{
    return current_grip;
};
