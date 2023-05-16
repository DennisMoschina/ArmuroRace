#include "wheels.h"
#include "usart.h"

int turningWheels[] = {0, 0};
int angleSetpoint[] = {0, 0};

void turnWheelByAngle(int wheel, int angle) {
    angleSetpoint[wheel] = angle;
    turningWheels[wheel] = 1;
    resetAngleMeasurement(wheel);
}

void turnWheelsTask() {
    for (int i = 0; i < 2; i++) {
        if (!turningWheels[i]) {
            continue;
        }

        int currentAngle = getAngleForWheel(i);

        print("Current angle for %s: %d\n", i == LEFT ? "LEFT" : "RIGHT", currentAngle);

        if (angleSetpoint[i] > 0 && currentAngle < angleSetpoint[i]) {
            turnMotor(i, FORWARD, 100);
        } else if (angleSetpoint[i] < 0 && currentAngle < abs(angleSetpoint[i])) {
            turnMotor(i, BACKWARD, 100);
        } else {
            stopMotor(i);
            turningWheels[i] = 0;
        }
    }
}