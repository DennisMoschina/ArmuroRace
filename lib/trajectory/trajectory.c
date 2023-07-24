#include "trajectory.h"

#include "wheels.h"
#include "armuro.h"
#include "lineFollow.h"

/**
 * @brief The state machine of the trajectory.
 * @ingroup trajectory
 */
typedef enum TrajectoryStateMachine {
    DRIVE_FIRST_TRAJECTORY_PART = 0,
    TURN_TO_SECOND_TRAJECTORY_PART = 1,
    DRIVE_SECOND_TRAJECTORY_PART = 2,
    TURN_TO_THIRD_TRAJECTORY_PART = 3,
    DRIVE_THIRD_TRAJECTORY_PART = 4,
    FOLLOW_LINE = 5,
    TRAJECTORY_DONE = -1
} TrajectoryStateMachine;

TrajectoryStateMachine currentTrajectoryState = FOLLOW_LINE;
TrajectoryStateMachine nextTrajectoryState = DRIVE_FIRST_TRAJECTORY_PART;
State trajectoryStateState = READY;

//MARK: - First Trajectory Part

void driveFirstTrajectoryPart() {
    if (trajectoryStateState == READY) {
        turnWheelsSynchronizedByAngle(70, 70, distanceToAngle(33), 1);
        trajectoryStateState = RUNNING;
        nextTrajectoryState = TURN_TO_SECOND_TRAJECTORY_PART;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            trajectoryStateState = FINISHED;
        }
    }
}

//MARK: - Turn To Second Trajectory Part

void turnToSecondTrajectoryPart() {
    if (trajectoryStateState == READY) {
        print("setting up turn to second trajectory part\n");
        turnArmuro(110);
        trajectoryStateState = RUNNING;
        nextTrajectoryState = DRIVE_SECOND_TRAJECTORY_PART;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            trajectoryStateState = FINISHED;
        }
    }
}

//MARK: - Drive Second Trajectory Part

void driveSecondTrajectoryPart() {
    if (trajectoryStateState == READY) {
        turnWheelsSynchronizedByAngle(70, 70, distanceToAngle(66.2), 1);
        trajectoryStateState = RUNNING;
        nextTrajectoryState = TURN_TO_THIRD_TRAJECTORY_PART;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            trajectoryStateState = FINISHED;
        }
    }
}

//MARK: - Turn To Third Trajectory Part

void turnToThirdTrajectoryPart() {
    if (trajectoryStateState == READY) {
        turnArmuro(-160);
        trajectoryStateState = RUNNING;
        nextTrajectoryState = DRIVE_THIRD_TRAJECTORY_PART;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            trajectoryStateState = FINISHED;
        }
    }
}

//MARK: - Drive Third Trajectory Part

void driveThirdTrajectoryPart() {
    if (trajectoryStateState == READY) {
        turnWheelsSynchronizedByAngle(70, 70, distanceToAngle(50), 1);
        trajectoryStateState = RUNNING;
        nextTrajectoryState = TRAJECTORY_DONE;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            trajectoryStateState = FINISHED;
        }
    }
}

void startTrajectory() {
    currentTrajectoryState = DRIVE_FIRST_TRAJECTORY_PART;
    trajectoryStateState = READY;
}

State driveTrajectoryTask() {
    if (currentTrajectoryState == TRAJECTORY_DONE) {
        return FINISHED;
    }
    if (trajectoryStateState == FINISHED) {
        print("finished state %d\n", currentTrajectoryState);
        currentTrajectoryState = nextTrajectoryState;
        print("new state %d\n", currentTrajectoryState);
        trajectoryStateState = READY;
    }
    switch (currentTrajectoryState) {
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
        default:
            break;
    }

    return RUNNING;
}