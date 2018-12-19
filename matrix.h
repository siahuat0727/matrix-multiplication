#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#define return_if_fail(p) \
    if(!(p)) { \
        printf("%s:%d Warning: "#p" failed.\n", \
        __func__, __LINE__); \
        return; \
    }
#define return_val_if_fail(p, ret) \
    if(!(p)) { \
        printf("%s:%d Warning: "#p" failed.\n", \
        __func__, __LINE__); \
        return (ret); \
    }

typedef struct _Matrix {
    int row;
    int col;
    int **values;
} Matrix;

Matrix matrix_create(int row, int col);
void matrix_create_all(Matrix * const mats, int num_mat, int row, int col);
Matrix matrix_free(const Matrix);
void matrix_free_all(Matrix * const mats, int num_mat);

Matrix create_mat_1s(int row, int col);
Matrix create_val_per_col(int row, int col);
Matrix matrix_read();
void matrix_print(const Matrix);
bool matrix_equal(const Matrix, const Matrix);

#endif
