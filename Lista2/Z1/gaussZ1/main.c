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

void calcNormall(double* firstTime, double* secondTime, double* completeTime) {
    //starting the time and choosing accuracy

    double timeStart = clock() / (CLOCKS_PER_SEC / 1000000);

    calculate("dane3");

    //getting the time
    double firstDataTime = clock() / (CLOCKS_PER_SEC / 1000000);

    calculate("dane2");

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
        calculate("dane3");

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
    
        double firstTime, secondTime, completeTime;

    for(int i = 0; i<10; i++){

    if (parallel == 0) {
        calcNormall(&firstTime, &secondTime, &completeTime);
    } else {
        calcParallel();
    }
    }
    return (0);
}

//------------------------------------>

//java generator
//        
//
//import java.lang.Math; // headers MUST be above the first class
//import java.util.concurrent.ThreadLocalRandom;
//
//// one class needs to have a main() method
//public class HelloWorld
//{
//  // arguments are passed using the text field below this editor
//  public static void main(String[] args)
//  {
//    	int minimum = 0;
//    	int maximum = 400;
//    for(int i = 0; i<10; i++){
//    	int randomNum = minimum + (int)(Math.random() * maximum); 
//		System.out.print(String.valueOf(randomNum) + "\t" );
//    }
//  }
//}
