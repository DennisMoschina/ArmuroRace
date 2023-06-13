#include "parcour.h"

#include "wheels.h"
#include "armuro.h"
#include "lineFollow.h"

typedef enum State {
    READY = 0,
    RUNNING = 1,
    FINISHED = 2
} State;

typedef enum StateMachine {
    DRIVE_FIRST_TRAJECTORY_PART = 0,
    TURN_TO_SECOND_TRAJECTORY_PART = 1,
    DRIVE_SECOND_TRAJECTORY_PART = 2,
    TURN_TO_THIRD_TRAJECTORY_PART = 3,
    DRIVE_THIRD_TRAJECTORY_PART = 4,
    FOLLOW_LINE = 5,
    IDLE = 6
} StateMachine;

StateMachine currentState = FOLLOW_LINE;
StateMachine nextState = DRIVE_FIRST_TRAJECTORY_PART;
State state = READY;

//MARK: - First Trajectory Part

void driveFirstTrajectoryPart() {
    if (state == READY) {
        turnWheelsSynchronizedByAngle(70, 70, distanceToAngle(50));
        state = RUNNING;
        nextState = TURN_TO_SECOND_TRAJECTORY_PART;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            state = FINISHED;
        }
    }
}

//MARK: - Turn To Second Trajectory Part

void turnToSecondTrajectoryPart() {
    if (state == READY) {
        print("setting up turn to second trajectory part\n");
        turnArmuro(-150);
        state = RUNNING;
        nextState = DRIVE_SECOND_TRAJECTORY_PART;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            state = FINISHED;
        }
    }
}

//MARK: - Drive Second Trajectory Part

void driveSecondTrajectoryPart() {
    if (state == READY) {
        turnWheelsSynchronizedByAngle(70, 70, distanceToAngle(35.5));
        state = RUNNING;
        nextState = TURN_TO_THIRD_TRAJECTORY_PART;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            state = FINISHED;
        }
    }
}

//MARK: - Turn To Third Trajectory Part

void turnToThirdTrajectoryPart() {
    if (state == READY) {
        turnArmuro(90);
        state = RUNNING;
        nextState = DRIVE_THIRD_TRAJECTORY_PART;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            state = FINISHED;
        }
    }
}

//MARK: - Drive Third Trajectory Part

void driveThirdTrajectoryPart() {
    if (state == READY) {
        turnWheelsSynchronizedByAngle(70, 70, distanceToAngle(32.8));
        state = RUNNING;
        nextState = FOLLOW_LINE;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
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
    currentState = FOLLOW_LINE;
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
        case DRIVE_FIRST_TRAJECTORY_PART:
            driveFirstTrajectoryPart();
            break;
        case TURN_TO_SECOND_TRAJECTORY_PART:
            turnToSecondTrajectoryPart();
            break;
        case DRIVE_SECOND_TRAJECTORY_PART:
            driveSecondTrajectoryPart();
            break;
        case TURN_TO_THIRD_TRAJECTORY_PART:
            turnToThirdTrajectoryPart();
            break;
        case DRIVE_THIRD_TRAJECTORY_PART:
            driveThirdTrajectoryPart();
            break;
        case FOLLOW_LINE:
            lineFollow();
            break;
        case IDLE:
            break;
        default:
            break;
    }
}