#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int validateExecutionArguments(int argc, char** argv,
        int* numOfPatterns, int* numOfThreads);


int main(int argc, char** argv) {

    int numOfPatterns;
    int numOfThreads;
    char** patterns;

    // Sprawdzenie poprawnosci argumentow
    if (validateExecutionArguments(argc, argv,
            &numOfPatterns, &numOfThreads) == 1) {
        return 1;
    }

    // Dodawanie wzorcow do tablicy
    patterns = (char**)malloc(sizeof(char*) * numOfPatterns);
    for (int i = 0; i < numOfPatterns; i++) {
        patterns[i] = argv[i + 2];
    }

    // Otwieranie pliku
    FILE *fp;
    char filePath[256];

    printf("Podaj sciezke do pliku z tekstem: ");
    scanf("%s", filePath);

    fp = fopen(filePath, "r");

    if (fp == NULL) {
        perror("Blad podczas otwierania pliku.\n");
        exit(EXIT_FAILURE);
    }

    size_t fileLength;
    char *text = NULL;

    fseek(fp, 0, SEEK_END);
    fileLength = ftell(fp);
    rewind(fp);

    text = malloc((fileLength + 1) * sizeof (*text));
    fread(text, fileLength, 1, fp);

    fclose(fp);
    
    double* completeTime = (double*)malloc(sizeof(double) * numOfPatterns);

    #pragma omp parallel num_threads(numOfThreads)
    {
        #pragma omp for
        for (int p = 0; p < numOfPatterns; p++) {
            
            double timeStart;
            double timeFinish;

            int patternLength = strlen(patterns[p]);
            
            int m, n, i, j;
            n = fileLength;
            m = patternLength;

            printf("--- %s START ---\n", patterns[p]);            
            timeStart = clock() / (CLOCKS_PER_SEC / 1000000);
            
            i = 0;
            while (i <= n - m) {
                j = 0;
                while ((j < m)&&(patterns[p][j] == text[i + j])) j++;
                if (j == m) printf("%s : %d\n", patterns[p], i + 1);
                i++;
            }
            
            timeFinish = clock() / (CLOCKS_PER_SEC / 1000000);
            completeTime[p] = ((timeFinish - timeStart) / 1000000);
        }
    }
    
    printf("\n\n\n");
    for (int p = 0; p < numOfPatterns; p++) {
        printf("\"%s\" processing time: %.6lf s\n", patterns[p], completeTime[p]);
    }

    return (EXIT_SUCCESS);
}

int validateExecutionArguments(int argc, char** argv,
        int* numOfPatterns, int* numOfThreads) {

    if (argc < 4) {
        fprintf(stderr, "Niewystarczajaca ilosc parametrow wywolania programu.\n");
        return (1);
    }

    // Check if first parameter is integer
    if (sscanf(argv[1], "%i", numOfPatterns) != 1) {
        fprintf(stderr, "Niepoprawny pierwszy parametr - liczba wzorcow.\n");
        return (1);
    }

    if (argc != 3 + *numOfPatterns) {
        fprintf(stderr, "Bledna ilosc parametrow wywolania programu.\n");
        return (1);
    }

    if (sscanf(argv[*numOfPatterns + 2], "%i", numOfThreads) != 1) {
        fprintf(stderr, "Niepoprawny ostatni parametr - liczba watkow.\n");
        return (1);
    }

    return (0);
}