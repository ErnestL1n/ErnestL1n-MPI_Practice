//author:  B0521229  Ernest Lin 
//for homework1 training1A-case1->Write a MPI program with send and recv to do the broadcast. (2, 4, 8, 16 processors)
//Please declare an integer two-dimensional array with size 4000×4000 at the master processor.


//the needed header file must be included
#include<stdio.h>
#include<stdlib.h>    //for malloc
#include<time.h>
#include<mpi.h>


//define dimension
#define ROW 4000
#define COL 4000


//begin with the main func()
int main(int argc,char* argv[]){
	
	
	
	//declare variables to get start time and end time of the parallel part 
	double start=0.0,end;  
	          
	
	//initialize the MPI execution enviroment
	MPI_Init(&argc,&argv);
	
	
	
	//Determines the rank of the calling process within the communicator
	int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	
	
	//Determines the number of processes in the group associated with a communicator   
	int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	
	
	
	//start
	start=MPI_Wtime();
	
	
	
	//dynamically allocate a 2-D array using malloc func()
	int **twoDarray=(int**)malloc(sizeof(int*)*ROW);
	for(int i=0;i<ROW;i++)twoDarray[i]=(int*)malloc(sizeof(int)*COL);

	
	
	
	//begin to send() and recv()
	
	//I am master
	if(world_rank==0){
		
		//first part:set value as 1
		for(int j=0;j<ROW;j++)
			for(int k=0;k<COL;k++)
				twoDarray[j][k]=1;
			
		
		
		//second part:send to others processors
		
		
		//MPI_Send(buf,count,datatype,dest,tag,comm)
		//tag is set same is allowed,since we have destination
		//send to others,begin from 1
		for(int l=1;l<world_size;l++)
			MPI_Send(twoDarray, ROW*COL, MPI_INT, l, 21229, MPI_COMM_WORLD);
	}
	
	
	//We are slave
	else{
		//MPI_Recv(buf,count(maximum number of items to receive),datatype,src,tag,comm,status)
		MPI_Recv(twoDarray, ROW*COL, MPI_INT, 0, 21229, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	
	
	
	
	//end
	end=MPI_Wtime();
	
	
	
	//print out the resulting time
	printf("  Processor %3d   took %8.6f unit times\n",world_rank,end-start);
	
	
	
	//free twoDarray
	free(twoDarray);

	
	
	//end of MPI
	MPI_Finalize();
	
	

	
	return 0;
}