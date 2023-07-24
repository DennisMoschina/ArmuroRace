#ifndef _TRAJECTORY_H_
#define _TRAJECTORY_H_

#include "stateMachine.h"

/**
 * @brief Configure the trajectory task
 */
void startTrajectory();

/**
 * @brief Run the trajectory task
 * 
 * @return RUNNING if task is still running, FINISHED if task is finished
 */
State driveTrajectoryTask();

#endif