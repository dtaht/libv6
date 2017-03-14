SRCS  = command_line.c commands.c init.c input.c io.c logger.c
SRCS += main.c  packet.c parser.c popvec.c test.c

#SRCS += kernel_netlink.c 

HEADERS = align.h c11.h command_line.h globals.h interface.h kernel.h logger.h \
          resend.h simd.h traps.h arch_detect.h c11threads.h config.h init.h io.h \
          kernel_input.h neighbor.h shared.h tabeld.h util.h

PROGS = tabeld
CFLAGS = -std=gnu11 -O3 -Wall -I../../lib

CFLAGS += -msse4.2

# Will this bit of crazyness work?
#BUILDS = $(shell $(make) -C $(C) CC=$(C))
#COMPILERS := gcc aarch-bla adapteva arm7 mips
#RESULT := $(foreach C, $(COMPILERS), $(BUILDS))

all: $(PROGS)

tabeld: $(HEADERS) $(SRCS)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -o tabeld

tabeld-whole.c: $(HEADERS) $(SRCS)
	cat $(SRCS) > tabeld-whole.c

tabeld-whole.s: tabeld-whole.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -S -o tabeld-whole.s

tabeld-whole: tabeld-whole.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -o tabeld-whole

tabeld-$(ARCH): $(HEADERS) $(SRCS)
	$(CROSSCC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -o tabled-$(ARCH)

#$(TARGET)/test: test.c
#	$(CC) $(CFLAGS) test.c -o $(TARGET)/test


tags: $(HEADERS) $(SRCS)
	ctags $(HEADERS) $(SRCS)

TAGS: $(HEADERS) $(SRCS)
	etags $(HEADERS) $(SRCS)

clean:
	rm -f *.o tabeld
