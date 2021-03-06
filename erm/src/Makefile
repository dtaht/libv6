PROGS = germ werm tabelb sqerm

SRCS  = command_line.c commands.c init.c input.c io.c logger.c
SRCS += parser.c popvec.c random_bytes.c interfaces.c
SRCS += erm_client.c
SRCS += arch.c
SRCS += traps.c
SRCS += print_global_t.c
SRCS += send.c recv.c

# SRCS += packet.c // unfinished

DLIB  = debug.o traps.o erm_client.o
DSRCS = debug.c traps.c erm_client.c
LINENOISE = ../../lib/linenoise.c

EHEADERS = erm_types.h erm.h erm_utypes.h erm_hash.h erm_types.h
BHEADERS = align.h c11.h command_line.h globals.h interfaces.h kernel.h \
	  logger.h resend.h simd.h traps.h arch/arch_detect.h c11threads.h \
	  config.h init.h io.h kernel_input.h neighbor.h shared.h \
	  tabeld.h util.h random_bytes.h traps.h preprocessor.h \
	  $(EHEADERS)

FORMAT = clang-format

TOOLS = ../tools
INC = ../includes
FILT = ../filters

VERSION = version.h
INCLUDES = -I$(INC) -I../../includes # -I/usr/include/linux -I/usr/include/asm
GENVERSION = $(TOOLS)/generate-version.sh

OMIT_REGS += -ffixed-q11
OMIT_REGS += -ffixed-q10

CFLAGS  = -std=gnu11 -O3 -Wall -L../../lib $(INCLUDES)
LDFLAGS = -lrt

OBJDIR_DEBUG = $(CC)

#$(OBJ_DEBUG): $(OBJDIR_DEBUG)/%.o: %.c
#	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $< -o $@

# GCC Notes:

# Computed gotos for readonly static code:
# https://gcc.gnu.org/onlinedocs/gcc/Labels-as-Values.html#Labels-as-Values
# static const int array[] = { &&foo - &&foo, &&bar - &&foo,
#                              &&hack - &&foo };
# goto *(&&foo + array[i]);

# Arch specific things - still rather fragile

# FIXME if CROSSCC is defined, reinvoke self with that as we do all the work with CC
# Worse, these methods break down when using lede's cross compiler regardless

ifeq ($(CC),e-gcc)          # adapteva compiler
	CFLAGS +=-m1reg-r63 # use reg -63 for negative 1. r43 is also allowed
	CFLAGS +=-falign-loops=8
	CFLAGS +=-falign-functions=8
	CFLAGS +=-mcmove
	CFLAGS +=-mfp-mode=int
	# CFLAGS +=-mnovect-double
	# CFLAGS +=-mshort-calls
	# CFLAGS +=-msmall16
endif

ARCH = $(shell uname -m)

# Arm notes:
# per function code generation __attribute__((target("+crc+nocrypto")))
# backtrace better on arm: -mpoke-function-name
# use neon more on aarch64: -mneon-for-64bits
# -mcmse - compile in secure mode:
# http://infocenter.arm.com/help/topic/com.arm.doc.ecm0359818/ECM0359818_armv8m_security_extensions_reqs_on_dev_tools_1_0.pdf

# https://software.intel.com/sites/default/files/m/4/8/8/2/a/31848-CompilerAutovectorizationGuide.pdf

ifeq ($(ARCH),aarch64)
CFLAGS += $(OMIT_REGS)
endif

# -ftree-vectorize -DHAVE_NATIVE_C11_THREADS $(OMIT_REGS) $(NNATIVE)

NNATIVE = -mfpu=neon -DHAVE_NEON -flax-vector-conversions

ifeq ($(CC),arm-linux-gnueabhf-gcc-5)       # arm cross compiler
	SRCS += knetlink.c
	CFLAGS += $(NNATIVE) $(OMIT_REGS)              # tell it we have neon
endif

ifeq ($(CC),arm-linux-gnueabhf-gcc)       # arm cross compiler
	CFLAGS += $(NNATIVE)              # tell it we have neon
	SRCS += knetlink.c
endif

# ARMv7 gcc-arm-linux-gnueabihf

ifeq ($(ARCH),armv7l)       		  # arm native compiler
	CFLAGS += $(NNATIVE)
	SRCS += knetlink.c
endif

ifeq ($(ARCH),aarch64)         # aarch64 native compiler
	CFLAGS += -DHAVE_NEON  # it knows we have neon, the code doesn't.
	SRCS += knetlink.c
endif

parser.dbg: parser.c $(DLIB)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g $< $(LINENOISE) $(DLIB) $(LDFLAGS) -o $@

logger.dbg: logger.c $(DLIB) io.o
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g $< $(DLIB) io.o $(LDFLAGS) -o $@

# Intel - use cpuid? have patch at run time?
# __assume_aligned(x,16)
# CFLAGS += -msse4.2
# SRCS += knetlink.c

# SRCS += knetlink.c

# MIPS

# Custom rules to produce debuggable code, gperf hash table, & bpf

%.profile: %.c $(DLIB)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g -pg $< $(DLIB) $(LDFLAGS) -o $@

%.dbg: %.c $(DLIB)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g $< $(DLIB) $(LDFLAGS) -o $@

%.static: %.c $(DLIB)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g -static $< $(DLIB) -lpthreads $(LDFLAGS) -o $@

%.c: %.gp
	gperf -m 16 -s 4 -c -C $< | $(FORMAT) > $@

%.bpf: $(FILT)/%.filter
	tcpdump -r $(FILT)/wtf.cap -dd -F $< > $@

#%.h: %.eh
#	sed s/TTYPE/%%/g $< > $@

