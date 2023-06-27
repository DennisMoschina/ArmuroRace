#include "lineFollow.h"

#include "armuro.h"
#include "pidController.h"
#include "tim.h"
#include "wheels.h"
#include "stdlib.h"

#define BLACK_THRESHOLD 800
#define WHITE_THRESHOLD 400

PIDConfig followLinePID;

uint32_t lineFollowTimeout = 0;
int baseLineSpeed = 0;

int lastState = 0;

uint32_t lastLineValues[3] = {0, 0, 0};

WheelAngle* wheelAngle;

void followLine(int speed) {
    baseLineSpeed = speed;
    followLinePID = initPID(0.001 * speed, 0, 0, 0, 0);
    lineFollowTimeout = HAL_GetTick();
}

FollowLineResult followLineTask() {
    if (HAL_GetTick() < lineFollowTimeout) { return lastState; }
    lineFollowTimeout = HAL_GetTick() + 100;

    uint32_t left;
    uint32_t right;
    uint32_t middle;
    getLineSensorReadings(&left, &middle, &right);

    CheckLineResult checkForLineResult = checkForLine();

    if (checkForLineResult == OFF_LINE) {
        lastState = checkForLineResult;
        return LOST_LINE;
    }

    int error = right - left;
    int speedAdjustement = calculatePIDOutput(0, error, &followLinePID);

    turnMotor(RIGHT, FORWARD, baseLineSpeed + speedAdjustement);
    turnMotor(LEFT, FORWARD, baseLineSpeed - speedAdjustement);

    lastState = 0;
    return 0;
}

CheckLineResult checkForLine() {
    uint32_t left;
    uint32_t right;
    uint32_t middle;
    getLineSensorReadings(&left, &middle, &right);

    lastLineValues[0] = left;
    lastLineValues[1] = middle;
    lastLineValues[2] = right;

    print("left: %d, middle: %d, right: %d\n", left, middle, right);

    if (left < WHITE_THRESHOLD && right < WHITE_THRESHOLD && middle < WHITE_THRESHOLD) {
        print("all white\n");
        return OFF_LINE;
    } else if (left > BLACK_THRESHOLD && right > BLACK_THRESHOLD && middle > BLACK_THRESHOLD) {
        print("all black\n");
        return ALL_BLACK;
    }

    return ON_LINE;
}

void searchLine() {
    lineFollowTimeout = HAL_GetTick();
    lastState = 0;
    wheelAngle = startAngleMeasurement();
}

SearchLineResult searchLineTask() {
    if (HAL_GetTick() < lineFollowTimeout) { return lastState; }
    lineFollowTimeout = HAL_GetTick() + 100;

    turnWheelsSynchronized(-50, 50);

    CheckLineResult checkForLineResult = checkForLine();
    int armuroAngle = 360 * angleToDistance(wheelAngle->right) / TURN_CIRCUMFERENCE;
    print("armuro angle: %d, rightWheelAngle: %d\n", armuroAngle, wheelAngle->right);
    switch (checkForLineResult) {
        case ON_LINE:
        case ALL_BLACK:
            print("detected line at angle: %d\n", armuroAngle);
            if (abs(armuroAngle - 180) < MIN_ANGLE) {
                return SEARCHING;
            } else {
                stopMotor(RIGHT);
                stopMotor(LEFT);
                lastState = FOUND;
                stopAngleMeasurement(wheelAngle);
                return FOUND;
            }
        case OFF_LINE:
            if (abs(armuroAngle - 360) < MIN_ANGLE) {
                stopMotor(RIGHT);
                stopMotor(LEFT);
                return END_OF_LINE;
            }
            lastState = SEARCHING;
            return SEARCHING;
    }
    return SEARCHING;
}