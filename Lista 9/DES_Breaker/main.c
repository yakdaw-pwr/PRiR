/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: yakdaw
 *
 * Created on May 9, 2017, 7:52 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <mpi.h>

int validateExecutionArguments(int argc, char** argv, char* zaszyfrowane,
        int* max_dlugosc);
long long powlong(long long base, int exp);
void generateNewPassword(char* passwordVariable, long long index, 
        int length, const char* characters);

int main(int argc, char** argv) {

    int worldRank;
    int worldSize;
    double timeStart;
    int i;
    
    const char* characters = "!\"#$%&'()*+,-./0123456789:;<=>?"
        "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
        "`abcdefghijklmnopqrstuvwxyz{|}~";
    
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
    
    // Rozpoczecie mierzenia czasu
    timeStart = MPI_Wtime();

    // Dla procesu MASTER
    if (worldRank == 0) {
        
        char* zaszyfrowane;
        int max_dlugosc;

        if (validateExecutionArguments(argc, argv, zaszyfrowane, &max_dlugosc) != 0) {
            MPI_Abort(MPI_COMM_WORLD, rc);
            return (1);
        }
        zaszyfrowane = argv[1];

        printf("Zaszyfrowane haslo: %s\n\n", zaszyfrowane);
        
        long long numOfCases;
        char* generatedPassword = (char*) calloc(max_dlugosc, sizeof (char));
        char* generatedHashPassword = (char*) calloc(13, sizeof (char));
        
        if (worldSize == 1) {
            
            for (i = 1; i <= max_dlugosc; i++) {
                numOfCases = powlong((long long)strlen(characters), i);
                
                long long c;
                for (c = 0; c < numOfCases; c++) {
                    generateNewPassword(generatedPassword, c, i, characters);
                    generatedHashPassword = crypt(generatedPassword, zaszyfrowane);
//                    printf("Generuje: %s | ", generatedPassword);
//                    printf("Ciag %d: %s |", j, crypt(generatedPassword, zaszyfrowane));
//                    printf("Zaszyfrowane: %s\n", zaszyfrowane);
                    if (!strcmp(generatedHashPassword, zaszyfrowane)) {
                        printf("Znalazlem haslo: %s | Czas trwania %lf\n", 
                                generatedPassword, MPI_Wtime() - timeStart);
                    }
                }
            }
        }
        else {           
            
            long long* startRowNumbers = (long long*) calloc(worldSize, sizeof (long long));
            long long* endRowNumbers = (long long*) calloc(worldSize, sizeof (long long));
            
            int p;
            for (p = 1; p < worldSize; p++) {
                // Wyslij zaszyfrowane haslo
                MPI_Send(zaszyfrowane, 13, MPI_CHAR, p, 1, MPI_COMM_WORLD);
                // Wyslij maksymalna dlugosc
                MPI_Send(&max_dlugosc, 1, MPI_INT, p, 2, MPI_COMM_WORLD);
            }

            for (i = 1; i <= max_dlugosc; i++) {

                numOfCases = powlong((long long) strlen(characters), i);

                // Wylicz poczatek i koniec
                long long casesPerProcess = numOfCases / worldSize;

//                printf("MASTER Cases per process: %lli\n\n", casesPerProcess);

                // Wylicz wiersze startowe i koncowe blokow dla poszczegolnych procesow
                int j;
                for (j = 0; j < worldSize; j++) {
                    startRowNumbers[j] = j * casesPerProcess;
                    endRowNumbers[j] = startRowNumbers[j] + casesPerProcess - 1;
                }
                endRowNumbers[worldSize - 1] = numOfCases - 1;
                
//                printf("World rank %d, start: %lli\n", worldRank, startRowNumbers[0]);
//                printf("World rank %d, end: %lli\n", worldRank, endRowNumbers[0]);

                for (p = 1; p < worldSize; p++) {
                    // Wyslij numer poczatkowy
                    MPI_Send(&startRowNumbers[p], 1, MPI_LONG_LONG, p, 10 + i, MPI_COMM_WORLD);                   
                    // Wyslij numer koncowy                    
                    MPI_Send(&endRowNumbers[p], 1, MPI_LONG_LONG, p, 50 + i, MPI_COMM_WORLD);
                }
                
                long long c;
                for (c = startRowNumbers[0]; c <= endRowNumbers[0]; c++) {
                    generateNewPassword(generatedPassword, c, i, characters);
                    generatedHashPassword = crypt(generatedPassword, zaszyfrowane);
//                    printf("%d generuje: %s\n", worldRank, generatedPassword);
                    if (!strcmp(generatedHashPassword, zaszyfrowane)) {
                        printf("Znalazlem haslo: %s | Czas trwania %lf\n", 
                                generatedPassword, MPI_Wtime() - timeStart);
                    }
                }
            }
        }
    }        
    // Dla procesów wykonawczych
    else {
        long long startRowNumber;
        long long endRowNumber;
        
        char* hashedPassword = (char*) calloc(13, sizeof(char));
        int max_length;
        
        // Odbierz haslo i maksymalna dlugosc
        MPI_Recv(hashedPassword, 13, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&max_length, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        
//        printf("WYKONAWCZY %d: Haslo: %s\n", worldRank, hashedPassword);
//        printf("WYKONAWCZY %d: Length max: %d\n", worldRank, max_length);
        
        char* generatedPassword = (char*) calloc(max_length, sizeof (char));
        char* generatedHashPassword = (char*) calloc(13, sizeof (char));
        
        for (i = 1; i <= max_length; i++) {
            MPI_Recv(&startRowNumber, 1, MPI_LONG_LONG, 0, 10 + i, MPI_COMM_WORLD, &status);
            MPI_Recv(&endRowNumber, 1, MPI_LONG_LONG, 0, 50 + i, MPI_COMM_WORLD, &status);

//            printf("World rank %d, start: %lli\n", worldRank, startRowNumber);
//            printf("World rank %d, end: %lli\n", worldRank, endRowNumber);

            long long c;
            for (c = startRowNumber; c <= endRowNumber; c++) {
                generateNewPassword(generatedPassword, c, i, characters);
                generatedHashPassword = crypt(generatedPassword, hashedPassword);
//                printf("%d generuje: %s\n", worldRank, generatedPassword);
                if (!strcmp(generatedHashPassword, hashedPassword)) {
                    printf("Proces %d znalazl haslo: %s | Czas trwania %lf\n",
                        worldRank, generatedPassword, MPI_Wtime() - timeStart);
                }
            }
        }
        
        // Wyslij info do mastera jak nie znajdziesz
    }

    MPI_Finalize();

    return (EXIT_SUCCESS);
}

int validateExecutionArguments(int argc, char** argv, char* zaszyfrowane,
        int* max_dlugosc) {

    if (argc != 3) {
        fprintf(stderr, "Zla ilosc parametrow wywolania programu.\n");
        return (1);
    }

    if (strlen(argv[1]) != 13) {
        fprintf(stderr, "Nieprawidlowa dlugosc hasla zaszyfrowanego.\n");
        return (1);
    }

    if (sscanf(argv[2], "%i", max_dlugosc) != 1) {
        fprintf(stderr, "Niepoprawna maksymalna dlugosc hasla.\n");
        return (1);
    }
    
    if (*max_dlugosc < 1) {
        fprintf(stderr, "Ujemna badz zerowa maksymalna dlugosc hasla.\n");
        return (1);
    }

    return (0);
}

long long powlong(long long base, int exp)
{
    long long result = 1LL;
    while( exp )
    {
        if ( exp & 1 )
        {
            result *= (long long)base;
        }
        exp >>= 1;
        base *= base;
    }
    return result;
}

void generateNewPassword(char* passwordVariable, long long index, 
        int length, const char* characters) {

    int charactersLength = strlen(characters);
    long long* permutationIndex = (long long*) calloc(length, sizeof (long long));
    char* newPassword = (char*) calloc(length, sizeof (char));
    
    int i;
    permutationIndex[0] = index % charactersLength;
    for (i = 1; i < length; i++) {
        permutationIndex[i] = (index / (powlong(charactersLength, i))) % charactersLength;        
    }

    for (i = 0; i < length; i++) {
        newPassword[i] = characters[permutationIndex[i]];        
    }
    
    strcpy(passwordVariable, newPassword);
    
    free(permutationIndex);
    free(newPassword);
}

