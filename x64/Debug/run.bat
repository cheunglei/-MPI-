@echo off

for /l %%i in (1,1,8) do ( 
mpiexec -n %%i 基于MPI的并行矩阵乘法.exe 200 200 200 5 
)

mpiexec -n 4 基于MPI的并行矩阵乘法.exe 800 800 800 5 

pause