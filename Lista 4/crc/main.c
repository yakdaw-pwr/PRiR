/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: yakdaw
 *
 * Created on April 8, 2017, 11:13 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *MakeCRC(char *BitString, int crcNumber);
char *MakeCRC3(char *BitString);
char *MakeCRC12(char *BitString);
char *MakeCRC16(char *BitString);
char *MakeCRC32(char *BitString);

char* reverseString(char *str);

char* convertHexToBinary(const char *hexString);

/*
 * 
 */
int main(int argc, char** argv) {

    char *hexData;
    char *binaryData;
    char *crcValue;

    hexData = "796F";
    binaryData = convertHexToBinary(hexData);

    printf("\nBinary data: %s\n", binaryData);

    crcValue = MakeCRC(binaryData, 3); // Calculate CRC
    char* crcNonGeneric = MakeCRC3(binaryData);

    printf("Wartosc generycznego CRC dla 0x%s to %s\n", hexData, crcValue);
    printf("Wartosc CRC dla 0x%s to %s\n", hexData, crcNonGeneric);


    return (EXIT_SUCCESS);
}

char *MakeCRC(char *BitString, int crcNumber) {
    char* Res = (char*) malloc(sizeof (char) * (crcNumber + 1));
    char* CRC = (char*) malloc(sizeof (char) * crcNumber);

    char* polynomial;
    
    switch(crcNumber) {
        case 3:
            polynomial = "011";
            break;
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
    // "011";
    // "100000001111";
    // "1000000000000101";
    // "00000100110000010001110110110111";
    polynomial = reverseString(polynomial);

    char DoInvert;

    for (int i = 0; i < crcNumber; ++i) {
        CRC[i] = 0;
    }

    for (int i = 0; i < strlen(BitString); ++i) {
        DoInvert = ('1' == BitString[i]) ^ CRC[crcNumber - 1];

        for (int j = crcNumber - 1; j > 0; j--) {
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

    for (int i = 0; i < crcNumber; ++i) {
        Res[(crcNumber - 1) - i] = CRC[i] ? '1' : '0'; // Convert binary to ASCII  
    }

    Res[crcNumber] = 0; // Set string terminator

    return (Res);
}

char *MakeCRC3(char *BitString) {
    static char Res[4]; // CRC Result
    char CRC[3];
    int i;
    char DoInvert;

    for (i = 0; i < 3; ++i) CRC[i] = 0; // Init before calculation

    for (i = 0; i < strlen(BitString); ++i) {
        DoInvert = ('1' == BitString[i]) ^ CRC[2]; // XOR required?

        CRC[2] = CRC[1];
        CRC[1] = CRC[0] ^ DoInvert;
        CRC[0] = DoInvert;
    }

    for (i = 0; i < 3; ++i) Res[2 - i] = CRC[i] ? '1' : '0'; // Convert binary to ASCII
    Res[3] = 0; // Set string terminator

    return (Res);
}

char *MakeCRC12(char *BitString) {
    static char Res[13]; // CRC Result
    char CRC[12];
    int i;
    char DoInvert;

    for (i = 0; i < 12; ++i) CRC[i] = 0; // Init before calculation

    for (i = 0; i < strlen(BitString); ++i) {
        DoInvert = ('1' == BitString[i]) ^ CRC[11]; // XOR required?

        CRC[11] = CRC[10] ^ DoInvert;
        CRC[10] = CRC[9];
        CRC[9] = CRC[8];
        CRC[8] = CRC[7];
        CRC[7] = CRC[6];
        CRC[6] = CRC[5];
        CRC[5] = CRC[4];
        CRC[4] = CRC[3];
        CRC[3] = CRC[2] ^ DoInvert;
        CRC[2] = CRC[1] ^ DoInvert;
        CRC[1] = CRC[0] ^ DoInvert;
        CRC[0] = DoInvert;
    }

    for (i = 0; i < 12; ++i) Res[11 - i] = CRC[i] ? '1' : '0'; // Convert binary to ASCII
    Res[12] = 0; // Set string terminator

    return (Res);
}

char *MakeCRC16(char *BitString) {
    static char Res[17]; // CRC Result
    char CRC[16];
    int i;
    char DoInvert;

    for (i = 0; i < 16; ++i) CRC[i] = 0; // Init before calculation

    for (i = 0; i < strlen(BitString); ++i) {
        DoInvert = ('1' == BitString[i]) ^ CRC[15]; // XOR required?

        CRC[15] = CRC[14] ^ DoInvert;
        CRC[14] = CRC[13];
        CRC[13] = CRC[12];
        CRC[12] = CRC[11];
        CRC[11] = CRC[10];
        CRC[10] = CRC[9];
        CRC[9] = CRC[8];
        CRC[8] = CRC[7];
        CRC[7] = CRC[6];
        CRC[6] = CRC[5];
        CRC[5] = CRC[4];
        CRC[4] = CRC[3];
        CRC[3] = CRC[2];
        CRC[2] = CRC[1] ^ DoInvert;
        CRC[1] = CRC[0];
        CRC[0] = DoInvert;
    }

    for (i = 0; i < 16; ++i) Res[15 - i] = CRC[i] ? '1' : '0'; // Convert binary to ASCII
    Res[16] = 0; // Set string terminator

    return (Res);
}

char *MakeCRC32(char *BitString) {
    static char Res[33]; // CRC Result
    char CRC[32];
    int i;
    char DoInvert;

    for (i = 0; i < 32; ++i) CRC[i] = 0; // Init before calculation

    for (i = 0; i < strlen(BitString); ++i) {
        DoInvert = ('1' == BitString[i]) ^ CRC[31]; // XOR required?

        CRC[31] = CRC[30];
        CRC[30] = CRC[29];
        CRC[29] = CRC[28];
        CRC[28] = CRC[27];
        CRC[27] = CRC[26];
        CRC[26] = CRC[25] ^ DoInvert;
        CRC[25] = CRC[24];
        CRC[24] = CRC[23];
        CRC[23] = CRC[22] ^ DoInvert;
        CRC[22] = CRC[21] ^ DoInvert;
        CRC[21] = CRC[20];
        CRC[20] = CRC[19];
        CRC[19] = CRC[18];
        CRC[18] = CRC[17];
        CRC[17] = CRC[16];
        CRC[16] = CRC[15] ^ DoInvert;
        CRC[15] = CRC[14];
        CRC[14] = CRC[13];
        CRC[13] = CRC[12];
        CRC[12] = CRC[11] ^ DoInvert;
        CRC[11] = CRC[10] ^ DoInvert;
        CRC[10] = CRC[9] ^ DoInvert;
        CRC[9] = CRC[8];
        CRC[8] = CRC[7] ^ DoInvert;
        CRC[7] = CRC[6] ^ DoInvert;
        CRC[6] = CRC[5];
        CRC[5] = CRC[4] ^ DoInvert;
        CRC[4] = CRC[3] ^ DoInvert;
        CRC[3] = CRC[2];
        CRC[2] = CRC[1] ^ DoInvert;
        CRC[1] = CRC[0] ^ DoInvert;
        CRC[0] = DoInvert;
    }

    for (i = 0; i < 32; ++i) Res[31 - i] = CRC[i] ? '1' : '0'; // Convert binary to ASCII
    Res[32] = 0; // Set string terminator

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
    char *hexDigitToBinary[16] = {
        "0000", "0001", "0010", "0011", "0100", "0101",
        "0110", "0111", "1000", "1001", "1010", "1011",
        "1100", "1101", "1110", "1111"
    };

    char hexDigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
        '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    char* binaryNumber = (char*) malloc((strlen(hexString)*4 + 1));

    for (int i = 0; hexString[i] != '\0'; i++) {
        for (int j = 0; j < 16; j++) {
            if (hexString[i] == hexDigits[j]) {
                strcat(binaryNumber, hexDigitToBinary[j]);
            }
        }
    }

    return binaryNumber;
}