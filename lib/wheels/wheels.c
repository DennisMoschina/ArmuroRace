#include "wheels.h"
#include "usart.h"

#include "pidController.h"

#include <stdlib.h>

TurnWheelsTaskType turningWheels[] = {NONE, NONE};
int angleSetpoint[] = {0, 0};
int speedSetpoint[] = {0, 0};

PIDConfig synchronizeWheelsPID;
uint32_t synchronizeWheelsTimeout = 0;

void turnWheelByAngle(int wheel, int angle) {
    angleSetpoint[wheel] = angle;
    turningWheels[wheel] = ANGLE;
    resetAngleMeasurement(wheel);
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

    synchronizeWheelsPID = initPID(0.5, 0.1, 0.1, 1, 0.9);
    synchronizeWheelsTimeout = HAL_GetTick() + 500;

    turnMotor(LEFT, FORWARD, leftSpeed);
    turnMotor(RIGHT, FORWARD, rightSpeed);
}

TurnWheelsTaskType* turnWheelsTask() {
    if (turningWheels[LEFT] == SYNCHRONIZED && turningWheels[RIGHT] == SYNCHRONIZED) {
        turnWheelsSynchronizedTask();
        return turningWheels;
    }

    for (int i = 0; i < 2; i++) {
        switch (turningWheels[i]) {
            case ANGLE:
                turnWheelByAngleTask(i);
                break;
            case SPEED:
                turnMotor(i, FORWARD, speedSetpoint[i]);
                break;
            case SYNCHRONIZED:
                turnWheelWithSpeed(i, speedSetpoint[i]);
                break;
            default:
                break;
        }
    }

    return turningWheels;
}

void turnWheelByAngleTask(int wheel) {
    int currentAngle = getAngleForWheel(wheel);

    print("Current angle for %s: %d\n", wheel == LEFT ? "LEFT" : "RIGHT", currentAngle);

    if (angleSetpoint[wheel] > 0 && currentAngle < angleSetpoint[wheel]) {
        turnMotor(wheel, FORWARD, 100);
    } else if (angleSetpoint[wheel] < 0 && currentAngle < abs(angleSetpoint[wheel])) {
        turnMotor(wheel, BACKWARD, 100);
    } else {
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

    //TODO: check if angle is zero

    double angleRatio = (double) leftAngle / (double) rightAngle;
    double speedRatio = (double) speedSetpoint[LEFT] / (double) speedSetpoint[RIGHT];

    double pidOutput = calculatePIDOutput(speedRatio, angleRatio, &synchronizeWheelsPID);
    int speedOutput = speedSetpoint[RIGHT] * pidOutput;
    print(
        "Angle left: %d, angle right: %d, angle ratio: %d, speed ratio: %d, pidOutput: %d, speedOutput: %d\n",
        leftAngle,
        rightAngle,
        (int) (angleRatio * 100),
        (int) (speedRatio * 100),
        (int) (pidOutput * 100),
        speedOutput
    );

    turnMotor(RIGHT, FORWARD, speedOutput);

    resetAngleMeasurement(RIGHT);
    resetAngleMeasurement(LEFT);
}