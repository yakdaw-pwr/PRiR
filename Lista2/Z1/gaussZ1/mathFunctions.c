#include "mathFunctions.h"
#include<stdio.h>
#include<stdlib.h>

void divide(double* row, double valueOfMiddleElement, int rowSize) {
#pragma omp parallel for
    for (int i = 0; i < rowSize; i++) {
        row[i] = row[i] / valueOfMiddleElement;
    }
}

void substract(double* baseRow, double* destRow, double multiValue, int rowSize) {
#pragma omp parallel for
    for (int i = 0; i < rowSize; i++) {
        destRow[i] = destRow[i] - (baseRow[i] / multiValue);
    }
}

void subRows(double** matrix, int rowIndex, int rowCount) {
#pragma omp parallel for
    for (int i = 0; i < rowCount; i++) {
        if (i != rowIndex && matrix[i][rowIndex] != 0) {
            double value = matrix[rowIndex][rowIndex] / matrix[i][rowIndex];
            substract(matrix[rowIndex], matrix[i], value, rowCount + 1);
        }
    }
}


