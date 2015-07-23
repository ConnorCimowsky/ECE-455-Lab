#include <lpc17xx.h>
#include "GLCD.h"
#include "led.h"
#include "sleep.h"
#include "int0.h"
#include "timer.h"

void init_int0(void)
{
    LPC_PINCON->PINSEL4 &= ~(3 << 20); // enable GPIO for P2.10
    LPC_GPIO2->FIODIR   &= ~(1 << 10); // use P2.10 for input

    LPC_GPIOINT->IO2IntEnF |= (1 << 10); // configure INT0 interrupts
    NVIC_EnableIRQ(EINT3_IRQn); // enable INT0 interrupts
}

void flash_message(void)
{
    GLCD_Clear(White);
    GLCD_DisplayString(0, 0, 1, "INT0 Pressed");
    led_on();

    sleep(1000);

    led_off();
    GLCD_Clear(White);
}
