/**************************************************************************
 * Merged Arduino Sketch: EMG-Controlled Servo with Haptic Feedback
 * 
 * This sketch combines two functionalities:
 * 1. Servo Control with Haptic Feedback:
 *    - Reads EMG signals (filtered using a trailing average).
 *    - Controls a servo motor (opening/closing) based on calibrated
 *      EMG thresholds.
 *    - Uses force-sensitive resistors (FSRs) to activate vibration motors.
 *
 * 2. Calibration Routine:
 *    - Calibrates the EMG sensor by measuring relaxed and flexed baselines.
 *    - Calculates threshold values for servo control.
 *    - Provides visual feedback via LEDs.
 *    - Saves (and loads) calibration data to/from EEPROM.
 *
 * Pin assignments:
 *   FSR pins      : A0, A1, A2
 *   Vibration pins: 5, 6, 3
 *   Servo pin     : 9
 *   EMG sensor    : A3
 *   LEDs          : LED_GREEN (8), LED_YELLOW (7), LED_RED (10)
 *
 **************************************************************************/

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
#define NUM_AVG 50         // Number of samples for trailing average

// ***********************
// *** Global Variables ***
// ***********************
// Calibration thresholds (overwritten during calibration)
int THRES_LOW = 80;
int THRES_HIGH = 160;

// Servo control variables
Servo servo1;
int pos = 0;              // Current servo position
int target = 0;           // Target servo position
bool isClosing = true;    // true = servo is closing; false = servo is opening

// Haptic feedback variables
int fsr[NUM_FSR];             // Array to store FSR readings
int lastreading[NUM_FSR];     // Track last reading state for each sensor
int intensity_mod[NUM_FSR];   // Modification factor to taper vibration intensity
int timer[NUM_FSR];           // Timer to track duration of applied pressure
bool isPressure = false;      // Global flag if any pressure is detected

// EMG averaging variables
int emgVals[NUM_AVG];     // Array for EMG samples in a trailing average
int counter = 0;          // Index for the circular buffer
int sum = 0;              // Running sum of last NUM_AVG samples

// ***********************
// *** Function Prototypes ***
// ***********************
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

// ***********************
// *** Setup Function ***
// ***********************
void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);

  // Attach the servo
  servo1.attach(SERVO_PIN1);
  servo1.write(0);

  // Vibration motor pins as outputs
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

  // LED pins for calibration feedback
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Attempt to load previously saved calibration
  int offset;
  if (loadCalibration(offset, THRES_LOW, THRES_HIGH)) {
    blinkLED(LED_GREEN, 3);  // Calibration data loaded
    Serial.println("Calibration data loaded from EEPROM.");
  } else {
    // If loading fails, run calibration
    int status = -1;
    while (status != 0) {  
      blinkLED(LED_RED, 3);  // Indicate calibration needed
      status = calibrateEMG();
    }
    blinkLED(LED_GREEN, 3);  // Calibration success
    Serial.println("Calibration complete and saved to EEPROM.");
  }
}

// ***********************
// *** Main Loop ***
// ***********************
void loop() {
  // Debug print
  Serial.print("loop Serial.available() = ");
  Serial.println(Serial.available());
  
  // While waiting for new EMG data, keep updating haptics
  while (Serial.available() == 0) {
    haptics();
  }

  // Read the EMG value from Serial input
  int val = (int)Serial.parseFloat();

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
      servo1.write(pos);
      Serial.println("Pressure: " + String(isPressure));
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
      Serial.println("Pressure: " + String(isPressure));
      if (!isPressure) {
        servo1.write(pos);
      } else {
        // Stop if pressure is detected
        break;
      }
      delay(5);
    }
  }
  delay(20);  
}

// ***********************
// *** EMG Signal Processing ***
// ***********************
int readEMG(int val) {
  // Trailing average
  sum = sum - emgVals[counter] + val;
  emgVals[counter] = val;
  counter = (counter + 1) % NUM_AVG;
  int average = sum / NUM_AVG;

  Serial.println("EMG (avg): " + String(average));
  return average;
}

