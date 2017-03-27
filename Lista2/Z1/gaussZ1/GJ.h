

#ifndef GJ_H
#define GJ_H

double* calculateGJ(double** matrix, int rowCount);
void releaseMemory(double** matrix, double* solution, int rowCount);
void calculate(const char* fileName);

#endif /* GJ_H */

