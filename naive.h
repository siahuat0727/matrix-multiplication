#ifndef NAIVE_H_
#define NAIVE_H_

#include "matrix.h"

void naive_matmul(const Matrix, const Matrix, const Matrix * const dst, void *ctx);
void cache_fri_matmul(const Matrix, const Matrix, const Matrix * const dst, void *ctx);

#endif
