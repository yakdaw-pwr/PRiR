#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

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
     
int main(int argc, char** argv) {

    FILE *inputFile, *outputFile;
    
    BMPFILEHEADER FileInfo;
    BMPINFOHEADER PictureInfo;
    
    int i, j;
    char** szary;
    short padding;
    Pixel24** pixmap;
    
    double timeStart;
    double timeFinish;
    double completeTime;
    
    char *fileName = (char*) malloc(128 * sizeof(char));
    char *filePath = (char*) malloc(132 * sizeof(char));
    
    printf("Insert path to BMP file: ");
    scanf("%s", fileName);
    strcpy(filePath, fileName);
    strcat(filePath, ".bmp");
    
    if(inputFile = fopen(filePath, "rb"))
    {       
        // Read BMP headers
        fread(&FileInfo.type,2,1,inputFile);
        fread(&FileInfo.fileSize,4,1,inputFile);
        fread(&FileInfo.reserved0,2,1,inputFile);
        fread(&FileInfo.reserved1,2,1,inputFile);
        fread(&FileInfo.dataOffset,4,1,inputFile);  
        fread(&PictureInfo.hdrSize,4,1,inputFile); 
        fread(&PictureInfo.width,4,1,inputFile);
        fread(&PictureInfo.height,4,1,inputFile);
        fread(&PictureInfo.planes,2,1,inputFile);
        fread(&PictureInfo.depth,2,1,inputFile); 
        fread(&PictureInfo.compression,4,1,inputFile);
        fread(&PictureInfo.bmpDataSize,4,1,inputFile);
        fread(&PictureInfo.hResolution,4,1,inputFile);
        fread(&PictureInfo.vResolution,4,1,inputFile);
        fread(&PictureInfo.nColors,4,1,inputFile);
        fread(&PictureInfo.nImportantColors,4,1,inputFile);
          
        // Memory allocation for PixMap
        pixmap = malloc(sizeof(Pixel24*) * PictureInfo.height);
        for(i = 0; i < PictureInfo.height; i++)
            pixmap[i] = (Pixel24*)malloc(sizeof(Pixel24) * PictureInfo.width);

        // Calculate row align to 4B multiplicity
        padding = (PictureInfo.width*3)%4 ?
                                          4-(PictureInfo.width*3)%4
                                          :
                                          0;
           
        char *outputName = (char*) malloc(133 * sizeof(char));
        strcpy(outputName, fileName);
        strcat(outputName, "M.bmp");
            
        // Operations on new file
        if(outputFile = fopen(outputName, "wb"))
        {        
            // Copying file
            fseek(inputFile, 0, SEEK_END);
            j = ftell(inputFile);
            fseek(inputFile, 0, SEEK_SET);
            for(i = 0; i < j; i++)
                fputc(fgetc(inputFile), outputFile);
         }
        else printf("Can't create file");

        fseek(inputFile, FileInfo.dataOffset,SEEK_SET);
        fseek(outputFile, FileInfo.dataOffset, SEEK_SET);

        szary = malloc(sizeof(char*) * PictureInfo.height);
        for(i = 0; i < PictureInfo.height; i++)
            szary[i] = (char*)malloc(sizeof(char) * PictureInfo.width);
        
        // Start timer
        timeStart = clock() / (CLOCKS_PER_SEC / 1000000);
        
        #pragma omp parallel num_threads(4)
        {
            #pragma omp single
            {
                for(i = 0; i < PictureInfo.height; i++) {
                    for(j = 0; j < PictureInfo.width; j++) {           
                        {
                            pixmap[i][j].B = fgetc(inputFile); 
                            pixmap[i][j].G = fgetc(inputFile);
                            pixmap[i][j].R = fgetc(inputFile);
                        }   
                    }
                    fseek(inputFile, padding, SEEK_CUR);
                }
            }

            #pragma omp for private(i, j) schedule(dynamic, 3)
            for(i = 0; i < PictureInfo.height; i++) {
                for(j = 0; j < PictureInfo.width; j++) {  
                    szary[i][j] = (char)(0.229*pixmap[i][j].R + 0.587*pixmap[i][j].G + 0.114*pixmap[i][j].B);
                }
            }
            
            #pragma omp single
            {
                // Saving bitmap + monochromatic conversion          
                for(i = 0; i < PictureInfo.height; i++) {         
                    for(j = 0; j < PictureInfo.width; j++) {
                        fputc(szary[i][j], outputFile);
                        fputc(szary[i][j], outputFile);
                        fputc(szary[i][j], outputFile);
                    }    
                    fseek(outputFile, padding, SEEK_CUR);       
                }
            } 
        }

        // Stop timer
        timeFinish  = clock() / (CLOCKS_PER_SEC / 1000000);
        completeTime = (timeFinish - timeStart) / 1000000;

        printf("Complete time: %.6lf\n", completeTime);

        // Free pixmap memory
        for(i = 0; i<PictureInfo.height; i++)
            free(pixmap[i]);
        free(pixmap);

        // Close files
        fclose(inputFile);
        fclose(outputFile);
    }
    else printf("Error reading file"); 
    
    return (EXIT_SUCCESS);
}

