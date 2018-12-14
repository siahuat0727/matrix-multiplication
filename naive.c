#include "naive.h"

Matrix naive_matmul(const Matrix a, const Matrix b, void *ctx)
{
    assert(("Dimension property of matrix multiplication",
                a.col == b.row));  // Error or warning?

    Matrix dst = matrix_create(a.row, b.col);

    for (int i = 0; i < a.row; ++i)
        for (int j = 0; j < b.col; ++j)
            for (int k = 0; k < a.col; ++k)
                dst.values[i][j] += a.values[i][k] * b.values[k][j];

    return dst;
}
