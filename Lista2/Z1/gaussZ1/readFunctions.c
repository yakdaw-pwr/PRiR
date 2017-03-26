/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "readFunctions.h"
#include<stdio.h>
#include<stdlib.h>

double** loadMatrix (const char* file_name, double** matrix, int* rowCount){
  FILE* file = fopen (file_name, "r");
  int i = 0;
  int indexColumn = 0;
  int indexRow = 0;
  double readdouble = 0;
  
  //scan number of rows/columns which is provided at the beginning
  fscanf (file, "%d", &i);    
  *rowCount = i;

  matrix = (double**)calloc(*rowCount, sizeof(double*));

  for(int j = 0; j < *rowCount; j++){
    matrix[j] = (double*)calloc((*rowCount)+1, sizeof(double));
  }
  
//printf("Row cout: %d", *rowCount);
  while (!feof (file)){       
      fscanf (file, "%lf", &readdouble);
      matrix[indexColumn][indexRow] = readdouble;
      indexRow++;      
      if(indexRow > (*rowCount)){
        indexRow=0;
        indexColumn++;
        if(indexColumn > *rowCount-1)
	  break;
      }
    }
  fclose (file);    
return matrix;    
}

