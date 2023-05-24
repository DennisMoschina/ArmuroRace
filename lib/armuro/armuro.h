#ifndef _ARMURO_H_
#define _ARMURO_H_

#define RIGHT 0
#define LEFT 1

#define FORWARD 0
#define BACKWARD 1

#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET

#include <stdint.h>
#include <stdarg.h>

extern uint16_t wheelEncoderTicksCount[2];

/**
 * @brief Print a message to the serial port.
 * 
 * @param format the format of the message
 * @param ... the parameters for the format
 */
void print(char* format, ...);

/**
 * @brief Map a value from one range to another
 * 
 * @param x the value to map
 * @param in_min the minimum value of the input range
 * @param in_max the maximum value of the input range
 * @param out_min the minimum value of the output range
 * @param out_max the maximum value of the output range
 * @return the value mapped to the output range
 */
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);

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
 * 
 * @param wheel the wheel for which the angle measurement should be reset
 */
void resetAngleMeasurement(int wheel);

/**
 * @brief et the Angle (in degrees) for the wheel
 * 
 * @param wheel the wheel for which the angle should be returned
 * @return the angle of the wheel in degrees
 */
int getAngleForWheel(int wheel);

/**
 * @brief Get the Angle (in degrees) for the wheels
 * 
 * @param leftAngle a pointer to a variable in which the left angle should be stored
 * @param rightAngle a pointer to a variable in which the right angle should be stored
 */
void getAngleForWheels(int* leftAngle, int* rightAngle);

#endif