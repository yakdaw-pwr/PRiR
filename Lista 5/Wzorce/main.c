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
    
    int p;

    // Sprawdzenie poprawnosci argumentow
    if (validateExecutionArguments(argc, argv,
            &numOfPatterns, &numOfThreads) == 1) {
        return 1;
    }

    // Dodawanie wzorcow do tablicy
    patterns = (char**) malloc(sizeof (char*) * numOfPatterns);
    for (p = 0; p < numOfPatterns; p++) {
        patterns[p] = argv[p + 2];
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

    // Zapisywanie zawartosci pliku do stringa
    size_t fileLength;
    char *text = NULL;

    fseek(fp, 0, SEEK_END);
    fileLength = ftell(fp);
    rewind(fp);

    text = malloc((fileLength + 1) * sizeof (*text));
    fread(text, fileLength, 1, fp);

    fclose(fp);

    // Inicjowanie tablicy czasow dla kazdego wzorca
    double* patternTime = (double*) malloc(sizeof (double) * numOfPatterns);
    double completeTime;
    
    int* patternCount = (int*)malloc(sizeof(int) * numOfPatterns);
    for (p = 0; p < numOfPatterns; p++) {
        patternCount[p] = 0;
    }
    
    // Ustawienie ilosci watkow na ktorych dziala program
    omp_set_num_threads(numOfThreads);
 
    printf("\nRozpoczynam dzialanie rownolegle:\n\n");
    
    #pragma omp parallel for private(p) shared(patternTime) schedule(dynamic, 1)
    for (p = 0; p < numOfPatterns; p++) {

        double timeStart;
        double timeFinish;

        int patternLength = strlen(patterns[p]);

        int m, n, i, j;
        n = fileLength;
        m = patternLength;

        printf("--------- %s START --------- Watek: %d \n", 
                patterns[p], omp_get_thread_num());
        timeStart = omp_get_wtime();

        for (i = 0; i < n - m; i++) {
            j = 0;
            while ((j < m) && (patterns[p][j] == text[i + j])) {
                j++;
            }
            if (j == m) {
                patternCount[p]++;
            }
        }

        timeFinish = omp_get_wtime();
        
        patternTime[p] = timeFinish - timeStart;

        printf("--------- %s KONIEC ---------\n", patterns[p]);
    }

    printf("\n-----------------------------------------\n");
    
    for (p = 0; p < numOfPatterns; p++) {
        printf("\"%s\" count: %d\n", patterns[p], patternCount[p]);
    }
    
    printf("-----------------------------------------\n");
    
    for (p = 0; p < numOfPatterns; p++) {
        printf("\"%s\" processing time: %.12lf s\n", patterns[p], patternTime[p]);
        completeTime += patternTime[p];
    }

    printf("\nComplete time: %.6lf s\n", completeTime);

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