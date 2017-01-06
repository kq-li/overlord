GCC = gcc -g

all: overlord.o underling.o
	$(GCC) overlord.o -o overlord
	$(GCC) underling.o -o underling

overlord.o: overlord.c
	$(GCC) -c overlord.c

underling.o: underling.c
	$(GCC) -c underling.c

clean:
	rm -rf *.o *~
