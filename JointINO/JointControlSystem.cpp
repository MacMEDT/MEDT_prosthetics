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
    }
    else if (emg >= 0.35)
    {
        current_grip = GRIP_FIST;
    }
    else
    {
        current_grip = GRIP_NONE;
    }
    moveJoints();
};

void JointControlSystem::moveJoints()
{
    /*
    we would have to change this to be incremental,
    switching between the joints since right now it's only
    moving one all the way before starting the next

    would also have to eventually make them move in different
    ways to accomodate different grip types
    */
    int angle;
    for (int i = 0; i < NUM_JOINTS; i++)
    {
        if (current_grip == GRIP_FLAT)
            angle = 0;
        else if (current_grip == GRIP_FIST)
            angle = 90;
        joints[i].setAngle(angle);
    }
}

int JointControlSystem::getGrip()
{
    return current_grip;
};
