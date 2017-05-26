/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: yakdaw
 *
 * Created on May 23, 2017, 4:27 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mpi.h>

int validateExecutionArguments(int argc, char** argv, int* seriesSize, 
        int* report, int procNumber);
int* generateSeries(int size);
int uniformDistribution(int rangeLow, int rangeHigh);
void oddEvenSort(int *series, int seriesSize);
void swap(int *x, int *y);
void removeFileIfExists(char* fileName);

int main(int argc, char** argv) {

    // Parametry OpenMPI
    int worldRank;    
    int worldSize;
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
    // Ilosc procesow
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    
    int i;

    // Dla procesu MASTER
    if (worldRank == 0) {

        int seriesSize;
        int report;
        int* series;
        
        char* fileName = "OEpS.lst";
        
        // Sprawdzenie poprawnosci argumentow
        if (validateExecutionArguments(argc, argv, &seriesSize,
                &report, worldSize) == 1) {
            MPI_Abort(MPI_COMM_WORLD, rc);
            return (1);
        }

        // Generowanie ciągu
        series = generateSeries(seriesSize);
        
        if (report) {
            printf("Generated series (%d):\n", seriesSize);
            for (i = 0; i < seriesSize; i++) {
                printf("%d ", series[i]);
            }
            printf("\n\n");
        }
        
        // Tworzenie nowego pliku raportu
        removeFileIfExists(fileName);
        FILE* pFile = fopen(fileName, "a");
        fprintf(pFile, "%d\n", seriesSize);

        for (i = 0; i < seriesSize; i++) {
            fprintf(pFile, "%d", series[i]);
            if (i != seriesSize - 1) {
                fprintf(pFile, "\t");
            }
        }
        fprintf(pFile, "\n");
        

        if (worldSize == 1) {
            timeStart = MPI_Wtime();
            
            oddEvenSort(series, seriesSize);
            
            printf("\nTIME TO SORT SERIES: %lf\n\n", MPI_Wtime() - timeStart);

            if (report) {
                printf("Sorted series (%d):\n", seriesSize);
                for (i = 0; i < seriesSize; i++) {
                    printf("%d ", series[i]);
                }
                printf("\n");
            }
            
            for (i = 0; i < seriesSize; i++) {
                fprintf(pFile, "%d", series[i]);
                if (i != seriesSize - 1) {
                    fprintf(pFile, "\t");
                }
            }
            fprintf(pFile, "\n");
        } 
        else {            
            int* startIndexes = (int*) calloc(worldSize, sizeof (int));
            int* endIndexes = (int*) calloc(worldSize, sizeof (int));

            // Wielkosc czesci ciagu
            int partSize = seriesSize / worldSize;

            int i;
            // Wylicz poczatek i koniec dla poszczegolnych procesow
            for (i = 0; i < worldSize; i++) {
                startIndexes[i] = i * partSize;
                endIndexes[i] = startIndexes[i] + partSize - 1;
            }

            int p;
            // Dla kazdego procesu wykonawczego
            for (p = 1; p < worldSize; p++) {
                // Wyslij wielkosc ciagu
                MPI_Send(&seriesSize, 1, MPI_INT, p, 1, MPI_COMM_WORLD);
                // Wyslij podciag
                MPI_Send(series + p * partSize, partSize, MPI_INT, p, 2, MPI_COMM_WORLD);
            }

            int j;
            int* temporary = (int*) calloc(partSize * 2, sizeof (int));
            int* neighbourSeries = (int*) calloc(partSize, sizeof (int));
            int* partSeries = (int*) calloc(partSize, sizeof (int));
            memcpy(partSeries, series, partSize * sizeof(int));  
            
            int* result = (int*) calloc(seriesSize, sizeof (int));
            int stop = 0;
            
            timeStart = MPI_Wtime();

            for (i = 0; i < worldSize; i++) {
                
                // Parzyste
                if (i % 2 == 0) {
                    // Wyslij + 1
                    MPI_Send(partSeries, partSize, MPI_INT, worldRank + 1, 10 + i, MPI_COMM_WORLD);

                    // Odbierz + 1
                    MPI_Recv(neighbourSeries, partSize, MPI_INT, worldRank + 1, 10 + i, MPI_COMM_WORLD, &status);  

                    // Polacz
                    memcpy(temporary, partSeries, partSize * sizeof(int));
                    memcpy(temporary + partSize, neighbourSeries, partSize * sizeof(int));

                    // Oddsort
                    oddEvenSort(temporary, partSize * 2);
                                                
                    // Zostaw[0] + part
                    memcpy(partSeries, temporary, partSize * sizeof(int));
                }
                
                memcpy(result, partSeries, partSize * sizeof(int));

                for (p = 1; p < worldSize; p++) {
                    MPI_Recv(result + partSize * p, partSize, MPI_INT, p, 5, MPI_COMM_WORLD, &status);  
                }

                if (report == 1) {
                    int k;
                    printf("Step %d result: \n", i);
                    for (j = 0; j < seriesSize; j = j + partSize) {
                        for (k = 0; k < partSize; k++) {
                            printf("%d ", result[j + k]);                        
                        }
                        printf("\t");
                    }
                    printf("\n\n");    
                }
                         
                for (p = 1; p < worldSize; p++) {
                    MPI_Send(&stop, 1, MPI_INT, p, 6, MPI_COMM_WORLD);  
                }
            }
            
            printf("\nTIME TO SORT SERIES: %lf\n\n", MPI_Wtime() - timeStart);
            
            if (report) {
                printf("Sorted series (%d):\n", seriesSize);
                for (i = 0; i < seriesSize; i++) {
                    printf("%d ", result[i]);
                }
                printf("\n");
            }

            for (i = 0; i < seriesSize; i++) {
                fprintf(pFile, "%d", result[i]);
                if (i != seriesSize - 1) {
                    fprintf(pFile, "\t");
                }
            }
            fprintf(pFile, "\n");
        }
    }
    // Watek wykonawczy
    else {
        
        int seriesSize;
        int even;
        
        // Odbierz wielkosc ciagu
        MPI_Recv(&seriesSize, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        
        int partSize = seriesSize / worldSize;
        int* series = (int*) calloc(partSize, sizeof (int));
        
        // Odbierz podciag
        MPI_Recv(series, partSize, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);  

        if (worldRank % 2 == 0) {
            even = 1;
        } else {
            even = 0;
        }
        
        int i, j;
        int* temporary = (int*) calloc(partSize * 2, sizeof (int));
        int* neighbourSeries = (int*) calloc(partSize, sizeof (int));
        int stop = 0;
        
        for (i = 0; i < worldSize; i++) {
            // Parzyste
            if (i % 2 == 0) {               
                if (even == 1) {                    
                    if ( worldRank == worldSize - 1) {

                    }
                    else {     
                        // Wyslij + 1
                        MPI_Send(series, partSize, MPI_INT, worldRank + 1, 10 + i, MPI_COMM_WORLD);

                        // Odbierz + 1
                        MPI_Recv(neighbourSeries, partSize, MPI_INT, worldRank + 1, 10 + i, MPI_COMM_WORLD, &status);  

                        // Polacz
                        memcpy(temporary, series, partSize * sizeof(int));
                        memcpy(temporary + partSize, neighbourSeries, partSize * sizeof(int));

                        // Oddsort
                        oddEvenSort(temporary, partSize * 2);

                        // Zostaw[0] + part
                        memcpy(series, temporary, partSize * sizeof(int));
                    }
                }
                else {                 
                    // Odbierz - 1
                    MPI_Recv(neighbourSeries, partSize, MPI_INT, worldRank - 1, 10 + i, MPI_COMM_WORLD, &status);  
                    
                    // Wyslij - 1
                    MPI_Send(series, partSize, MPI_INT, worldRank - 1, 10 + i, MPI_COMM_WORLD);
                    
                    // Polacz
                    memcpy(temporary, neighbourSeries, partSize * sizeof(int));
                    memcpy(temporary + partSize, series, partSize * sizeof(int));
                                       
                    // Oddsort
                    oddEvenSort(temporary, partSize * 2);

                    // Zostaw[0] + part
                    memcpy(series, temporary + partSize, partSize * sizeof(int));
                }
            }
            // Nieparzyste
            else {
                if (even == 0) {                   
                    if ( worldRank == worldSize - 1) {

                    }
                    else {
                        // Wyslij + 1
                        MPI_Send(series, partSize, MPI_INT, worldRank + 1, 10 + i, MPI_COMM_WORLD);

                        // Odbierz + 1
                        MPI_Recv(neighbourSeries, partSize, MPI_INT, worldRank + 1, 10 + i, MPI_COMM_WORLD, &status);  

                        // Polacz
                        memcpy(temporary, series, partSize * sizeof(int));
                        memcpy(temporary + partSize, neighbourSeries, partSize * sizeof(int));
                        
                        // Oddsort
                        oddEvenSort(temporary, partSize * 2);

                        // Zostaw[0] + part
                        memcpy(series, temporary, partSize * sizeof(int));
                    }
                }
                else {
                    // Odbierz - 1
                    MPI_Recv(neighbourSeries, partSize, MPI_INT, worldRank - 1, 10 + i, MPI_COMM_WORLD, &status);  
                    
                    // Wyslij - 1
                    MPI_Send(series, partSize, MPI_INT, worldRank - 1, 10 + i, MPI_COMM_WORLD);
                    
                    // Polacz
                    memcpy(temporary, neighbourSeries, partSize * sizeof(int));
                    memcpy(temporary + partSize, series, partSize * sizeof(int));
                    
                    // Oddsort
                    oddEvenSort(temporary, partSize * 2);
                            
                    // Zostaw[0] + part
                    memcpy(series, temporary + partSize, partSize * sizeof(int));
                }
            }
            
            MPI_Send(series, partSize, MPI_INT, 0, 5, MPI_COMM_WORLD);  
            MPI_Recv(&stop, 1, MPI_INT, 0, 6, MPI_COMM_WORLD, &status);  
        }
    }

    MPI_Finalize();

    return (EXIT_SUCCESS);
}

int validateExecutionArguments(int argc, char** argv, int* seriesSize, 
        int* report, int procNumber) {

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Zla ilosc parametrow wywolania programu.\n");
        return (1);
    }
    
    if (sscanf(argv[1], "%i", seriesSize) != 1) {
        fprintf(stderr, "Niepoprawna dlugosc ciagu do generowania.\n");
        return (1);
    }
    
    if (*seriesSize < 1 || *seriesSize > 10000) {
        fprintf(stderr, "Zla dlugosc ciagu dla podanej liczby procesorow. "
                "Przewidziany przedzial dlugosci: <1, 10000> \n");
        return (1);
    }
    
    if (*seriesSize % procNumber != 0) {
        fprintf(stderr, "Zla dlugosc ciagu dla podanej liczby procesorow. "
                "Reszta przy dzieleniu [Dlugosc]/[Liczba procesorow] = %d \n",
                *seriesSize % procNumber);
        return (1);
    }
    
    if (argc == 3 && (strcmp(argv[2], "p") == 0 || strcmp(argv[2], "P") == 0)
            && *seriesSize <= 20) {
        *report = 1;
    }
    else {
        *report = 0;
    }

    return (0);
}

int* generateSeries(int size) {
    
    int* series = (int*) calloc(size, sizeof(int));
    
    int i;
    for (i = 0; i < size; i++) {
        series[i] = uniformDistribution(100, 999);
    }
    
    return series;
}

int uniformDistribution(int rangeLow, int rangeHigh) {
    
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

void oddEvenSort(int *series, int seriesSize) {

    int sort = 0;
    int i;

    while (!sort) {
        sort = 1;

        for (i = 1; i < seriesSize - 1; i += 2) {
            if (series[i] > series[i + 1]) {
                swap(&series[i], &series[i + 1]);
                sort = 0;
            }
        }

        for (i = 0; i < seriesSize - 1; i += 2) {
            if (series[i] > series[i + 1]) {
                swap(&series[i], &series[i + 1]);
                sort = 0;
            }
        }
    }
}

void swap(int *x, int *y) {

    int temp;
    
    temp = *x;
    *x = *y;
    *y = temp; 
}

void removeFileIfExists(char* fileName) {

    if (access(fileName, F_OK) != -1) {
        int removed = remove(fileName);
    }
}