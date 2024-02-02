// #include <string> 
// using namespace std; 
// #include "./DefineJoints.cpp"
// #include <Servo.h> //**Need servo header file

// Servo servo1; //initializing a servo


// class index_finger: public DefineJoints{
//     void rotateJoint(int angle, int lower, int upper) override{ //Taking in a lower and upper bound, and taking it to the position if applicable
//         if (angle > upper) {
//             servo1.write(upper);
//         }
//         else if (angle < lower) {
//             servo1.write(lower);
//         }
//         else{
//             servo1.write(x);
//         }
//     }
//     int callName(String grip) override{
//         int indexPosition = 0;
//         if (grip == "fist"){ //Setting angles for the fist position, need to test these values
//             indexPosition = 90; 
//         }
//         if (grip == "open"){
//             indexPosition = 0;           
//         }
//         return indexPosition;
//     }
// };