GCC = gcc -g

all: overlord underling

overlord: overlord.o overlist.o network.o util.o
	$(GCC) overlord.o overlist.o network.o util.o -o overlord -lreadline

overlord.o: overlord.c network.h util.h
	$(GCC) -c overlord.c

overlist.o: overlist.c overlist.h 
	$(GCC) -c overlist.c

underling: underling.o network.o util.o
	$(GCC) underling.o network.o util.o -o underling -lreadline

underling.o: underling.c network.h util.h
	$(GCC) -c underling.c

network.o: network.c 
	$(GCC) -c network.c

util.o: util.c 
	$(GCC) -c util.c

clean:
	rm -rf *.o *~
