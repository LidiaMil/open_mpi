#include <stdio.h>
#include <random>
#include <chrono>
#include <iostream>

#include "mpi/mpi.h"

constexpr int MIN_DISTR_BOUND{ 1 };
constexpr int MAX_DISTR_BOUND{ 8 };


double get_random_double(std::mt19937& gen)
{
	std::uniform_real_distribution<double> dist(MIN_DISTR_BOUND, MAX_DISTR_BOUND);
	return dist(gen);
}

int main(int argc, char* argv[])
{
	std::random_device seeder;
	const auto seed = seeder.entropy() > 0 ?
		seeder() :
		std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 gen(static_cast<std::mt19937::result_type>(seed));

	int procNum, procRank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
	MPI_Comm_size(MPI_COMM_WORLD, &procNum);

	MPI_Group ALL_PROCS;

	MPI_Comm_group(MPI_COMM_WORLD, &ALL_PROCS);

	int* star_ranks = new int[procNum];
	int* ring_ranks = new int[procNum];

	for (int i = 0; i < procNum; ++i)
	{
		star_ranks[i] = i;
		ring_ranks[i] = i;
	}

	MPI_Group RING_TOP_PROCS;
	MPI_Group_incl(ALL_PROCS, procNum, ring_ranks, &RING_TOP_PROCS);
	MPI_Comm RING_COMM;
	MPI_Comm_create(MPI_COMM_WORLD, RING_TOP_PROCS, &RING_COMM);

	//RING topology
	int dims, periods, reorder;
	MPI_Group_size(RING_TOP_PROCS, &dims);
	periods = 1;
	reorder = 0;
	MPI_Cart_create(RING_COMM, 1, &dims, &periods, reorder, &RING_COMM);

	int rank;
	MPI_Comm_rank(RING_COMM, &rank);

	int ring_size;
	MPI_Comm_size(RING_COMM, &ring_size);

	double mes = 0;
	MPI_Status status;

	for (int i = 0; i < ring_size; ++i)
	{
		if (rank == 0)
		{
			mes = 1;
		}
		int next_proc = rank + 1;
		printf("i %d : Rank %d : mes %1.1f| ", i, rank, mes);
		MPI_Cart_shift(RING_COMM, 0, i+1, &rank, &next_proc);
	}

	MPI_Group_free(&RING_TOP_PROCS);
	MPI_Comm_free(&RING_COMM);

	//STAR topology
	MPI_Group STAR_TOP_PROCS;
	MPI_Group_incl(ALL_PROCS, procNum, star_ranks, &STAR_TOP_PROCS);
	MPI_Comm STAR_COMM;
	MPI_Comm_create(MPI_COMM_WORLD, STAR_TOP_PROCS, &STAR_COMM);

	int nnodes = 0;
	MPI_Comm_size(STAR_COMM, &nnodes);
	int* indexes = new int[nnodes];
	int *edges = new int[2*(nnodes - 1)];

	indexes[0] = nnodes - 1;
	for (int i = 1; i < nnodes; ++i)
	{
		indexes[i] = indexes[i-1] + 1;
	}
	for (int i = 0; i < (nnodes - 1); ++i)
	{
		edges[i] = i + 1;
	}
	for (int i = (nnodes - 1); i < 2 * (nnodes - 1); ++i)
	{
		edges[i] = 0;
	}

	rank;
	MPI_Comm_rank(STAR_COMM, &rank);

	if (rank == 0)
	{
		printf("\nnnodes: %d\n", nnodes);
		printf("indexes: ");
		for (int i = 0; i < nnodes; ++i)
		{
			printf("%d ", indexes[i]);
		}
		printf("\n");
		for (int i = 0; i < 2 * (nnodes - 1); ++i)
		{
			printf("%d ", edges[i]);
		}
		printf("\n");
	}

	MPI_Graph_create(MPI_COMM_WORLD, nnodes, indexes, edges, 0, &STAR_COMM);

	mes = 0;
	int neib_count;
	int* neib_ranks;
	if (rank == 0)
	{
		mes = 1;
		MPI_Graph_neighbors_count(STAR_COMM, 0, &neib_count);
		neib_ranks = new int[neib_count];
		MPI_Graph_neighbors(STAR_COMM, 0, neib_count, neib_ranks);
		for (int i = 0; i < neib_count; ++i)
		{
			MPI_Send(&mes, 1, MPI_DOUBLE, neib_ranks[i], rank, STAR_COMM);
		}
	}
	else
	{
		MPI_Recv(&mes, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, STAR_COMM, &status);
		printf("proc %d received mes %1.1f\n", rank, mes);
	}

	if (rank != 0)
	{
		mes = 0;
		MPI_Graph_neighbors_count(STAR_COMM, rank, &neib_count);
		neib_ranks = new int[neib_count];
		MPI_Graph_neighbors(STAR_COMM, rank, neib_count, neib_ranks);
		for (int i = 0; i < neib_count; ++i)
		{
			MPI_Send(&mes, 1, MPI_DOUBLE, neib_ranks[i], rank, STAR_COMM);
		}
	}
	else
	{
		MPI_Graph_neighbors_count(STAR_COMM, 0, &neib_count);
		for (int i = 1; i < neib_count + 1; ++i)
		{
			MPI_Recv(&mes, 1, MPI_DOUBLE, i, MPI_ANY_TAG, STAR_COMM, &status);
			printf("proc 0 received mes %1.1f from proc %d\n", mes, status.MPI_SOURCE);
		}
	}

	MPI_Group_free(&ALL_PROCS);
	MPI_Group_free(&STAR_TOP_PROCS);

	MPI_Comm_free(&STAR_COMM);

	MPI_Finalize();

	return 0;
}
