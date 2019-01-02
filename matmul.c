#include "matmul.h"

#define CHECK(l, r, dst)                                                      \
    do {                                                                      \
        return_if_fail(                                                       \
            "Dimension property of matrix multiplication" && l.col == r.row); \
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

void SIMD_matmul4(const Matrix l,
                  const Matrix r,
                  const Matrix dst,
                  int c_row,
                  int c_col)
{
    __m128i I[4], R[4], D[4], T[4], S[4], Sum[4];

    for (int i = 0; i < 4; i++)
        Sum[i] = _mm_setzero_si128();

    for (int k = 0; k < l.col; k += 4) {
        for (int i = 0; i < 4; i++)
            I[i] = _mm_load_si128((__m128i *) (&l.values[i + c_row][k]));

        for (int i = 0; i < 4; i++)
            R[i] = _mm_set_epi32(
                r.values[k][i + c_col], r.values[k + 1][i + c_col],
                r.values[k + 2][i + c_col], r.values[k + 3][i + c_col]);


        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++)
                T[j] = _mm_mullo_epi32(I[i], R[j]);

            for (int j = 0; j < 2; j++) {
                S[j] = _mm_unpacklo_epi32(T[j * 2], T[j * 2 + 1]);
                S[j + 2] = _mm_unpackhi_epi32(T[j * 2], T[j * 2 + 1]);
            }

            for (int j = 0; j < 2; j++) {
                D[j] = _mm_unpacklo_epi64(S[2 * j], S[2 * j + 1]);
                D[j + 2] = _mm_unpackhi_epi64(S[2 * j], S[2 * j + 1]);
            }

            for (int j = 0; j < 4; j++)
                Sum[i] = _mm_add_epi32(Sum[i], D[j]);
        }
    }

    for (int i = 0; i < 4; i++)
        _mm_store_si128((__m128i *) (&dst.values[c_row + i][c_col]), Sum[i]);
}

void SIMD_matmul(const Matrix l, const Matrix r, const Matrix dst, void *ctx)
{
    CHECK(l, r, dst);
    for (int i = 0; i < l.row; i += 4)
        for (int j = 0; j < r.col; j += 4)
            SIMD_matmul4(l, r, dst, i, j);
}

void SIMD_AVX_matmul8(const Matrix l,
                      const Matrix r,
                      const Matrix dst,
                      int c_row,
                      int c_col)
{
    __m256i I[8], R[8], S[8], Sum[8];

    for (int i = 0; i < 8; i++)
        Sum[i] = _mm256_setzero_si256();

    for (int k = 0; k < l.col; k += 8) {
        for (int i = 0; i < 8; i++)
            R[i] = _mm256_load_si256((__m256i *) (&r.values[k + i][c_col]));


        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                I[j] = _mm256_set1_epi32(l.values[c_row + i][k + j]);
                S[j] = _mm256_mullo_epi32(R[j], I[j]);
            }

            for (int j = 0; j < 8; j++)
                Sum[i] = _mm256_add_epi32(Sum[i], S[j]);
        }
    }

    for (int i = 0; i < 8; i++)
        _mm256_store_si256((__m256i *) (&dst.values[c_row + i][c_col]), Sum[i]);
}

void SIMD_AVX_matmul(const Matrix l,
                     const Matrix r,
                     const Matrix dst,
                     void *ctx)
{
    CHECK(l, r, dst);
    for (int i = 0; i < l.row; i += 8)
        for (int j = 0; j < r.col; j += 8)
            SIMD_AVX_matmul8(l, r, dst, i, j);
}
