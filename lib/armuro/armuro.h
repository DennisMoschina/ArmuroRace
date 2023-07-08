#ifndef _ARMURO_H_
#define _ARMURO_H_

#define FORWARD 0
#define BACKWARD 1

#define HIGH 1
#define LOW 0

#define WHEEL_CIRCUMFERENCE 12.566
#define TURN_CIRCUMFERENCE 24.1902634326

#define MIN_ANGLE 15

#define MAX_LEFT_LINE_SENSOR_VALUE 3380
#define MAX_MIDDLE_LINE_SENSOR_VALUE 3380
#define MAX_RIGHT_LINE_SENSOR_VALUE 2900
#define MIN_LEFT_LINE_SENSOR_VALUE 1250
#define MIN_MIDDLE_LINE_SENSOR_VALUE 1120
#define MIN_RIGHT_LINE_SENSOR_VALUE 170

#include <stdint.h>
#include <stdarg.h>

extern uint32_t minLineSensorValues[3];
extern uint32_t maxLineSensorValues[3];

extern uint16_t wheelEncoderTicksCount[2];
extern uint32_t buffer[6];

typedef enum {
    RIGHT = 0,
    LEFT = 1,
    MIDDLE = 3
} Side;

typedef struct {
    int left;
    int right;
    
    int leftTicks;
    int rightTicks;
} WheelAngle;

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
void turnMotor(Side motor, int direction, int speed);

/**
 * @brief Stop the motor
 * 
 * @param motor the motor to stop
 */
void stopMotor(Side motor);

/**
 * @brief Turn the led on the side on and off
 * 
 * @param led the side on which the led should be controlled
 * @param state the state of the LED (HIGH or LOW)
 */
void setLED(Side led, int state);

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
void resetAngleMeasurement(Side wheel);

/**
 * @brief Start measuring the angle for the wheels.
 * 
 * @return a pointer to a struct keeping the angle of the wheels
 */
WheelAngle* startAngleMeasurement();

/**
 * @brief Stop measuring the angle for the wheels.
 * 
 * @param angle the angle struct to stop measuring for
 */
void stopAngleMeasurement(WheelAngle* angle);

/**
 * @brief et the Angle (in degrees) for the wheel
 * 
 * @param wheel the wheel for which the angle should be returned
 * @return the angle of the wheel in degrees
 */
int getAngleForWheel(Side wheel);

/**
 * @brief Get the Angle (in degrees) for the wheels
 * 
 * @param leftAngle a pointer to a variable in which the left angle should be stored
 * @param rightAngle a pointer to a variable in which the right angle should be stored
 */
void getAngleForWheels(int* leftAngle, int* rightAngle);

/**
 * @brief Get the raw readings of the line sensors
 * 
 * @param left a pointer to a variable in which the left sensor reading should be stored 
 * @param middle a pointer to a variable in which the middle sensor reading should be stored
 * @param right a pointer to a variable in which the right sensor reading should be stored
 */
void getRawLineSensorReadings(uint32_t* left, uint32_t* middle, uint32_t* right);

/**
 * @brief Get the readings of the line sensors mapped to their typical range (0 - 1023)
 * 
 * @param left a pointer to a variable in which the left sensor reading should be stored
 * @param middle a pointer to a variable in which the middle sensor reading should be stored
 * @param right a pointer to a variable in which the right sensor reading should be stored
 */
void getLineSensorReadings(uint32_t* left, uint32_t* middle, uint32_t* right);

/**
 * @brief Map a line sensor reading to its typical range (0 - 1023)
 * 
 * @param value the value to map
 * @param side the side on which the sensor is located
 * @return the mapped value
 */
uint32_t mapLineSensorReadingToRange(uint32_t value, Side side);

/**
 * @brief Translate a distance (in cm) to an angle (in degrees)
 * 
 * @param distance the distance for which the angle should be returned
 * @return the angle for the distance 
 */
int distanceToAngle(double distance);

/**
 * @brief Get the Distance (in cm) for an angle (in degrees)
 * 
 * @param angle the angle for which the distance should be returned
 * @return the distance for the angle
 */
double angleToDistance(int angle);

/**
 * @brief Check if a switch is pressed.
 * 
 * @param side a pointer to store the pressed switch in
 * @return true if a switch is pressed, false otherwise
 */
uint8_t checkSwitchesPressed(Side* side);

#endif