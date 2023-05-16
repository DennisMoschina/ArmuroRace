#ifndef _WHEELS_H_
#define _WHEELS_H_

#include <armuro.h>

/**
 * @brief Start to turn the wheel by a certain angle
 * 
 * @param wheel the wheel to turn
 * @param angle the angle to turn the wheel by (positive for forward, negative for backward)
 */
void turnWheelByAngle(int wheel, int angle);

/**
 * @brief Manage the turning of the wheels
 * @details This function should be called in a loop
 */
void turnWheelsTask();

#endif