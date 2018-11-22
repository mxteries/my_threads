// Define a matrix struct, so we can keep track of the number of rows and columns
typedef struct Matrix {
    int rows;
    int cols;
    int** my_array; // a pointer to an array of pointers (ie. a 2d array)
} Matrix;

int** make_2d_int_array(int num_rows, int num_cols);
Matrix* create_matrix(int n, int m, int* some_1d_array);
void _free_my_array( int** arr, int rows);
void free_matrix(Matrix* A);
Matrix* read_matrix_from_stdin();
void write_matrix_to_stdout(Matrix *A);
int** transpose_array(int** arr1, int** arr2, int nrow, int ncol);