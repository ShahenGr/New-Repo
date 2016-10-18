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
            GumboAttribute* href;
            href = gumbo_get_attribute(&child->v.element.attributes, "class");
            if(href && ( !strcmp(href->value, "misc") || !strcmp(href->value, "left") ))
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

                if(tt == 1)
                {
                    char* name = correct_str(str[0]);
                    char* img = correct_str(str[0]);
                    char room[5];
                    sprintf(room, "%d", rnum);
                    write_into_db(db, NULL, zErrMsg, name, img, day, room, 2);
                    fprintf(stderr, "**************************************************************************\nA new apartment has written into DB\n");
                    free(name);
                    free(img);
                    tt = 0;
                    flag = 0;
                    longjmp(env_buffer, 1);
                }

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
    if(*flag == 1)
    {
        return date;               
    }
    if(*flag == 2)
    {
        *flag = 0;
        longjmp(env_buffer, 1);
    }
    char* td;
    td = strstr(date, "\"date\">");
    if(td)
    {
        if(strstr(td, "Today"))
        {
            *flag = 1;
            return NULL;
        }
        else
        {
            *flag = 2;
            return NULL;
        }
    }

    return NULL;
}

char* correct_str(char* attr)
{
    char* href;
    char* alt;
    static int f = 1;

    href = strstr(attr, "href=");
    alt = strstr(attr, "alt=");
    if(href && alt)
    {
        href += 6;
        alt += 5;
        char* end;
        int len;
        char* str = (char*)malloc(1000 * sizeof(char));
        if(!str)
        {
            perror("Malloc");
            exit(EXIT_FAILURE);
        }
        if(f)
        {
            end = strstr(href, ">");
            len = end - href;
            sprintf(str, "<a href=\"http://www.estate.am%.*s>", len, href);
            end = strstr(alt, "/>");
            len = end - alt;
            sprintf(str, "%s%.*s</a>", str, len, alt);
            f = 0;
            return str;
        }

        char* img = strstr(attr, "<img");
        end = strstr(img, "alt");
        len = end - img;
        memset(str, 0, 1000);
        sprintf(str, "%.*s>", len, img);
        f = 1;

        return str;

    }

    return NULL;
}
