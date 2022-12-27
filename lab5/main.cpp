#include <iostream>
#include <random>
#include "mpi.h"

const int MIN = 0;
const int MAX = 100000;
const int LONG_ARRAY_SIZE = 32;

// Generates a random integer in the range MIN <= .. <= MAX
int generateRandomInt() {
    std::random_device randomDevice;
    std::default_random_engine eng(randomDevice());
    std::uniform_int_distribution<int> distribution(MIN, MAX);
    return distribution(eng);
}

// Creates an array of random integers with values in the range MIN <= .. <= MAX
void generateRandomIntArray(int *array, int size) {
    for (int i = 0; i < size; ++i)
        array[i] = generateRandomInt();
}

// Prints an integer array to the console
void printIntArray(int *array, int size) {
    for (int i = 0; i < size; ++i)
        printf("%d ", array[i]);
    printf("\n");
}

/*
 * Gets an array containing two sorted arrays of the same size. Divides the array in half and inserts the values from
 * the sorted subarrays in ascending order.
 */
void merge(int *array, int leftIndex, int middleIndex, int rightIndex) {
    int i, j, k;

    int leftSubArraySize = middleIndex - leftIndex + 1;
    int rightSubArraySize = rightIndex - middleIndex;
    int leftSubArray[leftSubArraySize], rightSubArray[rightSubArraySize];

    for (i = 0; i < leftSubArraySize; i++)
        leftSubArray[i] = array[leftIndex + i];
    for (j = 0; j < rightSubArraySize; j++)
        rightSubArray[j] = array[middleIndex + 1 + j];

    i = 0;
    j = 0;
    k = leftIndex;

    while (i < leftSubArraySize && j < rightSubArraySize) {
        if (leftSubArray[i] <= rightSubArray[j]) {
            array[k] = leftSubArray[i];
            i++;
        } else {
            array[k] = rightSubArray[j];
            j++;
        }
        k++;
    }

    while (i < leftSubArraySize) {
        array[k] = leftSubArray[i];
        i++;
        k++;
    }
    while (j < rightSubArraySize) {
        array[k] = rightSubArray[j];
        j++;
        k++;
    }
}

// Recursive implementation of merge sort
void mergeSort(int *array, int leftIndex, int rightIndex) {
    int middleIndex;
    if (leftIndex < rightIndex) {
        middleIndex = leftIndex + (rightIndex - leftIndex) / 2;
        mergeSort(array, leftIndex, middleIndex);
        mergeSort(array, middleIndex + 1, rightIndex);
        merge(array, leftIndex, middleIndex, rightIndex);
    }
}

/*
 * Gets and merges arrays sorted in other processes into one array. Processes communicate through a communicator.
 * The process with rank 0 in this group sorts it and prints it to the console.
 */
void gatherAndMergeSortedSubarrays(
        MPI_Comm communicator,
        int *sendBuffer,
        int sendBufferSize,
        int *receiveBuffer,
        int receiveBufferSize
) {
    if (communicator != MPI_COMM_NULL) {
        int communicatorSize, communicatorRank;
        MPI_Comm_size(communicator, &communicatorSize);
        MPI_Comm_rank(communicator, &communicatorRank);

        MPI_Gather(sendBuffer,
                   sendBufferSize,
                   MPI_INT,
                   receiveBuffer,
                   sendBufferSize,
                   MPI_INT,
                   0,
                   communicator
        );

        if (communicatorRank == 0) {
            merge(receiveBuffer, 0, sendBufferSize - 1, receiveBufferSize - 1);
            printIntArray(receiveBuffer, receiveBufferSize);
        }
    }
}

