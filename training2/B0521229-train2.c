//author:  B0521229  Ernest Lin 
//for homework2 A MOTIVATING EXAMPLE 
//A major issue in devising a parallel algorithm for a given problem is the way in which the computational load is divided between the multiple processors. 
//Problem: Prime number finding
//Please write a MPI program to find all prime numbers less than 10,000,000,000.
//List the total number of prime numbers.
//Calculate the speedup ratio for p=2, 4, 8, 16.


//reference:Sieve of Eratosthenes algorithm
//Time complexity : O(n*log(log(n)))
//only use this algorithm on primes number within sqrt(10000000000)


//the needed header file must be included
#include<stdio.h>
#include<stdlib.h>  
#include<time.h>
#include<mpi.h>
#include<math.h>
#include<string.h>


#define Large 10000000000
#define SqrtOfLarge 100000


//This program calls a function Find_primes that includes MPI calls for parallel processing.
int main(int argc,char *argv[]){
	
	
	int world_rank;         
    int world_size;	
	
    unsigned long long int nsmall=SqrtOfLarge;   
    unsigned long long int primesFirstpart;	
	
    unsigned long long int nlarge=Large;	
    unsigned long long int primesSecondpart;
	
	
    double start=0.0,end;
	//numbers of primes be less than SqrtOfLarge
    unsigned long long int count=0;
	
	//for number of primes less than 100000
	unsigned long long int temp=0;

    //malloc a large array "ifprime[0..nsmall]"
    //initialize all entries it as true(=1).
    //A value in prime[i] will finally be false if i is Not a prime, else true.
	char *ifprime=(char*)malloc(sizeof(char)*(nsmall+1));
    memset(ifprime,'y',sizeof(ifprime[0])*nsmall);
	
	//allocate a array to accommodate prime numbers,it is ok to allocate a size nsmall array ,it is not large
	unsigned long long int *primetemp=(unsigned long long int*)malloc(sizeof(unsigned long long int)*nsmall);
    
	
	
	//initialize the MPI execution enviroment
	MPI_Init(&argc,&argv);
	//Determines the rank of the calling process within the communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	//Determines the number of processes in the group associated with a communicator   
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); 
	
	
	
	 
	/*                                          Announcement                                            */
	if (world_rank==0){
	printf ("\n\n");	
    printf ( "  Given a number n=%llu, print how many primes are smaller than or equal to it\n",nlarge);
    printf ( "  The number of processors is %d\n", world_size);
    printf ( "\n" );
	}
	

	/*                                            Times start                                           */
	

    start = MPI_Wtime();
    
	
	/*                    First part:count the numbers of primes less than SqrtOfLarge                  */
	/*                    this part is not parallel                                                     */
	
	
	for(unsigned long long int i=2;i*i<=nsmall;i++)
	{
		// If prime[i] is not changed, then it is a prime 
		if(ifprime[i]=='y'){
		// Update all multiples of p greater than or  
        // equal to the square of it 
        // numbers which are multiple of p and are 
        // less than p^2 are already been marked.  
            for (int j=i*i; j<=nsmall; j+=i) 
                ifprime[j] = 'n'; 
		}
	}	
	for(unsigned long long int i=2;i<nsmall;i++)
	{
		if(ifprime[i]=='y'){
			//store primes into the array
			primetemp[temp]=i;            
			temp++;
		}
	}
	primesFirstpart=temp;
	
	
	
	/*         Second part:count the numbers of primes large than SqrtOfLarge and smaller than Large     */
	/*                                this part is parallel !!!                                          */
	
	
	
	//start from 100001
	//skip even number to reduce time
	//i.e.(8 processors)  P0 check 100001,100017,100033,100049.............
	//                    P1 check 100003,100019,100035,100051.............
	for(unsigned long long int i=nsmall+1+(2*world_rank);i<=nlarge;i+=2*world_size){
		for(unsigned long long int j=0;j<primesFirstpart;j++){
			if(i%primetemp[j]==0)   
			{
				//I am not a prime number
				break;
			}
			if(j==primesFirstpart-1)
			{
				//ends check I am a prime number
				primesSecondpart++;          
			}
		}
	}
	
	//MPI_Reduce (&sendbuf, &recvbuf, count, datatype, op, dest, comm)
	//Applies a reduction operation on all tasks in the group and places the result in one task
	//MPI_SUM=>add the counts of primes to master(world_rank==0)
    MPI_Reduce(&primesSecondpart, &count, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    //I am master ,collective and print the result
    if(world_rank==0){
		count+=primesFirstpart;
		printf("There are %llu prime numbers within %llu.\n",count,nlarge);
	}
	
	
	end=MPI_Wtime();

    
	
	
	printf("  Processor %3d  took %8.6f unit times\n",world_rank,end-start);
    
    

    //end of MPI
	MPI_Finalize();
	
	

	return 0;
  	
}