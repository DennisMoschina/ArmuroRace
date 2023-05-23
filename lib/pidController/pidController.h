#ifndef _PID_CONTROLLER_H_
#define _PID_CONTROLLER_H_

#include <stdint.h>

typedef struct PIDConfig {
    double p_gain;
    double i_gain;
    double d_gain;
    double max_i;
    double i_relax;
    double old_input;
    double integral;
} PIDConfig;


PIDConfig initPID(double p_gain, double i_gain, double d_gain, double max_i, double i_relax);

/**
 * @brief Calculate the output of the PID controller
 * 
 * @param setpoint the desired value
 * @param input the current value
 * @param config the configuration of the PID controller
 * @return the value to write in order to reach the desired value
 */
double calculatePIDOutput(double setpoint, double input, PIDConfig* config);

#endif