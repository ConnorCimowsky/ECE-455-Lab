#include "fsm.h"

int next_state(struct finite_state_machine *fsm, int current_state, int event)
{
    int i;

    for (i = 0; i < fsm->num_transitions; i++) {
        if (fsm->transitions[i].source_state == current_state && fsm->transitions[i].event == event) {
            return fsm->transitions[i].dest_state;
        }
    }

    return -1;
}

void perform_state_transition(struct finite_state_machine *fsm, int event)
{
    int previous_state = fsm->current_state;
    fsm->current_state = next_state(fsm, previous_state, event);
    fsm->transition_function(previous_state, event, fsm->current_state);
}
