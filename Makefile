all: caesar queue

caesar:
	gcc -Wall -g -o caesar caesar.c

queue:
	gcc -Wall -g -o queue queue.c

clean:
	rm -f caesar queue *~
