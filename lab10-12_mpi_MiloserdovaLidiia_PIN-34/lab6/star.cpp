#include <iostream>
#include "mpi.h"

const int MIN = 0;
const int MAX = 100000;

int main(int argc, char *argv[]) {
    int worldSize, worldRank;
    double message = 0;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int *starRanks = new int[worldSize];
    for (int i = 0; i < worldSize; ++i)
        starRanks[i] = i;

    MPI_Group groupAll;
    MPI_Group groupStar;

    MPI_Comm groupStarComm;

    MPI_Comm_group(MPI_COMM_WORLD, &groupAll);
    MPI_Group_incl(groupAll, worldSize, starRanks, &groupStar);

    MPI_Comm_create(MPI_COMM_WORLD, groupStar, &groupStarComm);

    int nnodes, starRank;
    MPI_Comm_size(groupStarComm, &nnodes);
    int *indexes = new int[nnodes];
    int *edges = new int[2 * (nnodes - 1)];
    MPI_Comm_rank(groupStarComm, &starRank);

    indexes[0] = nnodes - 1;
    for (int i = 1; i < nnodes; ++i) {
        indexes[i] = indexes[i - 1] + 1;
    }
    for (int i = 0; i < (nnodes - 1); ++i) {
        edges[i] = i + 1;
    }
    for (int i = (nnodes - 1); i < 2 * (nnodes - 1); ++i) {
        edges[i] = 0;
    }

    if (starRank == 0) {
        printf("nodes number: %d\n", nnodes);
        printf("indexes: ");
        for (int i = 0; i < nnodes; ++i) {
            printf("%d ", indexes[i]);
        }
        printf("\nedges: ");
        for (int i = 0; i < 2 * (nnodes - 1); ++i) {
            printf("%d ", edges[i]);
        }
        printf("\n");
    }

    MPI_Graph_create(MPI_COMM_WORLD, nnodes, indexes, edges, 0, &groupStarComm);

    message = 0;
    int neighbours_count;
    int *neighbours_ranks;
    if (starRank == 0) {
        message = 1;
        MPI_Graph_neighbors_count(groupStarComm, 0, &neighbours_count);
        neighbours_ranks = new int[neighbours_count];
        MPI_Graph_neighbors(groupStarComm, 0, neighbours_count, neighbours_ranks);
        for (int i = 0; i < neighbours_count; ++i) {
            MPI_Send(&message, 1, MPI_DOUBLE, neighbours_ranks[i], starRank, groupStarComm);
        }
    } else {
        MPI_Recv(&message, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, groupStarComm, &status);
        printf("Process %d received message %1.1f\n", starRank, message);
    }

    if (starRank != 0) {
        message = 0;
        MPI_Graph_neighbors_count(groupStarComm, starRank, &neighbours_count);
        neighbours_ranks = new int[neighbours_count];
        MPI_Graph_neighbors(groupStarComm, starRank, neighbours_count, neighbours_ranks);
        for (int i = 0; i < neighbours_count; ++i) {
            MPI_Send(&message, 1, MPI_DOUBLE, neighbours_ranks[i], starRank, groupStarComm);
        }
    } else {
        MPI_Graph_neighbors_count(groupStarComm, 0, &neighbours_count);
        for (int i = 1; i < neighbours_count + 1; ++i) {
            MPI_Recv(&message, 1, MPI_DOUBLE, i, MPI_ANY_TAG, groupStarComm, &status);
            printf("Root received message %1.1f from proc %d\n", message, status.MPI_SOURCE);
        }
    }

    delete[] indexes;
    delete[] edges;
    delete[] starRanks;
    delete[] neighbours_ranks;

    MPI_Group_free(&groupAll);
    MPI_Group_free(&groupStar);

    MPI_Comm_free(&groupStarComm);

    MPI_Finalize();

    return 0;
}
