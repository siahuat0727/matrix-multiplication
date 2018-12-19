#ifndef NAIVE_H_
#define NAIVE_H_

#include "matrix.h"

#define INITIALIZE(dst) \
    memset(dst.values[0], 0, sizeof(*(dst.values[0])) * dst.row * dst.col)

void matmul_stride(const Matrix,
                   const Matrix,
                   const Matrix *const dst,
                   int c_row,
                   int c_col,
                   int stride);

void naive_matmul(const Matrix,
                  const Matrix,
                  const Matrix *const dst,
                  void *ctx);
void cache_fri_matmul(const Matrix,
                      const Matrix,
                      const Matrix *const dst,
                      void *ctx);
void sub_matmul(const Matrix, const Matrix, const Matrix *const dst, void *ctx);

#endif
