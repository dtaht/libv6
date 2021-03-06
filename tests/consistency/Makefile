# Test all compilation options

PROGS+=bench_Os bench_O1 bench_O2 bench_O3
PROGS+=bench_Os_neon bench_O3_neon
#NEON=-DHAVE_NEON
INCLUDES+=common.h native.h native_neon.h reset.h my_memcpy.h native_64bit.h
INCLUDES+=native_sse2.h
INCLUDESNEON=common_regs.h native_neon_regs.h
# FIXME this is different between arm7 and aarch64
RESERVED=-ffixed-q4 -ffixed-q5 -ffixed-q6 -ffixed-q7 -flax-vector-conversions -fverbose-asm
ARCH:=$(shell uname -m)

ifeq ($(ARCH),armv7l)
NEON=-DHAVE_NEON -mfpu=neon -mcpu=cortex-a7 -mfloat-abi=hard
else
ifeq ($(ARCH),aarch64)
NEON=-DHAVE_NEON
endif
endif

SRC=benchmark.c benchmark_data.c
#CFLAGS=-std=c11 -g -Wall c11 doesn't have the !@# endian macros
CFLAGS=-std=gnu99 -Wall $(NEON) #-fopenmp
#NATIVEARCH=$(())
#H64=-DHAVE_SSE2
H64=$(NEON)

CROSSCC=aarch64-linux-gnu-gcc

all: $(PROGS)

kill_the_martians: kill_the_martians.c kill_the_martians.h kill_the_martianslib.c
	$(CC) $(CFLAGS) -Os kill_the_martians.c kill_the_martianslib.c -o $@

kill_the_martians_O3: kill_the_martians.c kill_the_martians.h kill_the_martianslib.c
	$(CC) $(CFLAGS) -O3 kill_the_martians.c kill_the_martianslib.c -o $@

kill_the_martians.s: kill_the_martians.c kill_the_martians.h kill_the_martianslib.c
	$(CC) $(CFLAGS) -fverbose-asm -S -Os kill_the_martians.c -o $@

kill_the_martians_O3.s: kill_the_martians.c kill_the_martians.h kill_the_martianslib.c
	$(CC) $(CFLAGS) -O3 -fverbose-asm -S kill_the_martians.c -o $@

kill_the_martians_O3.o: kill_the_martians.c kill_the_martians.h kill_the_martianslib.c
	$(CC) $(CFLAGS) -O3 -c -fverbose-asm kill_the_martians.c -o $@

kill_the_martians.o: kill_the_martians.c kill_the_martians.h kill_the_martianslib.c
	$(CC) $(CFLAGS) -Os -c -fverbose-asm kill_the_martians.c -o $@


bench_Os: $(SRC) $(INCLUDES)
	$(CC) $(CFLAGS) -Os $(H64) $(SRC) -o $@
bench_O1: $(SRC) $(INCLUDES)
	$(CC) $(CFLAGS) -O1 $(H64) $(SRC) -o $@
bench_O2: $(SRC) $(INCLUDES)
	$(CC) $(CFLAGS) -O2 $(H64) $(SRC) -o $@
bench_O3: $(SRC) $(INCLUDES)
	$(CC) $(CFLAGS) -O3 $(H64) $(SRC) -o $@

bench_Os_neon: $(SRC) $(INCLUDES)
	$(CROSSCC) $(CFLAGS) -Os $(NEON) benchmark.c -o $@

bench_O3_neon: $(SRC) $(INCLUDES)
	$(CROSSCC) $(CFLAGS) -O3 $(NEON) $(RESERVED) benchmark.c -o $@

bench_O3_aarch64: $(SRC) $(INCLUDES) $(INCLUDESNEON)
	$(CROSSCC) $(CFLAGS) -O3 $(RESERVED) benchmark_data.c benchmarkregs.c -o $@

bench_O3_aarch64.o: $(SRC) $(INCLUDES) $(INCLUDESNEON)
	$(CROSSCC) $(CFLAGS) -O3 $(RESERVED) benchmarkregs.c -c -o $@

bench_O3_aarch64.s: $(SRC) $(INCLUDES) $(INCLUDESNEON)
	$(CROSSCC) $(CFLAGS) -O3 $(RESERVED) -S benchmarkregs.c -o $@

testregs: $(SRC) $(INCLUDES) $(INCLUDESNEON)
	$(CROSSCC) $(CFLAGS) -O3 $(RESERVED) benchmark_data.c testregs.c -o $@

testregs.o: $(SRC) $(INCLUDES) $(INCLUDESNEON)
	$(CROSSCC) $(CFLAGS) -O3 $(RESERVED) testregs.c -c -o $@

testregs.s: $(SRC) $(INCLUDES) $(INCLUDESNEON)
	$(CROSSCC) $(CFLAGS) -O3 $(RESERVED) -S testregs.c -o $@

# Compiler sanity check

arm7_compiler: compiler.c
	$(CC) $(CFLAGS) -g $(RESERVED) $(NEON) -fwhole-program -O2 compiler.c -o $@

aarch64_compiler: compiler.c
	$(CROSSCC) $(CFLAGS) $(RESERVED) -fwhole-program -O3 compiler.c -o $@

arm7_compiler.s: compiler.c
	$(CC) $(CFLAGS) $(RESERVED) $(NEON) -S -fwhole-program -fverbose-asm -O3 compiler.c -o $@

aarch64_compiler.s: compiler.c
	$(CROSSCC) $(CFLAGS) $(RESERVED) -S -fwhole-program -fverbose-asm -O3 compiler.c -o $@

#run: 	$(PROGS)
#	OUTPUT= $(foreach p,$(PROGS),$(shell ./$(p))) ## well it seemed like a good idea
#	echo $(OUTPUT)
run: 	$(PROGS)
	./bench_Os
	./bench_O1
	./bench_O2
	./bench_O3

clean: 
	rm -f $(PROGS) gmon.out *.s *.o
