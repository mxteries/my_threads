#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "my_threads.h"

// takes in two matrices separated by newline and prints the product
// uses my_threads to structure the matrix multiplication

// set up matrices
int** product = NULL; // product of mat1 x mat2
int** mat1 = NULL; // m*n matrix
int** mat2 = NULL; // n*p matrix
int m = 0;
int n = 0;
int p = 0;

/**
 * thread id multiplies row i in mat1 by column k in mat2
 * and stores the result in product[i][k]
 * i and k are gotten from param, which is an array of two ints
 */
void* multiply_row_by_column(void* param) {
    int* row_col = param;
    int i = row_col[0];
    int k = row_col[1];
    int sum = 0;

    my_thr_yield();
    // perform dot product on row i and col k
    for (int x = 0; x < n; x++) {
        sum += mat1[i][x] * mat2[x][k];
    }
    product[i][k] = sum;
}

// verifies that two matrices can be multiplied
int verify_matrices(Matrix* A, Matrix* B) {
    return (A->cols == B->rows);
}

// problem, what if one thread finishes before all other threads?
// it returns to main and all execution is lost
// so maybe have each thread link to the next thread in line... hmmm
int main(int argc, char *argv[]) {
    Matrix* A = read_matrix_from_stdin();
    Matrix* B = read_matrix_from_stdin();
    mat1 = A->my_array;
    mat2 = B->my_array;

    if (!verify_matrices(A, B)) {
        fprintf(stderr, "A and B can't be multiplied\n");
        exit(1);
    }

    // create a matrix for the product, and initialize it to 0
    m = A->rows;
    n = A->cols;
    p = B->cols;
    int* elements = malloc(m*p*sizeof(int));
    for (int i = 0; i < (m*p); i++) {
        elements[i] = 0;
    }
    Matrix* P = create_matrix(m, p, elements);
    product = P->my_array;

    // matrix multiplication code
    int** index_arr = make_2d_int_array(m*p, 2);
    int thr_id = 0;
    for (int i = 0; i < m; i++) {
        for (int k = 0; k < p; k++) {
            int* row_col = malloc(2*sizeof(int));
            row_col[0] = i;
            row_col[1] = k;
            index_arr[thr_id] = row_col; // keep track of row_col so we can free it later
            my_thr_create(thr_id++, multiply_row_by_column, row_col);
        }
    }
    // swap into the first thread (from the main thread)
    my_thr_start();
    
    // print the product matrix and free all our vars
    write_matrix_to_stdout(P);
    _free_my_array(index_arr, m*p);
    free_matrix(P);
    free_matrix(A);
    free_matrix(B);
}