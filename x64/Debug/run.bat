@echo off

for /l %%i in (1,1,8) do ( 
mpiexec -n %%i ����MPI�Ĳ��о���˷�.exe 200 200 200 5 
)

mpiexec -n 4 ����MPI�Ĳ��о���˷�.exe 800 800 800 5 

pause