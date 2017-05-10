#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

/*
Opracowanie na podstawie: The GNU C Library Reference Manual, 
rozdzial 32, Edition 0.12, last updated 2007-10-27
*/

int main(void)
{
    unsigned long seed[2];
    char salt[] = "a8";
    const char *const seedchars =
    "./0123456789ABCDEFGHIJKLMNOPQRST"
    "UVWXYZabcdefghijklmnopqrstuvwxyz";
    char vpass[] = "a";
    char vhash[34];
    char *vhash0;
    char *pass;
    char *hash;
    int i, ok;

    /* Generate a (not very) random seed.
    You should do it better than this... */
    seed[0] = time(NULL);
    seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);
    
    /* Turn it into printable characters from 'seedchars'. */
    for (i = 0; i < 8; i++)
        salt[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];

    /* encrypt the valid password */
    vhash0=crypt(vpass, salt);
    strcpy (vhash, vhash0);
    
    /* Read in the user's password and encrypt it,
       passing the expected password in as the salt. */
    pass=getpass("\nEnter password: ");
    hash = crypt(pass, vhash); 
 
    /* Print the strings */
    printf("\nThe password given: '%15s'. The hash: '%s'\n", pass,   hash);
    printf(  "The valid password: '%15s'. The hash: '%s'\n", vpass, vhash);
    /* Test the hash. */
    ok = strcmp (hash, vhash) == 0;
    puts(ok ? "Access granted.\n" : "Access denied.\n");
    return ok ? 0 : 1;
}
