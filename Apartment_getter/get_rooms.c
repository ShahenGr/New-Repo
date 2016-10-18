#include "get_rooms.h"

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

void get_content(const GumboNode* root, const char* tag_name, sqlite3* db, char* zErrMsg, int rnum)
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
            char* str[3];
            static int tt = 0;
            static int flag = 0;
            str[tt] = get_data(pace, &flag);
            if(str[tt])
            {
                tt++;
            }

                
            if(tt == 2)
            {
                if(strstr(str[0], "<img"))
                {
                    char room[5];
                    sprintf(room, "%d", rnum);
                    write_into_db(db, NULL, zErrMsg, str[1], str[0], day, room, 1);
                    fprintf(stderr, "**************************************************************************\nA new apartment has written into DB\n");
                }
                else
                {
                    char room[5];
                    sprintf(room, "%d", rnum);
                    write_into_db(db, NULL, zErrMsg, str[0], "NULL", day, room, 1);
                    fprintf(stderr, "**************************************************************************\nA new apartment has written into DB\n");
                }
                tt = 0;
                flag = 0;
                longjmp(env_buffer, 1);
            }
        }
        if(child->type == GUMBO_NODE_ELEMENT)
        {
            get_content(child, tag_name, db, zErrMsg, rnum);
        }
    }



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

char* get_data(char* date, int* flag)
{
    static int f = 0;
    if(*flag == 0)
    {
        f = 0;
    }
    char* dt = strstr(date, "headerd");
    if(dt)
    {
        dt = strstr(dt, ">");
        dt++;
        char* edt = strstr(dt, "<");
        int sz = edt - dt;
        dt[sz] = '\0';
        if(!strcmp(dt, day))
        {
            f = 1;
            *flag = 1;
        }
        else
        {
            printf("End of today's list\n");
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
                    return crted;
                }
            }
        }
    }

    return NULL;
}

char* correct_str(char* attr)
{
    char* beg = NULL;
    char* end;
    if(( beg = strstr(attr, "<img")) != NULL)
    {
        beg = strstr(beg, ".net");
        end = strstr(beg, "\">");
        if(beg && end)
        {
            int sz = end - beg - 2;
            char* str = (char*)malloc(strlen(attr) + 20);
            beg += 4;
            if(str)
            {
                sprintf(str, "<img src=\"https://images-listam.netdna-ssl.com%.*s", sz, beg);
                return str;
            }
        }
        return NULL;
    }
    beg = strstr(attr, "=\"/");
    end = strstr(attr, "\">");
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
