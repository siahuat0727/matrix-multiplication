#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "matrix.h"
#include "matmul.h"
#include "strassen.h"

#define MAXN 4096
#define PRINT_QUES(ques)                        \
    do {                                        \
        if (IS_SCRIPT)                          \
            break;                              \
        puts("------------------------------"); \
        char **q = ques;                        \
        int len = sizeof(ques)/sizeof(ques[0]); \
        puts(q[0]);                             \
        for (int i = 1; i < len; ++i)           \
            printf("%d: %s\n", i - 1, q[i]);    \
    } while (0)

enum MatmulType {MATMUL_NAIVE=1, MATMUL_CACHE_FRI, MATMUL_SUB_MATRIX,
    MATMUL_SIMD, MATMUL_SIMD_AVX, MATMUL_STRASSEN};
enum Choice {NO, YES};

MatrixMulFunc matmuls[] = {NULL, naive_matmul, cache_fri_matmul, sub_matmul,
    SIMD_matmul, SIMD_AVX_matmul, strassen_matmul};

bool IS_SCRIPT = false;

int read_interval(int min, int max)
{
    int val = min - 1;
    while (true) {
        if (IS_SCRIPT == false)
            printf("Please input value from %d to %d: ", min, max);
        int ret = scanf("%d", &val);
        if (ret == EOF)
            break;
        if (ret == 0)
            while(getchar() != '\n');  // Clear input buffer
        if (val >= min && val <= max)
            return val;
    }
    return 0;
}

void get_matmul_info(enum MatmulType m, char *str, MatmulCtx *matmul_ctx)
{
    int stride;
    switch (m) {
        case MATMUL_NAIVE:
            strcat(str, "Naive");
            break;
        case MATMUL_CACHE_FRI:
            strcat(str, "Cache-friendly-naive");
            break;
        case MATMUL_SUB_MATRIX:
            PRINT_QUES(((char*[]){"Stride?"}));
            stride = read_interval(2, MAXN);
            sprintf(str + strlen(str), "Submatrix(stride=%d)", stride);
            matmul_ctx->sub_matrix_info.stride = stride;
            break;
        case MATMUL_SIMD:
            strcat(str, "SIMD");
            break;
        case MATMUL_SIMD_AVX:
            strcat(str, "SIMD-AVX");
            break;
        default:
            printf("Switch case not match: %d\n", m);
            break;
    }
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--test") == 0)
        IS_SCRIPT = true;

    PRINT_QUES(((char*[]){"Print matrix?", "No", "Yes"}));
    bool is_output = read_interval(NO, YES);

    PRINT_QUES(((char*[]){"Square matrix?", "No", "Yes"}));
    bool is_square = read_interval(NO, YES);

    // Read matrix
    int m_row, m_col, n_row, n_col;
    if (is_square) {
        PRINT_QUES(((char*[]){"Matrix size?"}));
        m_row = m_col = n_row = n_col = read_interval(1, MAXN);
    } else {
        PRINT_QUES(((char*[]){"First matrix row?"}));
        m_row = read_interval(1, MAXN);
        PRINT_QUES(((char*[]){"First matrix column?"}));
        m_col = read_interval(1, MAXN);
        PRINT_QUES(((char*[]){"Second matrix row?"}));
        n_row = read_interval(1, MAXN);
        PRINT_QUES(((char*[]){"Second matrix column?"}));
        n_col = read_interval(1, MAXN);
    }
    printf("Matrix multiplication: (%d x %d) x (%d x %d)\n\n",
            m_row, m_col, n_row, n_col);

    Matrix m = create_mat_1s(m_row, m_col);
    if (is_output)
        matrix_print(m);
    Matrix n = create_val_per_col(n_row, n_col);
    if (is_output)
        matrix_print(n);
    Matrix o = matrix_create(m_row, n_col);
    Matrix ans = matrix_create(m_row, n_col);
    naive_matmul(m, n, ans, NULL);

    FILE *fp = fopen("runtime.txt", "w");
    return_val_if_fail(fp != NULL, -1);

    while (true) {
        PRINT_QUES(((char*[]){
                    "Choose a matrix multiplication method",
                    "quit",
                    "naive",
                    "cache friendly naive",
                    "submatrix",
                    "simd",
                    "simd_avx",
                    "strassen"}));
        enum MatmulType mm = read_interval(0, MATMUL_STRASSEN);
        if (mm == 0)
            break;
        char matmul_info[64] = "";
        union {
            MatmulCtx union_ctx;
            StrassenInfo strassen_ctx;
        } matmul_ctx;
        if (mm != MATMUL_STRASSEN) {
            get_matmul_info(mm, matmul_info, &(matmul_ctx.union_ctx));
        } else {
            PRINT_QUES(((char*[]){"threshold?"}));
            int threshold = read_interval(2, MAXN);
            matmul_ctx.strassen_ctx.threshold = threshold;
            sprintf(matmul_info, "Strassen(threshold=%d)+", threshold);

            PRINT_QUES(((char*[]){
                        "Matrix multiplication method when size < threshold)",
                        "naive",
                        "cache friendly naive",
                        "submatrix",
                        "simd",
                        "simd_avx"}));
            enum MatmulType nn = read_interval(MATMUL_NAIVE, MATMUL_SIMD_AVX);
            matmul_ctx.strassen_ctx.matmul = matmuls[nn];
            get_matmul_info(nn, matmul_info,
                    &(matmul_ctx.strassen_ctx.matmul_ctx));
        }
        INITIALIZE(o);

#if defined(PERF)
        int pid = getpid();
        int cpid = fork();
        if (cpid == 0) {
            // child process .  Run perf stat
            char buf[128];
            sprintf(buf, "perf stat -e cache-misses,cache-references,"
                    "instructions,cycles,branches,branch-misses -p %d", pid);
            execl("/bin/sh", "sh", "-c", buf, NULL);
        }
        setpgid(cpid, 0);
#endif
        clock_t tic = clock();
        matmuls[mm](m, n, o, &matmul_ctx);
        clock_t toc = clock();
        double time_used = (double)(toc - tic) / CLOCKS_PER_SEC;
#if defined(PERF)
        kill(-cpid, SIGINT);
        wait(&(int){0});
#endif

        if (is_output)
            matrix_print(o);
        puts(matmul_info);
        printf("%s!\n", matrix_equal(o, ans) ? "correct" : "wrong");
        printf("CPU time: %f seconds\n\n", time_used);
        fprintf(fp, "%s %.3f\n", matmul_info, time_used);
        puts("###############################################");
    }

    matrix_free(m);
    matrix_free(n);
    matrix_free(o);
    matrix_free(ans);
}