#babel.bpf: $(FILT)/babel.filter
#	tcpdump -r $(FILT)/wtf.cap -dd -F $< > $@

%.s: %.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -fverbose-asm -S $< -c -o $@

ksock.s: ksock.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(NNATIVE) -flax-vector-conversions -fverbose-asm -S $< -c -o $@

# Construct objects and added SRCS

OBJS += $(patsubst %.c,%.o,$(SRCS))

DOBJS2 += $(patsubst %.c,%.dbg,$(SRCS))

# We can't link core lib .dbg objects to themselves

traps.dbg: traps.c debug.o
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g $< debug.o $(LDFLAGS) -o $@

# arch is a generated file, no point in debugging it. Phony? It would actually be
# handy to have it as a program to use later...

arch.dbg: arch.c
	echo ""

HEADERS += $(patsubst %.h,$(INC)/%.h,$(BHEADERS))

# uniq -- dont-verify-checksums ? is that a bpf option?
# -Q direction in?

$(DOBJS): $(HEADERS) $(SRCS) $(VERSION)

# Will this bit of crazyness work?
# BUILDS = $(shell $(make) -C $(C) CC=$(C))
# COMPILERS := gcc aarch-bla adapteva arm7 mips
# RESULT := $(foreach C, $(COMPILERS), $(BUILDS))
# What I basically want to do is convince make
# to fire off builds for all the cross compilers 
# I have defined

OBJDIR := $(TARGET) # ./?
TOBJS  := $(addprefix $(OBJDIR)/,$(OBJS))

all: $(PROGS)
	echo $(OBJS)

$(OBJDIR)/%.o : %.c
	$(CROSSCC) $(CFLAGS) $<

# I did not know about the | trick
# https://www.gnu.org/software/make/manual/html_node/Prerequisite-Types.html#Prerequisite-Types

$(TOBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

# local shared memory command line client

CLIENTDEPS = erm_client.o commands.o debug.o traps.o

germ: germ.c $(CLIENTDEPS) $(OBJS)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $< $(CLIENTDEPS) $(LDFLAGS) -o $@

# noize version

berm: berm.c $(CLIENTDEPS)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $< $(CLIENTDEPS) $(LDFLAGS) -o $@

# Stress Tester

sqerm: sqerm.c $(CLIENTDEPS)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $< $(CLIENTDEPS) $(LDFLAGS) -o $@

# Warnings and monitoring tool

werm: werm.c $(CLIENTDEPS)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $< $(CLIENTDEPS) $(LDFLAGS) -o $@

# daemons

tabeld: $(HEADERS) $(SRCS) $(VERSION)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) $(LDFLAGS) -o $@

tabeld-whole.c: $(HEADERS) $(SRCS)
	cat $(SRCS) > $@

tabeld-whole.s: tabeld-whole.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $< -S -o $@

tabeld-whole: tabeld-whole.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -fwhole-program $< -o $@

tabeld-$(ARCH): $(HEADERS) $(SRCS)
	$(CROSSCC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -o $@

# Some preprocessed magic to get an arch string, endian, keys, etc

arch.c: $(INC)/arch/arch_detect.h
	$(CC) -E $< -o $@

# This must run using the local cc

extract_keys: commands.c includes/commands.h
	 cc -Iincludes -DCMD_DUMP $< -o extract_keys

cmds.gp: commands.c extract_keys
	 ./extract_keys > cmds.gp

cmds.c: cmds.gp

babel.bpf: babel.filter

version.h: $(SRCS) $(HEADERS)
	$(GENVERSION) > version.h

# Search code generation

# search uint4? two? :)

searches: search_uint8.o search_uint16.o search_uint32.o search_uint64.o # search_v6addr.o search_uint128.o

search_uint16.o: tables_search.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=u16 $< -c -o $@

search_uint32.o: tables_search.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=u32 $< -c -o $@

search_uint64.o: tables_search.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=u64 $< -c -o $@

search_uint128.o: tables_search.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=uint128_t $< -c -o $@

search_v6addr.o: tables_search.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=ip_addr $< -c -o $@

search_uint8.o: tables_search.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DGEN_TYPE=u8 $< -c -o $@

# Helper tools

# FIXME: enable full warnings (need to shut printf up)

plugins:
	$(MAKE) -C plugins

.PHONY: full_warnings debug_modules clean reallyclean show dm md

show:
	echo $(OBJS)
	echo $(HEADERS)

full_warnings: $(DOBJS)

debug_modules: $(DOBJS2)

dm: debug_modules

md: debug_modules

# Architectures and compilers I build for

.PHONY: mips arm aarch64 icc gcc clang parallella par vector

vector:
	$(MAKE) CC=icc EXTRA_DEFINES="-msse4.2 -qopt-report-phase=vec -simd  -diag-enable=vec -qopt-report=5" # " -qopt-report-file=stdout"

riscv: 
	$(MAKE) CC=riscv64-unknown-elf-gcc $(filter-out riscv,$@)

mips:
	$(MAKE) CC=mipsel-openwrt-linux-gcc $(filter-out mips,$@)

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

arm8:
	$(MAKE) CC=arm-linux-gnueabihf-gcc-5 $(filter-out arm8,$@)

parallella:
	$(MAKE) CC=e-gcc $(filter-out parallella,$@)

par:
	$(MAKE) CC=e-gcc $(filter-out par,$@)

# Basic helpers

tags: $(HEADERS) $(SRCS)
	ctags $(HEADERS) $(SRCS)

TAGS: $(HEADERS) $(SRCS)
	etags $(HEADERS) $(SRCS)

clean:
	rm -f *.o $(PROGS) *.dbg extract_keys version.h

reallyclean: clean
	rm -f *~ *.out *.prof tags TAGS *.optrpt
