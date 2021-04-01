/** Ciaran Winnan 2940836**/
#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

// used to define nodes and total size of nodes used in the program
int world_size;
int world_rank;

// method to print the elements of the passed array
void printArray(int *arr, int size)
{

	//std::cout << "Node: " << world_rank << std::endl;
	for (int i = 0; i < size - 1; i++)
	{
		printf("%d,", arr[i]);
	}

	// Will remove the last comma from the last element in the array
	if (size > 0)
	{
		printf("%d\n", arr[size - 1]);
	}
}

// Method used to count the number of prime numbers present in the array which is sent to it
int primeCounter(int *arr, int size)
{
	// variable which is used to
	int count = 0;
	for (int i = 0; i < size; i++)
	{
		int j = 2;
		int p = 1;
		while (j < arr[i])
		{
			if (arr[i] % j == 0)
			{
				p = 0;
				break;
			}
			j++;
		}
		if (p == 1)
		{
			count++;
		}
	}
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

	// Saving start time of the execution of program
	//double start_time = MPI_Wtime();

	// intializing the full size of the array
	int full_array_size = 0;
	// partition size used by each node
	int partiton_size = 0;

	if (world_rank == 0)
	{
		std::cout << "Name: Ciaran Winnan\nNo: 2940836\nuP: i7-8650U\nAssignment: 2\nPart: 2\n"
				  << std::endl;
		// getting user input for size of the array
		std::cout << "Input a size for the array:" << std::endl;
		std::cin >> full_array_size;
	}

	// intializing the array
	int *full_array = (int *)malloc(full_array_size * sizeof(int));

	if (world_rank == 0)
	{

		// coordinator will add random numbers to full array
		for (int i = 0; i < full_array_size; i++)
		{
			full_array[i] = (rand() % 4001) + 1;
		}

		// coordinator will decide on partition size
		partiton_size = full_array_size / world_size;
	}

	// Coordinator will broadcast the partition size to all slave nodes
	MPI_Bcast(&partiton_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// intializing the array that each node will be responcible for, will use the partition size determined by the coordinator
	int *node_array = (int *)malloc(partiton_size * sizeof(int));

	// Waiting for array to be filled
	MPI_Barrier(MPI_COMM_WORLD);

	//Scatter operation to all nodes by the coordinator, this method will evenly send a portion of the full generated array to each node
	MPI_Scatter(full_array, partiton_size, MPI_INT, node_array, partiton_size, MPI_INT, 0, MPI_COMM_WORLD);

	// Nodes will calculate their individual mean counts
	int num_of_primes = primeCounter(node_array, partiton_size);

	// print out the total prime numbers found by each node
	std::cout << "Node: " << world_rank << " = " << num_of_primes << " prime number(s) in array\n"
			  << std::endl;

	// Intializing the variable that will store the result of the reduce function carried out by the coordinator
	int results = 0;

	// Waiting for all nodes to finish
	MPI_Barrier(MPI_COMM_WORLD);

	// reduce operation for all nodes by the coordinator that will gather the results from each node.
	MPI_Reduce(&num_of_primes, &results, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	// coordinator will print out the total number totalled by the reduce function
	if (world_rank == 0)
	{
		std::cout << "Total primes found: " << results << std::endl;
	}

	// clear up our memory before the program finishes
	delete full_array;
	delete node_array;
	// finish program
	MPI_Finalize();
	return 0;
}