CC:=gcc
CFLAGS:=-g -Wall -Wextra -pedantic -O0 -Wno-unused-variable


all: generate-input.run lab1-parallel.run lab1-serial.run lab1-parallel-ku.run


generate-input.run: generate-input.c
	$(CC) $(CFLAGS) $^ -o $@


lab1-parallel.run: lab1-parallel.c universal.h
	$(CC) $(CFLAGS) lab1-parallel.c -lm -pthread -o $@


lab1-serial.run: lab1-serial.c universal.h
	$(CC) $(CFLAGS) lab1-serial.c -lm -o $@


lab1-parallel-ku.run: lab1-parallel-ku.c universal.h
	$(CC) $(CFLAGS) lab1-parallel-ku.c -lm -pthread -o $@


clean:
	$(RM) *.run *.o *.out
