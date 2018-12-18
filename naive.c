#include "naive.h"

#define INITIALIZE(dst)                                                       \
    do {                                                                      \
        assert(                                                               \
            ("Dimension property of matrix multiplication", a.col == b.row)); \
        assert(dst != NULL);                                                  \
        assert(dst->row == a.row);                                            \
        assert(dst->col == b.col);                                            \
        memset(dst->values[0], 0,                                             \
               sizeof(*(dst->values[0])) * dst->row * dst->col);              \
    } while (0)

void naive_matmul(const Matrix a, const Matrix b, const Matrix * const dst, void *ctx)
{
    INITIALIZE(dst);

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
    INITIALIZE(dst);

    for (int i = 0; i < a.row; ++i)
        for (int k = 0; k < a.col; ++k) {
            int r = a.values[i][k];
            for (int j = 0; j < b.col; ++j)
                dst->values[i][j] += r * b.values[k][j];
        }
}

void matmul_4(const Matrix a,
              const Matrix b,
              const Matrix *const dst,
              int c_row,
              int c_col)
{
    for (int k = 0; k < a.col; k += 4)
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                for (int m = k; m < k + 4; m++)
                    dst->values[i + c_row][j + c_col] +=
                        a.values[i + c_row][m] * b.values[m][j + c_col];
}

void sub_matmul(const Matrix a,
                const Matrix b,
                const Matrix *const dst,
                void *ctx)
{
    INITIALIZE(dst);

    for (int i = 0; i < a.row; i += 4)
        for (int j = 0; j < b.col; j += 4)
            matmul_4(a, b, dst, i, j);
}
