/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "mathFunctions.h"
#include<stdio.h>
#include<stdlib.h>

/**
 * A brief description. A more elaborate class description
 * @param bool somebool a boolean argument.
 * @see Test()
 * @return The test results
 */
void copyRows(double* baseRow, double* destRow, int size)
{
 for(int i = 0; i < size; i++){
   destRow[i] = baseRow[i];
 }
}

void switchRows(double* row1, double* row2, int rowSize) {
 double* calcRow = (double*) calloc(rowSize, sizeof(double)); 
 for(int i = 0; i < rowSize; i++) {
  calcRow[i] = row1[i];
  row1[i] = row2[i];
  row2[i] = calcRow[i];
 }
 free(calcRow);
}



void addRows(double* baseRow, double* destRow, int rowSize) {
 for(int i = 0; i < rowSize; i++){
   destRow[i] += baseRow[i];
 }
}

int findNonZeroElement(double** matrix, int rowCount, int position){
  for(int i = position+1; i < rowCount; i++){
     if(matrix[i][position] != 0){
       return i;
     }
  }
   printf("Error - Matrix is invalid");
  return 0;
}

void divideRowBy(double* row, double value, int rowSize){
 for(int i = 0; i < rowSize; i++){
   row[i] = row[i] / value;
 }
}

void subRow(double* baseRow, double* destRow, double multiValue, int rowSize){
  for(int i = 0; i < rowSize; i++) {
   destRow[i] = destRow[i] - (baseRow[i] / multiValue);
  }
}

void subRows(double** matrix, int baseRowPosition, int rowCount) {
  for(int i = 0; i < rowCount; i++) {
    if(i != baseRowPosition && matrix[i][baseRowPosition] != 0) {
	double value = matrix[baseRowPosition][baseRowPosition] 
        / matrix[i][baseRowPosition]; 
	subRow(matrix[baseRowPosition], matrix[i], value, rowCount+1);  
    }
  }
}


