#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string>
#include "mpi.h"
#define MAXSIZE 100
//mpiexec -n 2 基于MPI的并行矩阵乘法.exe 200 200 200 5
//编码采用gbk

void init(double * A,double * B,int m, int n, int p);

void serialCpt(double* A, double* B, double* C, int m, int n, int p);

// 矩阵初始化 数据区间[0,1]
void init(double* A, double* B,int m, int n, int p) {
    srand((unsigned)time(NULL));
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++) {
            A[i * n + j] = (double)rand() / RAND_MAX;
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < p; j++) {
            B[i * p + j] = (double)rand() / RAND_MAX;
        }
    }
}

//单进程串行计算
void serialCpt(double* A, double* B, double* C, int m, int n, int p) {
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < p; j++)
        {
            C[i * p + j] = 0.0;
            for (int k = 0; k < n; k++)
            {
                C[i * p + j] += A[i * n + k] * B[k * p + j];
            }
        }
    }
}


int main(int argc, char* argv[])
{
    int myid, option, numprocs, size;
    //矩阵A形状(m, n),B形状(n, p), 运行times次求平均值 
    int m = 1000, n = 1000, p = 1000, times = 5;
    if(strlen(argv[1]))
        sscanf_s(argv[1], "%d", &m);
    if (strlen(argv[2]))
        sscanf_s(argv[2], "%d", &n);
    if (strlen(argv[3]))
        sscanf_s(argv[3], "%d", &p);
    if (strlen(argv[4]))
        sscanf_s(argv[4], "%d", &times);
    char process_name[MPI_MAX_PROCESSOR_NAME];
    double startwtime = 0.0, endwtime = 0.0;
    double parallel_time = 0.0, serial_time = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int count = (int)ceil( m * 1.0 / size);
    //防止聚集时最后一个进程溢出，需要将m扩大至count * size
    double* A = new double[count * size * n];
    double* B = new double[n * p];
    double* C = new double[count * size * p];
    double* subA = new double[count * n];
    double* subC = new double[count * p];

    if (myid == 0) {
        printf("num of process: %d\n", size);
        printf("matrix A shape: (%d, %d)\n", m, n);
        printf("matrix B shape: (%d, %d)\n", n, p);
        //printf("count: %d\n", count);
        init(A, B, m, n, p);

        //for (int i = 0; i < m; i++)
        //{
        //    for (int j = 0; j < n; j++) {
        //        printf("%lf\t", A[i * n + j]);
        //    }
        //    printf("\n");
        //    //printf("on process: %d\n", myid);
        //}
        //for (int i = 0; i < n; i++)
        //{
        //    for (int j = 0; j < p; j++) {
        //        printf("%lf\t", B[i * p + j]);
        //    }
        //    printf("\n");
        //    //printf("on process: %d\n", myid);
        //}

    }
    for (int t = 0; t < times; t++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        startwtime = MPI_Wtime();
        //散播矩阵A
        MPI_Scatter(A, count * n, MPI_DOUBLE, subA, count * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(B, n * p, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // 等待所有进程执行完毕
        MPI_Barrier(MPI_COMM_WORLD);

        //并行计算
        for (int i = 0; i < count; i++)
        {
            for (int j = 0; j < p; j++)
            {
                //printf("%lf\t", subA[i * n + j]);
                subC[i * p + j] = 0.0;
                for (int k = 0; k < n; k++)
                {
                    subC[i * p + j] += subA[i * n + k] * B[k * p + j];
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        //收集
        MPI_Gather(subC, count * p, MPI_DOUBLE, C, count * p, MPI_DOUBLE, 0, MPI_COMM_WORLD);


        if (myid == 0)
        {
            //for (int i = 0; i < m; i++)
            //{
            //    for (int j = 0; j < p; j++)
            //    {
            //        printf("%lf\t", C[i * p + j]);
            //    }
            //    printf("\n");
            //}
            endwtime = MPI_Wtime();
            parallel_time += endwtime - startwtime;
            if(t == times - 1)
                printf("parallel time: %.4lf\n", 1000 * parallel_time/ times);
        }
    }

    
    for (int t = 0; t < times; t++)
    {
        if (myid == 0)
        {
            startwtime = MPI_Wtime();
            serialCpt(A, B, C, m, n, p);
            endwtime = MPI_Wtime();
            serial_time += endwtime - startwtime;
            if (t == times - 1) {
                printf("serial time: %.4lf\n", 1000 * serial_time/times);
                printf("Speedup ratio: %.4lf\n", serial_time / parallel_time);
            }
        }
    }

    MPI_Finalize();
}
