#ifndef _ARMURO_H_
#define _ARMURO_H_

#define RIGHT 0
#define LEFT 1

#define FORWARD 0
#define BACKWARD 1

#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET

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

#endif