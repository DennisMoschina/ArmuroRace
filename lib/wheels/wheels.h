#ifndef _WHEELS_H_
#define _WHEELS_H_

#include <armuro.h>

typedef enum {
    NONE = 0,
    ANGLE = 1,
    SPEED = 2,
    SYNCHRONIZED = 3,
    TIMED_ANGLE = 4,
    SYNCHRONIZED_ANGLE = 5,
    TURN = 6
} TurnWheelsTaskType;

/**
 * @brief Start to turn the wheel by a certain angle
 * 
 * @param wheel the wheel to turn
 * @param angle the angle to turn the wheel by (positive for forward, negative for backward)
 * @param speed the speed at which the wheel should turn (0-100)
 */
void turnWheelByAngle(Side wheel, int angle, int speed);

/**
 * @brief Start to turn the wheel by a certain angle in a certain time
 * 
 * @param wheel the wheel to turn
 * @param angle the angle to turn the wheel by (positive for forward, negative for backward)
 * @param time the time in which the wheel should complete the turn (in ms)
 */
void turnWheelByAngleInTime(Side wheel, int angle, int time);

/**
 * @brief Start to turn the wheel with a certain speed
 * 
 * @param wheel the wheel to turn
 * @param speed the speed at which the wheel should turn (0-100)
 */
void turnWheelWithSpeed(Side wheel, int speed);

/**
 * @brief Turn the wheels with a certain speed
 * @details This function initiates the turning of the wheels with a certain speed while
 *          regulating the speed difference between the wheels.
 * 
 * @param leftSpeed the speed of the left wheel
 * @param rightSpeed the speed of the right wheel
 */
void turnWheelsSynchronized(int leftSpeed, int rightSpeed);

/**
 * @brief Turn the wheels with a certain speed and a certain angle
 * 
 * @param leftSpeed the speed of the left wheel
 * @param rightSpeed the speed of the right wheel
 * @param rightAngle the angle the right wheel should be turned by
 */
void turnWheelsSynchronizedByAngle(int leftSpeed, int rightSpeed, int rightAngle);

/**
 * @brief Turn the armuro by a certain angle in a certain time.
 * 
 * @param angle the angle to turn by (positive for left, negative for right)
 * @param time the time in which the armuro should complete the turn (in ms)
 */
void turnArmuroInTime(int angle, int time);

/**
 * @brief Turn the armuro by a certain angle with a certain speed.
 * 
 * @param angle the angle to turn by (positive for left, negative for right)
 */
void turnArmuro(int angle);

/**
 * @brief Manage the turning of the wheels
 * @details This function should be called in a loop
 * 
 * @return TurnWheelsTaskType* the current state of the wheels
 */
TurnWheelsTaskType* turnWheelsTask();

void turnWheelByAngleTask(Side wheel);

void turnWheelsSynchronizedTask();

void turnWheelsSynchronizedByAngleTask();

void turnWheelByAngleInTimeTask(Side wheel);

void turnArmuroTask(Side wheel);

#endif