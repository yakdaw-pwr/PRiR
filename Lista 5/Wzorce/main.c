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
    double* patternTime = (double*)malloc(sizeof(double) * numOfPatterns);
    double completeTime;

    // Rownolegle wykonanie wyszukiwania wzorca
    #pragma omp parallel num_threads(numOfThreads)
    {
        #pragma omp for schedule(dynamic, 1)
        for (int p = 0; p < numOfPatterns; p++) {
            
            double timeStart;
            double timeFinish;

            int patternLength = strlen(patterns[p]);
            
            int m, n, i, j;
            n = fileLength;
            m = patternLength;

            printf("--------- %s START ---------\n", patterns[p]);            
            timeStart = clock() / (CLOCKS_PER_SEC / 1000000);
            
            // Algorytm naiwny
//            i = 0;
//            while (i <= n - m) {
//                j = 0;
//                while ((j < m)&&(patterns[p][j] == text[i + j])) j++;
//                if (j == m) printf("%s : %d\n", patterns[p], i + 1);
//                i++;
//            }
      
            for (int i = 0; i < n-m; i++) {
//                printf("Wzorzec: %s, Watek: %d", patterns[p], omp_get_thread_num());
                j = 0;
                while ((j < m) && (patterns[p][j] == text[i + j])) {
                    j++;
                }
                if (j == m) {
                    printf("%s : %d\n", patterns[p], i + 1);
                }
            }
            
            
            
            timeFinish = clock() / (CLOCKS_PER_SEC / 1000000);
            patternTime[p] = ((timeFinish - timeStart) / 1000000);
            
            printf("--------- %s KONIEC ---------\n", patterns[p]);  
        }
    }
    
    printf("\n\n\n");
    for (int p = 0; p < numOfPatterns; p++) {
        printf("\"%s\" processing time: %.6lf s\n", patterns[p], patternTime[p]);
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

//int find_patterns(char** words, char *pattern, int threads_count, int words_counter) {
//    int i = 0, j, count = 0, k = 0;
//    int n = 0; //dlugosc slowa
//    int m = strlen(pattern); //dlugosc wzorca
//    int pom = 0;
//    double start, times;
//
//    start = omp_get_wtime();
//    printf("\nwzorzec '%s': %d, watek %i ", pattern, m, omp_get_thread_num());
//
//#pragma omp parallel num_threads(threads_count) shared(m,words_counter,words,pattern)
//    {
//#pragma omp for private(k,i,j)  reduction(+:count)
//        for (int k = 0; k < words_counter; k++) {
//            printf("\nWatek nr %i, k: %d ", omp_get_num_threads(), k);
//            n = strlen(words[k]);
//            pom = n - m;
//            for (i = 0; i <= pom; i++) {
//                for (j = 0; j < m; j++) {
//                    if (words[k][i + j] != pattern[j])
//                        j = m + 1;
//                }
//                if (j == m) {
//                    count++;
//                    //    printf("nr %i k: %d count %d\n", omp_get_thread_num(), k, count);
//                }
//            }
//        }
//    }
//    times = omp_get_wtime() - start;
//    printf("Czas: %f\n ", times);
//    printf("liczba wystapien %s w pliku : %d\n", pattern, count);
//    return 0;
//}