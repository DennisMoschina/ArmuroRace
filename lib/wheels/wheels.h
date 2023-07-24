#ifndef _WHEELS_H_
#define _WHEELS_H_

#include <armuro.h>

/**
 * @defgroup wheels Wheels
 * @brief Control the wheels of the robot.
 */

/**
 * @brief The type of tasks the wheels are currently executing.
 * @ingroup wheels
 */
typedef enum TurnWheelsTaskType {
    /// @brief Wheel is stopped
    NONE = 0,
    /// @brief Wheel is turning by a certain angle
    ANGLE = 1,
    /// @brief Wheel is turning by a certain speed
    SPEED = 2,
    /// @brief Wheels are turning with a certain speed relative to each other
    SYNCHRONIZED = 3,
    /// @brief Wheel is turning by a certain angle in a certain time
    TIMED_ANGLE = 4,
    /// @brief Wheels are turning with a certain speed relative to each other and a certain angle
    SYNCHRONIZED_ANGLE = 5,
    /// @brief Robot is turning in place
    TURN = 6
} TurnWheelsTaskType;

/**
 * @brief Stop the wheel.
 * @ingroup wheels
 * 
 * @param wheel the wheel to stop
 */
void stopWheel(Side wheel);

/**
 * @brief Start to turn the wheel by a certain angle
 * @ingroup wheels
 * 
 * @param wheel the wheel to turn
 * @param angle the angle to turn the wheel by (positive for forward, negative for backward)
 * @param speed the speed at which the wheel should turn (0-100)
 */
void turnWheelByAngle(Side wheel, int angle, int speed);

/**
 * @brief Start to turn the wheel by a certain angle in a certain time
 * @ingroup wheels
 * 
 * @param wheel the wheel to turn
 * @param angle the angle to turn the wheel by (positive for forward, negative for backward)
 * @param time the time in which the wheel should complete the turn (in ms)
 */
void turnWheelByAngleInTime(Side wheel, int angle, int time);

/**
 * @brief Start to turn the wheel with a certain speed
 * @ingroup wheels
 * 
 * @param wheel the wheel to turn
 * @param speed the speed at which the wheel should turn (0-100)
 */
void turnWheelWithSpeed(Side wheel, int speed);

/**
 * @brief Turn the wheels with a certain speed
 * @details This function initiates the turning of the wheels with a certain speed while
 *          regulating the speed difference between the wheels.
 * @ingroup wheels
 * 
 * @param leftSpeed the speed of the left wheel
 * @param rightSpeed the speed of the right wheel
 */
void turnWheelsSynchronized(int leftSpeed, int rightSpeed);

/**
 * @brief Turn the wheels with a certain speed and a certain angle
 * @ingroup wheels
 * 
 * @param leftSpeed the speed of the left wheel
 * @param rightSpeed the speed of the right wheel
 * @param rightAngle the angle the right wheel should be turned by
 * @param softStart whether the wheels should be started slowly
 */
void turnWheelsSynchronizedByAngle(int leftSpeed, int rightSpeed, int rightAngle, uint8_t softStart);

/**
 * @brief Turn the armuro by a certain angle in a certain time.
 * @ingroup wheels
 * 
 * @param angle the angle to turn by (positive for left, negative for right)
 * @param time the time in which the armuro should complete the turn (in ms)
 */
void turnArmuroInTime(int angle, int time);

/**
 * @brief Turn the armuro by a certain angle with a certain speed.
 * @ingroup wheels
 * 
 * @param angle the angle to turn by (positive for left, negative for right)
 */
void turnArmuro(int angle);

/**
 * @brief Manage the turning of the wheels
 * @details This function should be called in a loop
 * @ingroup wheels
 * @note This method should be called in the main loop of the program as frequent as possible to assure the correct timing.
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