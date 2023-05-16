#ifndef _ARMURO_H_
#define _ARMURO_H_

#define RIGHT 0
#define LEFT 1

#define FORWARD 0
#define BACKWARD 1

#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET

#include <stdint.h>

extern uint16_t leftEncoderTicksCount;
extern uint16_t rightEncoderTicksCount;

/**
 * @brief Initialize the motors
 */
void initMotors();

/**
 * @brief Activate the motor
 * 
 * @param motor the motor to activate
 * @param direction the direction in which the motor should turn
 * @param speed the speed at which the motor should turn (in %)
 */
void turnMotor(int motor, int direction, int speed);

/**
 * @brief Stop the motor
 * 
 * @param motor the motor to stop
 */
void stopMotor(int motor);

/**
 * @brief Turn the led on the side on and off
 * 
 * @param led the side on which the led should be controlled
 * @param state the state of the LED (HIGH or LOW)
 */
void setLED(int led, int state);

/**
 * @brief Set the rear LED on or off
 * 
 * @param state the state of the LED (HIGH or LOW)
 */
void setRearLED(int state);

/**
 * @brief Handle a new value from the sensors
 * 
 * @param values an array of 6 values, containing the sensor readigns
 */
void didReadSensors(uint32_t* values);

/**
 * @brief Handle a new value from the wheel encoders
 * 
 * @param leftValue the value of the left wheel encoder
 * @param rightValue the value of the right wheel encoder
 */
void didReadWheelEncoder(uint32_t leftValue, uint32_t rightValue);

/**
 * @brief Reset the angle measurement for the wheels
 */
void resetAngleMeasurement();

/**
 * @brief Get the Angle (in degrees) for the wheels
 * 
 * @param leftAngle a pointer to a variable in which the left angle should be stored
 * @param rightAngle a pointer to a variable in which the right angle should be stored
 */
void getAngleForWheels(int* leftAngle, int* rightAngle);

#endif