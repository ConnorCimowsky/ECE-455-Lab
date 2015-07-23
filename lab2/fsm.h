#ifndef FSM_H
#define FSM_H

#include "constants.h"

struct transition {
    int source_state;
    int event;
    int dest_state;
};

struct finite_state_machine {
    int current_state;
    struct transition *transitions;
    int num_transitions;
    void (*transition_function)(int, int, int);
};

void perform_state_transition(struct finite_state_machine *fsm, int event);

#endif
