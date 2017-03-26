#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "readFunctions.h"
#include "displayFunctions.h"
#include "mathFunctions.h"

int main(){
        //creating two dimensional matrix
	double** matrix;
        int rowCount = 0;
	double* solution;
        
        //starting the time and choosing accuracy
	double timeStart = clock() / (CLOCKS_PER_SEC / 1000000);
	
        printf("\nFirst data\n");
        //calculating first data
	Calculate("dane1", matrix, solution, &rowCount);
       
        //getting the time
	double timeMiddle = clock() / (CLOCKS_PER_SEC / 1000000);
        
	printf("\nSecond data\n");
	Calculate("dane2", matrix, solution, &rowCount);
	
        printf("\n");
	double timeEnd = clock() / (CLOCKS_PER_SEC / 1000000);
	
        printf("First data time: %.6lf\n"
                "Second data time: %.6lf\n"
		"Complete time: %.6lf\n", 
                (timeMiddle-timeStart)/1000000, 
                (timeEnd - timeMiddle)/1000000, 
                (timeEnd-timeStart)/1000000);
	
return(0);
}

double* CalculateGJ(double** matrix, int rowCount){
  double* x = (double*) calloc(rowCount, sizeof(double)); // solution
  
  for(int i = 0; i < rowCount; i++){
    if(matrix[i][i] == 0){
	int newRow = FindNonZeroElement(matrix, rowCount, i);
	SwitchRows(matrix[i], matrix[newRow], rowCount+1);
    }
    DivideRowBy(matrix[i], matrix[i][i], rowCount+1);
    SubRows(matrix, i, rowCount);
  }
  
  for(int i = 0; i< rowCount; i++){
    x[i] = matrix[i][rowCount];
  }
  return x;
}

void Calculate(const char* fileName, double** matrix, double* solution, int* rowCount){
 matrix = ReadData(fileName, matrix, rowCount);
 solution = CalculateGJ(matrix, *rowCount);
 PrintFinalMatrix(matrix,*rowCount);
 ReleaseMemory(matrix, solution, *rowCount);
}

void ReleaseMemory(double** matrix, double* solution, int rowCount){
 for(int i = 0; i < rowCount; i++){
  free(matrix[i]);
 }
 free(matrix);
 free(solution);
}

