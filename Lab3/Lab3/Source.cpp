#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <windows.h>
#include <time.h>
using namespace std;

int main(int* argc, char** argv)
{
	int numtasks, rank;
	int* result = new int[1];
	*result = 0;
	int dataCount = atoi(argv[2]);


	MPI_Status status;
	MPI_Init(argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	int* data = new int[numtasks * dataCount];

	srand(time(NULL) + rank);

	int* partOfData = new int[dataCount];

	if (rank == 0) 
	{
		cout << "Number of processes: " << numtasks << "; data count: " << dataCount << endl;
		for (int i = 0; i < numtasks * dataCount; i++)
		{
			data[i] = rand() % 20;
		}

		MPI_Scatter(data, dataCount, MPI_INT, partOfData, dataCount, MPI_INT, 0, MPI_COMM_WORLD);
		for (int i = dataCount; i < numtasks * dataCount; i++)
		{
			cout << data[i] << " ";
			data[i] = 0;
		}
		cout << endl;
		MPI_Gather(partOfData, dataCount, MPI_INT, data, dataCount, MPI_INT, 0, MPI_COMM_WORLD);
		for (int i = dataCount; i < numtasks * dataCount; i++)
		{
			cout << data[i] << " ";
		}
		cout << endl;

		MPI_Reduce(&(partOfData[0]), result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		cout << "Sum of first elements: " << *result - data[0] << endl;
	}
	else 
	{
		MPI_Scatter(data, dataCount, MPI_INT, partOfData, dataCount, MPI_INT, 0, MPI_COMM_WORLD);
		for (int i = 0; i < dataCount; i++)
		{
			partOfData[i] += rank;
		}
		MPI_Gather(partOfData, dataCount, MPI_INT, data, dataCount, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Reduce(&(partOfData[0]), result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	}


	cout << "Process #" << rank << " finished" << endl;

	MPI_Finalize();
	return 0;
}