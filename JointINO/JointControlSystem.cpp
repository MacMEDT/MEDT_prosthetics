#include "./JointControlSystem.h"

JointControlSystem::JointControlSystem()
{
    joints[THUMB] = ThumbJoint(THUMB);
    joints[INDEX] = IndexJoint(INDEX);
    joints[OTHER] = OtherDigJoint(OTHER);
}

void JointControlSystem::processSignals(float signal[]){
    //To Do later
};

void JointControlSystem::setGrip(float emg){
    //values from emg_real.txt generally 0.2-0.5
    if (emg>0 && emg<0.35){grip = GRIP_FLAT;} 
    else if (emg>=0.35){grip = GRIP_FIST;} 
    else{grip = GRIP_NONE;}
}

int JointControlSystem::getGrip()
{
    return grip;
};
