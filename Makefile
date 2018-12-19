all: main.o matmul.o matrix.o strassen.o
	gcc -o main main.o matmul.o matrix.o strassen.o

main.o: main.c matrix.h matmul.h strassen.h
	gcc -c main.c

strassen.o: strassen.c strassen.h matrix.h
	gcc -c strassen.c

matmul.o: matmul.c matmul.h matrix.h
	gcc -c matmul.c

matrix.o: matrix.h matrix.c
	gcc -c matrix.c

debug: main.c matmul.h matmul.c matrix.h matrix.c
	gcc -g -o main -g matmul.c matrix.c main.c

clean:
	@rm *.o *.gch main 2> /dev/null || true
