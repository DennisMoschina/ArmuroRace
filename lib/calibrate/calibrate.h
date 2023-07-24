#ifndef _CALIBRATE_H_
#define _CALIBRATE_H_

#include "armuro.h"
#include "stateMachine.h"

/**
 * @defgroup calibrate Calibrate Robot
 * @brief Calibrate the hardware of the armuro robot.
 */

/**
 * @brief Configure the calibration task
 * @ingroup calibrate
 */
void calibrate();
/**
 * @brief Run the calibration task
 * @ingroup calibrate
 * 
 * @return RUNNING if task is still running, FINISHED if task is finished
 */
State calibrateTask();

/**
 * @brief Calibrate the white value for the line sensors
 * @ingroup calibrate
 */
void readWhiteLineSensors();
/**
 * @brief Calibrate the black value for the line sensors
 * @ingroup calibrate
 */
void readBlackLineSensors();

#endif