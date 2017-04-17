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

const int CRC8 = 8000;
const int CRC12 = 12000;
const int CRC16 = 16000;
const int CRC32 = 32000;
const int noCRC = 99999;

int checkIfHexValue(char* s);
int checkCrcVersion(char* s);

char *MakeCRC(char *BitString, int crcNumber);

char* reverseString(char *str);
char* convertHexToBinary(const char *hexString);
char* convertBinaryToHex(const char *binaryString);

int strlstchar(const char *str, const char ch);

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
        if (checkIfHexValue(hexData) != 0) {
            fprintf(stderr, "Dane argumentu 2 powinny byc liczba heksadecymalna\n");
            return (1);
        }

        int crcVersion = checkCrcVersion(argv[3]);
        if (crcVersion == -1) {
            fprintf(stderr, "Bledna wartosc wersji CRC. Mozliwosci: 12/16/32\n");
            return (1);
        }

        char* calculatedCRC = MakeCRC(convertHexToBinary(hexData), crcVersion);
        char* hexCrc = convertBinaryToHex(calculatedCRC);

        printf("Wartosc CRC: %s\n", hexCrc);
    } else if (strcasecmp(argv[1], "TESTUJ") == 0) {
        // SPRAWDZIĆ ARGUMENTY I TESTOWANIE CRC ZROBIC
        if (argc != 3) {

            return 1;
            fprintf(stderr, "Bledna ilosc parametrow dla funkcji OBLICZ.\n");
        }

        char* crcHex = argv[2];
        if (checkIfHexValue(crcHex) != 0 || strlen(crcHex) != 8) {
            fprintf(stderr, "Dane argumentu 2 powinny byc 32-bitowa "
                    "liczba zakodowana heksadecymalnie\n");
            return (1);
        }

        //        1. Sprawdzić ile bitów znaczacych ma ten podany string pzrez użytkownika
        //        2. Jeżeli 33+ to chuj, na pewno żaden z CRC naszych to nie był (w sumie powinno odrzucić w momencie podawania przez użytkownika stringa)
        //        3. Jeżeli 17+ bitow to CRC-32 bedzie na 100%
        //        4. Jeżeli 13+ to generuj liczby -MAXINT +MAXINT i sprawdzaj CRC-32 i CRC-16
        //        5. Jeżeli 12 i mniej to generuj liczby -MAXINT +MAXINT i sprawdzaj CRC-32 CRC-16 CRC-12

        char *binaryData;
        char *crcValue;
        binaryData = convertHexToBinary(crcHex);
        int meaningfulBytesCount = strlstchar(binaryData, '1');

        //        binaryStringToInt(binaryData);


        int result = testujCRC(binaryData);
        printf("Wynik %d", result);
        return (1);


    } else {
        fprintf(stderr, "Zly pierwszy argument wywolania programu.\n"
                "Przewidziane mozliwosci: oblicz/testuj\n");
        return (1);
    }

    return (EXIT_SUCCESS);
}

int testujCRC(char *binaryData) {
    int meaningfulBytesCount = strlstchar(binaryData, '1');
    int integerFormat = binaryStringToInt(binaryData);

    if (meaningfulBytesCount > 32) {
        return noCRC;
    } else if (meaningfulBytesCount > 16) {
        return CRC32;
    } else if (meaningfulBytesCount > 12) {
        return checkCRCTopTwo(integerFormat);
    } else if (meaningfulBytesCount > 8) {
        return checkCRCTopThree(integerFormat);

    }
}

int checkCRCTopTwo(int integerFormat) {
    int i;

    for (i = 0; i < 2^32; i++) {
        if (i == integerFormat && i < 2^16) {
            return CRC16;
        } else if (i == integerFormat) {
            return CRC32;
        }
    }
    return noCRC;
    //    printf("%d\n", strtol(b, &tmp, 2));
}

int checkCRCTopThree(int integerFormat) {
    int i;

    for (i = 0; i < 2^32; i++) {
        if (i == integerFormat && i < 2^16) {
            return CRC16;
        } else if (i == integerFormat) {
            return CRC32;
        }
    }
    return noCRC;
    //    printf("%d\n", strtol(b, &tmp, 2));
}

int binaryStringToInt(char* binaryS) {
    char *tmp;
    int integerFormat = strtoul(binaryS, &tmp, 2);
    return integerFormat;
    //    printf("%d\n", strtol(binaryS, &tmp, 2));
}

int strlstchar(const char *str, const char ch) {
    char *chptr = strrchr(str, ch);
    return chptr - str + 1;
}

int checkIfHexValue(char* s) {
    return s[strspn(s, "0123456789abcdefABCDEF")];
}

int checkCrcVersion(char* s) {
    if (strcmp(s, "12") == 0) {
        return 12;
    } else if (strcmp(s, "16") == 0) {
        return 16;
    } else if (strcmp(s, "32") == 0) {
        return 32;
    } else {
        return -1;
    }
}

char *MakeCRC(char *BitString, int crcNumber) {
    char* Res = (char*) malloc(sizeof (char) * (crcNumber + 1));
    char* CRC = (char*) malloc(sizeof (char) * crcNumber);

    int i, j;

    char* polynomial;

    switch (crcNumber) {
        case 12:
            polynomial = "100000001111";
            break;
        case 16:
            polynomial = "1000000000000101";
            break;
        case 32:
            polynomial = "00000100110000010001110110110111";
            break;
        default:
            return NULL;
    }
    polynomial = reverseString(polynomial);

    char DoInvert;

    for (i = 0; i < crcNumber; ++i) {
        CRC[i] = 0;
    }

    for (i = 0; i < strlen(BitString); ++i) {
        DoInvert = ('1' == BitString[i]) ^ CRC[crcNumber - 1];

        for (j = crcNumber - 1; j > 0; j--) {
            if (polynomial[j] == '1') {
                CRC[j] = CRC[j - 1] ^ DoInvert;
            } else {
                CRC[j] = CRC[j - 1];
            }
        }

        if (polynomial[0] == '1') {
            CRC[0] = DoInvert;
        }
    }

    for (i = 0; i < crcNumber; ++i) {
        Res[(crcNumber - 1) - i] = CRC[i] ? '1' : '0';
    }

    Res[crcNumber] = 0;

    return (Res);
}

char* reverseString(char *str) {
    static int i = 0;
    static char rev[100];
    if (*str) {
        reverseString(str + 1);
        rev[i++] = *str;
    }
    return rev;
}

char* convertHexToBinary(const char *hexString) {
    int i, j;
    char *hexDigitToBinary[16] = {
        "0000", "0001", "0010", "0011", "0100", "0101",
        "0110", "0111", "1000", "1001", "1010", "1011",
        "1100", "1101", "1110", "1111"
    };

    char hexDigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
        '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    char* binaryNumber = (char*) malloc((strlen(hexString)*4 + 1));

    for (i = 0; hexString[i] != '\0'; i++) {
        for (j = 0; j < 16; j++) {
            if (hexString[i] == hexDigits[j]) {
                strcat(binaryNumber, hexDigitToBinary[j]);
            }
        }
    }

    return binaryNumber;
}

char* convertBinaryToHex(const char *binaryString) {
    int value = (int) strtol(binaryString, NULL, 2);
    char* hexString = (char*) malloc(sizeof (char) * (strlen(binaryString) / 4));

    sprintf(hexString, "%x", value);

    return hexString;
}