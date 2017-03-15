// http://www.c4learn.com/c-programs/check-whether-number-is-prime-or-not.html //

#include<stdio.h>
 
int main() {
   int num, i, count = 0;
 
   printf("Enter a number:");
   scanf("%d", &num);
 
   for (i = 2; i <= num / 2; i++) {
      if (num % i == 0) {
         count++;
         break;
      }
   }
 
   if (count == 0)
      printf("%d is a prime number\n", num);
   else
      printf("%d is not a prime number\n", num);
 
   return 0;
}
