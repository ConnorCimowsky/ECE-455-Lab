#include <lpc17xx.h>
#include "timer.h"
#include "debounce.h"

#define DEBOUNCE_INTERVAL_MS 20

extern uint32_t SystemFrequency; // this is defined in system_LPC17xx.c
uint32_t g_timer_counter = 0;

void init_timer(void)
{
    SysTick_Config(SystemFrequency / 1000); // configure the clock frequency
}

void SysTick_Handler(void)
{
    __disable_irq();

    if ((g_timer_counter % DEBOUNCE_INTERVAL_MS) == 0) {
        debounce_next_tick();
    }

    g_timer_counter += 1;

    __enable_irq();
}
