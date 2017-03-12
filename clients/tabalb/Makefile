PROGS = test.c
CFLAGS = -std=gnu11

# Will this bit of crazyness work?
#BUILDS = $(shell $(make) -C $(C) CC=$(C))
#COMPILERS := gcc aarch-bla adapteva arm7 mips
#RESULT := $(foreach C, $(COMPILERS), $(BUILDS))

all: $(PROGS)

$(TARGET)/test: test.c
	$(CC) $(CFLAGS) test.c -o $(TARGET)/test

clean:
	rm -f *.o
