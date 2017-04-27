/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: yakdaw
 *
 * Created on April 24, 2017, 1:01 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

int validateExecutionArguments(int argc);
int checkIfFileIsReadable(char* filePath);
double** loadMatrix(const char* filePath, int* rowSize);
double calculateMatrixDeterminant(double** matrix);
void appendWithIdentityMatrix(double** matrix, int size);

int main(int argc, char** argv) {

    // Start zliczania czasu
    double start = MPI_Wtime();

    // Sprawdzenie poprawnosci argumentow
    if (validateExecutionArguments(argc) == 1) {
        return (1);
    }

    char* filePath = argv[1];

    // Sprawdzenie czy plik jest do odczytu
    if (checkIfFileIsReadable(filePath) == 1) {
        fprintf(stderr, "File not readable.\n");
        return (1);
    }

    // Wczytanie macierzy z pliku
    int rowSize;
    double** matrix = loadMatrix(filePath, &rowSize);

    printf("Row size: %d\n", rowSize);

    double determinant = calculateMatrixDeterminant(matrix);
    if (determinant == 0) {
        fprintf(stderr, "Wczytana macierz jest osobliwa (wyznacznik = 0).\n");
        return (1);
    }

    printf("determinant : %lf\n", determinant);

    appendWithIdentityMatrix(matrix, rowSize);

    int i, j;
    for (i = 0; i < rowSize; i++) {
        for (j = 0; j < rowSize * 2; j++) {
            printf("%lf\t", matrix[i][j]);
        }
        printf("\n");
    }



    //    double stop = MPI_Wtime();

    return (EXIT_SUCCESS);
}

int validateExecutionArguments(int argc) {

    if (argc != 2) {
        fprintf(stderr, "Zla ilosc parametrow wywolania programu.\n");
        return (1);
    }

    return (0);
}

int checkIfFileIsReadable(char* filePath) {

    if (access(filePath, R_OK) == -1) {
        return (1);
    }

    return (0);
}

double** loadMatrix(const char* filePath, int* rowSize) {
    FILE* data = fopen(filePath, "r");
    double matrixAgrument = 0;

    // Skanuj wielkość macierzy odwracanej
    int tempMemory = 0;
    fscanf(data, "%d", &tempMemory);
    *rowSize = tempMemory;

    double** matrix = (double**) calloc(*rowSize, sizeof (double*));

    int i;
    for (i = 0; i < *rowSize; i++) {
        matrix[i] = (double*) calloc(*rowSize * 2, sizeof (double));
    }

    int j;
    for (i = 0; i < *rowSize; i++) {
        for (j = 0; j < *rowSize; j++) {
            fscanf(data, "%lf", &matrixAgrument);
            matrix[i][j] = matrixAgrument;
        }
    }

    fclose(data);
    return matrix;
}

double calculateMatrixDeterminant(double** m) {
    double determinant = m[0][0] * ((m[1][1] * m[2][2]) - (m[2][1] * m[1][2]))
            - m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2])
            + m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]);

    return determinant;
}

void appendWithIdentityMatrix(double** matrix, int size) {
    int i;
    for (i = 0; i < size; i++) {
        matrix[i][i + size] = 1;
    }
}