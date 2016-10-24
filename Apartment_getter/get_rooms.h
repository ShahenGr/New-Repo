#ifndef __GET_ROOM_
#define __GET_ROOM_

#include <setjmp.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "gumbo.h"
#include "sqldb.h"
#include <string.h>

jmp_buf env_buffer;

void get_content(const GumboNode*, const char*,int);
size_t take_len(const char*, size_t);
char* get_data(char*, int*);
char* correct_str(char*);

#endif
