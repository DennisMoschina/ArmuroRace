#include "parcour.h"

#include "wheels.h"
#include "armuro.h"
#include "lineFollow.h"
#include "trajectory.h"
#include "stateMachine.h"

typedef enum StateMachine {
    DRIVE_TRAJECTORY = 0,
    FOLLOW_LINE = 1,
    SEARCH_LINE = 2,
    OVERCOME_GAP = 3,
    AVOID_OBSTACLE = 4,
    IDLE = -1
} StateMachine;

StateMachine currentState = DRIVE_TRAJECTORY;
StateMachine nextState = SEARCH_LINE;
State state = READY;

//MARK: - Drive Trajectory

void driveTrajectory() {
    if (state == READY) {
        startTrajectory();
        nextState = SEARCH_LINE;
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
        FollowLineResult result = followLineTask();
        switch (result) {
            case ALL_LINE:
                setLED(LEFT, HIGH);
                setLED(RIGHT, HIGH);
                break;
            case FOLLOWING:
                setLED(LEFT, LOW);
                setLED(RIGHT, HIGH);
                break;
            case LOST_LINE:
                print("lost line while following\n");
                stopMotor(LEFT);
                stopMotor(RIGHT);
                setLED(LEFT, HIGH);
                setLED(RIGHT, LOW);
                state = FINISHED;
                nextState = SEARCH_LINE;
                break;
            default:
                break;
        }
    }
}

//MARK: - Search Line

void searchTheLine() {
    if (state == READY) {
        print("setting up search line\n");
        searchLine();
        state = RUNNING;
    } else {
        SearchLineResult searchLineResult = searchLineTask();
        switch (searchLineResult) {
            case SEARCHING:
                break;
            case FOUND:
                print("found line\n");
                state = FINISHED;
                nextState = FOLLOW_LINE;
                break;
            case LOST:
                print("line is unrecoverably lost\n");
                state = FINISHED;
                nextState = IDLE;
                break;
            case END_OF_LINE:
                print("end of line\n");
                state = FINISHED;
                nextState = OVERCOME_GAP;
                break;
        }
    }
}

// MARK: - Overcome Gap

// void overcomeGap() {
//     if (state == READY) {
//         turnWheelsSynchronizedByAngle(70, 70, distanceToAngle(10));
//         state = RUNNING;
//     } else {
//         TurnWheelsTaskType result[2] = turnWheelsTask();
//         if (result[LEFT] == NONE && result[RIGHT] == NONE) {
//             state = FINISHED;
//             nextState = SEARCH_LINE;
//         }
//     }
// }

// MARK: - Avoid Obstacle

void avoidObstacle() {

}

void startParcour() {
    currentState = DRIVE_TRAJECTORY;
    state = READY;
}

void driveParcour() {
    if (state == FINISHED) {
        currentState = nextState;
        state = READY;
    }
    switch (currentState) {
        case DRIVE_TRAJECTORY:
            driveTrajectory();
            break;
        case SEARCH_LINE:
            searchTheLine();
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