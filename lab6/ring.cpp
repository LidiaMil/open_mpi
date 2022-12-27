#include <iostream>
#include "mpi.h"

int main(int argc, char *argv[]) {
    int worldSize, worldRank;
    double message = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int *ringRanks = new int[worldSize];
    for (int i = 0; i < worldSize; ++i)
        ringRanks[i] = i;

    MPI_Group groupAll;
    MPI_Group groupRing;
    MPI_Comm_group(MPI_COMM_WORLD, &groupAll);
    MPI_Group_incl(groupAll, worldSize, ringRanks, &groupRing);

    MPI_Comm groupRingComm;
    MPI_Comm_create(MPI_COMM_WORLD, groupRing, &groupRingComm);

    int dims, periods, reorder;
    MPI_Group_size(groupRing, &dims);
    periods = 1;
    reorder = 0;
    MPI_Cart_create(groupRingComm, 1, &dims, &periods, reorder, &groupRingComm);

    int ringRank, ringSize;
    MPI_Comm_rank(groupRingComm, &ringRank);
    MPI_Comm_size(groupRingComm, &ringSize);

    for (int i = 0; i < ringSize; ++i) {
        if (ringRank == 0)
            message = 1;
        int next_proc = ringRank + 1;
        printf("i %d : Rank %d : message %1.1f| ", i, ringRank, message);
        MPI_Cart_shift(groupRingComm, 0, i + 1, &ringRank, &next_proc);
        if (i == ringSize - 1)
            printf("\n");
    }

    delete[] ringRanks;

    MPI_Group_free(&groupAll);
    MPI_Group_free(&groupRing);

    MPI_Comm_free(&groupRingComm);

    MPI_Finalize();

    return 0;
}
