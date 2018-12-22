#include "strassen.h"

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

static int hibit(int n)
{
    n |= (n >>  1);
    n |= (n >>  2);
    n |= (n >>  4);
    n |= (n >>  8);
    n |= (n >> 16);
    return n - (n >> 1);
}

static Matrix matrix_shallow_create(int row)
{
    Matrix mat = {.row=row, .col=row, .values=malloc(sizeof(*(mat.values)) * row)};
    return_val_if_fail(mat.values != NULL, (Matrix){0});
    return mat;
}

static void matrix_shallow_create_all(Matrix * const mats, int num_mat, int row)
{
    for (int i = 0; i < num_mat; ++i)
        mats[i] = matrix_shallow_create(row);
}

static Matrix matrix_shallow_free(const Matrix m)
{
    free(m.values);
    return (Matrix){0};
}

static void matrix_shallow_free_all(Matrix * const mats, int num_mat)
{
    for (int i = 0; i < num_mat; ++i)
        mats[i] = matrix_shallow_free(mats[i]);
}

static void matrix_copy(const Matrix dst, const Matrix src, int row, int col)
{
    for (int i = 0; i < row; ++i)
        for (int j = 0; j < col; ++j)
            dst.values[i][j] = src.values[i][j];
}

static void matrix_split_4(const Matrix ori, const Matrix * const blocks)
{
    int row_half = ori.row >> 1;
    int col_half = ori.col >> 1;
    for (int i = 0; i < row_half; ++i) {
        blocks[0].values[i] = ori.values[i];
        blocks[1].values[i] = ori.values[i] + col_half;
        blocks[2].values[i] = ori.values[i+row_half];
        blocks[3].values[i] = ori.values[i+row_half] + col_half;
    }
}

static void mat_arith(const Matrix l, const Matrix r,
        const Matrix dst, int (*arith)(int, int))
{
    for (int i = 0; i < dst.row; ++i)
        for (int j = 0; j < dst.col; ++j)
            dst.values[i][j] = arith(l.values[i][j], r.values[i][j]);
}

static int add(int l, int r)
{
    return l + r;
}

static void matadd(const Matrix l, const Matrix r, const Matrix dst)
{
    mat_arith(l, r, dst, &add);
}

static int sub(int l, int r)
{
    return l - r;
}

static void matsub(const Matrix l, const Matrix r, const Matrix dst)
{
    mat_arith(l, r, dst, &sub);
}

static bool is_pow2(int n)
{
    return !(n & (n-1));
}

void strassen_matmul_pow2(const Matrix l, const Matrix r,
        const Matrix dst, void *ctx)
{
    assert(dst.values != NULL);
    memset(dst.values[0], 0, sizeof(*(dst.values[0])) * dst.row * dst.col);

    assert(ctx != NULL);
    StrassenInfo *info = ctx;
    MatrixMulFunc matmul = info->matmul;
    void *matmul_ctx = &(info->matmul_ctx);
    if (l.row > info->threshold) {
        matmul = strassen_matmul;
        matmul_ctx = ctx;
    }

    Matrix l_block[4] = {0},
           r_block[4] = {0},
           dst_block[4] = {0},
           M[7] = {0},
           T[2] = {0};

    int size_split = l.row >> 1;
    matrix_shallow_create_all(l_block, ARRAY_LEN(l_block), size_split);
    matrix_shallow_create_all(r_block, ARRAY_LEN(r_block), size_split);
    matrix_shallow_create_all(dst_block, ARRAY_LEN(dst_block), size_split);
    matrix_create_all(M, ARRAY_LEN(M), size_split, size_split);
    matrix_create_all(T, ARRAY_LEN(T), size_split, size_split);

    matrix_split_4(l, l_block);
    matrix_split_4(r, r_block);
    matrix_split_4(dst, dst_block);

    // 7 multiplication
    matadd(l_block[0], l_block[3], T[0]);
    matadd(r_block[0], r_block[3], T[1]);
    matmul(T[0], T[1], M[0], matmul_ctx);

    matadd(l_block[2], l_block[3], T[0]);
    matmul(T[0], r_block[0], M[1], matmul_ctx);

    matsub(r_block[1], r_block[3], T[0]);
    matmul(l_block[0], T[0], M[2], matmul_ctx);

    matsub(r_block[2], r_block[0], T[0]);
    matmul(l_block[3], T[0], M[3], matmul_ctx);

    matadd(l_block[0], l_block[1], T[0]);
    matmul(T[0], r_block[3], M[4], matmul_ctx);

    matsub(l_block[2], l_block[0], T[0]);
    matadd(r_block[0], r_block[1], T[1]);
    matmul(T[0], T[1], M[5], matmul_ctx);

    matsub(l_block[1], l_block[3], T[0]);
    matadd(r_block[2], r_block[3], T[1]);
    matmul(T[0], T[1], M[6], matmul_ctx);

    // Addition and subtraction
    matadd(M[0], M[3], T[0]);
    matsub(T[0], M[4], T[0]);
    matadd(T[0], M[6], dst_block[0]);
    matadd(M[2], M[4], dst_block[1]);
    matadd(M[1], M[3], dst_block[2]);
    matsub(M[0], M[1], T[0]);
    matadd(T[0], M[2], T[0]);
    matadd(T[0], M[5], dst_block[3]);

    matrix_shallow_free_all(l_block, ARRAY_LEN(l_block));
    matrix_shallow_free_all(r_block, ARRAY_LEN(r_block));
    matrix_shallow_free_all(dst_block, ARRAY_LEN(dst_block));
    matrix_free_all(M, ARRAY_LEN(M));
    matrix_free_all(T, ARRAY_LEN(T));
}

#define max(a, b) (a > b? a : b)

void strassen_matmul(const Matrix l, const Matrix r,
        const Matrix dst, void *ctx)
{
    if (is_pow2(l.row) && l.row == l.col && l.row == r.row && l.row == r.col) {
        strassen_matmul_pow2(l, r, dst, ctx);
        return;
    }

    int max_size = max(max(l.row, l.col), max(r.row, r.col));
    if (is_pow2(max_size) == false)
        max_size = hibit(max_size) << 1;


    Matrix l2 = matrix_create(max_size, max_size);
    Matrix r2 = matrix_create(max_size, max_size);
    Matrix dst2 = matrix_create(max_size, max_size);

    matrix_copy(l2, l, l.row, l.col);
    matrix_copy(r2, r, r.row, r.col);
    matrix_copy(dst2, dst, dst.row, dst.col);

    strassen_matmul_pow2(l2, r2, dst2, ctx);

    matrix_copy(l, l2, l.row, l.col);
    matrix_copy(r, r2, r.row, r.col);
    matrix_copy(dst, dst2, dst.row, dst.col);

    matrix_free(l2);
    matrix_free(r2);
    matrix_free(dst2);
}
