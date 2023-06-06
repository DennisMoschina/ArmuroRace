#include "finiteAutomaton.h"

#include "armuro.h"

FiniteAutomaton* handleFiniteAutomaton(FiniteAutomaton* automaton) {
    print("handle automaton: name=%s, state=%d\n", automaton->name, automaton->state);
    automaton->execute(automaton);
    if (automaton->state == FINISHED) {
        print("automaton finished\n");
        automaton->state = READY;
        return automaton->determinNext(automaton);
    } else {
        automaton->execute(automaton);
        return automaton;
    }
}