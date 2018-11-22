.PHONY: all clean
all: matrix_multiply
clean:
		-rm -f my_threads matrix_multiply

matrix_multiply: matrix_multiply.c
		gcc -Wall matrix.c my_threads.c matrix_multiply.c -o matrix_multiply