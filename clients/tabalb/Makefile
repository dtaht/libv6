SRCS  = command_line.c commands.c init.c input.c io.c logger.c
SRCS += packet.c parser.c popvec.c test.c

OBJS := $(patsubst %.c,%.o,$(SRCS))

#SRCS += kernel_netlink.c 

HEADERS = align.h c11.h command_line.h globals.h interface.h kernel.h logger.h \
          resend.h simd.h traps.h arch_detect.h c11threads.h config.h init.h io.h \
          kernel_input.h neighbor.h shared.h tabeld.h util.h

PROGS = tabeld
CFLAGS  = -std=gnu11 -O3 -Wall -I../../lib
CFLAGS += -msse4.2

all: $(PROGS)
	echo $(OBJS)

show: $(OBJS)
	echo $(OBJS)

# Will this bit of crazyness work?
#BUILDS = $(shell $(make) -C $(C) CC=$(C))
#COMPILERS := gcc aarch-bla adapteva arm7 mips
#RESULT := $(foreach C, $(COMPILERS), $(BUILDS))
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

%.c: %.gp
	gperf -m 16 -s 4 -c -C $< > $@

cmds.gp: commands.c
	extract_keys commands.c

cmds.c: cmds.gp

#$(TARGET)/test: test.c
#	$(CC) $(CFLAGS) test.c -o $(TARGET)/test


tags: $(HEADERS) $(SRCS)
	ctags $(HEADERS) $(SRCS)

TAGS: $(HEADERS) $(SRCS)
	etags $(HEADERS) $(SRCS)

clean:
	rm -f *.o tabeld

reallyclean: clean
	rm -f *~ *.out tags TAGS
