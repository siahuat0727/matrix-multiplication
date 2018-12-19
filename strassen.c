#include "strassen.h"
#include "naive.h"

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

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

static void mat_arith(const Matrix a, const Matrix b,
        const Matrix * const dst, int (*arith)(int, int))
{
    assert(a.row == b.row);
    assert(a.col == b.col);
    assert(dst != NULL);
    assert(dst->row == a.row);
    assert(dst->col == a.col);
    assert(dst->values != NULL);
    assert(arith != NULL);

    for (int i = 0; i < dst->row; ++i)
        for (int j = 0; j < dst->col; ++j)
            dst->values[i][j] = arith(a.values[i][j], b.values[i][j]);
}

static int add(int a, int b)
{
    return a + b;
}

static void matadd(const Matrix a, const Matrix b, const Matrix * const dst)
{
    mat_arith(a, b, dst, &add);
}

static int sub(int a, int b)
{
    return a - b;
}

static void matsub(const Matrix a, const Matrix b, const Matrix * const dst)
{
    mat_arith(a, b, dst, &sub);
}

static bool is_pow2(int n)
{
    return !(n & (n-1));
}

void strassen_matmul(const Matrix a, const Matrix b,
        const Matrix * const dst, void *ctx)
{
    return_if_fail(("Only accept square metrices whose size is power of 2",
                is_pow2(a.row) && a.row == a.col));
    assert(b.row == a.row);
    assert(b.col == a.col);
    assert(dst != NULL);
    assert(dst->row == a.row);
    assert(dst->col == a.col);
    memset(dst->values[0], 0, sizeof(*(dst->values[0])) * dst->row * dst->col);

    assert(ctx != NULL);
    StrassenInfo *info = ctx;
    MatrixMulFunc matmul = info->matmul;
    void *matmul_ctx = &(info->matmul_ctx);
    if (a.row > info->threshold) {
        matmul = strassen_matmul;
        matmul_ctx = ctx;
    }

    Matrix a_block[4] = {0},
           b_block[4] = {0},
           dst_block[4] = {0},
           M[7] = {0},
           T[2] = {0};

    int size_split = a.row >> 1;
    matrix_shallow_create_all(a_block, ARRAY_LEN(a_block), size_split);
    matrix_shallow_create_all(b_block, ARRAY_LEN(b_block), size_split);
    matrix_shallow_create_all(dst_block, ARRAY_LEN(dst_block), size_split);
    matrix_create_all(M, ARRAY_LEN(M), size_split, size_split);
    matrix_create_all(T, ARRAY_LEN(T), size_split, size_split);

    matrix_split_4(a, a_block);
    matrix_split_4(b, b_block);
    matrix_split_4(*dst, dst_block);

    // 7 multiplication
    matadd(a_block[0], a_block[3], &T[0]);
    matadd(b_block[0], b_block[3], &T[1]);
    matmul(T[0], T[1], &M[0], matmul_ctx);

    matadd(a_block[2], a_block[3], &T[0]);
    matmul(T[0], b_block[0], &M[1], matmul_ctx);

    matsub(b_block[1], b_block[3], &T[0]);
    matmul(a_block[0], T[0], &M[2], matmul_ctx);

    matsub(b_block[2], b_block[0], &T[0]);
    matmul(a_block[3], T[0], &M[3], matmul_ctx);

    matadd(a_block[0], a_block[1], &T[0]);
    matmul(T[0], b_block[3], &M[4], matmul_ctx);

    matsub(a_block[2], a_block[0], &T[0]);
    matadd(b_block[0], b_block[1], &T[1]);
    matmul(T[0], T[1], &M[5], matmul_ctx);

    matsub(a_block[1], a_block[3], &T[0]);
    matadd(b_block[2], b_block[3], &T[1]);
    matmul(T[0], T[1], &M[6], matmul_ctx);

    // Addition and subtraction
    matadd(M[0], M[3], &T[0]);
    matsub(T[0], M[4], &T[0]);
    matadd(T[0], M[6], &dst_block[0]);
    matadd(M[2], M[4], &dst_block[1]);
    matadd(M[1], M[3], &dst_block[2]);
    matsub(M[0], M[1], &T[0]);
    matadd(T[0], M[2], &T[0]);
    matadd(T[0], M[5], &dst_block[3]);

    matrix_shallow_free_all(a_block, ARRAY_LEN(a_block));
    matrix_shallow_free_all(b_block, ARRAY_LEN(b_block));
    matrix_shallow_free_all(dst_block, ARRAY_LEN(dst_block));
    matrix_free_all(M, ARRAY_LEN(M));
    matrix_free_all(T, ARRAY_LEN(T));
}
