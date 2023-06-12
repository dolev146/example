all: a b

a:
	gcc a.c -Wall -o a

b:
	gcc b.c -Wall -o b

clean:
	rm -f a b *.o