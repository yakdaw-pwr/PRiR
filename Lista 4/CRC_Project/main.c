/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: yakdaw
 *
 * Created on April 7, 2017, 6:56 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

int checkIfHexValue(char* s);
int checkCrcVersion(char* s);


int main(int argc, char** argv) {
    
    if (argc < 2) {
        fprintf(stderr, "Brak argumentow wywolania programu.\n");
        return (1);
    }
    
    if (strcasecmp(argv[1], "OBLICZ") == 0) {
        
        if (argc != 4) {
            fprintf(stderr, "Bledna ilosc parametrow dla funkcji OBLICZ.\n");
            return (1);
        }
        
        char* hexData = argv[2];      
        if (checkIfHexValue(hexData) != 0)
        {
            fprintf(stderr, "Dane argumentu 2 powinny byc liczba heksadecymalna");
            return (1);
        }

        int crcVersion = checkCrcVersion(argv[3]);       
        if (crcVersion == -1) {
            fprintf(stderr, "Bledna wartosc wersji CRC. Mozliwosci: 12/16/32");
            return (1);
        }
        
        // GENERUJ CRC
    }
    else if (strcasecmp(argv[1], "TESTUJ") == 0) {
        printf("Testuj dobrze!\n");
        
        // SPRAWDZIĆ ARGUMENTY I TESTOWANIE CRC ZROBIC
    }
    else {
        fprintf(stderr, "Zly pierwszy argument wywolania programu.\n"
                "Przewidziane mozliwosci: oblicz/testuj\n");
        return (1);
    }
    
    return (EXIT_SUCCESS);
}

int checkIfHexValue(char* s) {
    return s[strspn(s, "0123456789abcdefABCDEF")];
}

int checkCrcVersion(char* s) {
    if (strcmp(s, "12") == 0) {
        return 12;
    }
    else if (strcmp(s, "16") == 0) {
        return 16;
    }
    else if (strcmp(s, "32") == 0) {
        return 32;
    }
    else {
        return -1; 
   }
}

