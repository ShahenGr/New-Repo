#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <assert.h>
#include <time.h>
#include "gumbo.h"

struct string
{
    char *ptr;
    size_t len;
};
const char* head = "<!DOCTYPE html>\n<html>\n<head>\n\t<meta charset=\"UTF-8\">\n\t<title>rooms</title>\n</head>\n<body>\n";
const char* foot = "</body>\n</html>\n";
char day[64];

void init_string(struct string*);
size_t writefunc(void*, size_t, size_t, struct string*);
void get_string(struct string*, const char*);
void get_content(const GumboNode*, const char*, FILE*);
void today(void);
size_t take_len(const char*, size_t);
void get_data(char*, FILE*);
char* correct_str(char*);
 


int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "<usage: <%s> <room numbers>\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct string s;
    FILE* fptr;
    int i;
    GumboOutput* output;
    char buf[50];
    int rnum;
    char* end;

    rnum = strtol(argv[1], &end, 10);
    rnum = (rnum <= 0 && rnum >= 9) ? 1 : rnum;
    fptr = fopen("result.html", "w");
    if(!fptr)
    {
        perror("File open:");
        return EXIT_FAILURE;
    }
    today();
    fprintf(fptr, "%s\n", head);
    for(i = 0; i < 100; ++i)
    {
        init_string(&s);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "https://www.list.am/en/category/60/%d?rooms=%d", i + 1, rnum);
        get_string(&s, buf);
        output = gumbo_parse_with_options( &kGumboDefaultOptions, s.ptr, s.len );
        get_content(output->root, "td", fptr);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }

    fprintf(fptr, "%s\n", foot);
    fclose(fptr);
    puts("");

    free(s.ptr);
    return 0;
}



void init_string(struct string *s)
{
    s->len = 0;
    s->ptr = (char*)malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size * nmemb;
    s->ptr = (char*)realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

void get_string(struct string* get_str, const char* str)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, str);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, get_str);
        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
    }

    return;
}

void get_content(const GumboNode* root, const char* tag_name, FILE* file)
{
    GumboTag tag;
    if(!tag_name || GUMBO_TAG_UNKNOWN == (tag = gumbo_tag_enum(tag_name)) )
    {
        return;
    }

    int i;
    size_t strlen1, strlen2, strlen3;
    size_t print_size;
    const GumboVector* root_children = &root->v.element.children;
    for (i = 0; i < root_children->length; ++i)
    {
        GumboNode* child = root_children->data[i];
        if(child->v.element.tag == tag && child->v.element.start_pos.line > 0 && child->v.element.start_pos.line < 10000 )
        {
            strlen1 = strlen(child->v.element.original_tag.data);
            if(child->v.element.original_end_tag.length != 0)
            {
                strlen2 = strlen(child->v.element.original_end_tag.data);
                strlen3 = strlen(tag_name);
                print_size = strlen1 - strlen2 + strlen3 + 3;
            }
            else
            {
                strlen2 = take_len(child->v.element.original_tag.data, strlen1);
                print_size = strlen1 - strlen2;
            }
            char pace[print_size];
            strncpy(pace, child->v.element.original_tag.data, print_size);
            get_data(pace, file);
        }
        if(child->type == GUMBO_NODE_ELEMENT)
        {
            get_content(child, tag_name, file);
        }
    }



    return;
}


void today(void)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(day, sizeof(day), "%A, %B %d, %Y", tm);
    strcat(day, "</td>"); 
    return;
}

size_t take_len(const char* str, size_t len)
{
    int count = 0;
    while(*str++ != '\n')
    {
        count++;
    }

    return len - count;
}

void get_data(char* date, FILE* file)
{
    static int f = 0;
    char* dt = strstr(date, "headerd");
    if(dt)
    {
        dt = strstr(dt, ">");
        dt++;
        if(!strcmp(dt, day))
        {
            if(!f)
            {   
                fprintf(file, "<h2>%s</h2><br><br>\n", day);
            }
            f = 1; 
        }
        else
        {
            fprintf(file, "<h2>End of today's list</h2><br><br>\n");
            exit(EXIT_SUCCESS);
        }
    }

    if(f)
    {
        char *beg = strstr(date, "<a");
        if(beg)
        {
            char *end = strstr(beg, "</a>");
            if(end)
            {
                int sz = end - beg + 4;
                char cortags[sz + 50];
                sprintf(cortags,"%.*s", sz, beg);
                char* crted = correct_str(cortags);
                if(crted)
                {
                    fprintf(file, "%s<br><br>\n", crted);
                    free(crted);
                }
            }
        }
    }

    return;
}

char* correct_str(char* attr)
{
    if(strstr(attr, "<img"))
    {
        return NULL;
    }
    char* beg = strstr(attr, "=\"/");
    char* end = strstr(attr, "\">");
    if(beg && end)
    {
        int sz = end - beg - 2;
        char* str = (char*)malloc(strlen(attr) + 20);
        beg += 2;
        if(str)
        {
            sprintf(str, "<a href=\"https://www.list.am%.*s%s", sz, beg, end);
            return str;
        }
    }
    
    return NULL;
}
