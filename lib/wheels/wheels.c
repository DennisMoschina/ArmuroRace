#include "wheels.h"
#include "usart.h"

TurnWheelsTaskType turningWheels[] = {NONE, NONE};
int angleSetpoint[] = {0, 0};
int speedSetpoint[] = {0, 0};


void turnWheelByAngle(int wheel, int angle) {
    angleSetpoint[wheel] = angle;
    turningWheels[wheel] = ANGLE;
    resetAngleMeasurement(wheel);
}

void turnWheelWithSpeed(int wheel, int speed) {
    turningWheels[wheel] = SPEED;
    speedSetpoint[wheel] = speed;
}

TurnWheelsTaskType* turnWheelsTask() {
    for (int i = 0; i < 2; i++) {
        switch (turningWheels[i]) {
            case ANGLE:
                turnWheelByAngleTask(i);
                break;
            case SPEED:
                turnMotor(i, FORWARD, speedSetpoint[i]);
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