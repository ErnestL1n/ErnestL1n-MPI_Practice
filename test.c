#include<stdio.h>
#include<mpi.h>


//SPMD programming



int main(int argc,char* argv[]){
	int a[1000];
	int myrank,num_processes;      // use my rank to get each process's rank      
	                               //num_processes to get process numbers
	
	int i,j;            // const i j for loop variable
	int count;          //number of item to send
	double starttime,endtime;     //declare double variable to get starttime and endtime of the parallel part 
	MPI_Status Status;            //for reveive one to check the status
	
	MPI_Init(&argc,&argv);       //initialize the MPI execution enviroment
	
	
	
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);  //call by reference return the rank to myrank variable
	count=0;   //after the results all get from the p0,it reach 1000
	
	
	//begin to execute
	starttime=MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);
	
	//use if else to control different flows
	
	if(myrank==0){
		for(i=0;i<1000;i++)   //set array value as 1
		{
			a[i]=1;
		}
		
		//P0 sends to p1 p2 p3
		//MPI_Send(buf,count,datatype,dest,tag,comm)
		MPI_Send(&a,1000,MPI_INT,1,1,MPI_COMM_WORLD);
		MPI_Send(&a,1000,MPI_INT,2,2,MPI_COMM_WORLD);
		MPI_Send(&a,1000,MPI_INT,3,3,MPI_COMM_WORLD);
	}
	if(myrank!=0){                  //p1,p2,p3 get the results from p0
		//start to receive the items
		
		//MPI_Recv(buf,count(maximum number of items to receive),datatype,src,tag(myrank),comm,status)
		MPI_Recv(&a,1000*1000,MPI_INT,0,myrank,MPI_COMM_WORLD,&Status);
		for(i=0;i<1000;i++){
			if(a[i]==1)  //use count to check if it is successful to receive all items
			{
				count++;
			}
		}
		if(count==1000) printf("Processor %d is sucessful to get all items\n",myrank);
		else printf("Processor %d isn't sucessful to get all items\n",myrank);
	}
	
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	endtime=MPI_Wtime(); //end of execution


	printf("Processor %d took %f seconds\n",myrank,(endtime-starttime));
	
	
	MPI_Finalize (); //end of MPI
	
	return 0;
}