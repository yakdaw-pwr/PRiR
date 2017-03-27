#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "readFunctions.h"
#include "displayFunctions.h"
#include "mathFunctions.h"
#include "GJ.h"

double* calculateGJ(double** matrix, int rowCount) {
    double* solutionMatrix = (double*) calloc(rowCount, sizeof (double));

    for (int i = 0; i < rowCount; i++) {
        if (matrix[i][i] == 0) {
            int newRow = findNonZeroElement(matrix, rowCount, i);
            switchRows(matrix[i], matrix[newRow], rowCount + 1);
        }
        //jedynki na srodku
        divide(matrix[i], matrix[i][i], rowCount + 1);
        //    printf("Next step\n");
        //    printMatrix(matrix, rowCount);
        //    printf("\n");
        subRows(matrix, i, rowCount);
    }

    for (int i = 0; i < rowCount; i++) {
        solutionMatrix[i] = matrix[i][rowCount];
    }
    return solutionMatrix;
}

void releaseMemory(double** matrix, double* solution, int rowCount) {
    for (int i = 0; i < rowCount; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(solution);
}

void calculate(const char* fileName) {
    //creating two dimensional matrix
    double** matrix;
    double* solution;
    int rowCount = 0;

    matrix = loadMatrix(fileName, matrix, &rowCount);
    solution = calculateGJ(matrix, rowCount);
    printFinalMatrix(matrix, rowCount);
    saveFinalMatrix(matrix, rowCount);
    releaseMemory(matrix, solution, rowCount);
}

