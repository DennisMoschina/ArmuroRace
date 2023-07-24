#ifndef _OBSTACLE_AVOIDANCE_H_
#define _OBSTACLE_AVOIDANCE_H_

#include "stateMachine.h"
#include "armuro.h"

/**
 * @defgroup obstacleAvoidance Obstacle Avoidance
 * @brief Let the robot avoid obstacles.
 */

/**
 * @brief Check if there is an obstacle in front of the robot.
 * @details Checks all three switches in the front of the robot to detect an obstacle.
 * @ingroup obstacleAvoidance
 * 
 * @return true if there is an obstacle, false otherwise 
 */
uint8_t checkForObstacle();

/**
 * @brief Configure the obstacle avoidance task
 * @details Back off from the obstacle, and drive a circle around it to avoid it.
 *          This behaviour is implemented in a state machine in @ref ObstacleAvoidanceState.
 * @ingroup obstacleAvoidance
 */
void avoidObstacle();

/**
 * @brief Run the obstacle avoidance task
 * @ingroup obstacleAvoidance
 * @note This method should be called in the main loop of the program as frequent as possible to assure the correct timing.
 *
 * @return RUNNING if task is still running, FINISHED if task is finished
 */
State avoidObstacleTask();

#endif