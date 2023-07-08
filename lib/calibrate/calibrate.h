#ifndef _CALIBRATE_H_
#define _CALIBRATE_H_

#include "armuro.h"
#include "stateMachine.h"

void calibrate();
State calibrateTask();

void readWhiteLineSensors();
void readBlackLineSensors();

#endif