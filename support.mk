
FORMAT = clang-format
LIBV6 = $(HOME)/git/libv6

ERM= $(LIBV6)/erm

LINENOISE = $(LIBV6)/lib/linenoise.c
TOOLS = $(ERM)/tools
INC = $(ERM)/includes
FILT = $(ERM)/filters

VERSION = version.h
INCLUDES = -I$(INC) -I$(ERM)/includes
GENVERSION = $(TOOLS)/generate-version.sh

CFLAGS  = -std=gnu11 -O3 -Wall -L$(LIBV6)/lib $(INCLUDES) -ftree-vectorize
LDFLAGS = -lrt

OBJDIR_DEBUG = $(CC)

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

NNATIVE = -mfpu=neon -DHAVE_NEON

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
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -fverbose-asm -S $< -o $@

# Construct objects and added SRCS

OBJS += $(patsubst %.c,%.o,$(SRCS))

DOBJS2 += $(patsubst %.c,%.dbg,$(SRCS))

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

$(OBJDIR)/%.o : %.c
	$(CROSSCC) $(CFLAGS) $<

# I did not know about the | trick
# https://www.gnu.org/software/make/manual/html_node/Prerequisite-Types.html#Prerequisite-Types

$(TOBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

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

# FIXME: enable full warnings (need to shut printf up)

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

musl-gcc: 
	$(MAKE) CC=musl-gcc $(filter-out musl-gcc,$@)

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
