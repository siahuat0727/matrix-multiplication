CFLAGS = --std=gnu99 -O2 -DNDEBUG -msse4.1 -Wall
CC := gcc

OBJL = main.o matmul.o matrix.o strassen.o

all: $(OBJL)
	$(CC) $(CFLAGS) -o main $(OBJL)

main.o: matrix.h matmul.h strassen.h

strassen.o: strassen.h matrix.h

matmul.o: matmul.h matrix.h
	$(CC) $(CFLAGS) -msse4.1 -mavx -march=native -c matmul.c

matrix.o: matrix.h

test:
	./main --test < test_script/test.txt

clean:
	@rm $(OBJL) main 2> /dev/null || true
