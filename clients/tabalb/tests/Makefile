PREFIX = /usr/local
MANDIR = $(PREFIX)/share/man

PROGS = test_get_cycles percentz
SRCS = test_get_cycles.c
INCDIR = ../includes

CDEBUGFLAGS = -std=gnu11 -O3 -g -Wall
DEFINES = $(PLATFORM_DEFINES)
CFLAGS = $(CDEBUGFLAGS) $(DEFINES) $(EXTRA_DEFINES)
CFLAGS += -I$(INCDIR)

INCLUDES1 = $(wildcard ../*.h)

all: $(PROGS)

test_get_cycles: test_get_cycles.c $(INCDIR)/get_cycles.h

table_tester: table_tester.c

percentz: percentz.c

show_struct_packing: show_struct_packing.c $(INCLUDES1)

run: percentz test_get_cycles table_tester show_struct_packing
	./percentz
	./test_get_cycles
	./table_tester
	./show_struct_packing

clean:
	rm -f $(PROGS) $(OBJS) core *.out
