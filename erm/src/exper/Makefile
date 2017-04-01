#PROGS = structpassing bitendian
PROGS = bitendian blanusa packing

SRCS  = structpassing.c bitendian.c blanusa.c packing.c

DLIB  = ../debug.o ../traps.o
DSRCS = ../debug.c ../traps.c
FORMAT = clang-format

BHEADERS = align.h c11.h command_line.h globals.h interfaces.h kernel.h \
	  logger.h resend.h simd.h traps.h arch_detect.h c11threads.h \
	  config.h init.h io.h kernel_input.h neighbor.h shared.h \
	  tabeld.h util.h random_bytes.h traps.h preprocessor.h

TOOLS = ../../tools
INC = ../../includes
FILT = ../../filters

PITA = icc -I../../includes -msse4.2 -qopt-report-phase=vec -simd -diag-enable=vec -qopt-report=5 -DDEBUG_MODULE -c blanusa_powint.c

VERSION = version.h
INCLUDES = -I$(INC)
GENVERSION = $(TOOLS)/generate-version.sh

CFLAGS  = -msse4.2 -std=gnu11 -O3 -Wall -L../../lib $(INCLUDES)
LDFLAGS = -lm

ifeq ($(CC),musl-gcc)
	OMIT_REGS = -ffixed-xmm5
	OMIT_REGS += -ffixed-xmm6
	OMIT_REGS += -ffixed-xmm7
	OMIT_REGS += -ffixed-xmm8
	OMIT_REGS += -ffixed-xmm9
	OMIT_REGS += -ffixed-xmm10
	OMIT_REGS += -ffixed-xmm11
	OMIT_REGS += -ffixed-xmm12
	OMIT_REGS += -ffixed-xmm13
	OMIT_REGS += -ffixed-xmm14
	OMIT_REGS += -ffixed-xmm15
	OMIT_REGS += -ffixed-r9
	OMIT_REGS += -ffixed-r10
	OMIT_REGS += -ffixed-r11
	OMIT_REGS += -ffixed-r12
	CFLAGS += $(OMIT_REGS)
	CRAZED = -mfpmath=387 -ffast-math -Os
	CFLAGS += $(CRAZED)
endif

OBJDIR_DEBUG = $(CC)

#$(OBJ_DEBUG): $(OBJDIR_DEBUG)/%.o: %.c
#	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $< -o $@

# Arch specific things - still rather fragile

# FIXME if CROSSCC is defined, reinvoke self with that as we do all the work with CC
# Worse, these methods break down when using lede's cross compiler regardless

ifeq ($(CC),e-gcc)          # adapteva compiler
	CFLAGS +=-m1reg-r63 # use reg -63 for negative constants
	CFLAGS +=-falign-loops=8
	CFLAGS +=-falign-functions=8
endif

# Intel - use cpuid? have patch at run time?
# CFLAGS += -msse4.2
# SRCS += knetlink.c

# gcc-aarch64-
# AARCH64

# SRCS += knetlink.c

# ARMv7 gcc-arm-linux-gnueabihf

ifeq ($(CC),arm-linux-gnueabhf-gcc)       # arm cross compiler
	CFLAGS += -mfpu=neon -DHAVE_NEON  # tell it we have neon
	SRCS += knetlink.c
endif

# MIPS

# Custom rules to produce debuggable code, gperf hash table, & bpf

%.profile: %.c $(DLIB)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g -pg $< $(DLIB) $(LDFLAGS) -o $@

%.dbg: %.c $(DLIB)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g $< $(DLIB) $(LDFLAGS) -o $@

%.c: %.gp
	gperf -m 16 -s 4 -c -C $< | $(FORMAT) > $@

%.bpf: $(FILT)/%.filter
	tcpdump -r $(FILT)/wtf.cap -dd -F $< > $@

#babel.bpf: $(FILT)/babel.filter
#	tcpdump -r $(FILT)/wtf.cap -dd -F $< > $@

%.s: %.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -fverbose-asm -S $< -o $@

# Construct objects and added SRCS

OBJS += $(patsubst %.c,%.o,$(SRCS))

DOBJS2 += $(patsubst %.c,%.dbg,$(SRCS))

# We can't link core lib .dbg objects to themselves

all: $(PROGS)

traps.dbg: ../traps.c ../debug.o
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g $< ../debug.o $(LDFLAGS) -o $@

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

CLIENTDEPS = erm_client.o commands.o ../debug.o traps.o

germ: germ.c $(CLIENTDEPS)
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

arch.c: $(INC)/arch_detect.h
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
	$(MAKE) CC=icc EXTRA_DEFINES=-vec-report $(filter-out vector,$@)

mips:
	$(MAKE) CC=mipsel-openwrt-linux-gcc $(filter-out aarch64,$@)

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
	rm -f *.o $(PROGS) *.dbg extract_keys

reallyclean: clean
	rm -f *~ *.out *.prof tags TAGS
