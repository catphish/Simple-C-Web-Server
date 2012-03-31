all:
	gcc -c -lpthread *.c
	gcc -lpthread *.o -o server

clean:
	rm -vf *.o server
