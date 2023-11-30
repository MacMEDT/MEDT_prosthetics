#include <string> 
using namespace std; 

class DefineJoints{
public:
    virtual void rotateJoint(int angle, int lower, int upper) = 0; //Generalized methods in order to rotate the respective joints with a specific grip as string input
    virtual int callName(String grip) = 0;
};