// ***********************
// *** Haptic Feedback ***
// ***********************
void haptics() {
  // Read FSR sensor values
  fsr[0] = analogRead(fsrpin1);
  fsr[1] = analogRead(fsrpin2);
  fsr[2] = analogRead(fsrpin3);
  isPressure = false;
  int intensity[NUM_FSR];

  // Calculate initial intensities
  for (int i = 0; i < NUM_FSR; i++) {
    intensity[i] = (int)((fsr[i] / 300.0) * 255.0);

    // Pulse at initial contact
    if (lastreading[i] == 0 && fsr[i] >= 20) {
      intensity[i] = 200; 
    }

    // Apply modification
    intensity[i] = intensity[i] - intensity_mod[i];
    if (intensity[i] < 0) {
      intensity[i] = 0;
    }
  }
  
  // Update modification factors
  for (int i = 0; i < NUM_FSR; i++) {
    if (fsr[i] < 10) {
      // No pressure => reset
      lastreading[i] = 0;    // Fix: was previously 250
      timer[i] = 0;
      intensity_mod[i] = 0;
    } else {
      // Sustained pressure
      lastreading[i] = 1;
      timer[i] += 50;
      isPressure = true;
      Serial.println("FSR active on sensor: " + String(i));

      // If sustained high pressure, increase modification
      if (timer[i] > 500 && fsr[i] > 400) {
        intensity_mod[i] += 10;
      }
    }
  }
  
  // Output to vibration motors
  analogWrite(VIB_MOTOR_PIN1, intensity[0]);
  analogWrite(VIB_MOTOR_PIN2, intensity[1]);
  analogWrite(VIB_MOTOR_PIN3, intensity[2]);
}

// ***********************
// *** Calibration Functions ***
// ***********************
int calibrateEMG() {
  // Step 1: Compute offset
  int offset = calculateOffset();

  // Relaxed Baseline
  indicatePhase(LED_GREEN);
  blinkLED(LED_GREEN, 2);
  delay(1000);
  int relaxedBaseline = calculateBaseline(offset);

  // Flexed Baseline
  indicatePhase(LED_YELLOW);
  blinkLED(LED_YELLOW, 2);
  delay(1000);
  int flexedBaseline = calculateBaseline(offset);

  // Validate
  if (flexedBaseline <= relaxedBaseline) {
    blinkLED(LED_RED, 5);
    return -1;
  }

  // Initial Flex Peak
  indicatePhase(LED_YELLOW);
  blinkLED(LED_YELLOW, 2);
  delay(1000);
  int initialFlex = calculateInitialFlex(offset);

  // Compute thresholds
  THRES_HIGH = (int)((initialFlex - flexedBaseline) * 0.5 + flexedBaseline);
  THRES_LOW  = (int)(flexedBaseline * 0.75);

  blinkLED(LED_GREEN, 5);
  saveCalibration(offset, THRES_LOW, THRES_HIGH);
  return 0; // Success
}

int calculateOffset() {
  blinkLED(LED_GREEN, 1);
  delay(1000);

  long offsetSum = 0;
  int offsetCount = 100;
  for (int i = 0; i < offsetCount; i++) {
    offsetSum += analogRead(EMG_PIN);
    delay(10);
  }
  int offset = offsetSum / offsetCount;
  return offset;
}

int calculateBaseline(int offset) {
  long sum = 0;
  int count = 100;
  for (int i = 0; i < count; i++) {
    int reading = abs(analogRead(EMG_PIN) - offset);
    sum += reading;
    delay(10);
  }
  return (sum / count);
}

int calculateInitialFlex(int offset) {
  int maxPeak = 0;
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    int reading = abs(analogRead(EMG_PIN) - offset);
    if (reading > maxPeak) {
      maxPeak = reading;
    }
    delay(10);
  }
  return maxPeak;
}

void indicatePhase(int ledPin) {
  // Turn off all
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  // Turn on selected
  digitalWrite(ledPin, HIGH);
}

void blinkLED(int ledPin, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(300);
    digitalWrite(ledPin, LOW);
    delay(300);
  }
}

bool loadCalibration(int &offset, int &thres_low, int &thres_high) {
  EEPROM.get(0, offset);
  EEPROM.get(2, thres_low);
  EEPROM.get(4, thres_high);

  // Very simple check: if high > low, assume valid
  if (thres_high > thres_low) {
    return true;
  }
  return false;
}

void saveCalibration(int offset, int thres_low, int thres_high) {
  EEPROM.put(0, offset);
  EEPROM.put(2, thres_low);
  EEPROM.put(4, thres_high);
}
