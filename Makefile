CFLAGS = --std gnu99 -O0 -msse4.1 -Wall

VERIFY ?= 0
EXECUTABLE := naive cache_fd submatrix \
	strassen_cache_fd strassen_naive strassen_submatrix SIMD
OBJL := main.c matmul.o matrix.o matrix.h matmul.h
ALL_OBJ: main.c matmul.o matrix.o strassen.o matrix.h matmul.h strassen.h

all: $(EXECUTABLE)

all_function: $(ALL_OBJ)
	$(CC) $(CFLAGS) -DVERIFY=$(VERIFY) -Dall -c main.c
	$(CC) $(CFLAGS) -o main main.o matmul.o matrix.o strassen.o

naive: $(OBJL)
	$(CC) $(CFLAGS) -DVERIFY=$(VERIFY) -D$@ -c main.c
	$(CC) $(CFLAGS) -o $@ main.o matmul.o matrix.o

cache_fd: $(OBJL)
	$(CC) $(CFLAGS) -DVERIFY=$(VERIFY) -D$@ -c main.c
	$(CC) $(CFLAGS) -o $@ main.o matmul.o matrix.o

SIMD: $(OBJL)
	$(CC) $(CFLAGS) -DVERIFY=$(VERIFY) -D$@ -c main.c
	$(CC) $(CFLAGS) -o $@ main.o matmul.o matrix.o

submatrix: $(OBJL)
	$(CC) $(CFLAGS) -DVERIFY=$(VERIFY) -D$@ -c main.c
	$(CC) $(CFLAGS) -o $@ main.o matmul.o matrix.o

strassen_cache_fd: $(ALL_OBJ)
	$(CC) $(CFLAGS) -DVERIFY=$(VERIFY) -D$@ -c main.c
	$(CC) $(CFLAGS) -o $@ main.o matmul.o matrix.o strassen.o

strassen_naive: $(ALL_OBJ)
	$(CC) $(CFLAGS) -DVERIFY=$(VERIFY) -D$@ -c main.c
	$(CC) $(CFLAGS) -o $@ main.o matmul.o matrix.o strassen.o

strassen_submatrix: $(ALL_OBJ)
	$(CC) $(CFLAGS) -DVERIFY=$(VERIFY) -D$@ -c main.c
	$(CC) $(CFLAGS) -o $@ main.o matmul.o matrix.o strassen.o

strassen.o: strassen.c strassen.h matrix.h
	$(CC) -c strassen.c

matmul.o: matmul.c matmul.h matrix.h
	$(CC) -msse4.1 -c matmul.c

matrix.o: matrix.h matrix.c
	$(CC) -c matrix.c

debug: main.c matmul.h matmul.c matrix.h matrix.c
	$(CC) -g -o main -g matmul.c matrix.c main.c

cache-test: $(EXECUTABLE)
	echo 3 | sudo tee /proc/sys/vm/drop_caches;
	@for exe in $(EXECUTABLE); do\
	    perf stat \
		    -e cache-misses,cache-references,instructions,cycles \
		./$$exe; \
	done

clean:
	@rm *.o *.gch main 2> /dev/null || true
	$(RM) $(EXECUTABLE)
