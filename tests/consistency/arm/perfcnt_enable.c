/**
 * arm_kernel_module.c
 * http://stackoverflow.com/questions/34081183/compute-clock-cycle-count-on-arm-cortex-a8-beaglebone-black
 * Dave Taht
 * 2017-03-10
 */

#include "arm_kernel_module.h"

#include <linux/module.h>

static int __init perfcnt_enable_init(void)
{

    /* Enable user-mode access to the performance counter */
    asm ("mcr p15, 0, %0, C9, C14, 0\n\t" :: "r"(1));

    /* Disable counter overflow interrupts (just in case) */
    asm ("mcr p15, 0, %0, C9, C14, 2\n\t" :: "r"(0x8000000f));

    pr_debug("### perfcnt_enable module is loaded\n");
    return 0;
}

static void __exit perfcnt_enable_exit(void)
{
}

module_init(perfcnt_enable_init);
module_exit(perfcnt_enable_exit);

MODULE_AUTHOR("Sam Protsenko");
MODULE_DESCRIPTION("Module for enabling performance counter on ARMv7");
MODULE_LICENSE("GPL");
