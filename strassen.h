#ifndef STRASSEN_H_
#define STRASSEN_H_

#include "matrix.h"

void strassen_matmul(const Matrix, const Matrix,
        const Matrix * const dst, void *ctx);

typedef void (*MatrixMulFunc)(const Matrix, const Matrix,
        const Matrix * const dst, void *ctx);

typedef struct _StrassenInfo {
    MatrixMulFunc matmul;
    union {
        // matmul info
    } matmul_ctx;
    int threshold;
} StrassenInfo;

#endif
