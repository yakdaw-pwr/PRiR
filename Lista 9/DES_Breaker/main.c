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
    
    const int maxPasswordsFound = 50;
    int passwordsFound;
    
    // Wszystkie mozliwe znaki
    const char* characters = "0123456789"; 
   
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

        // Sprawdz argumenty wywolania
        if (validateExecutionArguments(argc, argv, zaszyfrowane, &max_dlugosc) != 0) {
            MPI_Abort(MPI_COMM_WORLD, rc);
            return (1);
        }
        zaszyfrowane = argv[1];
        
        // Liczba mozliwosci dla danej dlugosci
        long long numOfCases;
        
        // Zmienne przechowujace generowane hasla i ich hashe
        char* generatedPassword = (char*) calloc(max_dlugosc, sizeof (char));
        char* generatedHashPassword = (char*) calloc(13, sizeof (char));
        
        // Liczba znalezionych hasel
        int allPasswordsFound = 0;
        
        // Jezeli MASTER to jedyny proces
        if (worldSize == 1) {
            
            // Generuj stringi o dlugosci od 1 do max_dlugosc
            for (i = 1; i <= max_dlugosc; i++) {
                // Oblicz liczbe mozliwosci dla danej dlugosci
                numOfCases = powlong((long long)strlen(characters), i);
                
                printf("----------Rozpoczynam wysylanie dla dlugosci %d----------\n", i);
                
                long long c;
                // Wygeneruj kazda mozliwosc
                for (c = 0; c < numOfCases; c++) {
                    generateNewPassword(generatedPassword, c, i, characters);
                    generatedHashPassword = crypt(generatedPassword, zaszyfrowane);
                    
                    // Jezeli haslo sie zgadza
                    if (!strcmp(generatedHashPassword, zaszyfrowane)) {
                        printf("Znalazlem haslo: %s | Czas trwania %lf\n", 
                                generatedPassword, MPI_Wtime() - timeStart);
                        allPasswordsFound++;
                    }
                    
                    // Jezeli wystarczajaca ilosc hasel, zakoncz program
                    if (allPasswordsFound >= maxPasswordsFound) {
                        printf("Znaleziono wystarczajaco duzo hasel (%d)\n"
                                "Koncze dzialanie programu.\n", allPasswordsFound);

                        MPI_Finalize();
                        return (0);
                    }
                }
            }
        }
        // Jezeli wiecej niz 1 proces
        else {           
            
            long long* startRowNumbers = (long long*) calloc(worldSize, sizeof (long long));
            long long* endRowNumbers = (long long*) calloc(worldSize, sizeof (long long));
            
            int p;
            // Kazdemu procesowi wykonawczemu
            for (p = 1; p < worldSize; p++) {
                // Wyslij zaszyfrowane haslo
                MPI_Send(zaszyfrowane, 13, MPI_CHAR, p, 1, MPI_COMM_WORLD);
                // Wyslij maksymalna dlugosc
                MPI_Send(&max_dlugosc, 1, MPI_INT, p, 2, MPI_COMM_WORLD);
            }

            // Stringi o dlugosci od 1 do max_dlugosc
            for (i = 1; i <= max_dlugosc; i++) {

                printf("----------Rozpoczynam wysylanie dla dlugosci %d----------\n", i);
                
                numOfCases = powlong((long long) strlen(characters), i);

                // Wylicz ile przypadkow na 1 proces
                long long casesPerProcess = numOfCases / worldSize;

                // Wylicz przypadki startowe i koncowe blokow dla poszczegolnych procesow
                int j;
                for (j = 0; j < worldSize; j++) {
                    startRowNumbers[j] = j * casesPerProcess;
                    endRowNumbers[j] = startRowNumbers[j] + casesPerProcess - 1;
                }
                endRowNumbers[worldSize - 1] = numOfCases - 1;          

                for (p = 1; p < worldSize; p++) {
                    // Wyslij numer poczatkowy
                    MPI_Send(&startRowNumbers[p], 1, MPI_LONG_LONG, p, 10 + i, MPI_COMM_WORLD);                   
                    // Wyslij numer koncowy                    
                    MPI_Send(&endRowNumbers[p], 1, MPI_LONG_LONG, p, 50 + i, MPI_COMM_WORLD);
                }
                
                long long c;
                // Wygeneruj mozliwosci dla procesu 0
                for (c = startRowNumbers[0]; c <= endRowNumbers[0]; c++) {
                    generateNewPassword(generatedPassword, c, i, characters);
                    generatedHashPassword = crypt(generatedPassword, zaszyfrowane);
                    if (!strcmp(generatedHashPassword, zaszyfrowane)) {
                        printf("Znalazlem haslo: %s | Czas trwania %lf\n", 
                                generatedPassword, MPI_Wtime() - timeStart);
                    }
                }
                
                // Pobierz od innych procesow ilosc znalezionych hasel
                for (p = 1; p < worldSize; p++) {
                    MPI_Recv(&passwordsFound, 1, MPI_INT, p, 100 + i, MPI_COMM_WORLD, &status);
                    allPasswordsFound += passwordsFound;
                }
                
                // Jezeli ilosc znalezionych hasel jest wystarczajaca
                if (allPasswordsFound >= maxPasswordsFound) {
                    printf("Znaleziono wystarczajaco duzo hasel (%d)\n"
                            "Koncze dzialanie programu.\n", allPasswordsFound);
                    
                    long long endValue = -1;
                    // Wyslij sygnal zakonczajacy dzialanie procesow
                    for (p = 1; p < worldSize; p++) {
                        MPI_Send(&endValue, 1, MPI_LONG_LONG, p, 10 + i + 1, MPI_COMM_WORLD);  
                    }
                    
                    MPI_Finalize();
                    return (0);
                }
            }
        }
        
        if (allPasswordsFound == 0) {
            printf("Nie znaleziono zadnego pasujacego hasla.\n");
        }
        printf("Koncze dzialanie programu.\n");
    }        
    // Dla procesów wykonawczych
    else {
        // Numer startowy badanych przypadkow
        long long startRowNumber;
        // Numer koncowy badanych przypadkow
        long long endRowNumber;
        
        char* hashedPassword = (char*) calloc(13, sizeof(char));
        int max_length;
        
        // Odbierz haslo i maksymalna dlugosc
        MPI_Recv(hashedPassword, 13, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&max_length, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        
        // Zmienne dla generowanego hasla i jego hasha
        char* generatedPassword = (char*) calloc(max_length, sizeof (char));
        char* generatedHashPassword = (char*) calloc(13, sizeof (char));
        
        // Stringi o dlugosci od 1 do max_dlugosc
        for (i = 1; i <= max_length; i++) {
            // Odbierz numer startowy
            MPI_Recv(&startRowNumber, 1, MPI_LONG_LONG, 0, 10 + i, MPI_COMM_WORLD, &status);
            
            // Jezeli -1 (znaleziono wystarczajaca ilosc hasel), zakoncz dzialanie
            if (startRowNumber == -1) {
                MPI_Finalize();
                return(0);
            }
            
            // Odbierz numer koncowy
            MPI_Recv(&endRowNumber, 1, MPI_LONG_LONG, 0, 50 + i, MPI_COMM_WORLD, &status);
            
            passwordsFound = 0;

            long long c;
            // Sprawdz przypadki od numeru startowego do koncowego
            for (c = startRowNumber; c <= endRowNumber; c++) {
                generateNewPassword(generatedPassword, c, i, characters);
                generatedHashPassword = crypt(generatedPassword, hashedPassword);
                if (!strcmp(generatedHashPassword, hashedPassword)) {
                    printf("Proces %d znalazl haslo: %s | Czas trwania %lf\n",
                        worldRank, generatedPassword, MPI_Wtime() - timeStart);
                    passwordsFound++;
                }
            }
            
            // Wyslij liczbe znalezionych hasel
            MPI_Send(&passwordsFound, 1, MPI_INT, 0, 100 + i, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();

    return (EXIT_SUCCESS);
}

// Sprawdzanie argumentow wywolania
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

// Funkcja potegi dla zmiennych typu long long
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

// Generowanie stringow
// passwordVariable - zmienna przechowujaca haslo
// index - badany przypadek (liczba zamieniana na string)
// length - dlugosc generowanego hasla
// characters - string przechowujacy znaki dozwolone w generowanych wyrazach
void generateNewPassword(char* passwordVariable, long long index, 
        int length, const char* characters) {

    int charactersLength = strlen(characters);
    
    // Tablica indeksow kazdego ze znakow w stringu
    long long* permutationIndex = (long long*) calloc(length, sizeof (long long));
    // Generowane haslo
    char* newPassword = (char*) calloc(length, sizeof (char));
    
    int i;
    // Generowanie indeksow na podstawie dzielenia/modulo z indeksu
    permutationIndex[0] = index % charactersLength;
    for (i = 1; i < length; i++) {
        permutationIndex[i] = (index / (powlong(charactersLength, i))) % charactersLength;        
    }

    // Zapisywanie do stringa
    for (i = 0; i < length; i++) {
        newPassword[i] = characters[permutationIndex[i]];        
    }
    
    // Skopiowanie do podanej zmiennej zewnetrznej
    strcpy(passwordVariable, newPassword);
    
    free(permutationIndex);
    free(newPassword);
}