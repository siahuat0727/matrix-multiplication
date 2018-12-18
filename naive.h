#ifndef NAIVE_H_
#define NAIVE_H_

#include "matrix.h"

void matmul_4(const Matrix,
              const Matrix,
              const Matrix *const dst,
              int c_row,
              int c_col);

void naive_matmul(const Matrix, const Matrix, const Matrix * const dst, void *ctx);
void cache_fri_matmul(const Matrix, const Matrix, const Matrix * const dst, void *ctx);
void sub_matmul(const Matrix, const Matrix, const Matrix *const dst, void *ctx);

#endif
