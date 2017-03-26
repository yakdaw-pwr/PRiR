
#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

void CopyRows(double* baseRow, double* destRow, int size);

void SwitchRows(double* row1, double* row2, int rowSize);

void AddRows(double* baseRow, double* destRow, int rowSize);

int FindNonZeroElement(double** matrix, int rowCount, int position);

void DivideRowBy(double* row, double value, int rowSize);

void SubRow(double* baseRow, double* destRow, double multiValue, int rowSize);

void SubRows(double** matrix, int baseRowPosition, int rowCount);

#endif /* MATHFUNCTIONS_H */

