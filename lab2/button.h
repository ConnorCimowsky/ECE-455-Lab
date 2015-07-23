#ifndef BUTTON_H
#define BUTTON_H

#include "fsm.h"

enum button_state {
    BUTTON_PRESSED,
    BUTTON_RELEASED
};

enum button_event {
    BUTTON_PRESS,
    BUTTON_RELEASE
};

extern struct finite_state_machine g_button_fsm;

void init_button_fsm(struct finite_state_machine *fsm);

#endif
