CFLAGS = --std=gnu99 -O2 -DNDEBUG -msse4.1 -Wall

OBJ = main.o matmul.o matrix.o strassen.o

all: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

main.o: matrix.h matmul.h strassen.h

strassen.o: strassen.h matrix.h

matmul.o: matmul.h matrix.h

matrix.o: matrix.h

test:
	./main --test < test_script/test.txt

clean:
	@rm $(OBJ) main 2> /dev/null || true
