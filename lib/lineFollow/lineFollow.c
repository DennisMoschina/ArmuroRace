#include "lineFollow.h"

#include "armuro.h"
#include "pidController.h"
#include "tim.h"
#include "wheels.h"
#include "blinkLED.h"
#include "stdlib.h"

#include "stateMachine.h"

#define BLACK_THRESHOLD 900
#define WHITE_THRESHOLD 400

PIDConfig followLinePID;

uint32_t lineFollowTimeout = 0;
int baseLineSpeed = 0;

int lastState = 0;

CheckLineResult lastLineValues[3] = {OFF_LINE, OFF_LINE, OFF_LINE};

typedef enum SearchLineState {
    DRIVE,
    TURNING_LEFT,
    TURNING_RIGHT,
    TURN_LEFT_TO_RIGHT,
    TURN_RIGHT_TO_LEFT,
    TURN_RIGHT_TO_MIDDLE,
    TURN_LEFT_TO_MIDDLE,
    DONE
} SearchLineState;

SearchLineState searchLineState = TURNING_LEFT;
SearchLineState nextSearchState = TURNING_LEFT;
State searchLineStateState = READY;


void followLine(int speed) {
    baseLineSpeed = speed;
    followLinePID = initPID(0.0006 * speed, 0, 0, 0, 0);
    lineFollowTimeout = HAL_GetTick();
}

FollowLineResult followLineTask() {
    if (HAL_GetTick() < lineFollowTimeout) { return lastState; }
    lineFollowTimeout = HAL_GetTick() + 100;

    uint32_t left;
    uint32_t right;
    uint32_t middle;
    getLineSensorReadings(&left, &middle, &right);
    lastLineValues[LEFT] = left < WHITE_THRESHOLD ? OFF_LINE : (left > BLACK_THRESHOLD ? ALL_BLACK : ON_LINE);
    lastLineValues[RIGHT] = right < WHITE_THRESHOLD ? OFF_LINE : (right > BLACK_THRESHOLD ? ALL_BLACK : ON_LINE);;

    CheckLineResult checkForLineResult = checkForLine();

    if (checkForLineResult == OFF_LINE) {
        lastState = checkForLineResult;
        return LOST_LINE;
    }

    int error = right - left;
    int speedAdjustement = calculatePIDOutput(0, error, &followLinePID);

    turnMotor(RIGHT, FORWARD, baseLineSpeed + speedAdjustement);
    turnMotor(LEFT, FORWARD, baseLineSpeed - speedAdjustement);

    lastState = FOLLOWING;
    return FOLLOWING;
}

CheckLineResult checkForLine() {
    uint32_t left;
    uint32_t right;
    uint32_t middle;
    getLineSensorReadings(&left, &middle, &right);

    lastLineValues[LEFT] = left < WHITE_THRESHOLD ? OFF_LINE : (left > BLACK_THRESHOLD ? ALL_BLACK : ON_LINE);
    // lastLineValues[1] = ;
    lastLineValues[RIGHT] = right < WHITE_THRESHOLD ? OFF_LINE : (right > BLACK_THRESHOLD ? ALL_BLACK : ON_LINE);;

    print("left: %lu, middle: %lu, right: %lu\n", left, middle, right);

    if (left < WHITE_THRESHOLD && right < WHITE_THRESHOLD && middle < WHITE_THRESHOLD) {
        print("all white\n");
        return OFF_LINE;
    } else if (left > BLACK_THRESHOLD && right > BLACK_THRESHOLD && middle > BLACK_THRESHOLD) {
        print("all black\n");
        return ALL_BLACK;
    }

    print("on line\n");
    return ON_LINE;
}

void searchLine() {
    lineFollowTimeout = HAL_GetTick();
    lastState = 0;
    if (lastLineValues[LEFT] == ALL_BLACK) {
        nextSearchState = TURNING_LEFT;
    } else if (lastLineValues[RIGHT] == ALL_BLACK) {
        nextSearchState = TURNING_RIGHT;
    } else if (lastLineValues[LEFT] == ON_LINE) {
        nextSearchState = TURNING_LEFT;
    } else {
        nextSearchState = TURNING_RIGHT;
    }
    searchLineState = DRIVE;
    searchLineStateState = READY;
}

SearchLineResult searchLineTask() {
    TurnWheelsTaskType* turnWheelsTaskType = turnWheelsTask();
    if (turnWheelsTaskType[0] == NONE && turnWheelsTaskType[1] == NONE) {
        searchLineStateState = FINISHED;
    }

    if (HAL_GetTick() < lineFollowTimeout) { return lastState; }
    lineFollowTimeout = HAL_GetTick() + 100;

    if (searchLineStateState == FINISHED) {
        searchLineState = nextSearchState;
        searchLineStateState = READY;
    }
    if (searchLineStateState == READY) {
        switch (searchLineState) {
            case DRIVE:
                print("driving\n");
                turnWheelsSynchronizedByAngle(30, 30, distanceToAngle(2), 0);
                setLED(RIGHT, LOW);
                setLED(LEFT, LOW);
                break;
            case TURNING_LEFT:
                print("turning left\n");
                turnArmuro(90);
                setLED(RIGHT, LOW);
                setLED(LEFT, HIGH);
                nextSearchState = TURN_LEFT_TO_RIGHT;
                break;
            case TURN_RIGHT_TO_LEFT:
                print("turning right to left\n");
                turnArmuro(180);
                setLED(RIGHT, LOW);
                setLED(LEFT, HIGH);
                nextSearchState = TURN_LEFT_TO_MIDDLE;
                break;
            case TURN_LEFT_TO_MIDDLE:
                print("turning left to middle\n");
                turnArmuro(-90);
                setLED(RIGHT, HIGH);
                setLED(LEFT, LOW);
                nextSearchState = DONE;
                break;
            case TURNING_RIGHT:
                print("turning right\n");
                turnArmuro(-90);
                setLED(RIGHT, HIGH);
                setLED(LEFT, LOW);
                nextSearchState = TURN_RIGHT_TO_LEFT;
                break;
            case TURN_LEFT_TO_RIGHT:
                print("turning left to right\n");
                turnArmuro(-180);
                setLED(RIGHT, HIGH);
                setLED(LEFT, LOW);
                nextSearchState = TURN_RIGHT_TO_MIDDLE;
                break;
            case TURN_RIGHT_TO_MIDDLE:
                print("turning right to middle\n");
                turnArmuro(90);
                setLED(RIGHT, LOW);
                setLED(LEFT, HIGH);
                nextSearchState = DONE;
                break;
            case DONE:
                print("done lost line\n");
                setLED(RIGHT, LOW);
                setLED(LEFT, LOW);
                return END_OF_LINE;
        }
        searchLineStateState = RUNNING;
    }

    CheckLineResult checkForLineResult = checkForLine();
    switch (checkForLineResult) {
        case ON_LINE:
        case ALL_BLACK:
            print("found line after search\n");
            stopWheel(RIGHT);
            stopWheel(LEFT);
            lastState = FOUND;
            setLED(RIGHT, HIGH);
            setLED(LEFT, HIGH);
            return FOUND;
        case OFF_LINE:
            return SEARCHING;
    }
    return SEARCHING;
}