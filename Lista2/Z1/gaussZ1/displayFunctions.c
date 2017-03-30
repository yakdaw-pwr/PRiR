/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "displayFunctions.h"
#include<stdio.h>
#include<stdlib.h>

#define BUFFER_SIZE 50

void printMatrix(double** matrix, int rowCount) {
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < rowCount + 1; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void printSolution(double* x, int rowCount) {
    for (int i = 0; i < rowCount; i++) {
        //        printf("x%d: %.2lf\n", i, x[i]);
    }
}

void printFinalMatrix(double** matrix, int rowCount) {
    printf("Liczba rownan: %d\n", rowCount);
    for (int i = 0; i < rowCount; i++) {

        printf("Wiersz %d:\n", i);
        for (int j = 0; j < rowCount + 1; j++) {
            printf("%.2lf ", matrix[i][j]);
        }
        printf("\n");
    }
}

void saveFinalMatrix(double** matrix, int rowCount, const char* outputFileName) {
    FILE *pfile = NULL;
    pfile = fopen(outputFileName, "w");

    if (pfile == NULL) {
        printf("Error opening %s for writing. Program terminated.", outputFileName);
    }
    fprintf(pfile, "%d\n", rowCount);

    for (int i = 0; i < rowCount; i++) {
        fprintf(pfile, "\nWiersz %d\n", i + 1);

        for (int j = 0; j < rowCount + 1; j++) {
            fprintf(pfile, "%.2lf ", matrix[i][j]);
        }
    }

    fclose(pfile);
}
