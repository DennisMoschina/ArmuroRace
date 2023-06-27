#include "wheels.h"
#include "usart.h"

#include "pidController.h"

#include <stdlib.h>

#define MAX_ROTATION_RATE 900

TurnWheelsTaskType turningWheels[] = {NONE, NONE};
int angleSetpoint[] = {0, 0};
int speedSetpoint[] = {0, 0};
int currentSpeedSetpoint[] = {0, 0};
int rotationRateSetpoint[] = {0, 0};

int oldAngle[] = {0, 0};
uint32_t angleTimeout[] = {0, 0};
PIDConfig wheelsPID[2];

PIDConfig synchronizeWheelsPID;
uint32_t synchronizeWheelsTimeout = 0;

void turnWheelByAngle(Side wheel, int angle, int speed) {
    angleSetpoint[wheel] = angle;
    turningWheels[wheel] = ANGLE;
    speedSetpoint[wheel] = speed;
    resetAngleMeasurement(wheel);
}

void turnWheelByAngleInTime(Side wheel, int angle, int time) {
    angleSetpoint[wheel] = angle;
    turningWheels[wheel] = TIMED_ANGLE;
    rotationRateSetpoint[wheel] = (int) (angle / (time / 1000.0));
    if (rotationRateSetpoint[wheel] > MAX_ROTATION_RATE) {
        rotationRateSetpoint[wheel] = MAX_ROTATION_RATE;
    } else if (rotationRateSetpoint[wheel] < -MAX_ROTATION_RATE) {
        rotationRateSetpoint[wheel] = -MAX_ROTATION_RATE;
    }

    wheelsPID[wheel] = initPID(0.05, 0.02, 0.02, 100, 0.9);

    resetAngleMeasurement(wheel);
    oldAngle[wheel] = 0;
    angleTimeout[wheel] = HAL_GetTick();
}

void turnWheelWithSpeed(Side wheel, int speed) {
    turningWheels[wheel] = SPEED;
    speedSetpoint[wheel] = speed;
}

void setupSynchronizedPID() {
    synchronizeWheelsPID = initPID(0.05, 0.02, 0.03, 100, 1);
}

void turnWheelsSynchronized(int leftSpeed, int rightSpeed) {
    turningWheels[LEFT] = SYNCHRONIZED;
    turningWheels[RIGHT] = SYNCHRONIZED;

    speedSetpoint[LEFT] = leftSpeed;
    speedSetpoint[RIGHT] = rightSpeed;

    resetAngleMeasurement(LEFT);
    resetAngleMeasurement(RIGHT);

    setupSynchronizedPID();
    synchronizeWheelsTimeout = HAL_GetTick() + 500;

    turnMotor(LEFT, FORWARD, leftSpeed);
    turnMotor(RIGHT, FORWARD, rightSpeed);
}

void turnWheelsSynchronizedByAngle(int leftSpeed, int rightSpeed, int rightAngle, uint8_t softStart) {
    turningWheels[LEFT] = SYNCHRONIZED_ANGLE;
    turningWheels[RIGHT] = SYNCHRONIZED_ANGLE;

    speedSetpoint[LEFT] = leftSpeed;
    speedSetpoint[RIGHT] = rightSpeed;
    angleSetpoint[RIGHT] = rightAngle;

    if (softStart) {
        currentSpeedSetpoint[LEFT] = 0.1 * leftSpeed;
        currentSpeedSetpoint[RIGHT] = 0.1 * rightSpeed;
    } else {
        currentSpeedSetpoint[LEFT] = leftSpeed;
        currentSpeedSetpoint[RIGHT] = rightSpeed;
    }

    resetAngleMeasurement(LEFT);
    resetAngleMeasurement(RIGHT);

    setupSynchronizedPID();
    synchronizeWheelsTimeout = HAL_GetTick() + 100;

    turnMotor(LEFT, FORWARD, currentSpeedSetpoint[LEFT]);
    turnMotor(RIGHT, FORWARD, currentSpeedSetpoint[RIGHT]);
}

