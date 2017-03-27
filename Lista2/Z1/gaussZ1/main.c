/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "GJ.h"

int parallel = 1;

void calcNormall() {
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
            (firstDataTime - timeStart) / 1000000,
            (finishTime - firstDataTime) / 1000000,
            (finishTime - timeStart) / 1000000);
}

void calcParallel() {
    //starting the time and choosing accuracy
    double timeStart, firstDataTime, finishTime;

#pragma omp parallel sections num_threads(2) 
    {
    timeStart = clock() / (CLOCKS_PER_SEC / 1000000);

    #pragma omp section 
        {
        printf("\nFirst data\n");
        calculate("dane1");

        //getting the time
        firstDataTime = clock() / (CLOCKS_PER_SEC / 1000000);
        }
    #pragma omp section 
        {

        printf("\nSecond data\n");
        calculate("dane2");

        printf("\n");
        finishTime = clock() / (CLOCKS_PER_SEC / 1000000);
        }
    }


printf("First data time: %.6lf\n"
        "Second data time: %.6lf\n"
        "Complete time: %.6lf\n",
        (firstDataTime - timeStart) / 1000000,
        (finishTime - firstDataTime) / 1000000,
        (finishTime - timeStart) / 1000000);
} 

int main() {
    if (parallel == 0) {
        calcNormall();
    } else {
        calcParallel();
    }
    return (0);
}