/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "GJ.h"

void calcNormall(double* firstTime, double* secondTime, double* completeTime) {
    //starting the time and choosing accuracy
    double timeStart = clock() / (CLOCKS_PER_SEC / 1000000);

    calculate("dane1", "dane1out", 0);

    //getting the time
    double firstDataTime = clock() / (CLOCKS_PER_SEC / 1000000);

    calculate("dane2", "dane2out", 0);

    double finishTime = clock() / (CLOCKS_PER_SEC / 1000000);

    *firstTime += (firstDataTime - timeStart) / 1000000;
    *secondTime += (finishTime - firstDataTime) / 1000000;
    *completeTime += (finishTime - timeStart) / 1000000;
}

void calcParallel(double* firstTime, double* secondTime, double* completeTime) {
    //starting the time and choosing accuracy
    double timeStart, firstDataTime, finishTime;

#pragma omp parallel sections num_threads(2)
    {
        timeStart = clock() / (CLOCKS_PER_SEC / 1000000);

#pragma omp section
        {
            calculate("dane1", "dane1out", 0);

            //getting the time
            firstDataTime = clock() / (CLOCKS_PER_SEC / 1000000);
        }
#pragma omp section
        {
            calculate("dane2", "dane2out", 0);

            printf("\n");
            finishTime = clock() / (CLOCKS_PER_SEC / 1000000);
        }
    }

    *firstTime += (firstDataTime - timeStart) / 1000000;
    *secondTime += (finishTime - firstDataTime) / 1000000;
    *completeTime += (finishTime - timeStart) / 1000000;
}

void generateAnswerFiles() {
    calculate("dane1", "dane1out", 1);

    calculate("dane2", "dane2out", 1);
}

int main() {
    generateAnswerFiles();

    int parallel = 0;
    int numberOfLoops = 100;

    double firstTime, secondTime, completeTime;

    for (int i = 0; i < numberOfLoops; i++) {

        if (parallel == 0) {
            calcNormall(&firstTime, &secondTime, &completeTime);
        } else {
            calcParallel(&firstTime, &secondTime, &completeTime);
        }
    }

    printf("\nAVERAGE TIMES:\n"
            "First data: %.6lf\n"
            "Second data: %.6lf\n"
            "Complete time: %.6lf\n",
            firstTime / numberOfLoops,
            secondTime / numberOfLoops,
            completeTime / numberOfLoops);

    return (0);
}
