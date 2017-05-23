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
        int report = 3;
        int* series;

        // Sprawdzenie poprawnosci argumentow
        if (validateExecutionArguments(argc, argv, &seriesSize, 
                &report, worldSize) == 1) {
            MPI_Abort(MPI_COMM_WORLD, rc);
            return (1);
        }
        
        // Generowanie ciągu
        series = generateSeries(seriesSize);
        
        if (report) {
            printf("Generated series: %d\n", seriesSize);       
            for (i = 0; i < seriesSize; i++) {
                printf("%d ", series[i]);
            }
            printf("\n");
        }
        
        // Wyslij report do reszty procesow
        
        

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
        series[i] = uniformDistribution(0, 10);
    }
    
    return series;
}

int uniformDistribution(int rangeLow, int rangeHigh) {
    
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}