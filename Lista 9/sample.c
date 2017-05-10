#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>


int main(int* argc, char** argv)
{
    char salt[] = "a8";
    char* vhash;

    /* encrypt the valid password */
    vhash=crypt(argv[1], salt);
 
    /* Print the strings */
    printf("\nThe hash: '%s'\n", vhash);

return 0;
}
