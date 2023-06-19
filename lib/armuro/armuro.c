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

WheelAngle* wheelAngles;
size_t wheelAnglesCount = 0;
size_t wheelAnglesCapacity = 0;

inline void print(char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[strlen(format) * 2];
    vsprintf(buffer, format, args);
    va_end(args);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);
}

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
            for (int i = 0; i < wheelAnglesCount; i++) {
                wheelAngles[i].leftTicks++;
                wheelAngles[i].left = wheelAngles[i].leftTicks * MIN_ANGLE;
            }
        }
    }
    if (right != -1) {
        if (wheelEncoderOldValues[RIGHT] != right) {
            wheelEncoderOldValues[RIGHT] = right;
            wheelEncoderTicksCount[RIGHT]++;
            for (int i = 0; i < wheelAnglesCount; i++) {
                wheelAngles[i].rightTicks++;
                wheelAngles[i].right = wheelAngles[i].rightTicks * MIN_ANGLE;
            }
        }
    }
}

void resetAngleMeasurement(Side wheel) {
    wheelEncoderTicksCount[wheel] = 0;
}

WheelAngle* startAngleMeasurement() {
    if (wheelAnglesCapacity == wheelAnglesCount) {
        wheelAnglesCapacity *= 2;
        wheelAngles = realloc(wheelAngles, wheelAnglesCapacity * sizeof(WheelAngle));
    }
    wheelAnglesCount++;
    wheelAngles[wheelAnglesCount] = (WheelAngle) {
        .left = 0,
        .right = 0,
        .leftTicks = 0,
        .rightTicks = 0
    };
    return &wheelAngles[wheelAnglesCount];
}

void stopAngleMeasurement(WheelAngle* angle) {
    for (int i = 0; i < wheelAnglesCount; i++) {
        if (&wheelAngles[i] == angle) {
            for (int j = i; j < wheelAnglesCount - 1; j++) {
                wheelAngles[j] = wheelAngles[j + 1];
            }
            wheelAnglesCount--;
            break;
        }
    }

    free(angle);

    if (wheelAnglesCount < 0.25 * wheelAnglesCapacity) {
        wheelAnglesCapacity /= 2;
        wheelAngles = realloc(wheelAngles, wheelAnglesCapacity * sizeof(WheelAngle));
    }
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