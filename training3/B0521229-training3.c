//author:  B0521229  Ernest Lin 
//for homework3
//Please write a MPI program to sort a list with 100,000,000 numbers, each between 0 and 99999 using bucket sort technique.
//randomly generate numbers between 0~99999





//Further Parallelized Bucket Sort (with divide and conquer approach)


//Ideal Time complexity:O(n/m + n/m log(n/m)),where m is the numbers of processors


//Reference:
//https://github.com/Beachj/Parallelized-Bucket-Sort
//1995-MPI A Message-Passing Interface Standard 1.0



//the needed header file must be included
#include<stdio.h>
#include<stdlib.h>   //for random function
#include<time.h>
#include<mpi.h>
#include<math.h>




#define N 100000000
#define randLarge 100000



//for local sort
int compare(const void*,const void*);
void Sort_local(int*,int);


//Function to find what bucket a number belongs to 
//It depends on how many processors there are
int Find_bucket(int, int);






int main(int argc,char *argv[]){
	
	int world_rank;
	int world_size;
	
	double start=0.0,end; 
	
	//initialize the MPI execution enviroment
	MPI_Init(&argc,&argv);
	//Determines the rank of the calling process within the communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	//Determines the number of processes in the group associated with a communicator   
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); 
	
	
	
	/*                                            Times starts                                                */
	start = MPI_Wtime();
	
	
	
	/*                ---------------------------    step1      --------------------------------              */
	//Partition numbers to m parts/processes       =
	//Generate N/P Elements on each processor      =
	//Generate Array with random numbers          
	
	if(world_rank == 0){
		printf("\n-----------------------------------------------------------------\n");
		printf("\nThere are %d processors\n",world_size);
	}
	
	int NumbersPerProcessor=N/world_size;
	int *LocalArray = (int*)malloc(sizeof(int)*NumbersPerProcessor);
	
	//initially set count on each bucket
	int *count = (int*)calloc(world_size, sizeof(int));
	
	
	int bucket;
	int temp;
	
	
	//begin to generate numbers
	//store in LocalArray
	
	
	for(int i=0;i<NumbersPerProcessor;i++){
		
		//0~99999
		temp=rand()%randLarge;
		
		LocalArray[i]=temp;
	

    /*                ---------------------------    step2      --------------------------------              */
    //Each process divides its numbers to small buckets



		//find bucket 
		bucket=Find_bucket(temp,world_size);
		//and increment count of that bucket
		count[bucket]++;
	}
	
	
	
	
	/*                ---------------------------    step3      --------------------------------              */
	//Merge small buckets to large bucket
	
	//collect elements in small bucket to large bucket
	int *LargeBucketCount = (int*)malloc(sizeof(int)*world_size);
	//MPI ALLTOALL(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm)
	MPI_Alltoall(count, 1, MPI_INT, LargeBucketCount, 1, MPI_INT, MPI_COMM_WORLD);
	
	
	
	
	//accumulate numbers of elements in large buckets
	int LocalNumbers=0;
	for(int i = 0; i < world_size; i++){
		LocalNumbers+= LargeBucketCount[i]; 
	}
	
	
	//Allocate arrays based on counts
	int *BigBuckets = (int*)malloc(sizeof(int)*LocalNumbers);
     
	

    //Now,there are different numbers in different Large Buckets!!


	//Distribute LocalArray to other processors
	
	
	//Allocate arrays required for distribution
	
	int *Local = (int*)malloc(sizeof(int)*NumbersPerProcessor);
	int *index = (int*)calloc(world_size,sizeof(int));






	//Create Displacements for scatterv or gatherv
	
	
	
	//displacements begin from P1
	//Send displacements
	int *Senddis = (int*)malloc(sizeof(int)*world_size);
	Senddis[0] = 0;
	for(int i = 1; i < world_size; i++){
		Senddis[i] = count[i-1] + Senddis[i-1];
	}

	//Receive displacements
	int *Recdis = (int*)malloc(sizeof(int)*world_size);
	Recdis[0] = 0;
	for(int i = 1; i < world_size; i++){
		Recdis[i] = LargeBucketCount[i-1] + Recdis[i-1];
	}

	
	
	
	//Note:SPMD!!!
 	
	for(int i = 0; i < NumbersPerProcessor; i++){

		bucket = Find_bucket(LocalArray[i], world_size);
		
		Local[Senddis[bucket] + index[bucket]] = LocalArray[i];
		
		//update index
		index[bucket]++;
	}  
	
	 
	 
	free(LocalArray);
	
	
	
	//MPI ALLTOALLV(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm)
	MPI_Alltoallv(Local, count, Senddis, MPI_INT, BigBuckets, LargeBucketCount, Recdis, MPI_INT, MPI_COMM_WORLD);
	
	
	
	
	
	/*                ---------------------------    step4      --------------------------------              */
	//Sequential sort each bucket
	
	Sort_local(BigBuckets, LocalNumbers);
	
	
	
	
	//Gather counts of each bucket to root
	int GatherCounts[1];
	GatherCounts[0] = LocalNumbers;
	
	//MPI GATHER( sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm)
	MPI_Gather(GatherCounts, 1, MPI_INT, count, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	//Count at root now holds size each bucket
	
	if(world_rank==0){
	Senddis[0] = 0;
	for(int i = 1; i < world_size; i++){
		Senddis[i] = count[i-1] + Senddis[i-1];
	}
	}
	
	
	/*                ---------------------------    step5      --------------------------------              */
	//Gather all BigBuckets at root 
	
	
	
	int *Final = (int*)malloc(sizeof(int)*N);
	
	
	//MPI GATHERV( sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm)
	MPI_Gatherv(BigBuckets,LocalNumbers, MPI_INT, Final, count, Senddis, MPI_INT, 0, MPI_COMM_WORLD );
	
	
	
	
	
	
	/*                                            Times ends                                                 */
	end = MPI_Wtime();
	
     
	
	
	//print out the resulting time
	printf("  Processor %3d   took %8.6f unit times\n",world_rank,end-start);
	
	
	/*                                            Check results                                              */
	if(world_rank == 0){
		char sorted = 'Y';
		for(int i = 0; i < N - 2; i++){
			if(Final[i] > Final[i+1]){
				sorted = 'N';
				break;
			}
		}

		if(sorted == 'Y'){
			printf("\nSORTING is CORRECT\n");
		}else{
			printf("\nSORTING is NOT CORRECT\n");
		}
		printf("\n-----------------------------------------------------------------\n");
	}


	//Free allocated Arrays
	free(Senddis);
	free(Recdis);
	free(count);
	free(index);
	free(LargeBucketCount);
	free(BigBuckets);
	free(Final);
	
	
	
	//end of MPI
	MPI_Finalize();
	
	

	
	return 0;

	
}


int compare(const void *num1, const void *num2){
	int a1=*(int*)num1;
	int a2=*(int*)num2;
	if(a1<a2)return -1;
	else if(a1==a2)return 0;
	else return 1;
}

void Sort_local(int *array,int arrayLen){
	qsort(array, (size_t)arrayLen, sizeof(int), compare);
}


int Find_bucket(int number, int world_size){
  int x;
  for(x=1; x < world_size+1; x++){
	int BucketRange =(int){x *randLarge / world_size};
	if(number <= BucketRange){		
	  //return bucket number
	  return x - 1; 
	}
  }

}
