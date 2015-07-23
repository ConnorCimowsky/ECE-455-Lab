#include <lpc17xx.h>
#include "GLCD.h"

unsigned char time_string[] = "xx:xx";
uint32_t elapsed_time_s = 0;

char char_for_digit(int digit)
{
    return '0' + digit;
}

void update_time_string(unsigned char *time_string, uint32_t time_s)
{
    uint32_t s, m;

    s = time_s % 60;
    m = (time_s / 60) % 60;

    time_string[0] = char_for_digit(m / 10);
    time_string[1] = char_for_digit(m % 10);
    time_string[3] = char_for_digit(s / 10);
    time_string[4] = char_for_digit(s % 10);
}

void TIMER0_IRQHandler(void)
{
    __disable_irq();

    LPC_TIM0->IR = (1 << 0);

    update_time_string(time_string, elapsed_time_s);

    GLCD_Clear(White);
    GLCD_DisplayString(0, 0, 1, time_string);

    elapsed_time_s += 1;

    __enable_irq();
}

void init_timer(void)
{
    LPC_TIM_TypeDef *pTimer = (LPC_TIM_TypeDef *)LPC_TIM0;

    pTimer->PR = 12499;
    pTimer->MR0 = 2000;
    pTimer->MCR = 0x3;

    NVIC_EnableIRQ(TIMER0_IRQn);

    pTimer->TCR = 1;
}

int main(void)
{
    SystemInit();

    GLCD_Init();
    GLCD_Clear(White);

    init_timer();

    while (1) {}

    return 0;
}
