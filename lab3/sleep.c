#include "sleep.h"

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
