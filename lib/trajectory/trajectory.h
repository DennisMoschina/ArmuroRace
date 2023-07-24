#ifndef _TRAJECTORY_H_
#define _TRAJECTORY_H_

#include "stateMachine.h"

/**
 * @defgroup trajectory Trajectory
 * @brief Drive a predefined trajectory.
 * @details This module is implemented as a state machine in @ref TrajectoryState.
 */

/**
 * @brief Configure the trajectory task
 * @ingroup trajectory
 */
void startTrajectory();

/**
 * @brief Run the trajectory task
 * @note This method should be called in the main loop of the program as frequent as possible to assure the correct timing.
 * @ingroup trajectory
 *
 * @return RUNNING if task is still running, FINISHED if task is finished
 */
State driveTrajectoryTask();

#endif