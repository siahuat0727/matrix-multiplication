CFLAGS = --std=gnu99 -O2 -DNDEBUG -Wall
CC := gcc

OBJL = main.o matmul.o matrix.o strassen.o

.PHONY: all main-perf test perf clean

all: $(OBJL)
	$(CC) $(CFLAGS) -o main $(OBJL)

main.o: matrix.h matmul.h strassen.h

strassen.o: strassen.h matrix.h

matmul.o: matmul.h matrix.h matmul.c
	$(CC) $(CFLAGS) -msse4.1 -mavx -march=native -c matmul.c

matrix.o: matrix.h

test:
	./main --test < test_script/test.txt

main-perf:
	$(CC) $(CFLAGS) -DPERF -c main.c

perf: main-perf all

plot:
	gnuplot runtime.gp
	eog runtime.png

clean:
	@rm $(OBJL) main 2> /dev/null || true
