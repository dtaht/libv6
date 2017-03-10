CC = gcc
APP = perfcnt_test
SOURCES = perfcnt_test.c
CFLAGS = -Wall -O2 -static

default:
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(SOURCES) -o $(APP)

clean:
	-rm -f $(APP)

.PHONY: default clean
