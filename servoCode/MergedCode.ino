#include <Servo.h>
#include <EEPROM.h>

// ***********************
// *** Pin Definitions ***
// ***********************
#define fsrpin1 A0         // Force sensor 1
#define fsrpin2 A1         // Force sensor 2
#define fsrpin3 A2         // Force sensor 3

#define VIB_MOTOR_PIN1 5   // Vibration motor for sensor 1
#define VIB_MOTOR_PIN2 6   // Vibration motor for sensor 2
#define VIB_MOTOR_PIN3 3   // Vibration motor for sensor 3

#define SERVO_PIN1 9       // Servo motor control pin

#define EMG_PIN A3         // EMG sensor input

// LED Pins for Calibration Feedback
#define LED_GREEN 8        // Indicates relaxed phase or successful calibration
#define LED_YELLOW 7       // Indicates flexed phase
#define LED_RED 10         // Indicates error or initial flex phase

// ***********************
// *** Constant Definitions ***
// ***********************
#define NUM_FSR 3          // Number of force sensors
#define NUM_AVG 50        // Number of samples for trailing average

Servo servo1;
int pos = 0;              // Current servo position
int target = 0;           // Target servo position
bool isClosing = true;    // true = servo is closing; false = servo is opening

int THRES_LOW = 20;  //20 chris,55
int THRES_HIGH = 35;

// Haptic feedback variables
int fsr[NUM_FSR];             // Array to store FSR readings
int lastreading[NUM_FSR];     // Track last reading state for each sensor
int intensity_mod[NUM_FSR];   // Modification factor to taper vibration intensity
int timer[NUM_FSR];           // Timer to track duration of applied pressure
bool isPressure = false;      // Global flag if any pressure is detected

// EMG averaging variables
int emgVals[NUM_AVG];     // Array for EMG samples in a trailing average
// The following globals are not used because the readEMG() uses its own statics:
// int counter = 0;        
// int sum = 0;              

// Function prototypes
int calibrateEMG();
int calculateOffset();
int calculateBaseline(int offset);
int calculateInitialFlex(int offset);
void indicatePhase(int ledPin);
void blinkLED(int ledPin, int times);
bool loadCalibration(int &offset, int &thres_low, int &thres_high);
void saveCalibration(int offset, int thres_low, int thres_high);

int readEMG(int val);
void haptics();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo1.attach(SERVO_PIN1);
  servo1.write(0);
  pinMode(VIB_MOTOR_PIN1, OUTPUT);
  pinMode(VIB_MOTOR_PIN2, OUTPUT);
  pinMode(VIB_MOTOR_PIN3, OUTPUT);

  // Initialize arrays
  for (int i = 0; i < NUM_FSR; i++) {
    fsr[i] = 0;
    lastreading[i] = 0;
    intensity_mod[i] = 0;
    timer[i] = 0;
  }
  for (int i = 0; i < NUM_AVG; i++) {
    emgVals[i] = 0;
  }
}

void loop() {
  // Debug print
  Serial.print("loop Serial.available() = ");
  Serial.println(analogRead(EMG_PIN));

  // Read the EMG value from the analog input
  int val = analogRead(EMG_PIN);

  // Process EMG with trailing average
  val = readEMG(val);

  // Update haptic feedback
  haptics();

  // Servo control logic
  // If servo is opening, check if EMG < THRES_LOW => open
  // If servo is closing, check if EMG > THRES_HIGH (and no pressure) => close

  // Opening
  if (!isClosing && val <= THRES_LOW) {
    Serial.println("Opening servo.");
    isClosing = !isClosing; 
    target = 0;
    while (pos > target) {
      pos -= 1;
      haptics();
      moveMotor(pos);
      //Serial.println("Pressure: " + String(isPressure));
      delay(5);
    }
  }
  // Closing
  else if (val >= THRES_HIGH && isClosing && !isPressure) {
    Serial.println("Closing servo.");
    isClosing = !isClosing;
    target = 90;
    while (pos < target) {
      pos += 1;
      haptics();
      //Serial.println("Pressure: " + String(isPressure));
      if (!isPressure) {
        moveMotor(pos);
      } else {
        break;
      }
      delay(5);
    }
  }
  delay(20);  
  //Serial.println("FSR 1" + fsrpin1);
  //Serial.println("FSR 2" + fsrpin2);
  //Serial.println("FSR 3" + fsrpin3);

}

// ***********************
// *** EMG Signal Processing ***
// ***********************
int readEMG(int val) {
  if (NUM_AVG == 0) {
    Serial.println("Error: NUM_AVG is 0. Cannot compute average.");
    return 0;
  }
  
  static bool initialized = false;
  static long sum = 0;
  static int counter = 0;

  if (!initialized) {
    for (int i = 0; i < NUM_AVG; i++) {
      emgVals[i] = val;
      sum += val;
    }
    initialized = true;
  } else {
    // Remove the oldest value and add the new value
    sum = sum - emgVals[counter] + val;
    emgVals[counter] = val;
  }

  counter = (counter + 1) % NUM_AVG;
  int average = (int)(sum / NUM_AVG);
  Serial.println("EMG (avg): " + String(average));
  return average;
}

// ***********************
// *** Haptic Feedback ***
// ***********************

void moveMotor(int angle) {
  int pwmValue;
   // Map the angle from 0° to 270° to PWM values
  if (angle >= 0 && angle <= 90) {
      pwmValue = map(angle, 0, 90, 500, 1249);  // 0° - 90°: 0.5ms - 1.167ms
  } else if (angle > 90 && angle <= 180) {
      pwmValue = map(angle, 90, 180, 1249, 1749); // 90° - 180°: 1ms - 2.333ms
  } else if (angle > 180 && angle <= 270) {
      pwmValue = map(angle, 180, 270, 1749, 2500); // 180° - 270°: 2ms - 2.5ms
  } else {
      Serial.println("Error: Angle out of range!");
      return;
  }




  Serial.print("Moving to: ");
  Serial.println(angle);
  Serial.print("PWM Value: ");
  Serial.println(pwmValue);




  servo1.writeMicroseconds(pwmValue);  // Move the servo to the calculated PWM value
}




  //Serial.print("Moving to: ");
  //Serial.println(angle);
  //Serial.print("PWM Value: ");
  //Serial.println(pwmValue);




  //servo1.writeMicroseconds(pwmValue);  // Move the servo to the calculated PWM value



void haptics() {
  // Define sensor and vibration motor pins in arrays for easy iteration
  int fsrPins[NUM_FSR] = {fsrpin1, fsrpin2, fsrpin3};
  int vibPins[NUM_FSR] = {VIB_MOTOR_PIN1, VIB_MOTOR_PIN2, VIB_MOTOR_PIN3};
  const int FSR_THRESHOLD = 100; // Adjust this threshold based on your sensor characteristics

  isPressure = false;
  for (int i = 0; i < NUM_FSR; i++) {
    fsr[i] = analogRead(fsrPins[i]);
    if (fsr[i] > FSR_THRESHOLD) {
      digitalWrite(vibPins[i], HIGH);
      isPressure = true;
    } else {
      digitalWrite(vibPins[i], LOW);
    }
  }
}



