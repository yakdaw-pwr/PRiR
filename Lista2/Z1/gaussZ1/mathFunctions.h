
#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

void copy(double* baseRow, double* destRow, int size);

void switchRows(double* row1, double* row2, int rowSize);

void add(double* baseRow, double* destRow, int rowSize);

int findNonZeroElement(double** matrix, int rowCount, int position);

void divide(double* row, double value, int rowSize);

void substract(double* baseRow, double* destRow, double multiValue, int rowSize);

void subRows(double** matrix, int baseRowPosition, int rowCount);

#endif /* MATHFUNCTIONS_H */

