
all: list stree

list: 
	gcc -c list.c

stree: list
	gcc -o stree stree.c list.o
	
	
clean:
	rm -f stree.o stree *.o 
