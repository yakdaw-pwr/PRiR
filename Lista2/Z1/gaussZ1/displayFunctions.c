/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "displayFunctions.h"
#include<stdio.h>
#include<stdlib.h>

void PrintMatrix(double** matrix, int rowCount)
{
  int i;
  for(i = 0; i < rowCount; i++)
  {
    int j;
    for(j = 0; j < rowCount+1; j++)
    {
      printf("%.2f ", matrix[i][j]);     
    }
    printf("\n");
  }
}

void PrintSolution(double* x, int rowCount)
{
 int i;
 for(i = 0; i < rowCount; i++)
 {
   printf("x%d: %.2lf\n", i, x[i]);
 }
}

void PrintFinalMatrix(double** matrix, int rowCount)
{
int i;
 printf("Liczba rownan: %d\n", rowCount);
 for(i = 0; i < rowCount; i++)
 {
   int j;
   printf("Wiersz %d:\n", i);
   for(j = 0; j < rowCount+1 ; j++)
   {
	printf("%.2lf ", matrix[i][j]);
   }
   printf("\n");
 }
}