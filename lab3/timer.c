#include <lpc17xx.h>
#include "timer.h"

void init_timer(int period_ms)
{
    LPC_TIM0->PR = 25000; // increment TC every 25000 ticks (increment TC at 1000 Hz)
    LPC_TIM0->MR0 = period_ms; // match according to the input period
    LPC_TIM0->MCR = 0x03; // interrupt and reset on match
}

void start_timer(void)
{
    LPC_TIM0->TCR = 0x02; // reset timer
    LPC_TIM0->TCR = 0x01; // enable timer

    NVIC_EnableIRQ(TIMER0_IRQn); // enable timer interrupts
}
