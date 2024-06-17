//#include <string.h> 
//using namespace std; 
//#include "./DefineJoints.cpp"
//#include <Servo.h> //**Need servo header file
//
//Servo servo2; //initializing a servo
//
//class thumb: public DefineJoints{
//    void rotateJoint(int angle, int lower, int upper) override{ //Taking in a lower and upper bound, and taking it to the position if applicable
//        if (angle > upper) {
//            servo2.write(upper);
//        }
//        else if (angle < lower) {
//            servo2.write(lower);
//        }
//        else{
//            servo2.write(x);
//        }
//    }
//    int callName(String grip) override{
//        int thumbPosition = 0;
//        if (grip == "fist"){ //Setting angles for the fist position, need to test these values
//            thumbPosition = 90; 
//        }
//        if (grip == "open"){
//            thumbPosition = 0;           
//        }
//        return thumbPosition;
//    }
//};
