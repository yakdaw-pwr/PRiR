//www.algorytm.org
//Algorytm Naiwny - wyszukiwanie wzorca
//(c)2001 Tomasz Lubinski
//2012 adaptacja LS 
 
#include <stdio.h>
#include <string.h>
 
int main(void)
{
    char wzorzec[1000];
    char tekst[100];
    int m,n,i,j;
    printf("Podaj tekst\n");
    scanf("%s", tekst);
    printf("Podaj wzorzec\n");
    scanf("%s", wzorzec);
    n=strlen(tekst);
    m=strlen(wzorzec);
    printf("Indeksy wystapien wzorca w tekscie\n");
    i=0;
    while (i<=n-m)
    {
	j=0;
	while ((j<m)&&(wzorzec[j]==tekst[i+j])) j++;
	if (j==m) printf("%d\n", i+1);
	i++;
    }
    getchar();
}

