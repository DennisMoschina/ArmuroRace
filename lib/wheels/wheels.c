#include "wheels.h"
#include "usart.h"

#include "pidController.h"

#include <stdlib.h>

#define MAX_ROTATION_RATE 900

TurnWheelsTaskType turningWheels[] = {NONE, NONE};
int angleSetpoint[] = {0, 0};
int speedSetpoint[] = {0, 0};
int rotationRateSetpoint[] = {0, 0};

int oldAngle[] = {0, 0};
uint32_t angleTimeout[] = {0, 0};
PIDConfig wheelsPID[2];

PIDConfig synchronizeWheelsPID;
uint32_t synchronizeWheelsTimeout = 0;

void turnWheelByAngle(int wheel, int angle, int speed) {
    angleSetpoint[wheel] = angle;
    turningWheels[wheel] = ANGLE;
    speedSetpoint[wheel] = speed;
    resetAngleMeasurement(wheel);
}

void turnWheelByAngleInTime(int wheel, int angle, int time) {
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

void turnWheelWithSpeed(int wheel, int speed) {
    turningWheels[wheel] = SPEED;
    speedSetpoint[wheel] = speed;
}

void turnWheelsSynchronized(int leftSpeed, int rightSpeed) {
    turningWheels[LEFT] = SYNCHRONIZED;
    turningWheels[RIGHT] = SYNCHRONIZED;

    speedSetpoint[LEFT] = leftSpeed;
    speedSetpoint[RIGHT] = rightSpeed;

    resetAngleMeasurement(LEFT);
    resetAngleMeasurement(RIGHT);

    synchronizeWheelsPID = initPID(0.05, 0.02, 0.02, 100, 0.9);
    synchronizeWheelsTimeout = HAL_GetTick() + 500;

    turnMotor(LEFT, FORWARD, leftSpeed);
    turnMotor(RIGHT, FORWARD, rightSpeed);
}

void turnWheelsSynchronizedByAngle(int leftSpeed, int rightSpeed, int rightAngle) {
    turningWheels[LEFT] = SYNCHRONIZED_ANGLE;
    turningWheels[RIGHT] = SYNCHRONIZED_ANGLE;

    speedSetpoint[LEFT] = leftSpeed;
    speedSetpoint[RIGHT] = rightSpeed;
    angleSetpoint[RIGHT] = rightAngle;

    resetAngleMeasurement(LEFT);
    resetAngleMeasurement(RIGHT);

    synchronizeWheelsPID = initPID(0.05, 0.02, 0.02, 100, 0.9);
    synchronizeWheelsTimeout = HAL_GetTick() + 100;

    turnMotor(LEFT, FORWARD, leftSpeed);
    turnMotor(RIGHT, FORWARD, rightSpeed);
}

void turnArmuroInTime(int angle, int time) {
    print("Turning armuro by %d degrees in %d ms\n", angle, time);
    double distanceOnCircumference = angle / 360.0 * TURN_CIRCUMFERENCE;

    turnWheelByAngleInTime(RIGHT, distanceToAngle(distanceOnCircumference), time);
    turnWheelByAngleInTime(LEFT, -distanceToAngle(distanceOnCircumference), time);
}

void turnArmuro(int angle, int speed) {
    double distanceOnCircumference = angle / 360.0 * TURN_CIRCUMFERENCE;

    if (angle > 0) {
        turnWheelsSynchronizedByAngle(-speed, speed, distanceToAngle(distanceOnCircumference));
    } else {
        turnWheelsSynchronizedByAngle(speed, -speed, distanceToAngle(abs(distanceOnCircumference)));
    }
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
            default:
                break;
        }
    }

    return turningWheels;
}

void turnWheelByAngleTask(int wheel) {
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
    if (angleSetpoint[RIGHT] <= 0) {
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
    int speedDifference = speedSetpoint[LEFT] - speedSetpoint[RIGHT];

    int output = calculatePIDOutput(speedDifference, angleDifference, &synchronizeWheelsPID);
    int speedOutput = speedSetpoint[RIGHT] - output;

    turnMotor(RIGHT, FORWARD, speedOutput);

    angleSetpoint[RIGHT] -= rightAngle;
    resetAngleMeasurement(RIGHT);
    resetAngleMeasurement(LEFT);
}

void turnWheelByAngleInTimeTask(int wheel) {
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
