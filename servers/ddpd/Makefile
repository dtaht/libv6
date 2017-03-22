PROCS=ddpc
INCLUDE=-I../../include
LIBS=-l../../lib/v6lib.a

all: $(PROCS)

ddpc: ddpc.c
	$(CC) $(INCLUDE) $(LIBS) -DTEST ddpc.c -o ddpc
	@ln -s ddpc ddpd

clean:
	rm -f $(PROCS) ddpd
