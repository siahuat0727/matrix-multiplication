#include <time.h>
#include "matrix.h"
#include "matmul.h"
#include "strassen.h"

#define LIST_ADD(list, func, name, ctx) \
    matmul_listadd(list, func, name, ctx, ctx==NULL ? 0 : sizeof(*ctx))
#define SQUARE 16
#define M_ROW SQUARE
#define M_COL SQUARE
#define N_ROW SQUARE
#define N_COL SQUARE

bool bIsOutput = true;

typedef void (*MatrixMulFunc)(const Matrix,
        const Matrix, const Matrix, void *ctx);

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
    matmul_list = LIST_ADD(matmul_list, cache_fri_matmul,
            "Cache friendly method", NULL);
    matmul_list = LIST_ADD(matmul_list, sub_matmul, "Sub matrix method",
            &((SubMatrixInfo){.stride=4}));
    matmul_list = LIST_ADD(matmul_list, strassen_matmul,
            "Strassen + cache friendly method",
            &((StrassenInfo){.matmul=cache_fri_matmul, .threshold=4}));
    matmul_list = LIST_ADD(matmul_list, strassen_matmul,
            "Strassen + naive method",
            &((StrassenInfo){.matmul=naive_matmul, .threshold=4}));
    matmul_list = LIST_ADD(matmul_list, strassen_matmul,
            "Strassen + sub matrix method",
            &((StrassenInfo){
                .matmul = sub_matmul,
                .matmul_ctx = (SubMatrixInfo){.stride=4},
                .threshold = 4}));

    // Read matrix
    Matrix m = create_mat_1s(M_ROW, M_COL);
    if (bIsOutput)
        matrix_print(m);
    Matrix n = create_val_per_col(N_ROW, N_COL);
    if (bIsOutput)
        matrix_print(n);
    Matrix o = matrix_create(M_ROW, N_COL);
    Matrix ans = matrix_create(M_ROW, N_COL);
    naive_matmul(m, n, ans, NULL);

    // Clock
    clock_t tic, toc;

    for (MatrixMulFuncEle *it = matmul_list; it != NULL; it = it->next) {
        MatrixMulFunc matmul = it->matmul;

        INITIALIZE(o);

        tic = clock();
        matmul(m, n, o, it->ctx);
        toc = clock();

        printf("\n%s:\n", it->name);
        if (bIsOutput)
            matrix_print(o);
        printf("%s!\n", matrix_equal(o, ans) ? "correct" : "wrong");
        printf("CPU time:%f seconds\n", (double) (toc - tic) / CLOCKS_PER_SEC);
    }
    matrix_free(m);
    matrix_free(n);
    matrix_free(o);
    matrix_free(ans);
    matmul_freeall(matmul_list);
}
