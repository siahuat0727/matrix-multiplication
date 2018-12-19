#include "naive.h"

void naive_matmul(const Matrix a, const Matrix b, const Matrix * const dst, void *ctx)
{
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
    for (int i = 0; i < a.row; ++i)
        for (int k = 0; k < a.col; ++k) {
            int r = a.values[i][k];
            for (int j = 0; j < b.col; ++j)
                dst->values[i][j] += r * b.values[k][j];
        }
}

void matmul_stride(const Matrix a,
                   const Matrix b,
                   const Matrix *const dst,
                   int c_row,
                   int c_col,
                   int stride)
{
    for (int k = 0; k < a.col; k += stride)
        for (int i = 0; i < stride; i++)
            for (int j = 0; j < stride; j++)
                for (int m = k; m < k + stride; m++)
                    dst->values[i + c_row][j + c_col] +=
                        a.values[i + c_row][m] * b.values[m][j + c_col];
}

void sub_matmul(const Matrix a,
                const Matrix b,
                const Matrix *const dst,
                void *ctx)
{
    int stride = 4;
    for (int i = 0; i < a.row; i += stride)
        for (int j = 0; j < b.col; j += stride)
            matmul_stride(a, b, dst, i, j, stride);
}
