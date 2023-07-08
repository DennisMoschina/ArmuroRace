#include "blinkLED.h"

#include "tim.h"
#include "gpio.h"

enum LEDState {
    ON,
    OFF
};

uint16_t ledTimeInterval[] = {0, 0};
uint32_t taskLEDTimeout[] = {-1, -1};

enum LEDState ledState[] = {OFF, OFF};

void blinkLED(Side side, uint16_t timeInterval) {
    ledTimeInterval[side] = timeInterval;
    taskLEDTimeout[side] = HAL_GetTick() + timeInterval;
    ledState[side] = ON;
}

void stopBlinkingLED(Side side) {
    ledTimeInterval[side] = 0;
    taskLEDTimeout[side] = -1;
    ledState[side] = OFF;
    setLED(side, OFF);
}

void blinkLEDTask() {
    for (uint8_t i = 0; i < 2; i++) {
        int now = HAL_GetTick();
        if (now < taskLEDTimeout[i]) { return; }
        taskLEDTimeout[i] = now + ledTimeInterval[i];
        switch (ledState[i]) {
        case OFF:
            setLED(i, ON);
            ledState[i] = ON;
            break;
        case ON:
            setLED(i, OFF);
            ledState[i] = OFF;
            break;
        default:
            break;
        }
    }
}