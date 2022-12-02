all: pagesim

pagesim: pagesim.c
	gcc -o pagesim pagesim.c memory.h 

clean:
	rm -fr *~ pagesim