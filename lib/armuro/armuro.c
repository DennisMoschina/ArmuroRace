#include "armuro.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#define RIGHT_ENCODER_HIGH_THRESHOLD 2000
#define RIGHT_ENCODER_LOW_THRESHOLD 1000    

#define LEFT_ENCODER_HIGH_THRESHOLD 3300
#define LEFT_ENCODER_LOW_THRESHOLD 3100

#define MAX_PWM (1 << 16) - 1

uint16_t wheelEncoderTicksCount[2];
int wheelEncoderOldValues[2];

inline void print(char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[100];
    vsprintf(buffer, format, args);
    va_end(args);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);
}

void initMotors() {
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
}

void turnMotor(int motor, int direction, int speed) {
    switch (motor) {
    case RIGHT:
        switch (direction) {
            case BACKWARD:
                TIM1->CCR2 = ((double) (100 - speed) / (double) 100) * MAX_PWM;
                HAL_GPIO_WritePin(phase2_L_GPIO_Port, phase2_L_Pin, GPIO_PIN_SET);
                break;
            case FORWARD:
                TIM1->CCR2 = ((double) speed / (double) 100) * MAX_PWM;
                HAL_GPIO_WritePin(phase2_L_GPIO_Port, phase2_L_Pin, GPIO_PIN_RESET);
                break;
            default:
                break;
        }
        break;
    case LEFT:
        switch (direction) {
            case FORWARD:
                TIM1->CCR3 = ((double) (100 - speed) / (double) 100) * MAX_PWM;;
                HAL_GPIO_WritePin(phase2_R_GPIO_Port, phase2_R_Pin, GPIO_PIN_SET);
                break;
            case BACKWARD:
                TIM1->CCR3 = ((double) speed / (double) 100) * MAX_PWM;
                HAL_GPIO_WritePin(phase2_R_GPIO_Port, phase2_R_Pin, GPIO_PIN_RESET);
                break;
            default:
                break;
        }
        break;
    default:
        break;
    }
}

void stopMotor(int motor) {
    switch (motor) {
        case RIGHT:
            turnMotor(RIGHT, FORWARD, 0);
            break;
        case LEFT:
            turnMotor(LEFT, FORWARD, 0);
            break;
        default:
            break;
    }
}

void setLED(int led, int state) {
    switch (led) {
        case RIGHT:
            HAL_GPIO_WritePin(LED_right_GPIO_Port, LED_right_Pin, state);
            break;
        case LEFT:
            HAL_GPIO_WritePin(LED_left_GPIO_Port, LED_left_Pin, state);
            break;
        default:
            break;
    }
}

void setRearLED(int state) {
    HAL_GPIO_WritePin(LED_right_GPIO_Port, LED_rear_Pin, state);
}

void didReadSensors(uint32_t* values) {
    didReadWheelEncoder(values[1], values[4]);
}

void didReadWheelEncoder(uint32_t leftValue, uint32_t rightValue) {
    if (leftValue > LEFT_ENCODER_HIGH_THRESHOLD) {
        if (wheelEncoderOldValues[LEFT] == LOW) {
            wheelEncoderTicksCount[LEFT]++;
        }
        wheelEncoderOldValues[LEFT] = HIGH;
    } else if (leftValue < LEFT_ENCODER_LOW_THRESHOLD) {
        wheelEncoderOldValues[LEFT] = LOW;
    }
    if (rightValue > RIGHT_ENCODER_HIGH_THRESHOLD) {
        if (wheelEncoderOldValues[RIGHT] == LOW) {
            wheelEncoderTicksCount[RIGHT]++;
        }
        wheelEncoderOldValues[RIGHT] = HIGH;
    } else if (rightValue < RIGHT_ENCODER_LOW_THRESHOLD) {
        wheelEncoderOldValues[RIGHT] = LOW;
    }
}

void resetAngleMeasurement(int wheel) {
    wheelEncoderTicksCount[wheel] = 0;
}

int getAngleForWheel(int wheel) {
    return wheelEncoderTicksCount[wheel] * 30;
}

void getAngleForWheels(int* leftAngle, int* rightAngle) {
    *leftAngle = getAngleForWheel(LEFT);
    *rightAngle = getAngleForWheel(RIGHT);
}