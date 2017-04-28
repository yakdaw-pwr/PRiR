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
void calculateInverseMatrix(double** m, int size);

int main(int argc, char** argv) {

    // Tags: 1 - matrixSize , 2 - startRowNum , 3 - endRowNum

    int worldRank;

    int rowSize = 0;

    // Inicjalizacja MPI
    MPI_Status status;
    int rc = MPI_Init(&argc, &argv);
    if (rc != MPI_SUCCESS) {
        fprintf(stderr, "Error starting MPI program.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        return (1);
    }

    // Identyfikator procesu
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    // Dla procesu MASTER
    if (worldRank == 0) {

        // Sprawdzenie poprawnosci argumentow
        if (validateExecutionArguments(argc) == 1) {
            MPI_Abort(MPI_COMM_WORLD, rc);
            return (1);
        }

        char* filePath = argv[1];

        // Sprawdzenie czy plik jest do odczytu
        if (checkIfFileIsReadable(filePath) == 1) {
            fprintf(stderr, "File not readable.\n");
            MPI_Abort(MPI_COMM_WORLD, rc);
            return (1);
        }

        // Wczytanie macierzy z pliku
        double** matrix = loadMatrix(filePath, &rowSize);

        printf("Row size: %d\n", rowSize);

        double determinant = calculateMatrixDeterminant(matrix);
        if (determinant == 0) {
            fprintf(stderr, "Wczytana macierz jest osobliwa (wyznacznik = 0).\n");
            MPI_Abort(MPI_COMM_WORLD, rc);
            return (1);
        }
        printf("determinant : %lf\n", determinant);

        appendWithIdentityMatrix(matrix, rowSize);

        // Pobierz licze procesow
        int worldSize;
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
        printf("Processes: %d\n", worldSize);

        // Jest tylko proces master -> wykonaj algorytm sam
        if (worldSize == 1) {
            calculateInverseMatrix(matrix, rowSize);
        } else {
            // Domyslny przypadek jeżeli ilość procesów > wielkość macierzy
            int rowsPerProcess = 1;
            int rowsForMaster = 0;
            int activeProcesses = rowSize;

            if (worldSize < rowSize) {
                rowsPerProcess = rowSize / worldSize;
                rowsForMaster = rowSize % worldSize;
                activeProcesses = worldSize;
            }
            printf("Rows per process: %d\n", rowsPerProcess);
            printf("Rows for master process: %d\n", rowsForMaster);

            int* startRowNumbers = (int*) calloc(worldSize, sizeof (int));
            int* endRowNumbers = (int*) calloc(worldSize, sizeof (int));

            int i;
            for (i = 0; i < activeProcesses; i++) {
                startRowNumbers[i] = i * rowsPerProcess;
                endRowNumbers[i] = startRowNumbers[i] + rowsPerProcess - 1;
            }

            int masterStartRow = 0;
            if (rowsForMaster != 0) {
                masterStartRow = rowSize - rowsForMaster;
            }

            printf("Master start row: %d\n", masterStartRow);

            for (i = 1; i < activeProcesses; i++) {
                MPI_Send(&rowSize, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                MPI_Send(&startRowNumbers[i], 1, MPI_INT, i, 2, MPI_COMM_WORLD);
                MPI_Send(&endRowNumbers[i], 1, MPI_INT, i, 3, MPI_COMM_WORLD);
            }
            
            // Tymczasowe testy
            // *(*(data + i) + j) data[i][j]
//            MPI_Send(startRowNumbers + 1, 3, MPI_INT, 1, 10, MPI_COMM_WORLD);
//            MPI_Send(*(matrix + 3) + 1, 3, MPI_DOUBLE, 1, 10, MPI_COMM_WORLD);    -- tutaj z macierza

            int j, k;
            int p, q;

            // Pętla główna (k = 0, ..., N-1)
            for (k = 0; k < rowSize; k++) {
                // Normalizacja wiersza
                for (j = rowSize * 2 - 1; j >= k; j--) {
                    matrix[k][j] = matrix[k][j] / matrix[k][k];
                }

                for (p = 1; p < activeProcesses; p++) {
                    // Wysłać do wykonawców [k][k+1] -> [k][2N -1]
                    MPI_Send(*(matrix + k) + k, 2 * rowSize - k, MPI_DOUBLE, p, 10, MPI_COMM_WORLD);
                    
                    // Wysłać do wykonawców [poczatek -> koniec][k+1] -> [p -> k][2n-1] 
                    for (q = startRowNumbers[p]; q <= endRowNumbers[p]; q++) {
                        MPI_Send(*(matrix + q) + k, 2 * rowSize - k, MPI_DOUBLE, p, 10 + q, MPI_COMM_WORLD);
                    }
                }
                            
                // Zerowanie dla #0 bez nadmiarowego
                // Usuwanie zer poza przekatna, i - wiersz, j - kolumna
                for (i = startRowNumbers[0]; i <= endRowNumbers[0]; i++) {
                    if (i != k) {
                        for (j = rowSize * 2 - 1; j > k; j--) {
                            matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
                        }
                    }
                }

                // Odbieranie wyników i aktualizacja macierzy
                for (p = 1; p < activeProcesses; p++) {                 
                    for (q = startRowNumbers[p]; q <= endRowNumbers[p]; q++) {
                        MPI_Recv(*(matrix + q) + k, 2 * rowSize - k, MPI_DOUBLE, p, 10 + q, MPI_COMM_WORLD, &status);
                    }
                }

                // Zerowanie w niepełnym bloku
                if (masterStartRow != 0) {
                    for (i = masterStartRow; i < rowSize; i++) {
                        if (i != k) {
                            for (j = rowSize * 2 - 1; j > k; j--) {
                                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
                            }
                        }
                    }
                }
            }

            // Czekaj na komunikaty    
        }

        printf("\n\n\n\n\n");

        int i, j;
        // Wyprowadz wyniki odwracania macierzy
        for (i = 0; i < rowSize; i++) {
            for (j = 0; j < rowSize; j++) {
                printf("%.5lf\t", matrix[i][j + rowSize]);
            }
            printf("\n");
        }
    }
    // Dla procesów wykonawczych
    else {

        int startRowNumber;
        int endRowNumber;
        int q, j;
        
        int k = 0;

//        printf("-----------------------");
//        printf("Proces %d: czekam, obecnie %d\n", worldRank, rowSize);

        MPI_Recv(&rowSize, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&startRowNumber, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&endRowNumber, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);

//        printf("Proces %d: doszlo, obecnie %d\n", worldRank, rowSize);
//        printf("Proces %d: start, obecnie %d\n", worldRank, startRowNumber);
//        printf("Proces %d: end, obecnie %d\n", worldRank, endRowNumber);
//        printf("-----------------------");
        
        double* normalizedRow = (double*) calloc(rowSize * 2, sizeof (double));
        double* tempRow = (double*) calloc(rowSize * 2, sizeof (double));
        
        // Testy
//        int* blow = (int*) calloc(rowSize * 2, sizeof (int));
//        MPI_Recv(blow, 3, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);
//        MPI_Recv(normalizedRow, 3, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);

        while (1) {
            MPI_Recv(normalizedRow, rowSize * 2 - k, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);

//            printf("Proces %d: Dostalem znormalizowany\n", worldRank);

            for (q = startRowNumber; q <= endRowNumber; q++) {
                MPI_Recv(tempRow, rowSize * 2 - k, MPI_DOUBLE, 0, 10 + q, MPI_COMM_WORLD, &status);
//                printf("Proces %d: Dostalem moj\n", worldRank);
                if (q != k) {
                    for (j = rowSize * 2 - 1; j >= 0; j--) {
                        tempRow[j] = tempRow[j] - tempRow[0] * normalizedRow[j];
                    }
                }
//                printf("Proces %d: Przetworzylem moj\n", worldRank);


                // Zwroc prawe czesci swoich wierszow
                MPI_Send(tempRow, rowSize * 2 - k, MPI_DOUBLE, 0, 10 + q, MPI_COMM_WORLD);

//                printf("Proces %d: Wyslalem moj\n", worldRank);
            }

            // Zmniejsz bufor
            k++;
        }
    }


    //    double start = MPI_Wtime();

    //    double stop = MPI_Wtime();

    MPI_Finalize();

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

void calculateInverseMatrix(double** m, int size) {
    int i, j, k;

    // k = nr wiersza
    for (k = 0; k < size; k++) {
        // normalizacja wiersza
        // j = nr kolumny
        for (j = size * 2 - 1; j >= k; j--) {
            m[k][j] = m[k][j] / m[k][k];
        }

        // usuwanie zer poza przekatna
        for (i = 0; i < size; i++) {
            // i = nr wiersza
            if (i != k) {
                // j = nr kolumny
                for (j = size * 2 - 1; j >= k; j--)
                    m[i][j] = m[i][j] - m[i][k] * m[k][j];
            }
        }
    }
}