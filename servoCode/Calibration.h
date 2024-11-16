// Calibration.h
#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>

// Calibration Variables
extern int THRES_LOW;   // Low threshold for EMG
extern int THRES_HIGH;  // High threshold for EMG

// Calibration Functions
void calibrateEMG();
int calculateOffset();
bool loadCalibration(int &offset, int &thres_low, int &thres_high);
void saveCalibration(int offset, int thres_low, int thres_high);

#endif 
