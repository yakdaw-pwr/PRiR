/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "readFunctions.h"
#include<stdio.h>
#include<stdlib.h>

double** ReadData (const char* file_name, double** matrix, int* rowCount)
{
  FILE* file = fopen (file_name, "r");
  int i = 0;
  int index = 0;
  int index2 = 0;
  double readdouble = 0;
  fscanf (file, "%d", &i);    
  *rowCount = i;
  //matrix = (int*)calloc(i*(i+1), sizeof(int));
  matrix = (double**)calloc(*rowCount, sizeof(double*));
  int j = 0;
  for(j = 0; j < *rowCount; j++)
  {
    matrix[j] = (double*)calloc((*rowCount)+1, sizeof(double));
  }
  
//printf("Row cout: %d", *rowCount);
  while (!feof (file))
    {       
      fscanf (file, "%lf", &readdouble);
      matrix[index][index2] = readdouble;
      index2++;      
      if(index2 > (*rowCount))
      {
        index2=0;
        index++;
        if(index > *rowCount-1)
	  break;
      }
    }
  fclose (file);    
return matrix;    
}

