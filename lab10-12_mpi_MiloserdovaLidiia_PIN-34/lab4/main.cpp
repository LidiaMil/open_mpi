#include <iostream>
#include <random>
#include "mpi.h"

const int MIN = -10;
const int MAX = 10;
const int POLYNOMIAL_RANK = 3;
const double X = 1.0;

double generate_random_double() {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(MIN, MAX);

    return distr(eng);
}

void generate_random_double_polynomial(double *array, int size) {
    for (int i = 0; i < size; ++i)
        array[i] = generate_random_double();
}

int main(int argc, char *argv[]) {
    int process_number, process_rank;
    double process_result = 0.0, total_result;
    int array_size = POLYNOMIAL_RANK + 1;

    auto *polynomial_values_array = new double[array_size];

    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    // Create the datatype
    MPI_Datatype polynomial_type;
    MPI_Type_contiguous(POLYNOMIAL_RANK, MPI_DOUBLE, &polynomial_type);
    MPI_Type_commit(&polynomial_type);

    if (process_rank == 0) {
        for (int i = 0; i < process_number; ++i) {
            generate_random_double_polynomial(polynomial_values_array, array_size);
            if (i + 1 < process_number)
                MPI_Send(polynomial_values_array, 1, polynomial_type, i + 1, 0, MPI_COMM_WORLD);
        }
        process_result = 1.0;
        MPI_Reduce(&process_result, &total_result, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD);
        printf("ROOT: %f\n", total_result);
    } else {
        MPI_Recv(polynomial_values_array, 1, polynomial_type, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &Status);

        // Print coefficients of current process polynomial
        printf("PROCESS %d POLYNOMIAL COEFFICIENTS: ", process_rank);
        for (int i = 0; i < array_size; ++i)
            printf("%f ", polynomial_values_array[i]);
        printf("\n");

        for (int i = 0; i < POLYNOMIAL_RANK; ++i)
            process_result += pow(X, POLYNOMIAL_RANK - i) * polynomial_values_array[i];
        MPI_Reduce(&process_result, &process_result, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD);
        printf("PROCESS %d RESULT: %f\n", process_rank, process_result);
    }

    MPI_Type_free(&polynomial_type);
    MPI_Finalize();

    delete[] polynomial_values_array;

    return 0;
}
