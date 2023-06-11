#include "parcour.h"

#include "wheels.h"
#include "armuro.h"

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
    IDLE = 5
} StateMachine;


StateMachine currentState = DRIVE_FIRST_TRAJECTORY_PART;
State state = READY;

//MARK: - First Trajectory Part

void driveFirstTrajectoryPart() {
    if (state == READY) {
        turnWheelsSynchronizedByAngle(70, 70, distanceToAngle(50));
        state = RUNNING;
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
        turnArmuro(-330, 70);
        state = RUNNING;
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
        turnArmuro(90, 70);
        state = RUNNING;
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
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            state = FINISHED;
        }
    }
}


void startParcour() {
    currentState = DRIVE_FIRST_TRAJECTORY_PART;
    state = READY;
}

void driveParcour() {
    if (state == FINISHED) {
        currentState++;
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
        case IDLE:
            break;
        default:
            break;
    }
}