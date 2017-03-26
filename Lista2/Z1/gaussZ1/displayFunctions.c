/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "displayFunctions.h"
#include<stdio.h>
#include<stdlib.h>

void printMatrix(double** matrix, int rowCount){
  for(int i = 0; i < rowCount; i++){
    for(int j = 0; j < rowCount+1; j++){
      printf("%.2f ", matrix[i][j]);     
    }
    printf("\n");
  }
}

void printSolution(double* x, int rowCount){
 for(int i = 0; i < rowCount; i++){
   printf("x%d: %.2lf\n", i, x[i]);
 }
}

void printFinalMatrix(double** matrix, int rowCount){
 printf("Liczba rownan: %d\n", rowCount);
 for(int i = 0; i < rowCount; i++){
   
     printf("Wiersz %d:\n", i);
   for(int j = 0; j < rowCount+1 ; j++){
	printf("%.2lf ", matrix[i][j]);
   }
   printf("\n");
 }
}