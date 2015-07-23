#ifndef PATTERN_H
#define PATTERN_H

#include "fsm.h"

enum pattern_state {
    PATTERN_UNRECOGNIZED,
    PATTERN_S1_DOT,
    PATTERN_S2_DASH,
    PATTERN_S3_DASH,
    PATTERN_S4_DOT,
    PATTERN_S5_DASH,
    PATTERN_S6_DOT,
    PATTERN_S7_DOT
};

enum pattern_event {
    PATTERN_DOT,
    PATTERN_DASH
};

extern struct finite_state_machine g_pattern_fsm;

void init_pattern_fsm(struct finite_state_machine *fsm);

#endif
