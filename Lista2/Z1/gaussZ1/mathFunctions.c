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

void subRows(double** matrix, int rowNumber, int rowCount) {
  for(int i = 0; i < rowCount; i++) {
    if(i != rowNumber && matrix[i][rowNumber] != 0) {
	double value = matrix[rowNumber][rowNumber] 
        / matrix[i][rowNumber]; 
	substract(matrix[rowNumber], matrix[i], value, rowCount+1);  
    }
  }
}


