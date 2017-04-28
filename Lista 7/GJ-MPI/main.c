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

double** MultiplyMatrixes(double** m1, double** m2);

int main(int argc, char** argv) {

    int worldRank;
    int rowSize = 0;
    
    double timeStart;

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

        // Oblicz wyznacznik
        double determinant = calculateMatrixDeterminant(matrix);
        if (determinant == 0) {
            fprintf(stderr, "Wczytana macierz jest osobliwa (wyznacznik = 0).\n");
            MPI_Abort(MPI_COMM_WORLD, rc);
            return (1);
        }
        printf("Matrix determinant: %lf\n", determinant);

        // Poszerz macierz o macierz jednostkowa
        appendWithIdentityMatrix(matrix, rowSize);

        // Pobierz licze procesow
        int worldSize;
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
        printf("Processes: %d\n", worldSize);

        // Jest tylko proces master -> wykonaj pojedynczy algorytm
        if (worldSize == 1) {
            // Rozpoczecie mierzenia czasu
            timeStart = MPI_Wtime();
            
            calculateInverseMatrix(matrix, rowSize);
        } 
        else {          
            int rowsPerProcess = 1;
            int rowsForMaster = 0;
            int activeProcesses = rowSize;

            // Jeżeli liczba procesow >= wielkosci macierzy,
            // zostaw domyslne wartosci
            if (worldSize < rowSize) {
                rowsPerProcess = rowSize / worldSize;
                rowsForMaster = rowSize % worldSize;
                activeProcesses = worldSize;
            }
            printf("Rows per process: %d\n", rowsPerProcess);
            printf("Smaller block rows (for master process): %d\n", rowsForMaster);

            int* startRowNumbers = (int*) calloc(worldSize, sizeof (int));
            int* endRowNumbers = (int*) calloc(worldSize, sizeof (int));

            // Wylicz wiersze startowe i koncowe blokow dla poszczegolnych procesow
            int i;
            for (i = 0; i < activeProcesses; i++) {
                startRowNumbers[i] = i * rowsPerProcess;
                endRowNumbers[i] = startRowNumbers[i] + rowsPerProcess - 1;
            }

            // Wylicz wiersz startowy mniejszego bloku (jezeli istnieje)
            int masterStartRow = 0;
            if (rowsForMaster != 0) {
                masterStartRow = rowSize - rowsForMaster;
            }

            // Wyslij procesom wykonawczym informacje o wielkosci macierzy,
            // poczatku i koncu blokow
            for (i = 1; i < activeProcesses; i++) {
                MPI_Send(&rowSize, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                MPI_Send(&startRowNumbers[i], 1, MPI_INT, i, 2, MPI_COMM_WORLD);
                MPI_Send(&endRowNumbers[i], 1, MPI_INT, i, 3, MPI_COMM_WORLD);
            }
            
            int j, k;
            int p, q;
            
            timeStart = MPI_Wtime();

            // Petla glowna (k = 0, ..., N-1)
            for (k = 0; k < rowSize; k++) {
                
                // Normalizacja wiersza
                for (j = rowSize * 2 - 1; j >= k; j--) {
                    matrix[k][j] = matrix[k][j] / matrix[k][k];
                }

                // Wysylanie wiadomosci do procesow wykonawczych
                for (p = 1; p < activeProcesses; p++) {
                    // Wyslij prawa czesc znormalizowanego wiersza k
                    MPI_Send(*(matrix + k) + k, 2 * rowSize - k, MPI_DOUBLE, p, 10, MPI_COMM_WORLD);
                    
                    // Wyslij prawa czesc wierszy dla danego procesu
                    for (q = startRowNumbers[p]; q <= endRowNumbers[p]; q++) {
                        MPI_Send(*(matrix + q) + k, 2 * rowSize - k, MPI_DOUBLE, p, 10 + q, MPI_COMM_WORLD);
                    }
                }
                            
                // Zerowanie dla pierwszego bloku wykonywane przez master (proces 0)
                for (i = startRowNumbers[0]; i <= endRowNumbers[0]; i++) {
                    if (i != k) {
                        for (j = rowSize * 2 - 1; j > k; j--) {
                            matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
                        }
                    }
                }

                // Odbieranie wynikow z procesow wykonawczych i aktualizacja macierzy
                for (p = 1; p < activeProcesses; p++) {                 
                    for (q = startRowNumbers[p]; q <= endRowNumbers[p]; q++) {
                        MPI_Recv(*(matrix + q) + k, 2 * rowSize - k, MPI_DOUBLE, p, 10 + q, MPI_COMM_WORLD, &status);
                    }
                }

                // Zerowanie dla bloku niepelnego
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

            // Budowa programu zaklada ze wszystkie komunikaty doszly   
        }
           
        printf("\n\n\n");
        
        // Czas wykonywania operacji
        printf("Czas wykonywania operacji: %f\n\n", MPI_Wtime() - timeStart);

        // Wyniki odwracania macierzy
        printf("Odwrocona macierz:\n\n");

        int i, j;
        for (i = 0; i < rowSize; i++) {
            for (j = 0; j < rowSize; j++) {
                printf("%lf\t", matrix[i][j + rowSize]);
            }
            printf("\n");
        }
        
        // Sprawdzenie wyników
        double** inverseResult = getInverseMatrix(rowSize, matrix);
        
        double** multiplicityResult = MultiplyMatrixes(originalMatrix, inverseResult)
    }
    // Dla procesów wykonawczych
    else {

        int startRowNumber;
        int endRowNumber;
        int q, j;
        
        // Normalizowany wiersz
        int k = 0;

        // Odbierz informacje o wielkosci macierzy, wierszu poczatkowym i koncowym
        MPI_Recv(&rowSize, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&startRowNumber, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&endRowNumber, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
        
        // Zalokuj pamiec dla wiersza znormalizowanego
        // i wiersza nalezacego do bloku
        double* normalizedRow = (double*) calloc(rowSize * 2, sizeof (double));
        double* tempRow = (double*) calloc(rowSize * 2, sizeof (double));

        // Nasluchuj od MASTERA
        while (k < rowSize) {
            // Odbierz prawa strone wiersza znormalizowanego
            MPI_Recv(normalizedRow, rowSize * 2 - k, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);

            // Dla kazdego wiersza w przypisanym bloku
            for (q = startRowNumber; q <= endRowNumber; q++) {
                
                // Odbierz prawa strone wiersza
                MPI_Recv(tempRow, rowSize * 2 - k, MPI_DOUBLE, 0, 10 + q, MPI_COMM_WORLD, &status);

                // Wykonaj zerowanie
                if (q != k) {
                    for (j = rowSize * 2 - 1; j >= 0; j--) {
                        tempRow[j] = tempRow[j] - tempRow[0] * normalizedRow[j];
                    }
                }

                // Wyslij przetworzony wiersz do MASTERA
                MPI_Send(tempRow, rowSize * 2 - k, MPI_DOUBLE, 0, 10 + q, MPI_COMM_WORLD);
            }

            k++;
        }
    }

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

    // Skanuj wielkosc macierzy odwracanej
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

double** getInverseMatrix(int rowSize, double** extendedMatrix) {
    double** matrix = (double**) calloc(rowSize, sizeof (double*));

    int i;
    for (i = 0; i < rowSize; i++) {
        matrix[i] = (double*) calloc(rowSize, sizeof (double));
    }
    
    int j;
    for (i = 0; i < rowSize; i++) {
        for (j = 0; j < rowSize; j++) {
            matrix[i][j] = extendedMatrix[i][j + rowSize];               
        }
    }
    
    return matrix;
}

double** MultiplyMatrixes(double** m1, double** m2) {
    
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