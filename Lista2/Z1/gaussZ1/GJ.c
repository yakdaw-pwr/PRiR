#include<stdio.h>
#include<stdlib.h>
#include<time.h>


//load

double** loadMatrix(const char* file_name, double** matrix, int* rowCount) {
    FILE* file = fopen(file_name, "r");
    int i = 0;
    int indexColumn = 0;
    int indexRow = 0;
    double readdouble = 0;

    //scan number of rows/columns which is provided at the beginning
    fscanf(file, "%d", &i);
    *rowCount = i;

    matrix = (double**) calloc(*rowCount, sizeof (double*));

    for (int j = 0; j < *rowCount; j++) {
        matrix[j] = (double*) calloc((*rowCount) + 1, sizeof (double));
    }

    while (!feof(file)) {
        fscanf(file, "%lf", &readdouble);
        matrix[indexColumn][indexRow] = readdouble;
        indexRow++;
        if (indexRow > (*rowCount)) {
            indexRow = 0;
            indexColumn++;
            if (indexColumn > *rowCount - 1) break;
        }
    }
    fclose(file);
    return matrix;
}

//math

void divide(double* row, double valueOfMiddleElement, int rowSize) {
    for (int i = 0; i < rowSize; i++) {
        row[i] = row[i] / valueOfMiddleElement;
    }
}

void substract(double* baseRow, double* destRow, double multiValue, int rowSize) {
    for (int i = 0; i < rowSize; i++) {
        destRow[i] = destRow[i] - (baseRow[i] / multiValue);
    }
}

void subRows(double** matrix, int rowIndex, int rowCount) {
    for (int i = 0; i < rowCount; i++) {
        if (i != rowIndex && matrix[i][rowIndex] != 0) {
            double value = matrix[rowIndex][rowIndex] / matrix[i][rowIndex];
            substract(matrix[rowIndex], matrix[i], value, rowCount + 1);
        }
    }
}
//--

void printFinalMatrix(double** matrix, int rowCount) {
    printf("Liczba rownan: %d\n", rowCount);
    for (int i = 0; i < rowCount; i++) {

        printf("Wiersz %d:\n", i);
        for (int j = 0; j < rowCount + 1; j++) {
            printf("%.2lf ", matrix[i][j]);
        }
        printf("\n");
    }
}

double* calculateGJ(double** matrix, int rowCount) {
    double* solutionMatrix = (double*) calloc(rowCount, sizeof (double));

    for (int i = 0; i < rowCount; i++) {
        divide(matrix[i], matrix[i][i], rowCount + 1);
        subRows(matrix, i, rowCount);
    }

    for (int i = 0; i < rowCount; i++) {
        solutionMatrix[i] = matrix[i][rowCount];
    }
    return solutionMatrix;
}

void releaseMemory(double** matrix, double* solution, int rowCount) {
    for (int i = 0; i < rowCount; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(solution);
}

void calculate(const char* inputFileName) {
    //creating two dimensional matrix
    double** matrix;
    double* solution;
    int rowCount = 0;

    matrix = loadMatrix(inputFileName, matrix, &rowCount);
    solution = calculateGJ(matrix, rowCount);
    printFinalMatrix(matrix, rowCount);
    releaseMemory(matrix, solution, rowCount);
}

void calc(double* firstTime, double* secondTime, double* completeTime) {
    //starting the time and choosing accuracy
    double timeStart = clock() / (CLOCKS_PER_SEC / 1000000);

    calculate("dane1");

    //getting the time
    double firstDataTime = clock() / (CLOCKS_PER_SEC / 1000000);

    calculate("dane2");

    double finishTime = clock() / (CLOCKS_PER_SEC / 1000000);

    *firstTime += (firstDataTime - timeStart) / 1000000;
    *secondTime += (finishTime - firstDataTime) / 1000000;
    *completeTime += (finishTime - timeStart) / 1000000;
}

int main() {

    int numberOfLoops = 1;

    double firstTime, secondTime, completeTime;

    for (int i = 0; i < numberOfLoops; i++) {
        calc(&firstTime, &secondTime, &completeTime);
    }

    printf("\nAVERAGE TIMES:\n"
            "First data: %.6lf\n"
            "Second data: %.6lf\n"
            "Complete time: %.6lf\n",
            firstTime / numberOfLoops,
            secondTime / numberOfLoops,
            completeTime / numberOfLoops);

    return (0);
}

