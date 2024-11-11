#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Global variables
int MAX;
int **matA;
int **matB;
int **matSumResult;
int **matDiffResult;
int **matProductResult;

// Struct to hold indices
typedef struct {
    int row;
    int col;
} Index;

// Function to allocate memory for a matrix
int** allocateMatrix(int size) {
    int **matrix = (int**) malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        matrix[i] = (int*) malloc(size * sizeof(int));
    }
    return matrix;
}

// Function to free memory of a matrix
void freeMatrix(int **matrix, int size) {
    for (int i = 0; i < size; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to fill a matrix with random values
void fillMatrix(int **matrix, int size) {
    for(int i = 0; i< size; i++) {
        for(int j = 0; j< size; j++) {
            matrix[i][j] = rand()%10+1;
        }
    }
}

// Function to print a matrix
void printMatrix(int **matrix, int size) {
    for(int i = 0; i< size; i++) {
        for(int j = 0; j< size; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Thread function to compute sum
void* computeSum(void* args) {
    Index *index = (Index *) args;
    int i = index->row;
    int j = index->col;
    matSumResult[i][j] = matA[i][j] + matB[i][j];
    free(index);
    pthread_exit(0);
}

// Thread function to compute difference
void* computeDiff(void* args) {
    Index *index = (Index *) args;
    int i = index->row;
    int j = index->col;
    matDiffResult[i][j] = matA[i][j] - matB[i][j];
    free(index);
    pthread_exit(0);
}

// Thread function to compute product
void* computeProduct(void* args) {
    Index *index = (Index *) args;
    int i = index->row;
    int j = index->col;
    matProductResult[i][j] = 0;
    for (int k = 0; k < MAX; k++) {
        matProductResult[i][j] += matA[i][k] * matB[k][j];
    }
    free(index);
    pthread_exit(0);
}

// Main function
int main(int argc, char *argv[]) {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.

    // 0. Get the matrix size from the command line and assign it to MAX
    if (argc != 2) {
        printf("Usage: %s <matrix size>\n", argv[0]);
        exit(1);
    }
    MAX = atoi(argv[1]);

    // Allocate memory for matrices
    matA = allocateMatrix(MAX);
    matB = allocateMatrix(MAX);
    matSumResult = allocateMatrix(MAX);
    matDiffResult = allocateMatrix(MAX);
    matProductResult = allocateMatrix(MAX);

    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA, MAX);
    fillMatrix(matB, MAX);

    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA, MAX);
    printf("Matrix B:\n");
    printMatrix(matB, MAX);

    // 3. Create pthread_t objects for our threads.
    pthread_t *threadsSum = malloc(MAX * MAX * sizeof(pthread_t));
    pthread_t *threadsDiff = malloc(MAX * MAX * sizeof(pthread_t));
    pthread_t *threadsProduct = malloc(MAX * MAX * sizeof(pthread_t));

    // 4. Create a thread for each cell of each matrix operation.
    int count = 0;
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            Index *data = (Index *) malloc(sizeof(Index));
            data->row = i;
            data->col = j;
            pthread_create(&threadsSum[count], NULL, computeSum, data);
            count++;
        }
    }

    count = 0;
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            Index *data = (Index *) malloc(sizeof(Index));
            data->row = i;
            data->col = j;
            pthread_create(&threadsDiff[count], NULL, computeDiff, data);
            count++;
        }
    }

    count = 0;
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            Index *data = (Index *) malloc(sizeof(Index));
            data->row = i;
            data->col = j;
            pthread_create(&threadsProduct[count], NULL, computeProduct, data);
            count++;
        }
    }

    // 5. Wait for all threads to finish.
    for (int i = 0; i < MAX * MAX; i++) {
        pthread_join(threadsSum[i], NULL);
    }

    for (int i = 0; i < MAX * MAX; i++) {
        pthread_join(threadsDiff[i], NULL);
    }

    for (int i = 0; i < MAX * MAX; i++) {
        pthread_join(threadsProduct[i], NULL);
    }

    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult, MAX);
    printf("Difference:\n");
    printMatrix(matDiffResult, MAX);
    printf("Product:\n");
    printMatrix(matProductResult, MAX);

    // Free allocated memory
    freeMatrix(matA, MAX);
    freeMatrix(matB, MAX);
    freeMatrix(matSumResult, MAX);
    freeMatrix(matDiffResult, MAX);
    freeMatrix(matProductResult, MAX);

    free(threadsSum);
    free(threadsDiff);
    free(threadsProduct);

    return 0;
}
