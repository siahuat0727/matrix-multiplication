#include <stdio.h>
#include "matrix.h"

Matrix matrix_create(int row, int col)
{
    Matrix mat = {.row=row, .col=col};

    mat.values = malloc(sizeof(*(mat.values)) * row);
    return_val_if_fail(mat.values != NULL, (Matrix){0});

    // mat.values[0] = calloc(row, sizeof(*(mat.values[0])) * col);
    posix_memalign((void **) &mat.values[0], 32,
                   row * sizeof(*(mat.values[0])) * col);
    return_val_if_fail(mat.values[0] != NULL, (Matrix){0});

    for (int i = 1; i < row; ++i)
        mat.values[i] = mat.values[0] + i * col;
    return mat;
}

void matrix_create_all(Matrix * const mats, int num_mat, int row, int col)
{
    for (int i = 0; i < num_mat; ++i)
        mats[i] = matrix_create(row, col);
}

Matrix create_mat_1s(int row, int col)
{
    Matrix mat = matrix_create(row, col);

    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++)
            mat.values[i][j] = 1;
    return mat;
}

Matrix create_val_per_col(int row, int col)
{
    Matrix mat = matrix_create(row, col);

    for (int j = 0; j < col; j++) {
        mat.values[rand() % row][j] = rand() % 5 + 1;
        mat.values[rand() % row][j] = rand() % 5;
    }
    return (mat);
}

Matrix matrix_read()
{
    int row, col;
    scanf("%d %d", &row, &col);
    Matrix m = matrix_create(row, col);
    for (int i = 0; i < row; ++i)
        for (int j = 0; j < col; ++j)
            scanf("%d", &(m.values[i][j]));
    return m;
}

bool matrix_equal(const Matrix a, const Matrix b)
{
    if (a.row != b.row || a.col != b.col)
        return false;

    for (int i = 0; i < a.row; ++i)
        for (int j = 0; j < a.col; ++j)
            if (a.values[i][j] != b.values[i][j])
                return false;
    return true;
}

void matrix_print(const Matrix m)
{
    for (int i = 0; i < m.row; ++i) {
        for (int j = 0; j < m.col; ++j) {
            printf("%d ", m.values[i][j]);
        }
        puts("");
    }
}

Matrix matrix_free(const Matrix m)
{
    assert(m.values);
    free(m.values[0]);
    free(m.values);
    return (Matrix){0};
}

void matrix_free_all(Matrix * const mats, int num_mat)
{
    for (int i = 0; i < num_mat; ++i) {
        mats[i] = matrix_free(mats[i]);
    }
}
