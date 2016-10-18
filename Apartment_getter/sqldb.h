#ifndef __SQLDB_H_
#define __SQLDB_H_

#define __USE_XOPEN 500

void today(void);

static int callback(void *, int, char **, char **);
void print_db(sqlite3*, const char*, char*);
int write_into_db(sqlite3*, int*, char*, const char*, const char*, const char*, const char*, int);
int delete_from_db(sqlite3*, sqlite3_stmt*, const char*, char*);
struct tm* take_day(char*);
int get_day(char*);
char* get_name(int);

#endif //__SQLDB_H_
