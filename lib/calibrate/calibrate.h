#ifndef _CALIBRATE_H_
#define _CALIBRATE_H_

#include "armuro.h"
#include "stateMachine.h"

/**
 * @brief Configure the calibration task
 */
void calibrate();
/**
 * @brief Run the calibration task
 * 
 * @return RUNNING if task is still running, FINISHED if task is finished
 */
State calibrateTask();

/**
 * @brief Calibrate the white value for the line sensors
 */
void readWhiteLineSensors();
/**
 * @brief Calibrate the black value for the line sensors
 */
void readBlackLineSensors();

#endif