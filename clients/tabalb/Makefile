PROGS = tabeld

SRCS  = command_line.c commands.c init.c input.c io.c logger.c
SRCS += packet.c parser.c popvec.c random_bytes.c
#SRCS += knetlink.c

BHEADERS = align.h c11.h command_line.h globals.h interface.h kernel.h \
	  logger.h resend.h simd.h traps.h arch_detect.h c11threads.h \
	  config.h init.h io.h kernel_input.h neighbor.h shared.h \
	  tabeld.h util.h random_bytes.h

CFLAGS  = -std=gnu11 -O3 -Wall -I../../lib -Iincludes
CFLAGS += -msse4.2

OBJS := $(patsubst %.c,%.o,$(SRCS))

DOBJS := $(patsubst %.c,%.dbg,$(SRCS))

HEADERS := $(patsubst %.h,includes/%.h,$(BHEADERS))

# Custom rules to produce debuggable code as well as a gperf hash table

%.prof: %.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g -pg $< debug.c -o $@

%.dbg: %.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DDEBUG_MODULE -g $< debug.c -o $@

%.c: %.gp
	gperf -m 16 -s 4 -c -C $< > $@

%.bpf: %.filter
	tcpdump -r wtf.cap -dd -F $< > $@

# uniq -- dont-verify-checksums ? is that a bpf option?
# -Q direction in?

$(DOBJ): $(HEADERS)

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

tabeld: $(HEADERS) $(SRCS)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -o $@

tabeld-whole.c: $(HEADERS) $(SRCS)
	cat $(SRCS) > $@

tabeld-whole.s: tabeld-whole.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -S -o $@

tabeld-whole: tabeld-whole.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -o $@

tabeld-$(ARCH): $(HEADERS) $(SRCS)
	$(CROSSCC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -o $@

cmds.gp: commands.c
	extract_keys commands.c

cmds.c: cmds.gp

babel.bpf: babel.filter

#$(TARGET)/test: test.c
#	$(CC) $(CFLAGS) test.c -o $(TARGET)/test

tags: $(HEADERS) $(SRCS)
	ctags $(HEADERS) $(SRCS)

TAGS: $(HEADERS) $(SRCS)
	etags $(HEADERS) $(SRCS)

clean:
	rm -f *.o tabeld

reallyclean: clean
	rm -f *~ *.out *.dbg *.prof tags TAGS
