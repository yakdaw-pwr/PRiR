

#ifndef GJ_H
#define GJ_H

double* calculateGJ(double** matrix, int rowCount);

void releaseMemory(double** matrix, double* solution, int rowCount);

void calculate(const char* inputFileName, const char* outputFileName, int saveToFile);

#endif /* GJ_H */

