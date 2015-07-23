#include <lpc17xx.h>
#include "GLCD.h"
#include "led.h"
#include "timer.h"
#include "int0.h"

#define MAX_BURST_SIZE 3
#define MAX_ARRIVAL_RATE_S 10
#define MAX_ARRIVAL_RATE_MS (MAX_ARRIVAL_RATE_S * 1000)

int burst_counter = 0;

void TIMER0_IRQHandler(void)
{
    LPC_TIM0->IR = 0x01; // clear interrupt

    burst_counter = 0;

    NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void)
{
    LPC_GPIOINT->IO2IntClr |= (1 << 10); // clear interrupt

    if (++burst_counter >= MAX_BURST_SIZE) {
        NVIC_DisableIRQ(EINT3_IRQn); // disable INT0 interrupts
    }

    flash_message();
}

int main(void)
{
    SystemInit();

    init_led();
    init_timer(MAX_ARRIVAL_RATE_MS);
    init_int0();

    GLCD_Init();
    GLCD_Clear(White);

    start_timer();

    while (1) {}

    return 0;
}
