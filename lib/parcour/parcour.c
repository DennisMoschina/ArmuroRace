#include "parcour.h"

#include "wheels.h"
#include "armuro.h"
#include "lineFollow.h"
#include "trajectory.h"
#include "stateMachine.h"

typedef enum StateMachine {
    DRIVE_TRAJECTORY = 0,
    IDLE = -1
} StateMachine;

StateMachine currentState = DRIVE_TRAJECTORY;
StateMachine nextState = IDLE;
State state = READY;

//MARK: - Drive Trajectory

void driveTrajectory() {
    if (state == READY) {
        startTrajectory();
        nextState = IDLE;
        state = RUNNING;
    } else {
        if (driveTrajectoryTask() == FINISHED) {
            state = FINISHED;
        }
    }
}

//MARK: - Follow Line

void lineFollow() {
    if (state == READY) {
        print("setting up follow line\n");
        followLine(30);
        state = RUNNING;
    } else {
        int result = followLineTask();
        switch (result) {
            case 0:
                setLED(LEFT, HIGH);
                setLED(RIGHT, HIGH);
                break;
            case 1:
                setLED(LEFT, LOW);
                setLED(RIGHT, HIGH);
                break;
            case -1:
                stopMotor(LEFT);
                stopMotor(RIGHT);
                setLED(LEFT, HIGH);
                setLED(RIGHT, LOW);
                break;
            default:
                break;
        }
    }
}

void startParcour() {
    currentState = DRIVE_TRAJECTORY;
    state = READY;
}

void driveParcour() {
    if (state == FINISHED) {
        print("finished state %d\n", currentState);
        currentState = nextState;
        print("new state %d\n", currentState);
        state = READY;
    }
    switch (currentState) {
        case DRIVE_TRAJECTORY:
            driveTrajectory();
            break;
        case IDLE:
            break;
        default:
            break;
    }
}