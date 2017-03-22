QUAD = __uint128_t
#CC := clang
SRCS  = tables_search.c
FORMAT = clang-format
CFLAGS  = -std=gnu11 -O3 -Wall -ftree-vectorize
LDFLAGS = -lrt
OBJS += $(patsubst %.c,%.o,$(SRCS))
INC = tables_search.h preprocessor.h simd.h

searches: search_uint8.o search_uint16.o search_uint32.o search_uint64.o search_uint128.o # search_v6addr_t

search_uint8.o: tables_search.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=u8 $< -c -o $@

search_uint16.o: tables_search.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=u16 $< -c -o $@

search_uint32.o: tables_search.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=u32 $< -c -o $@

search_uint64.o: tables_search.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=u64 $< -c -o $@

search_uint128.o: tables_search.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=$(QUAD) $< -c -o $@

search_v6addr.o: tables_search.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=v6addr_t $< -c -o $@

.PHONY: mips arm aarch64 icc gcc clang parallella par vector clean reallyclean

vector:
	$(MAKE) CC=icc EXTRA_DEFINES="-msse4.2 -qopt-report-phase=vec -simd  -diag-enable=vec -qopt-report=5" # " -qopt-report-file=stdout"

mips:
	$(MAKE) CC=mipsel-openwrt-linux-gcc $(filter-out mipsel-openwrt-linux-gcc,$@)

clang:
	$(MAKE) CC=clang $(filter-out clang,$@)

icc:
	$(MAKE) CC=icc $(filter-out icc,$@)

gcc:
	$(MAKE) CC=gcc $(filter-out gcc,$@)

aarch64:
	$(MAKE) CC=aarch64-linux-gnu-gcc-5 $(filter-out aarch64,$@)

arm:
	$(MAKE) CC=arm-linux-gnueabihf-gcc $(filter-out arm,$@)

parallella:
	$(MAKE) CC=e-gcc $(filter-out parallella,$@)

par:
	$(MAKE) CC=e-gcc $(filter-out par,$@)

clean:
	rm -f *.o $(PROGS) *.dbg extract_keys

reallyclean: clean
	rm -f *~ *.out *.prof tags TAGS