void turnArmuroInTime(int angle, int time) {
    print("Turning armuro by %d degrees in %d ms\n", angle, time);
    double distanceOnCircumference = angle / 360.0 * TURN_CIRCUMFERENCE;

    turnWheelByAngleInTime(RIGHT, distanceToAngle(distanceOnCircumference), time);
    turnWheelByAngleInTime(LEFT, -distanceToAngle(distanceOnCircumference), time);
}

void turnArmuro(int angle) {
    double distanceOnCircumference = angle / 360.0 * TURN_CIRCUMFERENCE;

    angleSetpoint[RIGHT] = distanceToAngle(distanceOnCircumference);
    angleSetpoint[LEFT] = -distanceToAngle(distanceOnCircumference);

    // in this task the speedSetpoint is used to store the direction of an angle
    speedSetpoint[RIGHT] = distanceOnCircumference;
    speedSetpoint[LEFT] = distanceOnCircumference;

    resetAngleMeasurement(LEFT);
    resetAngleMeasurement(RIGHT);

    turningWheels[RIGHT] = TURN;
    turningWheels[LEFT] = TURN;

    wheelsPID[RIGHT] = initPID(1, 0, 0, 100, 0.9);
    wheelsPID[LEFT] = initPID(1, 0, 0, 100, 0.9);

    angleTimeout[RIGHT] = HAL_GetTick();
    angleTimeout[LEFT] = HAL_GetTick();
}

TurnWheelsTaskType* turnWheelsTask() {
    if (turningWheels[LEFT] == SYNCHRONIZED && turningWheels[RIGHT] == SYNCHRONIZED) {
        turnWheelsSynchronizedTask();
        return turningWheels;
    }

    for (int i = 0; i < 2; i++) {
        switch (turningWheels[i]) {
            case NONE:
                break;
            case ANGLE:
                turnWheelByAngleTask(i);
                break;
            case SPEED:
                turnMotor(i, FORWARD, speedSetpoint[i]);
                break;
            case SYNCHRONIZED:
                turnWheelWithSpeed(i, speedSetpoint[i]);
                break;
            case TIMED_ANGLE:
                turnWheelByAngleInTimeTask(i);
                break;
            case SYNCHRONIZED_ANGLE:
                turnWheelsSynchronizedByAngleTask();
                break;
            case TURN:
                turnArmuroTask(i);
                break;
            default:
                break;
        }
    }

    return turningWheels;
}

void turnWheelByAngleTask(Side wheel) {
    int currentAngle = getAngleForWheel(wheel);

    print("Current angle for %s: %d; setpoint: %d\n", wheel == LEFT ? "LEFT" : "RIGHT", currentAngle, angleSetpoint[wheel]);

    if (angleSetpoint[wheel] > 0 && currentAngle < angleSetpoint[wheel]) {
        turnMotor(wheel, FORWARD, speedSetpoint[wheel]);
    } else if (angleSetpoint[wheel] < 0 && currentAngle < abs(angleSetpoint[wheel])) {
        turnMotor(wheel, BACKWARD, speedSetpoint[wheel]);
    } else {
        print("finished turning wheel %s\n", wheel == LEFT ? "LEFT" : "RIGHT");
        stopMotor(wheel);
        turningWheels[wheel] = NONE;
    }
}

void turnWheelsSynchronizedTask() {
    if (HAL_GetTick() < synchronizeWheelsTimeout) {
        return;
    }

    synchronizeWheelsTimeout = HAL_GetTick() + 500;

    turnMotor(LEFT, FORWARD, speedSetpoint[LEFT]);

    int leftAngle = getAngleForWheel(LEFT);
    int rightAngle = getAngleForWheel(RIGHT);

    int angleDifference = leftAngle - rightAngle;
    int speedDifference = speedSetpoint[LEFT] - speedSetpoint[RIGHT];

    int output = calculatePIDOutput(speedDifference, angleDifference, &synchronizeWheelsPID);
    int speedOutput = speedSetpoint[RIGHT] - output;

    print(
        "Left angle: %d, right angle: %d, angle difference: %d, speed difference: %d, output: %d, speed output: %d\n",
        leftAngle,
        rightAngle,
        angleDifference,
        speedDifference,
        output,
        speedOutput
    );

    turnMotor(RIGHT, FORWARD, speedOutput);

    resetAngleMeasurement(RIGHT);
    resetAngleMeasurement(LEFT);
}

