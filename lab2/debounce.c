#include <lpc17xx.h>
#include "debounce.h"
#include "button.h"
#include "fsm.h"

int g_debounce_window[3] = {0};

void init_debounce(void)
{
    LPC_PINCON->PINSEL4 &= ~(3 << 20); // enable GPIO for P2.10
    LPC_GPIO2->FIODIR &= ~(1 << 10); // use P2.10 for input
}

void debounce_next_tick(void)
{
    g_debounce_window[0] = g_debounce_window[1];
    g_debounce_window[1] = g_debounce_window[2];
    g_debounce_window[2] = ~(LPC_GPIO2->FIOPIN >> 10) & 0x01;

    if (g_debounce_window[0] == 1 && g_debounce_window[1] == 1 && g_debounce_window[2] == 1) {
        perform_state_transition(&g_button_fsm, BUTTON_PRESS);
    } else if (g_debounce_window [0] == 0 && g_debounce_window [1] == 0 && g_debounce_window[2] == 0) {
        perform_state_transition(&g_button_fsm, BUTTON_RELEASE);
    }
}
