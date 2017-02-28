# Test all compilation options

PROGS+=bench_Os bench_O1 bench_O2 bench_O3
PROGS+=bench_Os_neon bench_O3_neon
NEON=-DHAVE_NEON -mfpu=neon

SRC=benchmark.c benchmark_data.c
#CFLAGS=-std=c11 -g -Wall c11 doesn't have the !@# endian macros
CFLAGS=-std=gnu99 -g -Wall -DHAVE_64BIT_ARCH
#NATIVEARCH=$(())
#H64=-DHAVE_SSE2
H64=$(NEON)

CROSSCC=aarch64-linux-gnu-gcc-5

all: $(PROGS)

bench_Os: $(SRC)
	$(CC) $(CFLAGS) -Os $(H64) $(SRC) -o $@
bench_O1: $(SRC)
	$(CC) $(CFLAGS) -O1 $(H64) $(SRC) -o $@
bench_O2: $(SRC)
	$(CC) $(CFLAGS) -O2 $(H64) $(SRC) -o $@
bench_O3: $(SRC)
	$(CC) $(CFLAGS) -O3 $(H64) $(SRC) -o $@

bench_Os_neon: $(SRC)
	$(CROSSCC) $(CFLAGS) -S -Os $(NEON) benchmark.c -o $@

bench_O3_neon: $(SRC)
	$(CROSSCC) $(CFLAGS) -S -O3 $(NEON) benchmark.c -o $@


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
