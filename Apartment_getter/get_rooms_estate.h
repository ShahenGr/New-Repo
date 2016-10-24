#ifndef __GET_EST_
#define __GET_EST_

#include <setjmp.h>
#include <sqlite3.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "gumbo.h"
#include "sqldb.h"
#include <stdlib.h>
#include <string.h>

jmp_buf env_buffer_est;

void get_content_est(const GumboNode*, const char*, int);
size_t take_len_est(const char*, size_t);
char* get_data_est(char*, int*);
char* correct_str_est(char*);

#endif
