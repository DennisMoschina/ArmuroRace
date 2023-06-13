#include "lineFollow.h"

#include "armuro.h"
#include "pidController.h"
#include "tim.h"

#define BLACK_THRESHOLD 900
#define WHITE_THRESHOLD 200

PIDConfig followLinePID;

uint32_t lineFollowTimeout = 0;
int baseLineSpeed = 0;

int lastState = 0;

void followLine(int speed) {
    baseLineSpeed = speed;
    followLinePID = initPID(0.001 * speed, 0, 0, 0, 0);
    lineFollowTimeout = HAL_GetTick();
}

int followLineTask() {
    if (HAL_GetTick() < lineFollowTimeout) { return lastState; }
    lineFollowTimeout = HAL_GetTick() + 100;

    uint32_t left;
    uint32_t right;
    uint32_t middle;
    getLineSensorReadings(&left, &middle, &right);

    if (left < WHITE_THRESHOLD && right < WHITE_THRESHOLD && middle < WHITE_THRESHOLD) {
        lastState = -1;
        return -1;
    } else if (left > BLACK_THRESHOLD && right > BLACK_THRESHOLD && middle > BLACK_THRESHOLD) {
        lastState = 1;
        return 1;
    }

    int error = right - left;
    int speedAdjustement = calculatePIDOutput(0, error, &followLinePID);

    turnMotor(RIGHT, FORWARD, baseLineSpeed + speedAdjustement);
    turnMotor(LEFT, FORWARD, baseLineSpeed - speedAdjustement);

    lastState = 0;
    return 0;
}