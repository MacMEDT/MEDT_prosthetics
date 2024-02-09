#include "archived_Joint.h"

int Joint::_all_angles[5] = {0,0,0,0,0};
int Joint::_all_signals[8] = {0,0,0,0,0,0,0,0};
int Joint::_select_pins[3] = {0,0,0}; //default selectpin values, there is a static method that allows them to be changed
int Joint::_signalin_pin = 0;
int Joint::_testsignalin_pin = 0;
int Joint::_default_speed = 20;
int Joint::_max_speed = 22;
int Joint::_min_speed = 10;

void Joint::init(int servo_pin, int default_angle, int flexthresh, int exthresh){
    _servo_pin = servo_pin;
#ifndef _DEBUG
    _servo.attach(_servo_pin);
#endif
    _default_angle = default_angle;

    //TODO: assign default angles to all angles, initial value is 0
    //setAngle(true); //set to default angle

    _flexthresh = flexthresh;
    _exthresh = exthresh;
}

void TestJoint::init(int servo_pin){
    _servo_pin = servo_pin;
#ifndef _DEBUG
    _servo.attach(_servo_pin);
#endif
}

void Joint::setupMultiplex(int pin, int selectPins[]){ //static method - performed once
    _signalin_pin = pin;
    /* //Whoever is taking over the MUX portion of the code, please look through the set-up below to ensure it is correct
    for (int i=0; i<3; i++) {
        _select_pins[i]=selectPins[i];
        pinMode(_select_pins[i], OUTPUT);
        digitalWrite(_select_pins[i], LOW);
    }

    pinMode(_analog_out_mux, INPUT); // Set up signal coming out of MUX as input to arduino
    */
    return;
}

void Joint::setupTestSignalIn(int pin){
    _testsignalin_pin = pin;
}

//Method to change default speed parameters and max/min
void Joint::changeSpeedVals(int def_speed, int max_speed, int min_speed){
    _default_speed=def_speed;
    _max_speed=max_speed;
    _min_speed=min_speed;
}

//Method to only change default speed
void Joint::changeSpeedVals(int def_speed){
    _default_speed=def_speed;
}


/* Return Grip State:  none = 0, power = 1, hook = 2, tripod = 3, mouse = 4, pointer = 5, flat = 6	*/
int Joint::getGripCode(){ //static method - performed as many times as necessary
    //For all of grips assign a coded value - using the angles (_all_angles) of the servos, return the value corresponding to the grip
/*takes _all_angles as an argument
 * _all_angles = [_indexjoint_index,_otherdigjoint_index,_thumbjoint_index,_wristrot_index,_wristflex_index]
 *  for fully curled finger: angle range = [finger_curl_min,finger_curl_max]
 *  for fully curled thumb: angle range = [thumb_curl_min,thumb_curl_max]
 *  for finger and thumb in tripod: angle range = [tripod_curl_min,tripod_curl_max]
 *  for flexed wrist: angle range = [wrist_flex_min,wrist_flex_max]
 */
	//TODO: update the degree of rotation		set the different ranges		return as 5 individual values 
	//			normalize the input as the system runs, tracking the sensor input for each respective joint
    int finger_curl_min = 75;
    int finger_curl_max = 80;
    int thumb_curl_min = 65;
    int thumb_curl_max = 75;
    int tripod_curl_min = 35;
    int tripod_curl_max = 40;
    int wrist_flex_min = 80;
    int wrist_flex_max = 85;

	int detectOffset = 5;

    //wrist is not rotated
    if(JA_WRISTROT <= 5)
    { 
        //wrist is unflexed
        if(JA_WRISTFLEX <= 5) { 
            //other digits are fully curled
            if(finger_curl_min <= JA_OTHER && JA_OTHER <= finger_curl_max) { 
                //index finger is fully curled
                if(finger_curl_min <= JA_INDEX && JA_INDEX <= finger_curl_max) 
                { 
					//TODO: the thumb state is a constant and never changing, by assigning it to a different value we can switch which grip is activated with the tumb
                    //thumb is fully curled
                    if(thumb_curl_min <= JA_THUMB && JA_THUMB <= thumb_curl_max) 
						return 2;
					// Checks if thumb is not curled
                    else if(JA_THUMB <= 5)  
						return 1;
                }
                // Checks if index and thumb are not curled
                else if(JA_INDEX <= 5 && JA_THUMB <= 5)
					return 5;
            }
            //other digits, thumb and index finger are in position for tripod grip
            else if(tripod_curl_min <= JA_OTHER && JA_OTHER <= tripod_curl_max && tripod_curl_min <= JA_INDEX && 
				JA_INDEX <= tripod_curl_max && tripod_curl_min <= JA_THUMB && JA_THUMB <= tripod_curl_max)
				return 3;
            //other digits, index finger and thumb are not curled
            else if(JA_OTHER <= 5 && JA_INDEX <= 5 && JA_THUMB <= 5)
				return 4;
        }
        //wrist is flexed		//index, thumb and other digits are not curled
        else if (wrist_flex_min <= JA_WRISTFLEX && JA_WRISTFLEX <= wrist_flex_max && JA_INDEX <= 5 && JA_OTHER <= 5 && JA_THUMB <= 5)
			return 6;
    }
	// hand grip is not recognized
    return 0; 
}

void Joint::updateSignals(){ //static methods - called on every loop
    //Using the select pins, loop through all items in the all_signals[] list and update to reflect new signal strength;
    return;
}

void Joint::simulateUpdateSignals(int data_array[]){
    //Function receives an array (text from serial communication has to be parsed into array format) to update all the signal values
    //Used for testing the other setAngle() code when you don't have access to sensors
    for (int i=0; i<8; i++) {
        _all_signals[i]=data_array[i];
    }
}