int main(int argc, char *argv[]) {
    int worldRank, worldSize;

    int *longArray = new int[LONG_ARRAY_SIZE];
    int *sortedLongArray = new int[LONG_ARRAY_SIZE];
    int *longArrayHalfSize = new int[LONG_ARRAY_SIZE / 2];
    int *longArrayQuarterSize = new int[LONG_ARRAY_SIZE / 4];
    int *longArrayOneEightSize = new int[LONG_ARRAY_SIZE / 8];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // Groups initializing
    MPI_Group groupAll;
    MPI_Group groupZeroOne;
    MPI_Group groupTwoThree;
    MPI_Group groupFourFive;
    MPI_Group groupSixSeven;
    MPI_Group groupZeroTwo;
    MPI_Group groupFourSix;
    MPI_Group groupZeroFour;

    // Communicators initializing
    MPI_Comm groupZeroOneComm;
    MPI_Comm groupTwoThreeComm;
    MPI_Comm groupFourFiveComm;
    MPI_Comm groupSixSevenComm;
    MPI_Comm groupZeroTwoComm;
    MPI_Comm groupFourSixComm;
    MPI_Comm groupZeroFourComm;

    // Create group with all processes
    MPI_Comm_group(MPI_COMM_WORLD, &groupAll);

    // Create a processes group with ranks 0 and 1
    int groupZeroOneRanks[2] = {0, 1};
    MPI_Group_incl(groupAll, sizeof(groupZeroOneRanks) / sizeof(int), groupZeroOneRanks, &groupZeroOne);

    // Create a processes group with ranks 2 and 3
    int groupTwoThreeRanks[2] = {2, 3};
    MPI_Group_incl(groupAll, sizeof(groupTwoThreeRanks) / sizeof(int), groupTwoThreeRanks, &groupTwoThree);

    // Create a processes group with ranks 4 and 5
    int groupFourFiveRanks[2] = {4, 5};
    MPI_Group_incl(groupAll, sizeof(groupFourFiveRanks) / sizeof(int), groupFourFiveRanks, &groupFourFive);

    // Create a processes group with ranks 6 and 7
    int groupSixSevenRanks[2] = {6, 7};
    MPI_Group_incl(groupAll, sizeof(groupSixSevenRanks) / sizeof(int), groupSixSevenRanks, &groupSixSeven);

    // Create a processes group with ranks 0 and 2
    int groupZeroTwoRanks[2] = {0, 2};
    MPI_Group_incl(groupAll, sizeof(groupZeroTwoRanks) / sizeof(int), groupZeroTwoRanks, &groupZeroTwo);

    // Create a processes group with ranks 4 and 6
    int groupFourSixRanks[2] = {4, 6};
    MPI_Group_incl(groupAll, sizeof(groupFourSixRanks) / sizeof(int), groupFourSixRanks, &groupFourSix);

    // Create a processes group with ranks 0 and 4
    int groupZeroFourRanks[2] = {0, 4};
    MPI_Group_incl(groupAll, sizeof(groupZeroFourRanks) / sizeof(int), groupZeroFourRanks, &groupZeroFour);

    // Create communicators for all created groups
    MPI_Comm_create(MPI_COMM_WORLD, groupZeroOne, &groupZeroOneComm);
    MPI_Comm_create(MPI_COMM_WORLD, groupTwoThree, &groupTwoThreeComm);
    MPI_Comm_create(MPI_COMM_WORLD, groupFourFive, &groupFourFiveComm);
    MPI_Comm_create(MPI_COMM_WORLD, groupSixSeven, &groupSixSevenComm);
    MPI_Comm_create(MPI_COMM_WORLD, groupZeroTwo, &groupZeroTwoComm);
    MPI_Comm_create(MPI_COMM_WORLD, groupFourSix, &groupFourSixComm);
    MPI_Comm_create(MPI_COMM_WORLD, groupZeroFour, &groupZeroFourComm);

    if (worldRank == 0) {
        generateRandomIntArray(longArray, LONG_ARRAY_SIZE);
        printIntArray(longArray, LONG_ARRAY_SIZE);
    }

    // Send own parts of a long array to each process
    MPI_Scatter(longArray,
                LONG_ARRAY_SIZE / 8,
                MPI_INT,
                longArrayOneEightSize,
                LONG_ARRAY_SIZE / 8,
                MPI_INT,
                0,
                MPI_COMM_WORLD
    );

    // Merge sort own parts of a long array in each process
    mergeSort(longArrayOneEightSize, 0, LONG_ARRAY_SIZE / 8 - 1);
    printIntArray(longArrayOneEightSize, LONG_ARRAY_SIZE / 8);

    // Gather and merge sorted subarrays in processes with ranks 0 and 1, 2 and 3, 4 and 5, 6 and 7
    gatherAndMergeSortedSubarrays(groupZeroOneComm,
                                  longArrayOneEightSize,
                                  LONG_ARRAY_SIZE / 8,
                                  longArrayQuarterSize,
                                  LONG_ARRAY_SIZE / 4
    );

    gatherAndMergeSortedSubarrays(groupTwoThreeComm,
                                  longArrayOneEightSize,
                                  LONG_ARRAY_SIZE / 8,
                                  longArrayQuarterSize,
                                  LONG_ARRAY_SIZE / 4
    );

    gatherAndMergeSortedSubarrays(groupFourFiveComm,
                                  longArrayOneEightSize,
                                  LONG_ARRAY_SIZE / 8,
                                  longArrayQuarterSize,
                                  LONG_ARRAY_SIZE / 4
    );

    gatherAndMergeSortedSubarrays(groupSixSevenComm,
                                  longArrayOneEightSize,
                                  LONG_ARRAY_SIZE / 8,
                                  longArrayQuarterSize,
                                  LONG_ARRAY_SIZE / 4
    );

    MPI_Barrier(MPI_COMM_WORLD);

    if (worldRank == 0 || worldRank == 2 || worldRank == 4 || worldRank == 6) {
        mergeSort(longArrayQuarterSize, 0, LONG_ARRAY_SIZE / 4 - 1);
    }

    // Gather and merge sorted subarrays in processes with ranks 0 and 2, 4 and 6
    gatherAndMergeSortedSubarrays(groupZeroTwoComm,
                                  longArrayQuarterSize,
                                  LONG_ARRAY_SIZE / 4,
                                  longArrayHalfSize,
                                  LONG_ARRAY_SIZE / 2
    );

    gatherAndMergeSortedSubarrays(groupFourSixComm,
                                  longArrayQuarterSize,
                                  LONG_ARRAY_SIZE / 4,
                                  longArrayHalfSize,
                                  LONG_ARRAY_SIZE / 2
    );

    MPI_Barrier(MPI_COMM_WORLD);

    if (worldRank == 0 || worldRank == 4) {
        mergeSort(longArrayHalfSize, 0, LONG_ARRAY_SIZE / 2 - 1);
    }

    // Gather and merge sorted subarrays in processes with ranks 0 and 4
    gatherAndMergeSortedSubarrays(groupZeroFourComm,
                                  longArrayHalfSize,
                                  LONG_ARRAY_SIZE / 2,
                                  longArray,
                                  LONG_ARRAY_SIZE
    );

    // Free all groups
    MPI_Group_free(&groupZeroOne);
    MPI_Group_free(&groupTwoThree);
    MPI_Group_free(&groupFourFive);
    MPI_Group_free(&groupSixSeven);
    MPI_Group_free(&groupZeroTwo);
    MPI_Group_free(&groupFourSix);
    MPI_Group_free(&groupZeroFour);

    // Free all communicators
    if (groupZeroOneComm != MPI_COMM_NULL)
        MPI_Comm_free(&groupZeroOneComm);
    if (groupTwoThreeComm != MPI_COMM_NULL)
        MPI_Comm_free(&groupTwoThreeComm);
    if (groupFourFiveComm != MPI_COMM_NULL)
        MPI_Comm_free(&groupFourFiveComm);
    if (groupSixSevenComm != MPI_COMM_NULL)
        MPI_Comm_free(&groupSixSevenComm);
    if (groupZeroTwoComm != MPI_COMM_NULL)
        MPI_Comm_free(&groupZeroTwoComm);
    if (groupFourSixComm != MPI_COMM_NULL)
        MPI_Comm_free(&groupFourSixComm);
    if (groupZeroFourComm != MPI_COMM_NULL)
        MPI_Comm_free(&groupZeroFourComm);

    MPI_Finalize();

    delete[] longArray;
    delete[] sortedLongArray;

    return 0;
}
