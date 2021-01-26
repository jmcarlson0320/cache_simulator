CC = gcc
CFLAGS = -g -Wall
EXE = cache_sim

$(EXE): main.o cache_sim.o
	$(CC) -o $@ $^

main.o: defs.h
cache_sim.o: defs.h

clean:
	rm *.o $(EXE)
