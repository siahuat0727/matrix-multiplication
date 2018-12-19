#include "matmul.h"

#define CHECK(l, r, dst)                                                      \
    do {                                                                      \
        return_if_fail(                                                       \
            ("Dimension property of matrix multiplication", l.col == r.row)); \
        assert(dst.row == l.row);                                             \
        assert(dst.col == r.col);                                             \
    } while (0)

void naive_matmul(const Matrix l, const Matrix r, const Matrix dst, void *ctx)
{
    CHECK(l, r, dst);

    for (int i = 0; i < l.row; ++i)
        for (int j = 0; j < r.col; ++j) {
            int sum = 0;
            for (int k = 0; k < l.col; ++k)
                sum += l.values[i][k] * r.values[k][j];
            dst.values[i][j] += sum;
        }
}

void cache_fri_matmul(const Matrix l, const Matrix r, const Matrix dst, void *ctx)
{
    CHECK(l, r, dst);

    for (int i = 0; i < l.row; ++i)
        for (int k = 0; k < l.col; ++k) {
            int s = l.values[i][k];
            for (int j = 0; j < r.col; ++j)
                dst.values[i][j] += s * r.values[k][j];
        }
}

void matmul_stride(const Matrix l,
                   const Matrix r,
                   const Matrix dst,
                   int c_row,
                   int c_col,
                   int stride)
{
    for (int k = 0; k < l.col; k += stride)
        for (int i = 0; i < stride && i + c_row < l.row; i++)
            for (int j = 0; j < stride && j + c_col < r.col; j++)
                for (int m = k; m < k + stride && m < l.col; m++)
                    dst.values[i + c_row][j + c_col] +=
                        l.values[i + c_row][m] * r.values[m][j + c_col];
}

void sub_matmul(const Matrix l, const Matrix r, const Matrix dst, void *ctx)
{
    CHECK(l, r, dst);

    assert(ctx != NULL);
    int stride = ((SubMatrixInfo*)ctx)->stride;
    for (int i = 0; i < l.row; i += stride)
        for (int j = 0; j < r.col; j += stride)
            matmul_stride(l, r, dst, i, j, stride);
}
