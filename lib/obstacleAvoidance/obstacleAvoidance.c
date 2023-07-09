#include "obstacleAvoidance.h"
#include "armuro.h"
#include "wheels.h"
#include "lineFollow.h"

#include "math.h"

#define OBSTACLE_RADIUS 4
#define SAFETY_DISTANCE 2
#define ATTACK_ANGLE 60

typedef enum ObstacleAvoidanceState {
    BACK_OFF,
    TURN_FROM_OBSTACLE,
    DRIVE_CIRCLE,
    OBSTACLE_AVOIDANCE_DONE
} ObstacleAvoidanceState;

ObstacleAvoidanceState obstacleAvoidanceState = TURN_FROM_OBSTACLE;
ObstacleAvoidanceState nextObstacleAvoidanceState = TURN_FROM_OBSTACLE;
State obstacleAvoidanceStateState = READY;

typedef struct ObstacleAvoidanceConfig {
    double circleRadius;
    double backOffDistance;
    double attackAngle;
} ObstacleAvoidanceConfig;

ObstacleAvoidanceConfig obstacleAvoidanceConfig;

ObstacleAvoidanceConfig configureObstacleAvoidance(double obstacleRadius, int attackAngle) {
    ObstacleAvoidanceConfig config;
    int beta = 90 - attackAngle;
    double betaRad = beta * M_PI / 180.0;
    config.circleRadius = 1.0 / (1 - sin(betaRad)) * (obstacleRadius + SAFETY_DISTANCE + 0.5 * WHEEL_DISTANCE);
    config.backOffDistance = cos(betaRad) * config.circleRadius;
    config.attackAngle = attackAngle;
    return config;
}

uint8_t checkForObstacle() {
    Side side = LEFT;
    return checkSwitchesPressed(&side);
}

void avoidObstacle() {
    obstacleAvoidanceState = BACK_OFF;
    obstacleAvoidanceStateState = READY;
    obstacleAvoidanceConfig = configureObstacleAvoidance(OBSTACLE_RADIUS, ATTACK_ANGLE);
}

State avoidObstacleTask() {
    if (obstacleAvoidanceStateState == FINISHED) {
        print("finished state %d when avoiding obstacle\n", obstacleAvoidanceState);
        obstacleAvoidanceState = nextObstacleAvoidanceState;
        print("new state %d when avoiding obstacle\n", obstacleAvoidanceState);
        obstacleAvoidanceStateState = READY;
    }
    if (obstacleAvoidanceStateState == READY) {
        switch (obstacleAvoidanceState) {
            case BACK_OFF: ;
                double backOffDistance = obstacleAvoidanceConfig.backOffDistance - ARMURO_LENGTH;
                print("backing off %d cm\n", (int) backOffDistance);
                turnWheelsSynchronizedByAngle(-50, -50, distanceToAngle(backOffDistance), 1);
                nextObstacleAvoidanceState = TURN_FROM_OBSTACLE;
                break;
            case TURN_FROM_OBSTACLE:
                print("turning from obstacle\n");
                turnArmuro(-obstacleAvoidanceConfig.attackAngle);
                nextObstacleAvoidanceState = DRIVE_CIRCLE;
                break;
            case DRIVE_CIRCLE:
                print("driving circle\n");
                double wheelSpeedDifferenceFactor = speedDifferenceForRadius(obstacleAvoidanceConfig.circleRadius);
                turnWheelsSynchronized(wheelSpeedDifferenceFactor * 50, 50);
                break;
            case OBSTACLE_AVOIDANCE_DONE:
                print("obstacle avoidance done\n");
                return FINISHED;
        }
        obstacleAvoidanceStateState = RUNNING;
    }
    switch (obstacleAvoidanceState) {
        case BACK_OFF:
        case TURN_FROM_OBSTACLE: ;
            TurnWheelsTaskType* turnWheelsResult = turnWheelsTask();
            if (turnWheelsResult[LEFT] == NONE && turnWheelsResult[RIGHT] == NONE) {
                obstacleAvoidanceStateState = FINISHED;
            }
            break;
        case DRIVE_CIRCLE:
            turnWheelsTask();
            CheckLineResult checkLineResult = checkForLine();
            if (checkLineResult != OFF_LINE) {
                nextObstacleAvoidanceState = OBSTACLE_AVOIDANCE_DONE;
                obstacleAvoidanceStateState = FINISHED;
            }
            break;
        case OBSTACLE_AVOIDANCE_DONE:
            return FINISHED;
    }
    return RUNNING;
}
