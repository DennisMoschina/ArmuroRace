#ifndef _OBSTACLE_AVOIDANCE_H_
#define _OBSTACLE_AVOIDANCE_H_

#include "stateMachine.h"
#include "armuro.h"

/**
 * @brief Check if there is an obstacle in front of the robot
 * 
 * @return true if there is an obstacle, false otherwise 
 */
uint8_t checkForObstacle();

/**
 * @brief Configure the obstacle avoidance task
 */
void avoidObstacle();

/**
 * @brief Run the obstacle avoidance task
 * 
 * @return RUNNING if task is still running, FINISHED if task is finished
 */
State avoidObstacleTask();

#endif