CC= gcc
CFLAGS= -O3

all:
	$(CC) $(CFLAGS) *.c -o main.out -lpthread -lm

clean:
	rm -f *.exe *.out *.o
