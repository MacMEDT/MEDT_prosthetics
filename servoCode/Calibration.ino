// Calibration.ino

// =======================
// Calibration Variables
// =======================

// These variables are used in the main sketch, so we declare them as global.
int THRES_LOW = 80;    // Default low threshold (will be overwritten by calibration)
int THRES_HIGH = 160;  // Default high threshold (will be overwritten by calibration)

// EMG Pin Definition (Ensure this matches your main sketch)
#define EMG_PIN A3

// =======================
// Calibration Function Prototypes
// =======================
void calibrateEMG();
int calculateOffset();
bool loadCalibration(int &offset, int &thres_low, int &thres_high);
void saveCalibration(int offset, int thres_low, int thres_high);

// =======================
// Calibration Function Implementations
// =======================

/**
 * @brief Initiates the EMG calibration process.
 * Guides the user through relaxation and flexion phases to determine thresholds.
 */
void calibrateEMG() 
{
  int offset = calculateOffset();
  
  Serial.println("Starting calibration. Please relax your arm and remain still.");
  delay(2000); // Give user time to relax

  // Collect Relaxed EMG Readings
  int relaxedSum = 0;
  int relaxedCount = 100; // Number of readings for relaxed baseline
  for (int i = 0; i < relaxedCount; i++) 
  {
    int reading = abs(analogRead(EMG_PIN) - offset);
    relaxedSum += reading;
    delay(10);
  }
  int relaxedBaseline = relaxedSum / relaxedCount;
  Serial.print("Relaxed Baseline: ");
  Serial.println(relaxedBaseline);

  Serial.println("Calibration step 1 complete. Now, please flex your arm.");
  delay(2000); // Give user time to flex

  // Collect Flexed EMG Readings
  int flexedSum = 0;
  int flexedCount = 100; // Number of readings for flexed baseline
  for (int i = 0; i < flexedCount; i++) 
  {
    int reading = abs(analogRead(EMG_PIN) - offset);
    flexedSum += reading;
    delay(10);
  }
  int flexedBaseline = flexedSum / flexedCount;
  Serial.print("Flexed Baseline: ");
  Serial.println(flexedBaseline);

  // Validate Calibration
  if (flexedBaseline <= relaxedBaseline) 
  {
    Serial.println("Error: Flexed baseline is not higher than relaxed baseline. Please retry calibration.");
    return;
  }

  // Set Thresholds
  THRES_LOW = relaxedBaseline + static_cast<int>((flexedBaseline - relaxedBaseline) * 0.25);
  THRES_HIGH = relaxedBaseline + static_cast<int>((flexedBaseline - relaxedBaseline) * 0.75);

  Serial.print("THRES_LOW set to: ");
  Serial.println(THRES_LOW);
  Serial.print("THRES_HIGH set to: ");
  Serial.println(THRES_HIGH);

  // Save Calibration Data to EEPROM
  saveCalibration(offset, THRES_LOW, THRES_HIGH);
  Serial.println("Calibration complete and saved.");
}

/**
 * @brief Calculates the dynamic offset based on initial EMG readings.
 * 
 * @return int Calculated offset value.
 */
int calculateOffset() 
{
  Serial.println("Calibrating offset. Please remain still.");
  delay(2000); // Allow user to stabilize
  
  int offsetSum = 0;
  int offsetCount = 100; // Number of readings for offset calculation
  for (int i = 0; i < offsetCount; i++) 
  {
    offsetSum += analogRead(EMG_PIN);
    delay(10);
  }
  int offset = offsetSum / offsetCount;
  Serial.print("Calculated Offset: ");
  Serial.println(offset);
  return offset;
}

/**
 * @brief Loads calibration data from EEPROM.
 * 
 * @param offset Reference to store the loaded offset.
 * @param thres_low Reference to store the loaded low threshold.
 * @param thres_high Reference to store the loaded high threshold.
 * @return true If calibration data is valid and loaded successfully.
 * @return false If calibration data is invalid or not present.
 */
bool loadCalibration(int &offset, int &thres_low, int &thres_high) 
{
  EEPROM.get(0, offset);
  EEPROM.get(2, thres_low);
  EEPROM.get(4, thres_high);
  
  // Simple validation: THRES_HIGH should be greater than THRES_LOW
  if (thres_high > thres_low) 
  {
    return true;
  }
  return false;
}

/**
 * @brief Saves calibration data to EEPROM.
 * 
 * @param offset The offset value to save.
 * @param thres_low The low threshold to save.
 * @param thres_high The high threshold to save.
 */
void saveCalibration(int offset, int thres_low, int thres_high) 
{
  EEPROM.put(0, offset);
  EEPROM.put(2, thres_low);
  EEPROM.put(4, thres_high);
}
