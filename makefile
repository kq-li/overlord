GCC = gcc -g

all: overlord underling

overlord: overlord.o overlist.o util.o
	$(GCC) overlord.o overlist.o util.o -o overlord -lreadline

overlord.o: overlord.c util.h
	$(GCC) -c overlord.c

overlist.o: overlist.c overlist.h 
	$(GCC) -c overlist.c

underling: underling.o util.o
	$(GCC) underling.o util.o -o underling -lreadline

underling.o: underling.c util.h
	$(GCC) -c underling.c

util.o: util.c 
	$(GCC) -c util.c

clean:
	rm -rf *.o *~
