#include <EEPROM.h> // Required for saving calibration data

// =======================
// Calibration Variables
// =======================
int THRES_LOW = 80;    // Default low threshold (will be overwritten by calibration)
int THRES_HIGH = 160;  // Default high threshold (will be overwritten by calibration)

// EMG Pin Definition (Ensure this matches your hardware setup)
#define EMG_PIN A3

// LED Pins for Calibration Phases
#define LED_GREEN 8    // Relaxed phase
#define LED_YELLOW 9  // Flexed phase
#define LED_RED 10     // Initial Flex phase

// =======================
// Function Prototypes
// =======================
int calibrateEMG();
int calculateOffset();
int calculateBaseline(int offset);
int calculateInitialFlex(int offset);
void indicatePhase(int ledPin);
void blinkLED(int ledPin, int times);
bool loadCalibration(int &offset, int &thres_low, int &thres_high);
void saveCalibration(int offset, int thres_low, int thres_high);

// =======================
// Arduino Setup and Loop
// =======================
void setup() {
  // Initialize LED pins
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  int offset;
  // Attempt to load existing calibration data
  if (loadCalibration(offset, THRES_LOW, THRES_HIGH)) {
    // Indicate successful calibration load with a quick blink
    blinkLED(LED_GREEN, 3);
  } else {
    int status = 0;
    while (!status)
    {
      // Indicate calibration is needed with a slow blink
      blinkLED(LED_RED, 3);
      // Start calibration process
      status = calibrateEMG();
    }
    // Indicate successful calibration load with a quick blink
    blinkLED(LED_GREEN, 3);
  }
}

void loop() {
  // Main code (replace with your application logic)
}

// =======================
// Calibration Function Implementations
// Returns status on if calibration was successful
// =======================
int calibrateEMG() {
  int offset = calculateOffset();

  // Step 1: Relaxed Baseline
  indicatePhase(LED_GREEN);
  blinkLED(LED_GREEN, 2); // Blink twice to indicate relaxed phase
  delay(1000); // Short delay before starting
  int relaxedBaseline = calculateBaseline(offset);

  // Step 2: Flexed Baseline
  indicatePhase(LED_YELLOW);
  blinkLED(LED_YELLOW, 2); // Blink twice to indicate flexed phase
  delay(1000); // Short delay before starting
  int flexedBaseline = calculateBaseline(offset);

  // Validate Baselines
  if (flexedBaseline <= relaxedBaseline) {
    // Indicate error with LED_RED blinking
    blinkLED(LED_RED, 5);
    return -1;
  }

  // Step 3: Initial Flex Peak
  indicatePhase(LED_YELLOW);
  blinkLED(LED_YELLOW, 2); // Blink twice to indicate initial flex phase
  delay(1000);
  int initialFlex = calculateInitialFlex(offset);

  // Threshold Calculations
  THRES_HIGH = (initialFlex - flexedBaseline) * 0.5 + flexedBaseline;
  THRES_LOW = flexedBaseline * 0.75;

  // Indicate successful calibration
  blinkLED(LED_GREEN, 5);
  // Save calibration data to EEPROM
  saveCalibration(offset, THRES_LOW, THRES_HIGH);
  return 0;
}

// =======================
// Helper Functions
// =======================

// Calculates the offset value by averaging multiple readings while the user remains still
int calculateOffset() {
  blinkLED(LED_GREEN, 1); // Indicate offset calibration
  delay(1000); // Allow user to stabilize

  int offsetSum = 0;
  int offsetCount = 100; // Number of readings for offset calculation
  for (int i = 0; i < offsetCount; i++) {
    offsetSum += analogRead(EMG_PIN);
    delay(10);
  }
  int offset = offsetSum / offsetCount;
  return offset;
}

// Calculates the baseline (either relaxed or flexed) by averaging readings over time
int calculateBaseline(int offset) {
  int sum = 0;
  int count = 100; // Number of readings for baseline calculation
  for (int i = 0; i < count; i++) {
    int reading = abs(analogRead(EMG_PIN) - offset);
    sum += reading;
    delay(10);
  }
  int baseline = sum / count;
  return baseline;
}

// Captures the initial flex peak by recording the highest value over a period
int calculateInitialFlex(int offset) {
  int maxPeak = 0;
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) { // Record for 5 seconds
    int reading = abs(analogRead(EMG_PIN) - offset);
    if (reading > maxPeak) {
      maxPeak = reading;
    }
    delay(10);
  }
  return maxPeak;
}

// Turns on the specified LED to indicate the current phase
void indicatePhase(int ledPin) {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(ledPin, HIGH);
}

// Blinks the specified LED a given number of times for user feedback
void blinkLED(int ledPin, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(300);
    digitalWrite(ledPin, LOW);
    delay(300);
  }
}

// Loads calibration data from EEPROM; returns true if successful
bool loadCalibration(int &offset, int &thres_low, int &thres_high) {
  EEPROM.get(0, offset);
  EEPROM.get(2, thres_low);
  EEPROM.get(4, thres_high);

  if (thres_high > thres_low) {
    return true;
  }
  return false;
}

// Saves calibration data to EEPROM
void saveCalibration(int offset, int thres_low, int thres_high) {
  EEPROM.put(0, offset);
  EEPROM.put(2, thres_low);
  EEPROM.put(4, thres_high);
}
