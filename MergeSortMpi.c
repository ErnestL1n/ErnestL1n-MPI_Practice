//for Final_Project MergeSort
//Please write a parallel program to sort 1000000000 numbers (value range: 1-100), randomly generated in master. 
//a MPI program with 2, 4, 8, 16 processors

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h> 
#include <mpi.h>


#define N 1000000000
#define range 100

int max_rank;
int tag;

//for qsort
int compare(const void*,const void*);

void merge(int[],int,int[]);

void mergesort_serial(int a[], int size, int temp[]);

void mergesort_parallel_mpi(int a[], int size, int temp[], int level, int my_rank, MPI_Comm comm);

int my_topmost_level_mpi(int my_rank);

void run_root_mpi (int a[], int size, int temp[], MPI_Comm comm);

void run_helper_mpi(int my_rank, MPI_Comm comm);


int main(int argc, char* argv[]) {
    
	MPI_Init(&argc, &argv);
	
    int comm_size; 
    int my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	// 0~n-1
    max_rank = comm_size - 1;
	
	
	
	//tag is your option
    tag = 21229;
	
	

	
    
    if (my_rank == 0) { 
	
			
	    int i;
		
	    int* array = malloc(sizeof(int)*N);	
	    //generate N numbers which range is : 1-100,put into array(bugs fiexd here ,since we are in MPI, generate numbers 
		// only on root is correct)
        for(i=0; i<N; i++) array[i]=(rand()%100)+1;			
		int* temp  = malloc(sizeof(int)*N);
		
		printf("\n\n--------------------There are %d processes created--------------------\n\n",comm_size);

       		
		// Sort with root process
		double start = MPI_Wtime();
		
		run_root_mpi(array, N, temp, MPI_COMM_WORLD);
		
	    double end = MPI_Wtime();
		
		
		printf("Total execution time of mergesort_parallel_mpi is %f \n",end-start);
		
		
		// Result check
		
		char sorted = 'Y';
        for(i = 0; i < N - 2; i++){
		if(array[i] > array[i+1]){
				sorted = 'N';
				break;
			}			
		}
		if(sorted == 'Y'){
			printf("\nSORTING is CORRECT\n");
		}else{
			printf("\nSORTING is NOT CORRECT\n");
		}
		printf("\n--------------------------------------------------------------------\n");	
	
	
	
	
			
 
	}      // Root process end
	
	       // Other processes  
		   
    else {
        run_helper_mpi(my_rank, MPI_COMM_WORLD);     
    }
    MPI_Finalize();
	
	
    return 0;
}


// Root process code
void run_root_mpi (int a[], int size, int temp[], MPI_Comm comm) {
    //int my_rank;
    //MPI_Comm_rank(comm, &my_rank);
    //if (my_rank != 0) {
    //   printf("Error: run_root_mpi called from process %d; must be called from process 0 only\n", my_rank);
    //  MPI_Abort(MPI_COMM_WORLD, 1);
    //}
    mergesort_parallel_mpi(a, size, temp, 0, 0, comm); 
    /* level=0; my_rank=root_rank=0;*/
    return;
}


// Helper process code
void run_helper_mpi(int my_rank, MPI_Comm comm) {
    int level = my_topmost_level_mpi(my_rank);
    // probe for a message and determine its size and sender
    MPI_Status status; int size;
    MPI_Probe(MPI_ANY_SOURCE, tag, comm, &status);
    MPI_Get_count(&status, MPI_INT, &size);
    int parent_rank = status.MPI_SOURCE;
    // allocate int a[size], temp[size] 
    int *a = malloc(sizeof(int)*size); 
    int *temp = malloc(sizeof(int)*size);
    MPI_Recv(a, size, MPI_INT, parent_rank, tag, comm, &status);
    mergesort_parallel_mpi(a, size, temp, level, my_rank, comm);  
    // Send sorted array to parent process
    MPI_Send(a, size, MPI_INT, parent_rank, tag, comm);
    return;
}

// Given a process rank, calculate the top level of the process tree in which the process participates
// Root assumed to always have rank 0 and to participate at level 0 of the process tree
int my_topmost_level_mpi(int my_rank) {
    int level = 0;
    while ((1<<level)<= my_rank) level++;
    return level;    
}



// MPI merge sort
void mergesort_parallel_mpi(int a[], int size, int temp[], 
                        int level, int my_rank, MPI_Comm comm) {
    int helper_rank = my_rank + (1<<level);
    if (helper_rank > max_rank) { // no more processes available
    	mergesort_serial(a, size, temp);
    } else {
//printf("Process %d has helper %d\n", my_rank, helper_rank);
        MPI_Request request; MPI_Status status;
        // Send second half, asynchronous
        MPI_Isend(a + size/2, size - size/2, MPI_INT, helper_rank, tag, comm, &request);
        // Sort first half
        mergesort_parallel_mpi(a, size/2, temp, level+1, my_rank, comm);
        // Receive second half sorted
        MPI_Recv(a + size/2, size - size/2, MPI_INT, helper_rank, tag, comm, &status);
        // Merge the two sorted sub-arrays through temp
        merge(a, size, temp);
    } 
    return;
}

void mergesort_serial(int a[], int size, int temp[]) {
    // Switch to qsort for small arrays
     if (size < 50) {
       qsort(a,size,sizeof(int), compare);
       return;
    }    
    mergesort_serial(a, size/2, temp);
    mergesort_serial(a + size/2, size - size/2, temp);
    merge(a, size, temp);
}


void merge(int a[], int size, int temp[]) {
    int left = 0;
    int right = size/2;
    int tempindex = 0;
    while (left < size/2 && right < size) {
        if (a[left] < a[right]) {
            temp[tempindex] = a[left];
            left++;
        } else {
            temp[tempindex] = a[right];
            right++;
        }
        tempindex++;
    }
    while (left < size/2) {
        temp[tempindex] = a[left];
        left++;
        tempindex++;
    }
    while (right < size) {
        temp[tempindex] = a[right];
        right++;
        tempindex++;
    }
    // Copy sorted temp array into main array
    memcpy(a, temp, size*sizeof(int));
}



int compare(const void *num1, const void *num2){
	int a1=*(int*)num1;
	int a2=*(int*)num2;
	if(a1<a2)return -1;
	else if(a1==a2)return 0;
	else return 1;
}


