build:
	gcc -std=c99 -g -o tema2 tema2.c StackQueue.c Node.c
	
run:
	./tema2
clean:
	rm *.o