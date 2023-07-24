#ifndef _PID_CONTROLLER_H_
#define _PID_CONTROLLER_H_

#include <stdint.h>

/**
 * @defgroup pidController PID Controller
 * @brief PID controller
 */

/**
 * @brief Configuration for the PID controller
 * @ingroup pidController
 */
typedef struct PIDConfig {
    double p_gain;
    double i_gain;
    double d_gain;
    double max_i;
    double i_relax;
    double old_input;
    double integral;
} PIDConfig;

/**
 * @brief Configure the PID controller
 * @ingroup pidController
 * 
 * @param p_gain the gain for the proportional part
 * @param i_gain the gain for the integral part
 * @param d_gain the gain for the derivative part
 * @param max_i the maximum value for the integral part
 * @param i_relax the relaxation value for the integral part
 * @return a configuration for the PID controller
 */
PIDConfig initPID(double p_gain, double i_gain, double d_gain, double max_i, double i_relax);

/**
 * @brief Calculate the output of the PID controller
 * @ingroup pidController
 * 
 * @param setpoint the desired value
 * @param input the current value
 * @param config the configuration of the PID controller
 * @return the value to write in order to reach the desired value
 */
double calculatePIDOutput(double setpoint, double input, PIDConfig* config);

#endif