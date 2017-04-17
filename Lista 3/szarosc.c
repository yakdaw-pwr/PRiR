/* Zamiana plików bmp do skali szarosci */
/* www.algorytm.org */
/* 2012 (c) adaptacja do gcc LS */

#include <stdio.h>
#include <stdlib.h>

/* File Header */
typedef struct BMPFILEHEADER {
        short type;
        int fileSize;
        short reserved0;
        short reserved1;
        int dataOffset;
} BMPFILEHEADER;
 
/* Info Header */
typedef struct BMPINFOHEADER {
        int hdrSize;
        int width;
        int height;
        short planes;
        short depth;
        int compression;
        int bmpDataSize;
        int hResolution;
        int vResolution;
        int nColors;
        int nImportantColors;
} BMPINFOHEADER;

typedef struct RGB {
      unsigned char R;
      unsigned char G;
      unsigned char B;
     } Pixel24;

int main(int argc, char *argv[], char *envp[])
{
    FILE *plik, *plik2;
    BMPFILEHEADER FileInfo;
    BMPINFOHEADER PictureInfo;
    int i, j;
    char szary;
    short padding;
    //char** grayPicture;
    Pixel24** pixmap;
    
    if(plik = fopen("lena2.bmp","rb"))
    {       
          //wczytanie nag³ówków BMP
          fread(&FileInfo.type,2,1,plik);
          fread(&FileInfo.fileSize,4,1,plik);
          fread(&FileInfo.reserved0,2,1,plik);
          fread(&FileInfo.reserved1,2,1,plik);
          fread(&FileInfo.dataOffset,4,1,plik);  
          fread(&PictureInfo.hdrSize,4,1,plik); 
          fread(&PictureInfo.width,4,1,plik);
          fread(&PictureInfo.height,4,1,plik);
          fread(&PictureInfo.planes,2,1,plik);
          fread(&PictureInfo.depth,2,1,plik); 
          fread(&PictureInfo.compression,4,1,plik);
          fread(&PictureInfo.bmpDataSize,4,1,plik);
          fread(&PictureInfo.hResolution,4,1,plik);
          fread(&PictureInfo.vResolution,4,1,plik);
          fread(&PictureInfo.nColors,4,1,plik);
          fread(&PictureInfo.nImportantColors,4,1,plik);
           //alokacja pamiêci pod pixmape
           pixmap = malloc(sizeof(Pixel24*) * PictureInfo.height);
           for(i = 0; i < PictureInfo.height; i++)
                 pixmap[i] = (Pixel24*)malloc(sizeof(Pixel24) * PictureInfo.width);
           //obliczenie wyrównania wiersza do wielokrotnoœci 4B
           padding = (PictureInfo.width*3)%4 ?
                                             4-(PictureInfo.width*3)%4
                                             :
                                             0;
           //wczytanie bitmapy do pix mapy :P
           fseek(plik, FileInfo.dataOffset,SEEK_SET);
           for(i = 0; i < PictureInfo.height; i++, fseek(plik, padding, SEEK_CUR))
                 for(j = 0; j < PictureInfo.width; j++)
                 {
                       pixmap[i][j].B = fgetc(plik); 
                       pixmap[i][j].G = fgetc(plik);
                       pixmap[i][j].R = fgetc(plik);
                 }
           //operacje na nowym pliku
           if(plik2 = fopen("nowy3.bmp","wb"))
           {        //kopiowanie pliku
                    fseek(plik, 0, SEEK_END);
                    j = ftell(plik);
                    fseek(plik, 0, SEEK_SET);
                    for(i = 0; i < j; i++)
                          fputc(fgetc(plik), plik2);
                    //zapis bitmapy + konwersja do szaroœci                      
                    fseek(plik2, FileInfo.dataOffset, SEEK_SET);
                    for(i = 0; i < PictureInfo.height; i++, fseek(plik2, padding, SEEK_CUR))
                          for(j = 0; j < PictureInfo.width; j++)
                          {
                                szary = (char)(0.229*pixmap[i][j].R + 0.587*pixmap[i][j].G + 0.114*pixmap[i][j].B);
                                fputc(szary, plik2);
                                fputc(szary, plik2);
                                fputc(szary, plik2);
                          }
                    fclose(plik2);  
           }
           else printf("niepowodzenie tworzenia pliku");
           //zwolnienie pamiêci pixmapy
           for(i = 0; i<PictureInfo.height; i++)
                 free(pixmap[i]);
           free(pixmap);   
           //zamkniecie plików
           fclose(plik);
    }
    else printf("blad odczytu pliku"); 
    //system("pause");
return 0;   
}

