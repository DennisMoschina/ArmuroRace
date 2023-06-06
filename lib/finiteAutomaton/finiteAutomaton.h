#ifndef _FINITE_AUTOMATON_H_
#define _FINITE_AUTOMATON_H_

typedef enum FinteAutomatonState {
    READY,
    RUNNING,
    FINISHED
} FinteAutomatonState;

typedef struct FiniteAutomaton FiniteAutomaton;

typedef struct FiniteAutomaton {
    FinteAutomatonState state;
    FiniteAutomaton* (*determinNext)(FiniteAutomaton* automaton);

    void (*execute)(FiniteAutomaton* automaton);

    char* name;
} FiniteAutomaton;

FiniteAutomaton* handleFiniteAutomaton(FiniteAutomaton* automaton);

#endif