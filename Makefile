CC:=gcc
CFLAGS:=-g -Wall -Wextra -pedantic


all: generate-input.run lab1-parallel.run lab1-serial.run


generate-input.run: generate-input.c
	$(CC) $(CFLAGS) $^ -o $@


lab1-parallel.run: lab1-parallel.c universal.h
	$(CC) $(CFLAGS) $^ -lm -pthread -o $@


lab1-serial.run: lab1-serial.c universal.h
	$(CC) $(CFLAGS) lab1-serial.c -lm -o $@


clean:
	$(RM) *.run *.o *.out
