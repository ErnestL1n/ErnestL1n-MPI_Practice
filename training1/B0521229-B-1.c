//author:  B0521229  Ernest Lin 
//for homework1 training1B-case1->Write a MPI program with send and recv to do the scatter. (2, 4, 8, 16 processors)
//Please declare an integer one-dimensional array with size 100,000,000 at the master processor. 


//the needed header file must be included
#include<stdio.h>
#include<stdlib.h>    //for malloc
#include<time.h>
#include<mpi.h>


//define dimension
#define LARGE 100000000



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
	
	
	
	//dynamically allocate a 1-D large array using malloc func()
	int *OneDarray=(int*)malloc(sizeof(int)*LARGE);
		
	
	
	//dynamically allocate a 1-D "buffer" for each processor
	//cut it into pieces	
	int *OneDbuffer=(int*)malloc(sizeof(int)*(LARGE/world_size));
   
	
	
	//begin to send() and recv()
	
	
	//I am master
	if(world_rank==0){
		
		
		
		//first part:set value as 1
		for(int i=0;i<LARGE;i++)
			OneDarray[i]=1;
		
		//second part:send to others processors
		
		//MPI_Send(buf,count,datatype,dest,tag,comm)
		//tag is set same is allowed,since we have destination
		//send to others,begin from 1
		
		
		//Sending each element of an array in root process to a separate process. Contents of jth location of array sent to jth process.
		for(int j=1;j<world_size;j++)
			MPI_Send(&OneDarray[(LARGE/world_size)*j], LARGE/world_size, MPI_INT, j, 21229, MPI_COMM_WORLD);
	

	}
	
	
	//We are slave
	else{

		//MPI_Recv(buf,count(maximum number of items to receive),datatype,src,tag,comm,status)
		MPI_Recv(OneDbuffer, LARGE/world_size, MPI_INT, 0, 21229, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
	}
	

	
	
	//end
	end=MPI_Wtime();
	
	
	
	//print out the resulting time
	printf("  Processor %3d   took %8.6f unit times\n",world_rank,end-start);
	
	
	
	//free OneDarray
	free(OneDarray);
	
	
	
	//free OneDbuffer
	free(OneDbuffer);
	
	
	
	//end of MPI
	MPI_Finalize();
	
	

	
	return 0;
}