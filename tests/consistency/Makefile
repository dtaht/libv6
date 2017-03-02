# Test all compilation options

PROGS+=bench_Os bench_O1 bench_O2 bench_O3
PROGS+=bench_Os_neon bench_O3_neon
NEON=-DHAVE_NEON -mfpu=neon
#NEON=-DHAVE_NEON
INCLUDES+=common.h native.h native_neon.h reset.h my_memcpy.h native_64bit.h
INCLUDES+=native_sse2.h
RESERVED=-ffixed-q4 -ffixed-q5 -ffixed-q6 -ffixed-q7

SRC=benchmark.c benchmark_data.c
#CFLAGS=-std=c11 -g -Wall c11 doesn't have the !@# endian macros
CFLAGS=-std=gnu99 -g -Wall -DHAVE_64BIT_ARCH
#NATIVEARCH=$(())
#H64=-DHAVE_SSE2
H64=$(NEON)

CROSSCC=aarch64-linux-gnu-gcc-5

all: $(PROGS)

bench_Os: $(SRC) $(INCLUDES)
	$(CC) $(CFLAGS) -Os $(H64) $(SRC) -o $@
bench_O1: $(SRC) $(INCLUDES)
	$(CC) $(CFLAGS) -O1 $(H64) $(SRC) -o $@
bench_O2: $(SRC) $(INCLUDES)
	$(CC) $(CFLAGS) -O2 $(H64) $(SRC) -o $@
bench_O3: $(SRC) $(INCLUDES)
	$(CC) $(CFLAGS) -O3 $(H64) $(SRC) -o $@

bench_Os_neon: $(SRC) $(INCLUDES)
	$(CROSSCC) $(CFLAGS) -S -Os $(NEON) benchmark.c -o $@

bench_O3_neon: $(SRC) $(INCLUDES)
	$(CROSSCC) $(CFLAGS) -S -O3 $(NEON) benchmark.c -o $@

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
	rm -f $(PROGS) gmon.out
