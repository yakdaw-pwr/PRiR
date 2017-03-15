//www.algorytm.org
//Algorytm Naiwny - wyszukiwanie wzorca
//(c)2001 Tomasz Lubinski
//2012 adaptacja LS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    FILE *fp;
    char *bufor = NULL;

    size_t dlugoscPliku;
    int dlugoscWzorca;

    char wzorzec[1000];
    char sciezkaPliku[1000];
    int m,n,i,j;

    printf("Podaj sciezke do pliku z tekstem: ");
    scanf("%s", sciezkaPliku);

    fp = fopen(sciezkaPliku, "r");

    if(fp == NULL)
    {
            perror("Blad podczas otwierania pliku.\n");
            exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    dlugoscPliku = ftell(fp);
    rewind(fp);

    bufor = malloc((dlugoscPliku + 1) * sizeof(*bufor));
    fread(bufor, dlugoscPliku, 1, fp);

    fclose(fp);

    printf("Podaj wzorzec: ");
    scanf("%s", wzorzec);

    dlugoscWzorca = strlen(wzorzec);

    printf("\nDlugosc pliku: %zu\n", dlugoscPliku);
    printf("Dlugosc wzorca: %i\n\n", dlugoscWzorca);

    n = dlugoscPliku;
    m = dlugoscWzorca;

    printf("Indeksy wystapien wzorca w tekscie\n");
    i = 0;

    while (i<=n-m)
    {
            j=0;
            while ((j<m)&&(wzorzec[j]==bufor[i+j])) j++;
            if (j==m) printf("%d\n", i+1);
            i++;
    }

    getchar();
}
