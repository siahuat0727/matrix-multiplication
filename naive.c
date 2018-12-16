#include "naive.h"

void naive_matmul(const Matrix a, const Matrix b, const Matrix * const dst, void *ctx)
{
    assert(("Dimension property of matrix multiplication",
                a.col == b.row));
    assert(dst != NULL);
    assert(dst->row == a.row);
    assert(dst->col == b.col);
    memset(dst->values[0], 0, sizeof(*(dst->values[0])) * dst->row * dst->col);

    for (int i = 0; i < a.row; ++i)
        for (int j = 0; j < b.col; ++j) {
            int sum = 0;
            for (int k = 0; k < a.col; ++k)
                sum += a.values[i][k] * b.values[k][j];
            dst->values[i][j] += sum;
        }
}

void cache_fri_matmul(const Matrix a, const Matrix b, const Matrix * const dst, void *ctx)
{
    assert(("Dimension property of matrix multiplication",
            a.col == b.row));
    assert(dst != NULL);
    assert(dst->row == a.row);
    assert(dst->col == b.col);
    memset(dst->values[0], 0, sizeof(*(dst->values[0])) * dst->row * dst->col);

    for (int i = 0; i < a.row; ++i)
        for (int k = 0; k < a.col; ++k) {
            int r = a.values[i][k];
            for (int j = 0; j < b.col; ++j)
                dst->values[i][j] += r * b.values[k][j];
        }
}
