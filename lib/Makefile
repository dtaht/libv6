LIB=libv6.a
SRC=sse.c util.c
INCLUDE=-I../../include
CFLAGS=-std=c11

libv6.a: $(SRC)
	$(CC) $(INCLUDE) $(SRC) -DTEST -o libv6.o

clean:
	rm -f $(PROCS) ddpd
