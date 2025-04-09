#include <Servo.h>
//#include <cmath>
#define fsrpin1 A0
#define fsrpin2 A1
#define fsrpin3 A2
#define VIB_MOTOR_PIN1 5
#define VIB_MOTOR_PIN2 6
#define VIB_MOTOR_PIN3 3
// additional vibration motors at pins 10, 11
#define NUM_FSR 3 // number of force sensors added
#define SERVO_PIN1 9
// TO DO: add more servo pins here once we actually get the motors
#define EMG_PIN A3

// Variables for servo control
Servo servo1;
int pos = 0;
int val = 0;
int target = 0;
bool isClosing = true; //closing = true, opening = false

// Variables for haptics
int fsr[NUM_FSR]; // array for storing force sensor readings
int lastreading[NUM_FSR];
int intensity_mod[NUM_FSR];
int timer[NUM_FSR];
void haptics();
bool isPressure = false;

// Variables for signal conditioning
//int const THRES_HIGH = 160;
//int const THRES_LOW = 80;
int const NUM_AVG = 50;
int emgVals[NUM_AVG]; //array to store trailing average 
int counter = 0;
int sum = 0;

void setup() 
{
  servo1.attach(SERVO_PIN1);
  servo1.write(0);
  pinMode(VIB_MOTOR_PIN1, OUTPUT); // vibration motor 1
  pinMode(VIB_MOTOR_PIN2, OUTPUT); // vibration motor 2
  pinMode(VIB_MOTOR_PIN3, OUTPUT); // vibration motor 2
  for (int i = 0; i<NUM_FSR; i++){ fsr[i] = 0;}
  for (int i = 0; i<NUM_AVG; i++){ emgVals[i] = 0;}
  Serial.begin(9600);
}

void loop() 
{
  Serial.print("loop");
  
  // Read value from monitor - temporary
  Serial.println(Serial.available());
  while (Serial.available() == 0)
  { 
    haptics(); // check if there is pressure even when the motor is not moving
  }
  val = Serial.parseFloat();
  
  val = readEMG(val);
  
  haptics();

  if(!isClosing && val<=80){ // opening
    Serial.print("IF 1");
    isClosing = !isClosing; 
    Serial.println(isClosing);
    target = 0;
    while(target<pos){
      pos-=1;
      haptics();
      Serial.println(isPressure);
      servo1.write(pos);
      delay(5);
    }
  }
  else if(val>=160 && isClosing && !isPressure){ // closing
    Serial.println("IF 2");
    isClosing = !isClosing;
    Serial.println(isClosing);
    target = 90;
    while(target>pos){
      pos+=1;
      haptics();
      Serial.println(isPressure);
      if(!isPressure){servo1.write(pos);}
      else{break;}
      delay(5);
    }
  }
  delay(20);
}
//high 160, low 80

//Reads the value in from the EMG and turns it into usable output
int readEMG(val)
{
  //val = abs(analogRead(EMG_PIN) - 464);
  sum = sum - emgVal[counter] + val;
  emgVal[counter] = val;
  counter = (counter+1) %50;
  average = sum/NUM_AVG;
  Serial.println("EMG voltage: " + str(average)); //debugging
  return average;
}

// Checks if there is pressure on the FSRs and vibrates the vibration motors with a tapering off effect
void haptics()
{
  fsr[0] = analogRead(fsrpin1);
  fsr[1] = analogRead(fsrpin2);
  fsr[2] = analogRead(fsrpin3);
  isPressure = 0;
  int intensity[NUM_FSR];

  for (int i = 0; i<NUM_FSR; i++)
  {
    intensity[i] = (int)((fsr[i] / 300.0)*255.0); //multiply actual force by a factor of the possible max force
    if (lastreading[i] == 0 && fsr[i] >= 20){ intensity[i] = 200; } //provide a higher amplitude pulse at the beginning of touch
    /*
      Serial.print(intensity[0]);
      Serial.print(", ");
      Serial.print(intensity_mod[0]);
      Serial.println();
    */
    intensity[i] = (intensity[i] - intensity_mod[i]); //applying intensity modification factor
    if (intensity[i] < 0){ intensity[i] = 0; } //remove negative intensities
    /*if (intensity[i] < 70 && intensity[i] >20){
      intensity[i] = 80; //sets minimum for PWM to 80 because intensity below ~70 doesn't actually produce vibration.
    }*/
  }
  
  // Deducing the intensity modification factor
  for(int i = 0; i<NUM_FSR; i++)
  {
    if (fsr[i] <10)
    {
      lastreading[i] = 250; // checks if there's no force, which resets the modifications and flags that the last reading was 0
      timer[i] = 0;
      intensity_mod[i] = 0;
    } 
    else 
    {
      lastreading[i] = 1; //sustained pressure will result in the intensity tapering off.
      timer[i]+=50;
      /*Serial.print(timer[i]);
      Serial.print(", ");
      Serial.println(fsr[0]);*/
      isPressure = 1;
      Serial.println("FSR number: " + String(i));
      if (timer[i] > 500 and fsr[0]>400 or fsr[1]>400 or fsr[3]>400)
      {
      intensity_mod[i]+=10; //increase the intensity modification factor over time
      }
      
    }
    //intensity[i] = abs(intensity[i] - intensity_mod[i]);
    analogWrite(5, intensity[0]); // write the intensity to vibration motors
    //analogWrite(6, intensity[1]);
    //analogWrite(3, intensity[2]); // Are these supposed to be in the for loop?
  }
}
