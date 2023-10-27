CC:=gcc
CFLAGS:=-g -Wall -Wextra -pedantic -Wno-unused-variable -O3 #-Wno-builtin-macro-redefined -m128bit-long-double


all: generate-input.run lab1-parallel.run lab1-serial.run lab1-parallel-ku.run


generate-input.run: generate-input.c
	$(CC) $(CFLAGS) $< -o $@


lab1-parallel.run: lab1-parallel.c universal.h
	$(CC) $(CFLAGS) $< -lm -pthread -o $@


lab1-serial.run: lab1-serial.c universal.h
	$(CC) $(CFLAGS) $< -lm -o $@


lab1-parallel-ku.run: lab1-parallel-ku.c universal.h
	$(CC) $(CFLAGS) $< -lm -pthread -o $@


clean:
	$(RM) *.run *.o *.out
