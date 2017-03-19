PROGS = tabeld

SRCS  = command_line.c commands.c init.c input.c io.c logger.c
SRCS += packet.c parser.c popvec.c random_bytes.c interfaces.c
SRCS += erm_client.c
SRCS += arch.c

BHEADERS = align.h c11.h command_line.h globals.h interfaces.h kernel.h \
	  logger.h resend.h simd.h traps.h arch_detect.h c11threads.h \
	  config.h init.h io.h kernel_input.h neighbor.h shared.h \
	  tabeld.h util.h random_bytes.h traps.h

CFLAGS  = -std=gnu11 -O3 -Wall -I../../lib -Iincludes
LDFLAGS = -lrt

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

%.prof: %.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g -pg $< debug.c $(LDFLAGS) -o $@

%.dbg: %.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g $< debug.c $(LDFLAGS) -o $@

%.c: %.gp
	gperf -m 16 -s 4 -c -C $< > $@

%.bpf: %.filter
	tcpdump -r wtf.cap -dd -F $< > $@

%.s: %.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -fverbose-asm -S $< -o $@

# Construct objects and added SRCS

OBJS += $(patsubst %.c,%.o,$(SRCS))

DOBJS += $(patsubst %.c,%.dbg,$(SRCS))

HEADERS += $(patsubst %.h,includes/%.h,$(BHEADERS))

# uniq -- dont-verify-checksums ? is that a bpf option?
# -Q direction in?

$(DOBJS): $(HEADERS) $(SRCS) includes/version.h

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

show: # $(HEADERS)
	echo $(OBJS)
	echo $(HEADERS)

tabeld: $(HEADERS) $(SRCS) includes/version.h
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) $(LDFLAGS) -o $@

tabeld-whole.c: $(HEADERS) $(SRCS) 
	cat $(SRCS) > $@

tabeld-whole.s: tabeld-whole.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $< -S -o $@

tabeld-whole: tabeld-whole.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -fwhole-program $< -o $@

tabeld-$(ARCH): $(HEADERS) $(SRCS)
	$(CROSSCC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -o $@

# Some preprocessed magic to get an arch string, keys, etc

arch.c: includes/arch_detect.h
	$(CC) -E $< -o $@

extract_keys: commands.c includes/commands.h
	 cc -Iincludes -DCMD_DUMP $< -o extract_keys

cmds.gp: commands.c extract_keys
	 ./extract_keys > cmds.gp

cmds.c: cmds.gp

babel.bpf: babel.filter

includes/version.h: $(SRCS) $(HEADERS)
	./generate-version.sh > includes/version.h

# Helper tools

tags: $(HEADERS) $(SRCS)
	ctags $(HEADERS) $(SRCS)

TAGS: $(HEADERS) $(SRCS)
	etags $(HEADERS) $(SRCS)

clean:
	rm -f *.o tabeld *.dbg extract_keys

reallyclean: clean
	rm -f *~ *.out *.prof tags TAGS