void turnWheelsSynchronizedByAngleTask() {
    if (angleSetpoint[RIGHT] - getAngleForWheel(RIGHT) <= 0) {
        turningWheels[RIGHT] = NONE;
        turningWheels[LEFT] = NONE;

        stopMotor(RIGHT);
        stopMotor(LEFT);

        return;
    }
    if (HAL_GetTick() < synchronizeWheelsTimeout) {
        return;
    }
    synchronizeWheelsTimeout = HAL_GetTick() + 100;

    int leftAngle = getAngleForWheel(LEFT);
    int rightAngle = getAngleForWheel(RIGHT);

    int angleDifference = leftAngle - rightAngle;
    int speedDifference = abs(currentSpeedSetpoint[LEFT]) - abs(currentSpeedSetpoint[RIGHT]);

    int output = calculatePIDOutput(speedDifference, angleDifference, &synchronizeWheelsPID);
    int speedOutput = currentSpeedSetpoint[RIGHT] - output;

    turnMotor(RIGHT, FORWARD, speedOutput);
    turnMotor(LEFT, FORWARD, currentSpeedSetpoint[LEFT]);

    for (int i = 0; i < 2; i++) {
        if (currentSpeedSetpoint[i] != speedSetpoint[i]) {
            currentSpeedSetpoint[i] *= 1.5;
        }
        if (abs(currentSpeedSetpoint[i]) > abs(speedSetpoint[i])) {
            currentSpeedSetpoint[i] = speedSetpoint[i];
        }
    }

    angleSetpoint[RIGHT] -= rightAngle;
    resetAngleMeasurement(RIGHT);
    resetAngleMeasurement(LEFT);
}

void turnWheelByAngleInTimeTask(Side wheel) {
    if (HAL_GetTick() < angleTimeout[wheel]) { return; }
    angleTimeout[wheel] = angleTimeout[wheel] + 100;

    int currentAngle = getAngleForWheel(wheel);
    int currentRotationRate = (currentAngle - oldAngle[wheel]) * 10;
    oldAngle[wheel] = currentAngle;

    int rotationRateToPut = (int) calculatePIDOutput(rotationRateSetpoint[wheel], currentRotationRate, &wheelsPID[wheel]);

    speedSetpoint[wheel] = abs(map(rotationRateToPut, -MAX_ROTATION_RATE, MAX_ROTATION_RATE, -100, 100));

    print("Current angle for %s: %d; setpoint: %d; rotation rate: %d; rotation rate setpoint: %d; speed setpoint: %d\n",
        wheel == LEFT ? "LEFT" : "RIGHT",
        currentAngle,
        angleSetpoint[wheel],
        currentRotationRate,
        rotationRateSetpoint[wheel],
        speedSetpoint[wheel]);

    turnWheelByAngleTask(wheel);
}

void turnArmuroTask(Side wheel) {
    if (abs(abs(angleSetpoint[wheel]) - abs(getAngleForWheel(wheel))) <= 0.5 * MIN_ANGLE) {
        print("finished turning wheel %s with angle %d\n", wheel == LEFT ? "LEFT" : "RIGHT", getAngleForWheel(wheel));
        turningWheels[wheel] = NONE;
        stopMotor(wheel);
        return;
    }

    if (HAL_GetTick() < angleTimeout[wheel]) { return; }
    angleTimeout[wheel] = angleTimeout[wheel] + 100;

    int8_t angleSign = speedSetpoint[wheel] > 0 ? 1 : -1;
    print("calculating pid for wheel %s\n", wheel == LEFT ? "LEFT" : "RIGHT");
    int speed = calculatePIDOutput(angleSetpoint[wheel], angleSign * getAngleForWheel(wheel), &wheelsPID[wheel]);

    speedSetpoint[wheel] = speed;

    if (speed > 50) { speed = 50; }
    if (speed < -50) { speed = -50; }

    turnMotor(wheel, FORWARD, speed);
}