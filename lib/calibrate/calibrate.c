#include "calibrate.h"

typedef enum {
    CALIBRATE_START,
    CALIBRATE_WHITE,
    CALIBRATE_BLACK,
    CALIBRATE_DONE
} CalibrateState;

CalibrateState calibrateState = CALIBRATE_START;
CalibrateState nextCalibrateState = CALIBRATE_START;
State calibrateStateState = READY;

void startCalibrate() {
    Side button;
    if (checkSwitchesPressed(&button)) {
        switch (button) {
            case LEFT:
                nextCalibrateState = CALIBRATE_WHITE;
                break;
            case RIGHT:
                nextCalibrateState = CALIBRATE_BLACK;
                break;
            case MIDDLE:
                nextCalibrateState = CALIBRATE_DONE;
                break;
        }

        calibrateStateState = FINISHED;
    } else {
        calibrateStateState = RUNNING;
    }
}

void calibrate() {
    calibrateState = CALIBRATE_START;
    calibrateStateState = READY;
}
State calibrateTask() {
    if (calibrateStateState == FINISHED) {
        print("finished state %d when calibrating\n", calibrateState);
        calibrateState = nextCalibrateState;
        print("new state %d when calibrating\n", calibrateState);
        calibrateStateState = READY;
    }
    switch (calibrateState) {
        case CALIBRATE_START:
            startCalibrate();
            break;
        case CALIBRATE_WHITE:
            readWhiteLineSensors();
            break;
        case CALIBRATE_BLACK:
            readBlackLineSensors();
            break;
        case CALIBRATE_DONE:
            print("state is calibrate done\n");
            return FINISHED;
    }
    return RUNNING;
}

void readWhiteLineSensors() {
    print("read white line sensors\n");
    getRawLineSensorReadings(
        &minLineSensorValues[LEFT],
        &minLineSensorValues[MIDDLE],
        &minLineSensorValues[RIGHT]
    );
    calibrateStateState = FINISHED;
    nextCalibrateState = CALIBRATE_START;
}
void readBlackLineSensors() {
    print("read black line sensors\n");
    getRawLineSensorReadings(
        &maxLineSensorValues[LEFT],
        &maxLineSensorValues[MIDDLE],
        &maxLineSensorValues[RIGHT]
    );
    calibrateStateState = FINISHED;
    nextCalibrateState = CALIBRATE_START;
}