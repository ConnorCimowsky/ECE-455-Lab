#include <lpc17xx.h>
#include "GLCD.h"

unsigned char time_string[] = "xx:xx";
uint32_t elapsed_time_s = 0;

char char_for_digit(int digit)
{
    return '0' + digit;
}

void sleep(int delay_ms)
{
    int i, j;
    int noop_counter;

    __disable_irq();

    for (i = 0; i < (10 * delay_ms); i++) {
        for (j = 0; j < 1536; j++) {
            noop_counter += 1;
        }
    }

    __enable_irq();
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

int main(void)
{
    SystemInit();

    GLCD_Init();
    GLCD_Clear(White);

    while (1) {
        update_time_string(time_string, elapsed_time_s);

        GLCD_Clear(White);
        GLCD_DisplayString(0, 0, 1, time_string);

        elapsed_time_s += 1;

        sleep(1000);
    }

    return 0;
}
