#include "lineFollow.h"

#include "armuro.h"
#include "pidController.h"
#include "tim.h"

PIDConfig followLinePID;

uint32_t lineFollowTimeout = 0;
int baseLineSpeed = 0;

void followLine(int speed) {
    baseLineSpeed = speed;
    followLinePID = initPID(0.03, 0, 0, 0, 0);
    lineFollowTimeout = HAL_GetTick();
}

int followLineTask() {
    if (HAL_GetTick() < lineFollowTimeout) { return 0; }
    lineFollowTimeout = HAL_GetTick() + 100;

    uint32_t left;
    uint32_t right;
    uint32_t middle;
    getLineSensorReadings(&left, &middle, &right);

    int error = right - left;
    int speedAdjustement = calculatePIDOutput(0, error, &followLinePID);

    turnMotor(RIGHT, FORWARD, baseLineSpeed + speedAdjustement);
    turnMotor(LEFT, FORWARD, baseLineSpeed - speedAdjustement);

    return 0;
}