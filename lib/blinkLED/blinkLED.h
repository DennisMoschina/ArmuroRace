#ifndef _BLINK_LED_H_
#define _BLINK_LED_H_

#include "armuro.h"

/**
 * @defgroup blinkLED Blink LED
 * @brief Let the LEDs blink.
 */

/**
 * @brief Blink the LED on the given side with the given time interval
 * @ingroup blinkLED
 * 
 * @param side the side to blink the LED on
 * @param timeInterval the interval at which the LED should blink
 */
void blinkLED(Side side, uint16_t timeInterval);

/**
 * @brief Stop blinking the LED and turn it off on the given side
 * @ingroup blinkLED
 * 
 * @param side the side to of the LED
 */
void stopBlinkingLED(Side side);

/**
 * @brief Run the blinking Task
 * @ingroup blinkLED
 * 
 * @note This method should be called in the main loop of the program as frequent as possible to assure the correct timing.
 */
void blinkLEDTask();

#endif