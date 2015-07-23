#include <lpc17xx.h>
#include "led.h"

void init_led(void)
{
    // configure LEDs for output
    LPC_GPIO1->FIODIR |= 0xb0000000;
    LPC_GPIO2->FIODIR |= 0x0000007c;

    // clear all LEDs
    LPC_GPIO1->FIOCLR |= ~(0x0);
    LPC_GPIO2->FIOCLR |= ~(0x0);
}

void led_on(void)
{
    LPC_GPIO1->FIOSET = (1 << 28);
}

void led_off(void)
{
    LPC_GPIO1->FIOCLR = (1 << 28);
}
