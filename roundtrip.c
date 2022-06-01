#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include <omp.h>

#define N (1024 * 1024 * 1)

#define NT 4
#define thrd_no omp_get_thread_num

int main(int argc, char *argv[])
{
  int size, rank;
  char hostname[256];
  int hostname_len;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(hostname,&hostname_len);

  // Allocate a 1 MiB buffer
  char *buffer = malloc(sizeof(char) * N);

#pragma omp parallel for num_threads(NT) // GGG
for(int i=0; i<NT; i++) printf("thrd no %d of rank %d\n",thrd_no(),rank); // GGG
	
  // Communicate along the ring
  if (rank == 0) {
        printf("Rank %d (running on '%s'): sending the message rank %d\n",rank,hostname,1);
	MPI_Send(buffer, N, MPI_BYTE, 1, 1, MPI_COMM_WORLD);
       	MPI_Recv(buffer, N, MPI_BYTE, size-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank %d (running on '%s'): received the message from rank %d\n",rank,hostname,size-1);

  } else {
       	MPI_Recv(buffer, N, MPI_BYTE, rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank %d (running on '%s'): receive the message and sending it to rank %d\n",rank,hostname,(rank+1)%size);
	MPI_Send(buffer, N, MPI_BYTE, (rank+1)%size, 1, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
