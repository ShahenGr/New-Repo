#include "get_web.h"
#include "get_rooms.h"
#include "get_rooms_estate.h"
#include <pthread.h>

#define webs 2
#define SQLITE_THREADSAFE 1

extern jmp_buf env_buffer;
extern jmp_buf env_buffer_est;
extern char day[];


void* estate(void*);
void* list(void*);

int main(int argc, char* argv[])
{
    today();
    printf("%s\n", day);
    
    pthread_t thread[webs];

    pthread_create(&thread[0], NULL, &estate, NULL);
    pthread_create(&thread[1], NULL, &list, NULL);

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);


    return 0;
}

void* list(void* par)
{
    struct string s;
    int i;
    GumboOutput* output;
    char buf[50];
    int rnum;
    int val;

    rnum = 0;
    int day_count = 0;
    val = setjmp( env_buffer );
    if(rnum == 7)
    {
        rnum = 0;
    }
    rnum++;
    if(val != 0)
    {
        free(s.ptr);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        day_count++;
        sleep(1);
    }
    if(day_count == 560)
    {
        today();
        printf("\n%s\n", day);
        day_count = 0;
    }
    init_string(&s);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "https://www.list.am/en/category/60/1?_a4=%d", rnum);
    get_string(&s, buf);
    output = gumbo_parse_with_options( &kGumboDefaultOptions, s.ptr, s.len );
    get_content(output->root, "td", rnum);

    return NULL;
}

void* estate(void* par)
{
    struct string s;
    int i;
    GumboOutput* output;
    char buf[50];
    int rnum;
    int val;
    rnum = 0;
    int arr[4] = {66624, 96815, 45616, 43933};
    val = setjmp( env_buffer_est );
    if(rnum == 4)
    {
        rnum = 0;
    }
    rnum++;
    if(val != 0)
    {
        free(s.ptr);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        sleep(4);
    }
    init_string(&s);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "http://www.estate.am/en/%d-rooms-apartments-for-sale-s%d", rnum, arr[rnum - 1]);
    get_string(&s, buf);
    output = gumbo_parse_with_options( &kGumboDefaultOptions, s.ptr, s.len );
    get_content_est(output->root, "div", rnum);
    
    return NULL;
}