void Joint::updateTestSig(){
    //Updating signal array for testing purposes - uses single force sensor instead of multiplexed EMG
    int fsrreading = analogRead(_testsignalin_pin);
    _all_signals[0]=fsrreading;
}

int * Joint::getSignal(){
    static int copy_array[8];
    //Creates a shallow copy of array and returns pointer to it
    for (int i = 0; i < 8; ++i) {
      copy_array[i] = _all_signals[i];
   }

   return copy_array;
}

int Joint::getAngle(){
    return _curr_angle;
}
void Joint::calcAngle(int angleIndex) 
{
    int perceived_angle; //calculated angle -local to this function
    int calc_speed; //calculated speed -local to this function

	//TODO: fix -- all servo rotation calculations will be 0
	//TODO: Wait to hear back from ben on how we will rewrite the coefficients
	//	the coefficients need to be assigned to weigh the input values for the angle
	//		some muscles play a larger role than other movements do, the idea is to reduce the sensitivity of each signal 
	float a1, a2, a3, a4;
	a1 = a2 = a3 = a4 = 0.f;
	
	servo_rotation = a1 * _all_signals[_Flex_Carp_Rad_pin] + a2 * _all_signals[_Flex_Carp_Uln_pin] - a3 * _all_signals[_Ext_Carp_Rad_pin] - a4 * _all_signals[_Ext_Carp_Uln_pin]; //a# represents coefficient, testing needs to be done to determine sources of noise

    if (_all_signals[0] > _flexthresh) {
        perceived_angle = map(_all_signals[0], _flexthresh, 1023, 20, 160);
        calc_speed = _default_speed;
    }
    else {
        perceived_angle = _default_angle;
        calc_speed = _default_speed;
    }

    _curr_angle = perceived_angle;
    _all_angles[angleIndex] = _curr_angle;
}
void Joint::servoWriteAngle(bool setDefault, int angleIndex) 
{
    int perceived_angle; //calculated angle -local to this function
    int calc_speed; //calculated speed -local to this function

    if (setDefault) { //If setDefault is true, then will set to default angle at default speed
        perceived_angle = _default_angle;
        calc_speed = _default_speed;
    }
    else { //if setDefault is false, update based on what is stored in the calcAngle() array - may want to modify calc_speed
        perceived_angle = _all_angles[angleIndex];
        calc_speed = _default_speed;
    }
#ifndef _DEBUG
    _servo.write(perceived_angle, calc_speed);
#endif
}

void WristFlexJoint::calcAngle(){
    int perceived_angle; //calculated angle -local to this function
    int calc_speed; //calculated speed -local to this function
    float servo_rotation;

    //TODO: fix -- all servo rotation calculations will be 0
	//TODO: Wait to hear back from ben on how we will rewrite the coefficients
	//	the coefficients need to be assigned to weigh the input values for the angle
	//		some muscles play a larger role than other movements do, the idea is to reduce the sensitivity of each signal 
	float a1, a2, a3, a4;
    a1 = a2 = a3 = a4 = 0.f;

    servo_rotation = a1*_all_signals[_Flex_Carp_Rad_pin] + a2*_all_signals[_Flex_Carp_Uln_pin] - a3*_all_signals[_Ext_Carp_Rad_pin] - a4*_all_signals[_Ext_Carp_Uln_pin]; //a# represents coefficient, testing needs to be done to determine sources of noise
    if(servo_rotation >_flexthresh){
        perceived_angle=map(servo_rotation,_flexthresh,1023,_default_angle,180); //1023 was taken from sample
        calc_speed=_default_speed;
    }
    else if(servo_rotation < _exthresh){
        perceived_angle=map(servo_rotation,_flexthresh,1023,0,_default_angle); //1023 was taken from sample
        calc_speed=_default_speed;
    }
    else{
        perceived_angle=_default_angle;
        calc_speed=_default_speed;
    }
#ifndef _DEBUG
    _servo.write(perceived_angle,calc_speed);
#endif
}

void WristRotJoint::calcAngle(){
    int perceived_angle; //calculated angle -local to this function
    int calc_speed; //calculated speed -local to this function
    float servo_rotation;

    //TODO: fix -- all servo rotation calculations will be 0
    float a1, a2, a3, a4;
    a1 = a2 = a3 = a4 = 0.f;
    //Insert your calculations here - Servo_rotation seperated by abduction and adduction
    servo_rotation = a1*_all_signals[_Flex_Carp_Rad_pin] + a2*_all_signals[_Ext_Carp_Rad_pin] - a3*_all_signals[_Flex_Carp_Uln_pin] - a4*_all_signals[_Ext_Carp_Uln_pin]; //a# represents coefficient, testing needs to be done to determine sources of noise
    if(servo_rotation >_flexthresh){
        perceived_angle=map(servo_rotation,_flexthresh,1023,_default_angle,180); //1023 was taken from sample
        calc_speed=_default_speed;
    }
    else if(servo_rotation < _exthresh){
        perceived_angle=map(servo_rotation,_flexthresh,1023,0,_default_angle); //1023 was taken from sample
        calc_speed=_default_speed;
    }
    else{
        perceived_angle=_default_angle;
        calc_speed=_default_speed;
    }

    _curr_angle = perceived_angle;
    JA_WRISTROT = _curr_angle;
}

void TestJoint::calcAngle(){
    int perceived_angle;
    float servo_rotation = _all_signals[0];

    perceived_angle=map(_all_signals[0],0,1023,1,179);

    _curr_angle = perceived_angle;
    //_all_angles[_angle_index] = _curr_angle; not relevant for testjoint
}

void TestJoint::setAngle(bool setDefault){
#ifndef _DEBUG
    _servo.write(_curr_angle,_default_speed);
#endif
}
