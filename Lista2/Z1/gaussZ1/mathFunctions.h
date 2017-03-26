
#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

void copyRows(double* baseRow, double* destRow, int size);

void switchRows(double* row1, double* row2, int rowSize);

void addRows(double* baseRow, double* destRow, int rowSize);

int findNonZeroElement(double** matrix, int rowCount, int position);

void divideRowBy(double* row, double value, int rowSize);

void subRow(double* baseRow, double* destRow, double multiValue, int rowSize);

void subRows(double** matrix, int baseRowPosition, int rowCount);

#endif /* MATHFUNCTIONS_H */

