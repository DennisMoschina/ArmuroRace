 #include "pidController.h"

PIDConfig initPID(double p, double i, double d, double max_i, double i_relaxation) {
    return (PIDConfig) {
        p_gain: p,
        i_gain: i,
        d_gain: d,
        max_i: max_i,
        i_relax: i_relaxation,
        old_input: 0,
        integral: 0
    };
}

double calculatePIDOutput(double setpoint, double input, PIDConfig* config) {
    double error = setpoint - input;

    double p_term = error * config->p_gain;
    config->integral = (config->integral + error) * config->i_relax;
    double i_term = config->integral * config->i_gain;
    double d_term = (input - config->old_input) * config->d_gain;

    config->old_input = input;

    if (i_term > config->max_i) {
        i_term = config->max_i;
        config->integral = i_term / config->i_gain;
    } else if (i_term < -config->max_i) {
        i_term = -config->max_i;
        config->integral = i_term / config->i_gain;
    }

    double output = p_term + i_term + d_term;
    // print("setpoint: %d.%d, input: %d.%d, error: %d.%d, p_term: %d.%d, i_term: %d.%d, d_term: %d.%d, output: %d.%d\n",
    //     (int) setpoint, (int) (abs(setpoint) * 1000) % 1000,
    //     (int) input, (int) (abs(input) * 1000) % 1000,
    //     (int) error, (int) (abs(error) * 1000) % 1000,
    //     (int) p_term, (int) (abs(p_term) * 1000) % 1000,
    //     (int) i_term, (int) (abs(i_term) * 1000) % 1000,
    //     (int) d_term, (int) (abs(d_term) * 1000) % 1000,
    //     (int) output, (int) (abs(output) * 1000) % 1000
    // );
    return output;
}
