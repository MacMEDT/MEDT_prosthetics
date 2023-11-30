#include <string> 
using namespace std; 
#include "./DefineJoints.cpp"
#include <Servo.h> //**Need servo header file

Servo servo3; //initializing a servo

class remaining_three: public DefineJoints{
    void rotateJoint(int angle, int lower, int upper) override{ //Taking in a lower and upper bound, and taking it to the position if applicable
        if (angle > upper) {
            servo3.write(upper);
        }
        else if (angle < lower) {
            servo3.write(lower);
        }
        else{
            servo3.write(x);
        }
    }
    int callName(String grip) override{
        int remainingPosition = 0;
        if (grip == "fist"){ //Setting angles for the fist position, need to test these values
            remainingPosition = 90; 
        }
        if (grip == "open"){
            remainingPosition = 0;           
        }
        return indexPosition;
    }
};