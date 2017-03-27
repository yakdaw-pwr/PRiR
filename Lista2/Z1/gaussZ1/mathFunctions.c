#include "mathFunctions.h"
#include<stdio.h>
#include<stdlib.h>

void copy(double* mainRow, double* copyRow, int size)
{
 for(int i = 0; i < size; i++){
   copyRow[i] = mainRow[i];
 }
}

void divide(double* row, double value, int rowSize){
 for(int i = 0; i < rowSize; i++){
   row[i] = row[i] / value;
 }
}

void substract(double* baseRow, double* destRow, double multiValue, int rowSize){
  for(int i = 0; i < rowSize; i++) {
   destRow[i] = destRow[i] - (baseRow[i] / multiValue);
  }
}

void add(double* baseRow, double* destRow, int rowSize) {
 for(int i = 0; i < rowSize; i++){
   destRow[i] += baseRow[i];
 }
}

void switchRows(double* firstRow, double* secondRow, int rowLength) {
 double* tempMat = (double*) calloc(rowLength, sizeof(double)); 
 for(int i = 0; i < rowLength; i++) {
  tempMat[i] = firstRow[i];
  firstRow[i] = secondRow[i];
  secondRow[i] = tempMat[i];
 }
 free(tempMat);
}


int findNonZeroElement(double** matrix, int rowCount, int pos){
  for(int i = pos+1; i < rowCount; i++){
     if(matrix[i][pos] != 0){
       return i;
     }
  }
   printf("Oops, something went wrong.");
  return -1;
}

void subRows(double** matrix, int rowIndex, int rowCount) {
  for(int i = 0; i < rowCount; i++) {
    if(i != rowIndex && matrix[i][rowIndex] != 0) {
	double value = matrix[rowIndex][rowIndex] / matrix[i][rowIndex]; 
	substract(matrix[rowIndex], matrix[i], value, rowCount+1);  
    }
  }
}


