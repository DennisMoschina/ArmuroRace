#include "parcour.h"

#include "wheels.h"
#include "armuro.h"
#include "lineFollow.h"
#include "trajectory.h"
#include "stateMachine.h"
#include "blinkLED.h"
#include "calibrate.h"
#include "obstacleAvoidance.h"

/**
 * @brief The state machine of the parcour
 * @ingroup parcour
 */
typedef enum StateMachine {
    /// @brief Drive the trajectory
    DRIVE_TRAJECTORY = 0,
    /// @brief Follow the line
    FOLLOW_LINE = 1,
    /// @brief Search the line
    SEARCH_LINE = 2,
    /// @brief Overcome a gap in the line
    OVERCOME_GAP = 3,
    /// @brief Avoid an obstacle on the line
    AVOID_OBSTACLE = 4,
    /// @brief Calibrate the hardware of the robot
    CALIBRATE = 5,
    /// @brief Stop the robot
    IDLE = -1
} StateMachine;

/**
 * @brief The current state of the parcour state machine
 * @ingroup parcour
 */
StateMachine currentState = DRIVE_TRAJECTORY;
/**
 * @brief The next state of the parcour state machine
 * @ingroup parcour
 */
StateMachine nextState = SEARCH_LINE;
/**
 * @brief The status of the parcour state machine's state
 * @ingroup parcour
 */
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
        followLine(50);
        state = RUNNING;
    } else {
        if (checkForObstacle()) {
            print("obstacle detected\n");
            state = FINISHED;
            nextState = AVOID_OBSTACLE;
            return;
        }
        FollowLineResult result = followLineTask();
        switch (result) {
            case ALL_LINE:
                break;
            case FOLLOWING:
                break;
            case LOST_LINE:
                print("lost line while following\n");
                stopWheel(LEFT);
                stopWheel(RIGHT);
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
                setLED(LEFT, LOW);
                setLED(RIGHT, LOW);
                break;
            case LOST:
                print("line is unrecoverably lost\n");
                setLED(LEFT, HIGH);
                setLED(RIGHT, HIGH);
                state = FINISHED;
                nextState = IDLE;
                break;
            case END_OF_LINE:
                setLED(LEFT, HIGH);
                setLED(RIGHT, HIGH);
                print("end of line\n");
                state = FINISHED;
                nextState = OVERCOME_GAP;
                break;
        }
    }
}

// MARK: - Overcome Gap

void overcomeGap() {
    if (state == READY) {
        turnWheelsSynchronizedByAngle(70, 70, distanceToAngle(10), 0);
        nextState = SEARCH_LINE;
        state = RUNNING;
        setLED(LEFT, LOW);
        setLED(RIGHT, LOW);
    } else {
        print("overcoming gap\n");
        TurnWheelsTaskType* result = turnWheelsTask();
        if (result[LEFT] == NONE && result[RIGHT] == NONE) {
            state = FINISHED;
        }
    }
}

// MARK: - Calibrate

void calibrateArmuro() {
    if (state == READY) {
        print("setting up calibrate\n");
        calibrate();
        state = RUNNING;
        nextState = DRIVE_TRAJECTORY;
    } else {
        if (calibrateTask() == FINISHED) {
            state = FINISHED;
            print("done calibrating\n");
        }
    }
}

// MARK: - Avoid Obstacle

void avoidingObstacle() {
    if (state == READY) {
        print("setting up avoid obstacle\n");
        avoidObstacle();
        state = RUNNING;
    } else {
        if (avoidObstacleTask() == FINISHED) {
            nextState = FOLLOW_LINE;
            state = FINISHED;
            print("done avoiding obstacle\n");
        }
    }
}

void startParcour() {
    currentState = CALIBRATE;
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
        case OVERCOME_GAP:
            overcomeGap();
            break;
        case AVOID_OBSTACLE:
            avoidingObstacle();
            break;
        case CALIBRATE:
            calibrateArmuro();
            break;
        case IDLE:
            break;
        default:
            break;
    }
}