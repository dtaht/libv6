PROGS = ebabeld

all: $(PROGS)

ebabeld: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) $(SRCS) -c -o $@

include ../../support.mk

VERSION = version.h
OBJDIR_DEBUG = $(CC)

HEADERS = babel_messages.h protocol_defaults.h babel_tlv.h
SRCS  = command_line.c commands.c

ESRCS += erm_client.c init.c input.c io.c logger.c traps.c send.c recv.c
ESRCS += arch.c

EHEADERS = erm.h

HEADERS += $(patsubst %.h,$(INC)/%.h,$(EHEADERS))

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

# local shared memory command line client

CLIENTDEPS = erm_client.o commands.o debug.o traps.o

packing: aligned maxaligned wire

aligned: packing.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DPACK_ALIGNED $< -o $@

maxaligned: packing.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DPACK_ALIGNED -DUNPACK $< -o $@

wire: packing.c
	$(CC) $(CFLAGS) $(EXTRA_DEFINES) -DWIRE $< -o $@

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

