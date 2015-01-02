
all: final.o gfxModified.o
	gcc final.o gfxModified.o -lm -lX11 -o runit

final.o: final.c gfxModified.h
	gcc -c final.c

gfxModified.o: gfxModified.c
	gcc -c gfxModified.c

clean:
	rm *.o runit

