#ifndef _WHEELS_H_
#define _WHEELS_H_

#include <armuro.h>

typedef enum {
    NONE = 0,
    ANGLE = 1,
    SPEED = 2
} TurnWheelsTaskType;

/**
 * @brief Start to turn the wheel by a certain angle
 * 
 * @param wheel the wheel to turn
 * @param angle the angle to turn the wheel by (positive for forward, negative for backward)
 */
void turnWheelByAngle(int wheel, int angle);

/**
 * @brief Start to turn the wheel with a certain speed
 * 
 * @param wheel the wheel to turn
 * @param speed the speed at which the wheel should turn (0-100)
 */
void turnWheelWithSpeed(int wheel, int speed);

/**
 * @brief Manage the turning of the wheels
 * @details This function should be called in a loop
 * 
 * @return TurnWheelsTaskType* the current state of the wheels
 */
TurnWheelsTaskType* turnWheelsTask();

void turnWheelByAngleTask(int wheel);

#endif