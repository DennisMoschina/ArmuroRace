#include "finiteAutomaton.h"

#include "armuro.h"

FiniteAutomaton* handleFiniteAutomaton(FiniteAutomaton* automaton) {
    automaton->execute(automaton);
    if (automaton->state == FINISHED) {
        print("%s finished\n", automaton->name);
        automaton->state = READY;
        print("determin next at: %x\n", automaton->determinNext);
        FiniteAutomaton* next = automaton->determinNext(automaton);
        print("next: %s\n", next->name);
        return next;
    } else {
        automaton->execute(automaton);
        return automaton;
    }
}