# MPI_Practice
MPI Programming Practice In C

MPI common commands on putty ubuntu environment:(32 processors)
 

port:163.25.101.181


// XXXX file name

mpicc XXXX.c -o test                      

// 4 indicates core numbers

mpirun -np  4 ./test                      

Train1-MPI send/recv compares to collective call- 1.Broadcast 2.Scatter 3.Gather

Train2-Find how mant primes within 10 Billions with MPI parallel programming

Train3-Bucket sort 100 Millions numbers(Range 0-99999) with MPI parallel programming


![Mpi useful function](https://imgur.com/PuyS1bM)
