#include "pattern.h"
#include "GLCD.h"

struct transition pattern_transitions[] = {
    { PATTERN_UNRECOGNIZED, PATTERN_DOT,  PATTERN_S1_DOT       },
    { PATTERN_UNRECOGNIZED, PATTERN_DASH, PATTERN_UNRECOGNIZED },
    { PATTERN_S1_DOT,       PATTERN_DOT,  PATTERN_S1_DOT       },
    { PATTERN_S1_DOT,       PATTERN_DASH, PATTERN_S2_DASH      },
    { PATTERN_S2_DASH,      PATTERN_DOT,  PATTERN_S1_DOT       },
    { PATTERN_S2_DASH,      PATTERN_DASH, PATTERN_S3_DASH      },
    { PATTERN_S3_DASH,      PATTERN_DOT,  PATTERN_S4_DOT       },
    { PATTERN_S3_DASH,      PATTERN_DASH, PATTERN_UNRECOGNIZED },
    { PATTERN_S4_DOT,       PATTERN_DOT,  PATTERN_S1_DOT       },
    { PATTERN_S4_DOT,       PATTERN_DASH, PATTERN_S5_DASH      },
    { PATTERN_S5_DASH,      PATTERN_DOT,  PATTERN_S6_DOT       },
    { PATTERN_S5_DASH,      PATTERN_DASH, PATTERN_S3_DASH      },
    { PATTERN_S6_DOT,       PATTERN_DOT,  PATTERN_S7_DOT       },
    { PATTERN_S6_DOT,       PATTERN_DASH, PATTERN_S2_DASH      }
};

void pattern_transition_function(int previous_state, int event, int new_state)
{
    switch (event) {
        case PATTERN_DOT:
            GLCD_Clear(White);
            GLCD_DisplayString(0, 0, 1, "DOT");

            break;
        case PATTERN_DASH:
            GLCD_Clear(White);
            GLCD_DisplayString(0, 0, 1, "DASH");

            break;
        default:
            break;
    }

    switch (new_state) {
        case PATTERN_S7_DOT:
            GLCD_Clear(White);
            GLCD_DisplayString(0, 0, 1, "CORRECT");

            break;
        default:
            break;
    }
}

void init_pattern_fsm(struct finite_state_machine *fsm)
{
    fsm->current_state = PATTERN_UNRECOGNIZED;
    fsm->transitions = pattern_transitions;
    fsm->num_transitions = sizeof(pattern_transitions) / sizeof(pattern_transitions[0]);
    fsm->transition_function = &pattern_transition_function;
}
