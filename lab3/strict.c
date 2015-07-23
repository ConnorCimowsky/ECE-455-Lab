#include <lpc17xx.h>
#include "GLCD.h"
#include "led.h"
#include "timer.h"
#include "int0.h"

#define INTER_ARRIVAL_TIME_S 5
#define INTER_ARRIVAL_TIME_MS (INTER_ARRIVAL_TIME_S * 1000)

void TIMER0_IRQHandler(void)
{
    LPC_TIM0->IR = 0x01; // clear interrupt

    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void)
{
    LPC_GPIOINT->IO2IntClr |= (1 << 10); // clear interrupt

    NVIC_DisableIRQ(EINT3_IRQn); // disable INT0 interrupts

    flash_message();
    start_timer();
}

int main(void)
{
    SystemInit();

    init_led();
    init_timer(INTER_ARRIVAL_TIME_MS);
    init_int0();

    GLCD_Init();
    GLCD_Clear(White);

    while (1) {}

    return 0;
}
