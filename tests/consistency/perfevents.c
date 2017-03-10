#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <linux/hw_breakpoint.h>

/*
       int perf_event_open(struct perf_event_attr *attr,
                           pid_t pid, int cpu, int group_fd,
                           unsigned long flags);

*/

//ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
 //                         group_fd, flags);

int fddev = -1;
__attribute__((constructor)) static void init(void)
{
 static struct perf_event_attr attr;
 attr.type = PERF_TYPE_HARDWARE;
 attr.config = PERF_COUNT_HW_CPU_CYCLES;
 fddev = syscall(__NR_perf_event_open, &attr, 0, -1, -1, 0);
}

__attribute__((destructor)) static void
fini(void)
{
 close(fddev);
}

/*
static inline long long
cpucycles(void)
{
 long long result = 0;
 if (read(fddev, &result, sizeof(result)) < sizeof(result)) return 0;
 return result;
}
*/
