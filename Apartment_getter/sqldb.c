#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "sqldb.h"

char day[64];


static int callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    for(i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


int take_val(void *data, int argc, char **argv, char **azColName)
{
    int i;
    char* str = (char*)data;
    for(i = 0; i < argc; i++)
    {
        if(!strcmp(azColName[i], "NAME"))
        {
            strcpy(str,argv[i]);
        }
    }
    return 0;
}

void print_db(sqlite3* db, const char* data, char* zErrMsg)
{
    int rc;
    char* sql = "SELECT * from ROOMS";

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL print error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Operation done successfully\n");

    }

    return;

}

int write_into_db(int* data, const char* name, const char* link, const char* time, const char* room, int web)
{
    int rc;
    char str[100];
    char* q = "SELECT NAME FROM ROOMS";
    char* s = "INSERT INTO ROOMS (NAME,IMG,DATE,ROOM_NUMBER, WEBS) SELECT ";
    char* s1 = "WHERE NOT EXISTS(SELECT 1 FROM ROOMS WHERE NAME = ";
    int sz = strlen(s) + strlen(s1) + 2 * strlen(name) + strlen(link) + strlen(time) + strlen(room) + 22;
    char sql[sz];
    char* zErrMsg;
    sqlite3* db;
    rc = sqlite3_open("rooms.db", &db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    sqlite3_mutex* sql_mut;
    sql_mut = sqlite3_db_mutex(db);
    if(sql_mut == NULL)
    {
        fprintf(stderr, "MUTEX ERROR IN DB\n");
    }
    int f = sqlite3_mutex_try(sql_mut);
    while(f == SQLITE_BUSY )
    {
        sqlite3_sleep(5);
        f = sqlite3_mutex_try(sql_mut);
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
    sprintf(sql, "%s '%s', '%s', '%s', '%s', %d %s '%s');", s, name, link, time, room, web, s1, name);
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    while(rc == SQLITE_BUSY )
    {
        sqlite3_sleep(5);
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    }

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL write error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_mutex_leave(sql_mut);
        return 1;
    }
    sqlite3_mutex_leave(sql_mut);
    sqlite3_close(db);

    return 0;
}

int delete_from_db(sqlite3* db, sqlite3_stmt* stmt, const char* data, char* zErrMsg)
{
    int rc;
    char* sql = "DELETE FROM ROOMS WHERE DATE LIKE ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL delete error: %s\n", zErrMsg);
        return 1;
    }
    char* week = strtok(day, " ");
    int wday = get_day(week);
    week = get_name(wday);

    sqlite3_bind_text(stmt, 1, week, -1, 0);
    int step;

    do{
        step = sqlite3_step(stmt);
        switch(step)
        {
            case SQLITE_DONE:
                break;

            case SQLITE_ROW:
                {
                    printf("deleted %s: ", sqlite3_column_text(stmt, 0));
                }

            default :
                break;
        }
    }while(step == SQLITE_ROW);
    sqlite3_finalize(stmt);

    printf("\nThe DB is cleaned succesfuly\n");
    return 0;
}

int get_day(char* date)
{
    if(!strcmp(date, "Monday,"))
    {
        return 0;
    }
    if(!strcmp(date, "Tuesday,"))
    {
        return 1;
    }
    if(!strcmp(date, "Wednesday,"))
    {
        return 2;
    }
    if(!strcmp(date, "Thursday,"))
    {
        return 3;
    }
    if(!strcmp(date, "Friday,"))
    {
        return 4;
    }
    if(!strcmp(date, "Saturday,"))
    {
        return 5;
    }
    if(!strcmp(date, "Sunday,"))
    {
        return 6;
    }

    return -1;
}

char* get_name(int week)
{
    int wday = ( week + 2 ) % 7;
    switch(wday)
    {
        case 0:
            return "Monday%";
        case 1:
            return "Tuesday%";
        case 2:
            return "Wednesday%";
        case 3:
            return "Thursday%";
        case 4:
            return "Friday%";
        case 5:
            return "Saturday%";
        case 6:
            return "Sunday%";
        default:
            return NULL;
    }
}

void today(void)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    memset(day, 0, sizeof(day));
    strftime(day, sizeof(day), "%A, %B %d, %Y", tm);
    return;
}
