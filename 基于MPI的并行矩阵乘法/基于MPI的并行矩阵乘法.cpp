#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
//mpiexec -n 10 xx
void Hello(void);

int main(int argc, char* argv[])
{
    int me, option, namelen, size;
    char process_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr, "system requires at least 2 processes");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Get_processor_name(process_name, &namelen);
    fprintf(stderr, "Process %d is alive on %s\n", me, process_name);
    MPI_Barrier(MPI_COMM_WORLD);
    Hello();
    MPI_Finalize();
}

void Hello()
{
    int nproc, me;
    int type = 1;
    int buffer[2], node;
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    if (me == 0) {
        printf("\nHello test from all to all\n");
        fflush(stdout);
    }
    for (node = 0; node < nproc; node++)
    {
        if (node != me) {
            buffer[0] = me;
            buffer[1] = node;
            MPI_Send(buffer, 2, MPI_INT, node, type, MPI_COMM_WORLD);
            MPI_Recv(buffer, 2, MPI_INT, node, type, MPI_COMM_WORLD, &status);
            if (buffer[0] != node || buffer[1] != me) {
                fprintf(stderr, "Hello: %d != %d or %d != %d\n", buffer[0], node, buffer[1], me);
                printf("Mismatch on hello process ids; node = %d\n", node);
            }
            printf("Hello from %d to %d\n", me, node);
            fflush(stdout);
        }
    }
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
