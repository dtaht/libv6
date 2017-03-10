/**
 * arm_perfcnt_test.c
 *
 * Dave Taht
 * 2017-03-10
 */
//#include "arm_perfcnt_test.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>


static unsigned int get_cyclecount(void)
{
    unsigned int value;

    /* Read CCNT Register */
    asm volatile ("mrc p15, 0, %0, c9, c13, 0\t\n": "=r"(value));

    return value;
}

static void init_perfcounters(int32_t do_reset, int32_t enable_divider)
{
    /* In general enable all counters (including cycle counter) */
    int32_t value = 1;

    /* Peform reset */
    if (do_reset) {
        value |= 2; /* reset all counters to zero */
        value |= 4; /* reset cycle counter to zero */
    }

    if (enable_divider)
        value |= 8; /* enable "by 64" divider for CCNT */

    value |= 16;

    /* Program the performance-counter control-register */
    asm volatile ("mcr p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));

    /* Enable all counters */
    asm volatile ("mcr p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));

    /* Clear overflows */
    asm volatile ("mcr p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
}

int main(void)
{
    unsigned int overhead;
    unsigned int t;

    /* Init counters */
    init_perfcounters(1, 0);

    /* Measure the counting overhead */
    overhead = get_cyclecount();
    overhead = get_cyclecount() - overhead;

    /* Measure ticks for some operation */
    t = get_cyclecount();
    sleep(1);
    t = get_cyclecount() - t;

    printf("function took exactly %d cycles (including function call)\n",
            t - overhead);

    return EXIT_SUCCESS;
}
