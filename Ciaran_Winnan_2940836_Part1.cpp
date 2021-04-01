/** Ciaran Winnan 2940836 **/
/** includes **/
#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <windows.h>
#include <stdio.h>
#include <math.h>

/** the world rank and size that will be useful in many functions **/
int world_size, world_rank;
double start_time, time_taken;

// method called to count the number of prime numbers in the passed number
int primeCounter(int n)
{
	int count = 0;
	for (int a = 2; a < n; a++)
	{
		bool isPrime = true;
		for (int c = 2; c * c <= a; c++)
		{
			if (a % c == 0)
			{
				isPrime = false;
				break;
			}
		}
		// if a prime number is found, increment the counter
		if (isPrime)
			count++;
	}
	// return the result
	return count;
}

int main(int argc, char **argv)
{
	// see if we can initialise the mpi library this is always the first thing that we
	// must do at the start of an MPI program
	MPI_Init(NULL, NULL);
	// one of the important tasks that we have to establish is how many processes are in
	// this MPI instance. this will determine who and what we have to communicate with
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	// another important job is finding out which rank we are. we can use this rank
	// number to assign seperate jobs to different mpi units
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// Intializing the arrays that will hold the full number of primes and the node specific primes
	double *full_array = (double *)malloc((world_size * 2) * sizeof(double));
	double *partition = (double *)malloc(2 * sizeof(double));

	if (world_rank == 0)
	{
		std::cout << "Name: Ciaran Winnan\nNo: 2940836\nuP: i7-8650U\nAssignment: 2\nPart: 1\n"
				  << std::endl;
	}

	// starting the timer
	start_time = MPI_Wtime();

	// sending the int of each node to the primeCounter method
	int result = primeCounter((int)pow(25, world_rank + 2));

	// calculating the total time taken for the node to calculate the prime number count
	time_taken = MPI_Wtime() - start_time;

	// storing the node's world rank and time taken to complete the calculation in the partition array
	partition[0] = (double)world_rank;
	partition[1] = time_taken;

	// Waiting for all nodes to finish their calculations before calling the gather method
	MPI_Barrier(MPI_COMM_WORLD);

	// coordinator gathering all the data from the other nodes, results from the gather will be stored in the full_array variable
	MPI_Gather(partition, 2, MPI_DOUBLE, full_array, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	// coordinator printing out the results from the gather function
	if (world_rank == 0)
	{
		std::cout << "Results:\n"
				  << std::endl;
		for (int i = 0; i < (world_size * 2); i = i + 2)
		{
			std::cout << "Node: " << full_array[i] << " = " << full_array[i + 1] << " seconds" << std::endl;
		}
	}

	// before we can end our application we need to finalise everything so MPI can shut down properly
	MPI_Finalize();
	// standard C/C++ thing to do at the end of main
	return 0;
}