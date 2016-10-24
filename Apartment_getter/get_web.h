#ifndef __GET_WEB_
#define __GET_WEB_

#include <curl/curl.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

struct string
{
    char *ptr;
    size_t len;
};

void init_string(struct string*);
size_t writefunc(void*, size_t, size_t, struct string*);
void get_string(struct string*, const char*);

#endif
