#include <stdio.h>
#include "mpi.h"
#include <clocale>
#include <Windows.h>
int main(int argc, char* argv[])
{
	int ProcNum, ProcRank, RecvRank;
	MPI_Status Status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	printf("PN = %d", ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	if (ProcRank == 0)
	{
		// ��������, ����������� ������ ��������� � ������ 0
		printf("\n Hello from process %3d", ProcRank);
		for (int i = 1; i < ProcNum; i++)
		{
			MPI_Recv(&RecvRank, 1, MPI_INT, MPI_ANY_SOURCE,
				MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
			printf("\n Hello from process %3d", RecvRank);
		}
	}
	else // ���������, ������������ ����� ����������,
  // ����� �������� � ������ 0
		MPI_Send(&ProcRank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	MPI_Finalize();
	system("pause");
	return 0;
}
