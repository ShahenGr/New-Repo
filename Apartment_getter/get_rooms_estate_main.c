#include "get_rooms.h"

extern jmp_buf env_buffer;
extern char day[];

int main(int argc, char* argv[])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    struct string s;
    int i;
    GumboOutput* output;
    char buf[50];
    int rnum;
    char* end;
    int val;

    rc = sqlite3_open("rooms.db", &db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }
    today();
    rnum = 0;
    int day_count = 0;
    printf("%s\n", day);
    int arr[4] = {66624, 96815, 45616, 43933};
    val = setjmp( env_buffer );
    if(rnum == 4)
    {
        rnum = 0;
    }
    rnum++;
    if(val != 0)
    {
        free(s.ptr);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        day_count++;
        sleep(4);
    }
    if(day_count == 560)
    {
        today;
        printf("\n%s\n", day);
        day_count = 0;
    }
    init_string(&s);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "http://www.estate.am/en/%d-rooms-apartments-for-sale-s%d", rnum, arr[rnum - 1]);
    get_string(&s, buf);
    output = gumbo_parse_with_options( &kGumboDefaultOptions, s.ptr, s.len );
    get_content(output->root, "div", db, zErrMsg, rnum);


    return 0;
}
