
all: final.o gfxModified.o
	gcc final.o gfxModified.o -lm -lX11 -o runit -I/opt/X11/include

final.o: final.c gfxModified.h
	gcc -c final.c -I/opt/X11/include

gfxModified.o: gfxModified.c
	gcc -c gfxModified.c -I/opt/X11/include

clean:
	rm *.o runit

