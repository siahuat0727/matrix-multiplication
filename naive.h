#ifndef NAIVE_H_
#define NAIVE_H_

#include "matrix.h"

Matrix naive_matmul(const Matrix, const Matrix, void *ctx);
Matrix cache_fri_matmul(const Matrix, const Matrix, void *ctx);

#endif
