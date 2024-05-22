// module load OpenMPI/4.1.0-GCC-10.2.0 
// mpicc mpi_test.c -o mpi_test
// srun --reservation=fri --mpi=pmix -n2 -N2 ./mpi_test

#include <stdio.h>
#include <string.h> 
#include <mpi.h>

int main(int argc, char *argv[]) 
{ 
	int rank; // process rank 
	int num_p; // total number of processes 
	int source; // sender rank
	int destination; // receiver rank 
	int tag = 0; // message tag 
	char message[100]; // message buffer 
	MPI_Status status; // message status 
	char node_name[MPI_MAX_PROCESSOR_NAME]; //node name
	int name_len; //true length of node name
    

	MPI_Init(&argc, &argv); // initialize MPI 
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get process rank 
	MPI_Comm_size(MPI_COMM_WORLD, &num_p); // get number of processes
	MPI_Get_processor_name( node_name, &name_len ); // get node name

	if( rank != 0 ) 
	{ 
		sprintf(message, "Hello from process %d/%d running on %s!", rank,num_p,node_name); 
		destination = 0; 
		MPI_Send(message, (int)strlen(message)+1, MPI_CHAR, 
			destination, tag, MPI_COMM_WORLD); 
	} 
	else 
	{ 
		printf("Hello from root process running on %s!\n\n", node_name); 
		fflush(stdout); 
		for( source = 1; source < num_p; source++) 
		{ 
			MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status); 
			printf("%s\n", message); 
			fflush(stdout); 
		} 
	} 
	MPI_Finalize(); 

	return 0; 
} 
