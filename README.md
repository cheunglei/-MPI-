# 基于MPI的并行矩阵乘法
## 运行环境
Windows 10 64位  
MS-MPI v10.1.2 64位  

## 库函数依赖
mpi.h 及一些c++常用头文件

## 运行方式
### 方式1
在 \基于MPI的并行矩阵乘法\x64\Debug 目录下  
运行cmd命令  
`mpiexec -n 2 基于MPI的并行矩阵乘法.exe m n p times`   
参数m，n，p 分别表示m行n列的矩阵A和n行p列的矩阵B，times表明循环times次求平均值  
### 方式2
直接双击运行 \基于MPI的并行矩阵乘法\x64\Debug\run.bat
