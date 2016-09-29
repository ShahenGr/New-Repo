#include <stdio.h>
#include <string.h>
#include "sha256.h"

#define LEN 256/8

int main(int argc, char* argv[])
{
    if(argc < 2 || argc > 3)
    {
        fprintf(stderr, "usage: %s <string>\n", argv[0]);
        return 1;
    }

    unsigned char* str = (unsigned char*)argv[1];
    char code[2 * LEN + 1]; 
    int i;
    int is = (argc == 3) ? 1 : 0;
    if(is)
    {
        strncpy(code, argv[2], 2 * LEN + 1);
    }
    SHA256_CTX sha2;
    sha256_init(&sha2);
    sha256_update(&sha2, str, strlen((const char*)str));
    sha256_final(&sha2, str);

    char buf[2 * LEN + 1];
    buf[2 * LEN] = '\0';
    for(i = 0; i < LEN; i++)
    {
        sprintf(buf + i * 2, "%02x", str[i]);
    }
    
    if(!is)
    {
        printf("%s\n", buf);
    }
    else
    {
        if(!strcmp(buf, code))
        {
            printf("It's a true hash code\n");
        }
        else
        {
            printf("It's a wrong hash code\n");
        }
    }


    return 0;
}

