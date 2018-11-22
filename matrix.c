#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "matrix.h"

/**
* mallocs a 2 dimensional array of n rows and m columns
* prints to stderr if malloc fails (all functions in matformatter do this)
*/
int** make_2d_int_array(int num_rows, int num_cols) {
    int** the_array = (int**) malloc(num_rows * sizeof(int*));
    if (the_array == NULL) {
        fprintf(stderr, "2d array malloc failed\n");
    } else {
        for (int i = 0; i < num_rows; i++) {
            the_array[i] = (int*) malloc(num_cols * sizeof(int));

            if (the_array[i] == NULL) {
                fprintf(stderr, "2d array malloc failed\n");
            }
        }
    }
    return the_array;
}

/**
* turns an array of ints into a 2d matrix of n rows and m columns
* assumption: some_1d_array has (n*m) number of elements
* returns: a pointer to a matrix (that should be freed with free_matrix)
*/
Matrix* create_matrix(int n, int m, int* some_1d_array) {
    Matrix* my_matrix = (Matrix*) malloc(sizeof(Matrix));
    if (my_matrix == NULL) {
        fprintf(stderr, "matrix malloc failed\n");
    } else {
        int count = 0;
        int** my_2d_array = make_2d_int_array(n, m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                my_2d_array[i][j] = some_1d_array[count++];
            }
        }
        my_matrix->rows = n;
        my_matrix->cols = m;
        my_matrix->my_array = my_2d_array;
    }
    return my_matrix;
}

// frees a 2d array created by make_2d_int_array
// called by free_matrix
void _free_my_array( int** arr, int rows) {
    for (int i = 0; i < rows; i++) {
        free(arr[i]);
    }
    free(arr);
}

// frees a matrix and all its components
void free_matrix(Matrix* A) {
    _free_my_array(A->my_array, A->rows);
    free(A);
}


/**
 * From matmult_p page:
 * Reads a row of a matrix from stdin until it encounters a blank line
 * return: a pointer to a matrix struct (call free_matrix() when done)
 * if columns are inconsistent, prints warning for now
 */
Matrix* read_matrix_from_stdin() {
    // allocates data for a 5x5 matrix initially, reallocs if more is needed
    int matrix_size = 25;
    int* matrix_data = (int*) malloc(matrix_size * sizeof(int));
    if (matrix_data == NULL) {
        fprintf(stderr, "matrix_data malloc failed\n");
    }

    // used for the getline() function
    char* line = NULL;
    size_t len = 0;
    ssize_t nread;

    // used for strtol() function
    char* ptr;

    // count the dimensions of this input matrix
    int num_rows = 0;
    int num_cols = 0;

    // verify nxm matrix by comparing the number of columns each row
    int temp_num_cols = 0;
    int have_counted_columns = 0;

    // used to index and store numbers into matrix_data
    int count = 0;

    // read line and get tokens using strtok
    const char* delim = " ";
    char* token;
    // while we havent reached EOF
    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (strcmp(line, "\n") == 0) { // if blank line, stop reading stdin
            break;
        }

        temp_num_cols = 0; // reset on each row
        num_rows++;
        line[strlen(line) - 1] = '\0'; //overwrite newline character
        token = strtok(line, delim);

        /* walk though other tokens */
        while(token != NULL) {
            temp_num_cols++;
            if (!have_counted_columns) {
                num_cols++; // count the number of columns for the first line only
            }

            // realloc if needed
            if (count >= matrix_size) {
                matrix_size *= 2;
                matrix_data = realloc(matrix_data, matrix_size * sizeof(int));
                if (matrix_data == NULL) {
                    fprintf(stderr, "matrix_data realloc failed\n");
                }
            }

            // convert token to int and store in matrix_data
            matrix_data[count++] = strtol(token, &ptr, 10);
            token = strtok(NULL, delim);
        }
        if (have_counted_columns) {
            if (temp_num_cols != num_cols) {
                fprintf(stderr, "Warning: mismatch in number of columns\n");
            }
        } else {
            have_counted_columns = 1;
        }
    }

    Matrix* input = create_matrix(num_rows, num_cols, matrix_data);
    free(matrix_data);
    return input;
}

// prints out a matrix with an extra new line at the end
void write_matrix_to_stdout(Matrix *A) {
    for (int i = 0; i < A->rows; i++) {

        printf("%d", A->my_array[i][0]); // print first column manually
        for (int j = 1; j < A->cols; j++) {
            printf(" %d", A->my_array[i][j]);
        }
        printf("\n");
    }
    printf("\n"); // print extra blank line
}

int** transpose_array(int** arr1, int** arr2, int nrow, int ncol) {
    // Store each A[i][j] into T[j][i]
    for (int i = 0; i < nrow; i++) {
        for (int j = 0; j < ncol; j++) {
            arr2[j][i] = arr1[i][j];
        }
    }
    return arr2;
}