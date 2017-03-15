#include <stdio.h>
#include <stdlib.h>

int main() 
{
   int num, i, j, tryCount;
   int primeCount = 0;
   int isNotPrime;

   printf("Wprowadz liczbe prob: ");
   scanf("%d", &tryCount);
   printf("\n");

   for(j = 0; j < tryCount; j++) 
   {
       num = lrand48();
       printf("Sprawdzam %d wygenerowana liczbe..\r", j+1);
       fflush(stdout);
       
       isNotPrime = 0;
       
       for (i = 2; i <= num / 2; i++) 
       {
          if (num % i == 0) 
          {
             isNotPrime = 1;
             break;
          }
       }

       if (isNotPrime == 0)
       {
          primeCount++;  
       }

   }

    printf("\rNa %d prob, %d z wygenerowanych wartosci to liczby pierwsze\n",
            tryCount, primeCount);

   return 0;
}
