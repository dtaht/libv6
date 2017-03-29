#ifndef ARMPMU_LIB_H
#define ARMPMU_LIB_H

static inline u32
get_cycles(void)
{
        u32 r = 0;
#if  defined __aarch64__     
	asm volatile("mrs %0, pmccntr_el0" : "=r" (r)); 
#elif defined(__ARM_ARCH_7A__)
        asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(r) );
#endif
        return r;
#endif
}

#define ARMV8_PMEVTYPER_P              (1 << 31) /* EL1 modes filtering bit */
#define ARMV8_PMEVTYPER_U              (1 << 30) /* EL0 filtering bit */
#define ARMV8_PMEVTYPER_NSK            (1 << 29) /* Non-secure EL1 (kernel) modes filtering bit */
#define ARMV8_PMEVTYPER_NSU            (1 << 28) /* Non-secure User mode filtering bit */
#define ARMV8_PMEVTYPER_NSH            (1 << 27) /* Non-secure Hyp modes filtering bit */
#define ARMV8_PMEVTYPER_M              (1 << 26) /* Secure EL3 filtering bit */
#define ARMV8_PMEVTYPER_MT             (1 << 25) /* Multithreading */
#define ARMV8_PMEVTYPER_EVTCOUNT_MASK  0x3ff

static inline void
init_cycles(u32 evtCount)
{
#if defined __aarch64__
	evtCount &= ARMV8_PMEVTYPER_EVTCOUNT_MASK;
	asm volatile("isb");
	/* Just use counter 0 */
	asm volatile("msr pmevtyper0_el0, %0" : : "r" (evtCount));
	/*   Performance Monitors Count Enable Set register bit 30:1 disable, 31,1 enable */
	u32 r = 0;

	asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
	asm volatile("msr pmcntenset_el0, %0" : : "r" (r|1));
#else
#error Unsupported architecture/compiler!
#endif
}

static inline u32
read_pmu(void)
{
#if defined __aarch64__
        u32 r = 0;
	asm volatile("mrs %0, pmevcntr0_el0" : "=r" (r)); 
	return r;
#else
#error Unsupported architecture/compiler!
#endif
}

static inline void
disable_pmu(u32 evtCount)
{
#if defined __aarch64__
	/*   Performance Monitors Count Enable Set register: clear bit 0 */
	u32 r = 0;

	asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
	asm volatile("msr pmcntenset_el0, %0" : : "r" (r&&0xfffffffe));
#else
#error Unsupported architecture/compiler!
#endif
}


#endif /* ARMPMU_LIB_H */
