#include "armuro.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define RIGHT_ENCODER_HIGH_THRESHOLD 2000
#define RIGHT_ENCODER_LOW_THRESHOLD 1000    

#define LEFT_ENCODER_HIGH_THRESHOLD 3300
#define LEFT_ENCODER_LOW_THRESHOLD 3000

#define MAX_PWM (1 << 16) - 1

uint16_t wheelEncoderTicksCount[2];
int wheelEncoderOldValues[2];

typedef struct WheelAngleListItem {
    WheelAngle angle;
    struct WheelAngleListItem* next;
    struct WheelAngleListItem* prev;
} WheelAngleListItem;

WheelAngleListItem* wheelAngleList = NULL;
WheelAngleListItem* wheelAngleListEnd = NULL;

inline void print(char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[strlen(format) * 2];
    vsprintf(buffer, format, args);
    va_end(args);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);
}

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
  int32_t result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return result < out_min ? out_min : result > out_max ? out_max : result;
}

void initMotors() {
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
}

void turnMotor(Side motor, int direction, int speed) {
    if (speed > 100) { speed = 100; }
    else if (speed < -100) { speed = -100; }
    if (speed < 0) {
        speed = -speed;
        direction = (direction == FORWARD) ? BACKWARD : FORWARD;
    }

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

void stopMotor(Side motor) {
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

void setLED(Side led, int state) {
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

/**
 * @brief Check if the value is above the high threshold or below the low threshold
 * 
 * @param side 
 * @param value 
 * @return 1 for HIGH, 0 for LOW, -1 for invalid
 */
int schmittTrigger(Side side, u_int32_t value) {
    if (side == LEFT) {
        if (value > LEFT_ENCODER_HIGH_THRESHOLD) {
            return HIGH;
        } else if (value < LEFT_ENCODER_LOW_THRESHOLD) {
            return LOW;
        }
    } else if (side == RIGHT) {
        if (value > RIGHT_ENCODER_HIGH_THRESHOLD) {
            return HIGH;
        } else if (value < RIGHT_ENCODER_LOW_THRESHOLD) {
            return LOW;
        }
    }
    return -1;
}

void didReadWheelEncoder(uint32_t leftValue, uint32_t rightValue) {
    int left = schmittTrigger(LEFT, leftValue);
    int right = schmittTrigger(RIGHT, rightValue);

    if (left != -1) {
        if (wheelEncoderOldValues[LEFT] != left) {
            wheelEncoderOldValues[LEFT] = left;
            wheelEncoderTicksCount[LEFT]++;
            WheelAngleListItem* current = wheelAngleList;
            while (current != NULL) {
                current->angle.leftTicks++;
                current->angle.left = current->angle.leftTicks * MIN_ANGLE;
                current = current->next;
            }
        }
    }
    if (right != -1) {
        if (wheelEncoderOldValues[RIGHT] != right) {
            wheelEncoderOldValues[RIGHT] = right;
            wheelEncoderTicksCount[RIGHT]++;
            WheelAngleListItem* current = wheelAngleList;
            while (current != NULL) {
                current->angle.rightTicks++;
                current->angle.right = current->angle.rightTicks * MIN_ANGLE;
                current = current->next;
            }
        }
    }
}

void resetAngleMeasurement(Side wheel) {
    wheelEncoderTicksCount[wheel] = 0;
}

WheelAngle* startAngleMeasurement() {
    WheelAngleListItem* newItem = malloc(sizeof(WheelAngleListItem));
    newItem->next = NULL;
    newItem->prev = NULL;
    newItem->angle.left = 0;
    newItem->angle.right = 0;
    newItem->angle.leftTicks = 0;
    newItem->angle.rightTicks = 0;

    if (wheelAngleList == NULL) {
        wheelAngleList = newItem;
        wheelAngleListEnd = newItem;
    } else {
        wheelAngleListEnd->next = newItem;
        newItem->prev = wheelAngleListEnd;
        wheelAngleListEnd = newItem;
    }
    
    return &newItem->angle;
}

void stopAngleMeasurement(WheelAngle* angle) {
    WheelAngleListItem* itemToRemove = NULL;
    if (&wheelAngleList->angle == angle) {
        itemToRemove = wheelAngleList;
        wheelAngleList = wheelAngleList->next;
        wheelAngleList->prev = NULL;
    } else {
        WheelAngleListItem* current = wheelAngleList->next;
        while (current != NULL) {
            if (&wheelAngleList->angle == angle) {
                itemToRemove = current;
                current->prev->next = current->next;
                current->next->prev = current->prev;
                break;
            }
        }
    }
    free(itemToRemove);
}

int getAngleForWheel(Side wheel) {
    return wheelEncoderTicksCount[wheel] * MIN_ANGLE;
}

void getAngleForWheels(int* leftAngle, int* rightAngle) {
    *leftAngle = getAngleForWheel(LEFT);
    *rightAngle = getAngleForWheel(RIGHT);
}

void getRawLineSensorReadings(uint32_t* left, uint32_t* middle, uint32_t* right) {
    *left = buffer[5];
    *middle = buffer[0];
    *right = buffer[2];
}

void getLineSensorReadings(uint32_t* left, uint32_t* middle, uint32_t* right) {
    *left = map(buffer[5], MIN_LEFT_LINE_SENSOR_VALUE, MAX_LEFT_LINE_SENSOR_VALUE, 0, 1023);
    *middle = map(buffer[0], MIN_MIDDLE_LINE_SENSOR_VALUE, MAX_MIDDLE_LINE_SENSOR_VALUE, 0, 1023);
    *right = map(buffer[2], MIN_RIGHT_LINE_SENSOR_VALUE, MAX_RIGHT_LINE_SENSOR_VALUE, 0, 1023);
}

uint32_t mapLineSensorReadingToRange(uint32_t value, Side side) {
    switch (side) {
        case LEFT:
            return map(value, MIN_LEFT_LINE_SENSOR_VALUE, MAX_LEFT_LINE_SENSOR_VALUE, 0, 1023);
        case MIDDLE:
            return map(value, MIN_MIDDLE_LINE_SENSOR_VALUE, MAX_MIDDLE_LINE_SENSOR_VALUE, 0, 1023);
        case RIGHT:
            return map(value, MIN_RIGHT_LINE_SENSOR_VALUE, MAX_RIGHT_LINE_SENSOR_VALUE, 0, 1023);
        default:
            return 0;
    }
}

int distanceToAngle(double distance) {
    return 360 * distance / WHEEL_CIRCUMFERENCE;
}

double angleToDistance(int angle) {
    return (angle * WHEEL_CIRCUMFERENCE) / 360;
}