#ifndef _BLINK_LED_H_
#define _BLINK_LED_H_

#include "armuro.h"

void blinkLED(Side side, uint16_t timeInterval);
void stopBlinkingLED(Side side);

void blinkLEDTask();

#endif