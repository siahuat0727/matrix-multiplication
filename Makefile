all: main.o naive.o matrix.o strassen.o
	gcc -o main main.o naive.o matrix.o strassen.o

main.o: main.c matrix.h naive.h strassen.h
	gcc -c main.c

strassen.o: strassen.c strassen.h matrix.h
	gcc -c strassen.c

naive.o: naive.c naive.h matrix.h
	gcc -c naive.c

matrix.o: matrix.h matrix.c
	gcc -c matrix.c

clean:
	@rm *.o *.gch main 2> /dev/null || true
