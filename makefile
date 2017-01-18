GCC = gcc -g

all: overlord underling

overlord: overlord.o overlist.o network.o 
	$(GCC) overlord.o overlist.o network.o -o overlord -lreadline

overlord.o: overlord.c network.h
	$(GCC) -c overlord.c

overlist.o: overlist.c overlist.h
	$(GCC) -c overlist.c

underling: underling.o network.o
	$(GCC) underling.o network.o -o underling -lreadline

underling.o: underling.c network.h
	$(GCC) -c underling.c

network.o: network.c network.h
	$(GCC) -c network.c

clean:
	rm -rf *.o *~
