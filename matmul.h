#ifndef NAIVE_H_
#define NAIVE_H_

#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include "matrix.h"

#define INITIALIZE(dst) \
    memset(dst.values[0], 0, sizeof(*(dst.values[0])) * dst.row * dst.col)

typedef struct _SubMatrixInfo {
    int stride;
} SubMatrixInfo;

void matmul_stride(const Matrix l,
                   const Matrix r,
                   const Matrix dst,
                   int c_row,
                   int c_col,
                   int stride);
void naive_matmul(const Matrix, const Matrix, const Matrix, void *ctx);
void cache_fri_matmul(const Matrix, const Matrix, const Matrix, void *ctx);
void sub_matmul(const Matrix, const Matrix, const Matrix, void *ctx);
void SIMD_matmul4(const Matrix,
                  const Matrix,
                  const Matrix,
                  int c_row,
                  int c_col);
void SIMD_matmul(const Matrix, const Matrix, const Matrix, void *ctx);
void SIMD_AVX_matmul8(const Matrix,
                      const Matrix,
                      const Matrix,
                      int c_row,
                      int c_col);
void SIMD_AVX_matmul(const Matrix, const Matrix, const Matrix, void *ctx);


#endif
