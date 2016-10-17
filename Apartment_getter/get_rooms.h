#include <assert.h>
#include <curl/curl.h>
#include <setjmp.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "gumbo.h"
#include "sqldb.h"

struct string
{
    char *ptr;
    size_t len;
};
char day[64];
jmp_buf env_buffer;

void init_string(struct string*);
size_t writefunc(void*, size_t, size_t, struct string*);
void get_string(struct string*, const char*);
void get_content(const GumboNode*, const char*, sqlite3*, char*, int);
size_t take_len(const char*, size_t);
char* get_data(char*, int*);
char* correct_str(char*);


