#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "readFunctions.h"
#include "displayFunctions.h"
#include "mathFunctions.h"

double* calculateGJ(double** matrix, int rowCount){
  double* solutionMatrix = (double*) calloc(rowCount, sizeof(double)); 
  
  for(int i = 0; i < rowCount; i++){
    if(matrix[i][i] == 0){
	int newRow = findNonZeroElement(matrix, rowCount, i);
	switchRows(matrix[i], matrix[newRow], rowCount+1);
    }
    divide(matrix[i], matrix[i][i], rowCount+1);
    subRows(matrix, i, rowCount);
  }
  
  for(int i = 0; i< rowCount; i++){
    solutionMatrix[i] = matrix[i][rowCount];
  }
  return solutionMatrix;
}

void releaseMemory(double** matrix, double* solution, int rowCount){
 for(int i = 0; i < rowCount; i++){
  free(matrix[i]);
 }
 free(matrix);
 free(solution);
}

void calculate(const char* fileName){ 
 //creating two dimensional matrix
 double** matrix;
 double* solution;
 int rowCount = 0;

 matrix = loadMatrix(fileName, matrix, &rowCount);
 solution = calculateGJ(matrix, rowCount);
 printFinalMatrix(matrix,rowCount);
 releaseMemory(matrix, solution, rowCount);
}

int main(){
   
        
        //starting the time and choosing accuracy
	double timeStart = clock() / (CLOCKS_PER_SEC / 1000000);
	
        printf("\nFirst data\n");
	calculate("dane1");
       
        //getting the time
	double firstDataTime = clock() / (CLOCKS_PER_SEC / 1000000);
        
	printf("\nSecond data\n");
	calculate("dane2");
	
        printf("\n");
	double finishTime = clock() / (CLOCKS_PER_SEC / 1000000);
	
        printf("First data time: %.6lf\n"
                "Second data time: %.6lf\n"
		"Complete time: %.6lf\n", 
                (firstDataTime - timeStart)/1000000, 
                (finishTime - firstDataTime)/1000000, 
                (finishTime-timeStart)/1000000);
	
return(0);
}