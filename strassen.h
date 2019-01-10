#ifndef STRASSEN_H_
#define STRASSEN_H_

#include "matrix.h"
#include "matmul.h"

void strassen_matmul(const Matrix, const Matrix, const Matrix, void *ctx);

typedef void (*MatrixMulFunc)(const Matrix,
        const Matrix, const Matrix, void *ctx);

typedef union {
    SubMatrixInfo sub_matrix_info;
} MatmulCtx;

typedef struct _StrassenInfo {
    MatrixMulFunc matmul;
    MatmulCtx matmul_ctx;
    int threshold;
} StrassenInfo;

#endif
