#include "button.h"
#include "pattern.h"
#include "timer.h"

#define DASH_THRESHOLD_MS 500

int g_last_pressed_tick = 0;

struct transition button_transitions[] = {
    { BUTTON_RELEASED, BUTTON_PRESS,   BUTTON_PRESSED  },
    { BUTTON_RELEASED, BUTTON_RELEASE, BUTTON_RELEASED },
    { BUTTON_PRESSED,  BUTTON_PRESS,   BUTTON_PRESSED  },
    { BUTTON_PRESSED,  BUTTON_RELEASE, BUTTON_RELEASED }
};

void button_transition_function(int previous_state, int event, int new_state)
{
    if (previous_state == new_state) {
        // ignore cyclic transitions
        return;
    }

    switch (new_state) {
        case BUTTON_PRESSED:
            // record a timestamp to track when the button was pressed
            g_last_pressed_tick = g_timer_counter;

            break;
        case BUTTON_RELEASED:
            if (g_timer_counter - g_last_pressed_tick < DASH_THRESHOLD_MS) {
                perform_state_transition(&g_pattern_fsm, PATTERN_DOT);
            } else {
                perform_state_transition(&g_pattern_fsm, PATTERN_DASH);
            }

            break;
    }
}

void init_button_fsm(struct finite_state_machine *fsm)
{
    fsm->current_state = BUTTON_RELEASED;
    fsm->transitions = button_transitions;
    fsm->num_transitions = sizeof(button_transitions) / sizeof(button_transitions[0]);
    fsm->transition_function = &button_transition_function;
}
