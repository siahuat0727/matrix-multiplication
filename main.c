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
#define MAXN 4096

enum MatmulType {MAT_NAIVE, MAT_CACHE_FRI, MAT_SUB_MATRIX, MAT_SIMD,
    MAT_STRASSEN};
enum Choice {NO, YES};

bool bIsOutput = false;

typedef void (*MatrixMulFunc)(const Matrix,
        const Matrix, const Matrix, void *ctx);

typedef struct _MatrixMulFuncEle {
    MatrixMulFunc matmul;
    struct _MatrixMulFuncEle *next;
    char *name;
    char ctx[0];
} MatrixMulFuncEle;

MatrixMulFunc matmuls[] = {naive_matmul, cache_fri_matmul, sub_matmul,
    SIMD_matmul, strassen_matmul};

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

int read_interval(int min, int max)
{
    int ret = min - 1;
    printf("Please input value from %d to %d\n", min, max);
    while (scanf("%d", &ret) != EOF) {
        while(getchar() != '\n');
        if (ret >= min && ret <= max)
            return ret;
    }
    return -1;
}

void get_matmul_info(enum MatmulType m, char *str, MatmulCtx *matmul_ctx)
{
    int stride;
    switch (m) {
        case MAT_NAIVE:
            strcat(str, "Naive method");
            break;
        case MAT_CACHE_FRI:
            strcat(str, "Cache friendly method");
            break;
        case MAT_SUB_MATRIX:
            puts("Stride?");
            stride = read_interval(4, MAXN);
            sprintf(str + strlen(str), "Sub matrix method with stride = %d", stride);
            matmul_ctx->sub_matrix_info.stride = stride;
            break;
        case MAT_SIMD:
            strcat(str, "SIMD");
            break;
        default:
            printf("Switch case not match: %d\n", m);
            break;
    }
}

int main()
{
    // Add matmul methods
    MatrixMulFuncEle *matmul_list = NULL;

    // Read matrix
    int m_row, m_col, n_row, n_col;
    puts("Square matrix?");
    puts("0: No");
    puts("1: Yes");
    if (read_interval(NO, YES) == YES) {
        m_row = m_col = n_row = n_col = read_interval(1, MAXN);
    } else {
        puts("Enter first matrix row");
        m_row = read_interval(1, MAXN);
        puts("Enter first matrix col");
        m_col = read_interval(1, MAXN);
        puts("Enter second matrix row");
        n_row = read_interval(1, MAXN);
        puts("Enter second matrix col");
        n_col = read_interval(1, MAXN);
    }
    Matrix m = create_mat_1s(m_row, m_col);
    if (bIsOutput)
        matrix_print(m);
    Matrix n = create_val_per_col(n_row, n_col);
    if (bIsOutput)
        matrix_print(n);
    Matrix o = matrix_create(m_row, n_col);
    Matrix ans = matrix_create(m_row, n_col);
    naive_matmul(m, n, ans, NULL);

    // Clock
    clock_t tic, toc;
    while (true) {
        puts("Which matmul?");
        puts("0: naive");
        puts("1: cache friendly naive");
        puts("2: submatrix");
        puts("3: simd");
        puts("4: strassen");
        enum MatmulType mm = read_interval(MAT_NAIVE, MAT_STRASSEN);
        char str[64] = "";
        union {
            MatmulCtx union_info;
            StrassenInfo strassen_info;
        } matmul_ctx;
        if (mm != MAT_STRASSEN) {
            get_matmul_info(mm, str, &(matmul_ctx.union_info));
        } else {
            puts("threshold?");
            int threshold = read_interval(2, MAXN);
            matmul_ctx.strassen_info.threshold = threshold;
            sprintf(str + strlen(str), "Strassen with threshold = %d + ", threshold);
            puts("Which matmul (when size < threshold)?");
            puts("0: naive");
            puts("1: cache friendly naive");
            puts("2: submatrix");
            puts("3: simd");
            enum MatmulType n = read_interval(MAT_NAIVE, MAT_SIMD);
            matmul_ctx.strassen_info.matmul = matmuls[n];
            get_matmul_info(n, str, &(matmul_ctx.strassen_info.matmul_ctx));
        }
        // TODO: No need list
        matmul_list = LIST_ADD(matmul_list, matmuls[mm], str, &matmul_ctx);
        INITIALIZE(o);

        tic = clock();
        matmul_list->matmul(m, n, o, matmul_list->ctx);
        toc = clock();

        printf("\n%s:\n", matmul_list->name);
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
