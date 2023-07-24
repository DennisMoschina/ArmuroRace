#include "obstacleAvoidance.h"
#include "armuro.h"
#include "wheels.h"
#include "lineFollow.h"

#include "math.h"

#define OBSTACLE_RADIUS 4
#define SAFETY_DISTANCE 3
#define ATTACK_ANGLE 60

/**
 * @brief The state machine for the obstacle avoidance
 * @ingroup obstacleAvoidance
 */
typedef enum ObstacleAvoidanceState {
    /// @brief Backing off from the obstacle
    BACK_OFF,
    /// @brief Turning from the obstacle
    TURN_FROM_OBSTACLE,
    /// @brief Driving a circle around the obstacle
    DRIVE_CIRCLE,
    /// @brief Finished driving around the obstacle
    OBSTACLE_AVOIDANCE_DONE
} ObstacleAvoidanceState;

ObstacleAvoidanceState obstacleAvoidanceState = TURN_FROM_OBSTACLE;
ObstacleAvoidanceState nextObstacleAvoidanceState = TURN_FROM_OBSTACLE;
State obstacleAvoidanceStateState = READY;

/**
 * @brief The configuration for the obstacle avoidance.
 * @ingroup obstacleAvoidance
 */
typedef struct ObstacleAvoidanceConfig {
    /// @brief The radius of the circle to drive around the obstacle
    double circleRadius;
    /// @brief The distance to back off from the obstacle
    double backOffDistance;
    /// @brief The angle to start the circle drive
    double attackAngle;
    /// @brief The distance to drive around the obstacle
    double distanceToDrive;
} ObstacleAvoidanceConfig;

ObstacleAvoidanceConfig obstacleAvoidanceConfig;

/**
 * @brief Configure the obstacle avoidance task with the given parameters.
 * @details Calculate the distance to back off from the obstacle,
 *          the radius of the circle to drive around the obstacle,
 *          and the distance to drive around the obstacle
 *          based on the desired angel of attack and the radius of the obstacle.
 *          The minimum distance to the obstacle is always the same, no matter the parameters.
 *          @image html ObstacleAvoidance.png
 *          @image latex ObstacleAvoidance.png
 *          \f[
 *              \beta = 90 - \alpha
 *          \f]
 *          \f[
 *              s = sin(\beta) \cdot r
 *          \f]
 *          \f[
 *              h = r - s = r_o + \delta + \frac{1}{2} w
 *          \f]
 *          \f[
 *              r = \frac{1}{1 - sin(\beta)} \cdot h
 *          \f]
 *          \f[
 *              d = cos(\beta) \cdot r
 *          \f]
 *          \f[
 *              b = r \cdot (\pi - 2 \cdot sin(\beta))
 *          \f]
 *          where \f$\alpha\f$ is the attack angle, \n
 *          \f$\delta\f$ is the safety distance, \n
 *          \f$w\f$ is the wheel distance, \n
 *          \f$d\f$ is the distance to back off from the obstacle, \n
 *          \f$r\f$ is the radius of the circle to drive around the obstacle, \n
 *          \f$b\f$ is the distance to drive around the obstacle
 * 
 * @ingroup obstacleAvoidance
 * 
 * @param obstacleRadius the radius of the obstacle
 * @param attackAngle the angle to start the circle drive
 * @return ObstacleAvoidanceConfig 
 */
ObstacleAvoidanceConfig configureObstacleAvoidance(double obstacleRadius, int attackAngle) {
    ObstacleAvoidanceConfig config;
    int beta = 90 - attackAngle;
    double betaRad = beta * M_PI / 180.0;
    config.circleRadius = 1.0 / (1 - sin(betaRad)) * (obstacleRadius + SAFETY_DISTANCE + 0.5 * WHEEL_DISTANCE);
    config.backOffDistance = cos(betaRad) * config.circleRadius;
    config.attackAngle = attackAngle;
    config.distanceToDrive = (config.circleRadius * (M_PI - 2 * betaRad)) + 5;
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
                turnWheelsSynchronizedByAngle(wheelSpeedDifferenceFactor * 50, 50, distanceToAngle(obstacleAvoidanceConfig.distanceToDrive), 0);
                nextObstacleAvoidanceState = OBSTACLE_AVOIDANCE_DONE;
                break;
            case OBSTACLE_AVOIDANCE_DONE:
                print("obstacle avoidance done\n");
                return FINISHED;
        }
        obstacleAvoidanceStateState = RUNNING;
    }
    switch (obstacleAvoidanceState) {
        case BACK_OFF:
        case TURN_FROM_OBSTACLE:
        case DRIVE_CIRCLE: ;
            TurnWheelsTaskType* turnWheelsResult = turnWheelsTask();
            if (turnWheelsResult[LEFT] == NONE && turnWheelsResult[RIGHT] == NONE) {
                obstacleAvoidanceStateState = FINISHED;
            }
            break;
        case OBSTACLE_AVOIDANCE_DONE:
            return FINISHED;
    }
    return RUNNING;
}
