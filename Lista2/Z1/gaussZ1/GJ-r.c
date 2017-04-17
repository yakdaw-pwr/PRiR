#include<stdio.h>
#include<stdlib.h>
#include<time.h>


//load

double** loadMatrix(const char* file_name, double** matrix, int* rowSize) {
    FILE* data = fopen(file_name, "r");
    int columnNum = 0;
    int rowNum = 0;
    double readdouble = 0;

    //scan number of rows/columns which is provided at the beginning
    int tempMemory = 0;
    fscanf(data, "%d", &tempMemory);
    *rowSize = tempMemory;

    matrix = (double**) calloc(*rowSize, sizeof (double*));

    int j;
    for (j = 0; j < *rowSize; j++) {
        matrix[j] = (double*) calloc((*rowSize) + 1, sizeof (double));
    }

    while (!feof(data)) {
        fscanf(data, "%lf", &readdouble);
        matrix[columnNum][rowNum] = readdouble;
        rowNum++;
        if (rowNum > (*rowSize)) {
            rowNum = 0;
            columnNum++;
            if (columnNum > *rowSize - 1) break;
        }
    }
    fclose(data);
    return matrix;
}

//math

void divide(double* row, double valueOfMiddleElement, int rowSize) {
    int i;
    for (i = 0; i < rowSize; i++) {
        row[i] = row[i] / valueOfMiddleElement;
    }
}

void substract(double* baseRow, double* destRow, double multiValue, int rowSize) {
    int i;
    for (i = 0; i < rowSize; i++) {
        destRow[i] = destRow[i] - (baseRow[i] / multiValue);
    }
}

void subRows(double** matrix, int rowIndex, int rowCount, int thr) {
    int i;

#pragma omp parallel for
    for (i = 0; i < rowCount; i++) {
        if (i != rowIndex && matrix[i][rowIndex] != 0) {
            double value = matrix[rowIndex][rowIndex] / matrix[i][rowIndex];
            substract(matrix[rowIndex], matrix[i], value, rowCount + 1);
        }
    }
}
//--

void printFinalMatrix(double** matrix, int rowCount) {
    printf("Liczba rownan: %d\n", rowCount);
    int i;
    for (i = 0; i < rowCount; i++) {

        printf("Wiersz %d:\n", i);
        int j;
        for (j = 0; j < rowCount + 1; j++) {
            printf("%.2lf ", matrix[i][j]);
        }
        printf("\n");
    }
}

double* calculateGJ(double** matrix, int rowCount, int thr) {
    double* solutionMatrix = (double*) calloc(rowCount, sizeof (double));
    int i;
    for (i = 0; i < rowCount; i++) {
        divide(matrix[i], matrix[i][i], rowCount + 1);
        subRows(matrix, i, rowCount, thr);
    }

    for (i = 0; i < rowCount; i++) {
        solutionMatrix[i] = matrix[i][rowCount];
    }
    return solutionMatrix;
}

void releaseMemory(double** matrix, double* solution, int rowCount) {
    int i;
    for (i = 0; i < rowCount; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(solution);
}

void calculate(const char* inputFileName, int thr) {
    //creating two dimensional matrix
    double** matrix;
    double* solution;
    int rowCount = 0;
    matrix = loadMatrix(inputFileName, matrix, &rowCount);
    solution = calculateGJ(matrix, rowCount, thr);
    printFinalMatrix(matrix, rowCount);
    releaseMemory(matrix, solution, rowCount);
}

void calc(double* firstTime, double* secondTime, double* completeTime, int thr) {
    //starting the time and choosing accuracy
    double timeStart = clock() / (CLOCKS_PER_SEC / 1000000);

    calculate("dane1", thr);

    double firstDataTime = clock() / (CLOCKS_PER_SEC / 1000000);
    double firstTimeForPrint = (firstDataTime - timeStart) / 1000000;
    printf("\nCzas obliczeń: %.6lf\n\n", firstTimeForPrint);

    //----------

    calculate("dane2", thr);

    double finishTime = clock() / (CLOCKS_PER_SEC / 1000000);
    double secondTimeForPrint = (finishTime - firstDataTime) / 1000000;
    printf("\nCzas obliczeń: %.6lf\n\n", secondTimeForPrint);

    *firstTime += (firstDataTime - timeStart) / 1000000;
    *secondTime += (finishTime - firstDataTime) / 1000000;
    *completeTime += (finishTime - timeStart) / 1000000;
}

int main() {

    int numberOfLoops = 1;
    int numberOfThreads = 1;
    int thr;
    int i;

    for (thr = 1; thr <= numberOfThreads; thr++) {
        double firstTime = 0,
                secondTime = 0,
                completeTime = 0;


        for (i = 0; i < numberOfLoops; i++) {
            calc(&firstTime, &secondTime, &completeTime, thr);
        }
        //        printf("\nAVERAGE TIMES:\n"
        //                "First data: %.6lf\n"
        //                "Second data: %.6lf\n"
        //                "Complete time: %.6lf\n\n"
        //                "Number of threads: %d\n",
        //                firstTime / numberOfLoops,
        //                secondTime / numberOfLoops,
        //                completeTime / numberOfLoops,
        //                thr);

    }

    return (0);
}

