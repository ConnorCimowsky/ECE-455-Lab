#include <lpc17xx.h>
#include "GLCD.h"
#include "debounce.h"
#include "button.h"
#include "pattern.h"
#include "timer.h"

struct finite_state_machine g_button_fsm;
struct finite_state_machine g_pattern_fsm;

int main(void)
{
    SystemInit();

    GLCD_Init();
    GLCD_Clear(White);

    init_debounce();
    init_button_fsm(&g_button_fsm);
    init_pattern_fsm(&g_pattern_fsm);
    init_timer();

    while (1) {}

    return 0;
}
