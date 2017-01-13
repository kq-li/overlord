GCC = gcc -g

all: overlord underling

overlord: overlord.o network.o
	$(GCC) overlord.o network.o -o overlord

overlord.o: overlord.c network.h
	$(GCC) -c overlord.c

underling: underling.o network.o
	$(GCC) underling.o network.o -o underling

underling.o: underling.c network.h
	$(GCC) -c underling.c

clean:
	rm -rf *.o *~
