#include "blinkLED.h"

#include "tim.h"
#include "gpio.h"

enum LEDState {
    ON,
    OFF
};

enum LEDState ledState = OFF;
int taskLED_timeout = 0;

void setupLED() {
    taskLED_timeout = HAL_GetTick() + 500;
}

void blinkLED() {
    int now = HAL_GetTick();
    if (now < taskLED_timeout) {
        return;
    }
    switch (ledState) {
    case OFF:
        HAL_GPIO_WritePin(LED_left_GPIO_Port, LED_left_Pin, GPIO_PIN_SET);
        ledState = ON;
        break;
    case ON:
        HAL_GPIO_WritePin(LED_left_GPIO_Port, LED_left_Pin, GPIO_PIN_RESET);
        ledState = OFF;
        break;
    default:
        break;
    }
    setupLED();
}