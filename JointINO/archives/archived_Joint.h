#ifndef Joint_h
#define _DEBUG 1
#define Joint_h
#ifndef _DEBUG
#include <Arduino.h>
#include <VarSpeedServo.h>
#endif // !_DEBUG


class Joint { //abstract parent class - getAngle() is an overridden method whose calcs are unique to each subclass

#ifdef _DEBUG    
public:
    static int analogRead(int val) { return 0; };
    static int map(int val1, int val2, int val3, int val4, int val5) { return 0; };
#endif
protected: //all these variables+methods are accesible to objects of subclass
#ifndef _DEBUG
    VarSpeedServo _servo;
#endif

    ///joint indices
#define J_TEST       0
#define J_INDEX      0
#define J_OTHER      1
#define J_THUMB      2
#define J_WRISTROT   3
#define J_WRISTFLEX  4

    int _servo_pin;
    int _curr_angle;
    int _default_angle;
    int _exthresh;
    int _flexthresh;


    static int _default_speed; // should this be static (same for all joints) or specific?
    static int _max_speed;
    static int _min_speed;

    static int _select_pins[3]; //these pins are used to select which signal comes in from the MUX;
    static int _signalin_pin; //Pin that goes into arduino, out of multiplexer
    static int _testsignalin_pin;

    static int _all_signals[8]; //index of signal corresponds to muxpin index (might be less than 8)
    //list of all muscles - rename sig*_muxpin to corresponding muscle location name
    //enum automatically assigns int values in order of appearance, ie _sig0_muxpin will have value 0
	//wrist		//0, 1, 2		//3
	enum { _Flex_Carp_Rad_pin, _Flex_Carp_Uln_pin, _Flex_Dig_pin, _Ext_Carp_Rad_pin, _Ext_Carp_Uln_pin, _Ext_Dig_pin, _Ext_Min_pin, _Ext_Ind_pin };
	
	
    static int _all_angles[5]; //stores angles of all servos - respectives indices declared below ie _indexjoint_index is 0, otherdig is 1 etc.

#define JA_TEST       _all_angles[J_TEST]
#define JA_INDEX      _all_angles[J_INDEX]
#define JA_OTHER      _all_angles[J_OTHER]
#define JA_THUMB      _all_angles[J_THUMB]
#define JA_WRISTROT   _all_angles[J_WRISTROT]
#define JA_WRISTFLEX  _all_angles[J_WRISTFLEX]

public: //these methods will be accessible in arduino sketch
    ///empty constructor, called outside setup (don't want to attach any pins outside setup)
    Joint() : 
        _curr_angle(0),
        _default_angle(0),
        _exthresh(0),
        _flexthresh(0),
        _servo_pin(0)
    {}; 
    ~Joint() {};
    void setDefaultAngle(int angle) { _default_angle = angle; }
    void setCurrentAngle(int angle) { _curr_angle = angle; }

    void init(int servo_pin, int default_angle, int flexthresh, int exthresh); //call inside setup() to initialize variables

    void calcAngle(int angleIndex); //Calculates the angle based off of signals, inherited by subclasses (updates stored angle for further possible manipuations)
    
    void servoWriteAngle(bool setDefault, int angleIndex); //empty method inherited by subclasses - sets the angles based on angle array if setDefault==0

    int getAngle(); //returns current angle of servo

    static int getGripCode(); //returns code corresponding to a specific grip
    //TODO
    static void updateSignals(); //this code will loop through all input pins of the multiplexer to update analog signal vals from all EMG
    //TODO validate
    static void simulateUpdateSignals(int data_array[]); //update signals array based on input from text file

    static void updateTestSig(); //updates first element of signals array using force sensor data (to test functionality)

    static int * getSignal(); //returns shallow copy of signals array

    static void changeSpeedVals(int def_speed, int max_speed, int min_speed); //update the speed parameters

    static void changeSpeedVals(int def_speed);
    //TODO validate
    static void setupMultiplex(int pin, int selectPins[]); //call in in setup(), specifies input and output pins

    static void setupTestSignalIn(int pin); //
};

class IndexJoint : public Joint{
public:
    void setAngle(bool setDefault) { Joint::servoWriteAngle(setDefault, J_INDEX); };
    void calcAngle() { Joint::calcAngle(J_INDEX); };
};
class OtherDigJoint: public Joint{
public:
    void setAngle(bool setDefault) { Joint::servoWriteAngle(setDefault, J_OTHER); };
    void calcAngle() { Joint::calcAngle(J_OTHER); };
};
class ThumbJoint: public Joint{
public:
    void setAngle(bool setDefault) { Joint::servoWriteAngle(setDefault, J_THUMB); };
    void calcAngle() { Joint::calcAngle(J_THUMB); };
};
class WristFlexJoint: public Joint{
public:
    void setAngle(bool setDefault) { Joint::servoWriteAngle(setDefault, J_WRISTFLEX); };
    void calcAngle();
};
class WristRotJoint: public Joint{
public:
    void setAngle(bool setDefault) { Joint::servoWriteAngle(setDefault, J_WRISTROT); };
    void calcAngle();
};
class TestJoint: public Joint{
public:
    void init(int servo_pin);
    void setAngle(bool setDefault);
    void calcAngle();
};
#endif
