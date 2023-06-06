#include "parcour.h"

#include "wheels.h"
#include "armuro.h"
#include "finiteAutomaton.h"


FiniteAutomaton* currentAutomaton = 0;

int trajectoryAutomatonIndex = 0;
FiniteAutomaton* trajectoryAutomatons[7];

FiniteAutomaton* determinNextTrajectoryAutomaton(FiniteAutomaton* automaton) {
    print("current Automaton Index: %d\n", trajectoryAutomatonIndex);
    return trajectoryAutomatons[++trajectoryAutomatonIndex];
}

FiniteAutomaton* getSelf(FiniteAutomaton* automaton) {
    return automaton;
}

//MARK: - Parcour Trajectory

void doNothing(FiniteAutomaton* automaton) { }

FiniteAutomaton parcourTrajectoryAutomaton = {
    .state = FINISHED,
    .execute = &doNothing,
    .determinNext = &determinNextTrajectoryAutomaton,
    .name = "trajecotryStart"
};

//MARK: - First Trajectory Part

void driveFirstTrajectoryPart(FiniteAutomaton* automaton) {
    print("driveFirstTrajectoryPart\n");
    if (automaton->state == READY) {
        turnWheelByAngleInTime(LEFT, distanceToAngle(50), 5000);
        turnWheelByAngleInTime(RIGHT, distanceToAngle(50), 5000);
        automaton->state = RUNNING;
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            automaton->state = FINISHED;
        }
    }
}

FiniteAutomaton driveFirstTrajectoryPartAutomaton = {
    .state = READY,
    .execute = &driveFirstTrajectoryPart,
    .determinNext = &determinNextTrajectoryAutomaton,
    .name = "driveFirstTrajectoryPart"
};

//MARK: - Turn To Second Trajectory Part

void turnToSecondTrajectoryPart(FiniteAutomaton* automaton) {
    if (automaton->state == READY) {
        turnArmuro(-30, 500);
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            automaton->state = FINISHED;
        }
    }
}

FiniteAutomaton turnToSecondTrajectoryPartAutomaton = {
    .state = READY,
    .execute = &driveFirstTrajectoryPart,
    .determinNext = &determinNextTrajectoryAutomaton,
    .name = "turnToSecondTrajectoryPart"
};

//MARK: - Drive Second Trajectory Part

void driveSecondTrajectoryPart(FiniteAutomaton* automaton) {
    if (automaton->state == READY) {
        turnWheelByAngleInTime(LEFT, distanceToAngle(35.5), 2000);
        turnWheelByAngleInTime(RIGHT, distanceToAngle(35.5), 2000);
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            automaton->state = FINISHED;
        }
    }
}

FiniteAutomaton driveSecondTrajectoryPartAutomaton = {
    .state = READY,
    .execute = &driveSecondTrajectoryPart,
    .determinNext = &determinNextTrajectoryAutomaton,
    .name = "driveSecondTrajectoryPart"
};

//MARK: - Turn To Third Trajectory Part

void turnToThirdTrajectoryPart(FiniteAutomaton* automaton) {
    if (automaton->state == READY) {
        turnArmuro(90, 1000);
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            automaton->state = FINISHED;
        }
    }
}

FiniteAutomaton turnToThirdTrajectoryPartAutomaton = {
    .state = READY,
    .execute = &turnToThirdTrajectoryPart,
    .determinNext = &determinNextTrajectoryAutomaton,
    .name = "turnToThirdTrajectoryPart"
};

//MARK: - Drive Third Trajectory Part

void driveThirdTrajectoryPart(FiniteAutomaton* automaton) {
    if (automaton->state == READY) {
        turnWheelByAngleInTime(LEFT, distanceToAngle(32.8), 4000);
        turnWheelByAngleInTime(RIGHT, distanceToAngle(32.8), 4000);
    } else {
        TurnWheelsTaskType* wheelsState = turnWheelsTask();
        if (wheelsState[LEFT] == NONE && wheelsState[RIGHT] == NONE) {
            automaton->state = FINISHED;
        }
    }
}

FiniteAutomaton driveThirdTrajectoryPartAutomaton = {
    .state = READY,
    .execute = &driveThirdTrajectoryPart,
    .determinNext = &determinNextTrajectoryAutomaton,
    .name = "driveThirdTrajectoryPart"
};

//MARK: - IDLE

FiniteAutomaton idleAutomaton = {
    .state = FINISHED,
    .execute = &doNothing,
    .determinNext = &getSelf,
    .name = "idle"
};



void startParcour() {
    trajectoryAutomatons[0] = &parcourTrajectoryAutomaton;
    trajectoryAutomatons[1] = &driveFirstTrajectoryPartAutomaton;
    trajectoryAutomatons[2] = &turnToSecondTrajectoryPartAutomaton;
    trajectoryAutomatons[3] = &driveSecondTrajectoryPartAutomaton;
    trajectoryAutomatons[4] = &turnToThirdTrajectoryPartAutomaton;
    trajectoryAutomatons[5] = &driveThirdTrajectoryPartAutomaton;
    trajectoryAutomatons[6] = &idleAutomaton;

    trajectoryAutomatonIndex = 0;
    currentAutomaton = trajectoryAutomatons[trajectoryAutomatonIndex];
}

void driveParcour() {
    currentAutomaton = handleFiniteAutomaton(currentAutomaton);
}