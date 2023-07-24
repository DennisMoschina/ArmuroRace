#ifndef _PARCOUR_H_
#define _PARCOUR_H_

/**
 * @defgroup parcour Parcour
 * @brief Let the robot run the parcour.
 * @details The parcour is implemented as a hirachical state machine. The robot is in a certain state and performs a certain action depending on the state.
 *          The differenet states of the robot while running the parcour are defined in the @ref StateMachine enum.
 */

/**
 * @ingroup parcour 
 * 
 * @brief Configure the parcour task
 */
void startParcour();

/**
 * @ingroup parcour 
 * @note This method should be called in the main loop of the program as frequent as possible to assure the correct timing.
 *
 * @brief Run the parcour task
 */
void driveParcour();

#endif