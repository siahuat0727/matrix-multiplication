#include "matrix.h"
#include "naive.h"

#define LIST_ADD(list, func, name, ctx) \
    matmul_listadd(list, func, name, ctx, ctx==NULL ? 0 : sizeof(*ctx))

#define M_ROW 16
#define M_COL 16
#define N_ROW 16
#define N_COL 16

typedef void (*MatrixMulFunc)(const Matrix, const Matrix,
        const Matrix * const dst, void *ctx);

typedef struct _MatrixMulFuncEle {
    MatrixMulFunc matmul;
    struct _MatrixMulFuncEle *next;
    char *name;
    char ctx[0];
} MatrixMulFuncEle;

MatrixMulFuncEle *matmul_listadd(MatrixMulFuncEle *list, MatrixMulFunc func,
        char *name, void *ctx, size_t extra_size)
{
    MatrixMulFuncEle *node = malloc(sizeof(MatrixMulFuncEle) + extra_size);
    return_val_if_fail(node != NULL, 0);
    node->matmul = func;
    node->name = name;
    node->next = list;
    memcpy(node->ctx, ctx, extra_size);
    return node;
}

void matmul_freeall(MatrixMulFuncEle *list)
{
    while (list != NULL) {
        MatrixMulFuncEle *temp = list;
        list = list->next;
        free(temp);
    }
}

int main()
{
    // Add matmul methods
    MatrixMulFuncEle *matmul_list = NULL;
    matmul_list = LIST_ADD(matmul_list, naive_matmul, "Naive method", NULL);
    matmul_list =
        LIST_ADD(matmul_list, cache_fri_matmul, "cache friendly method", NULL);

    // Read matrix
    Matrix m = create_mat_1s(M_ROW, M_COL);
    matrix_print(m);
    Matrix n = create_val_per_col(N_ROW, N_COL);
    matrix_print(n);
    Matrix o = matrix_create(M_ROW, N_COL);
    Matrix ans = matrix_create(M_ROW, N_COL);
    naive_matmul(m, n, &ans, NULL);

    for (MatrixMulFuncEle *it = matmul_list; it != NULL; it = it->next) {
        MatrixMulFunc matmul = it->matmul;

        // Time start
        matmul(m, n, &o, it->ctx);
        // Time end

        matrix_print(o);
        printf("%s %s!\n", it->name, matrix_equal(o, ans) ? "correct" : "wrong");
    }
    matrix_free(m);
    matrix_free(n);
    matrix_free(o);
    matrix_free(ans);
    matmul_freeall(matmul_list);
}
