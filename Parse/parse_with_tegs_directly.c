#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <assert.h> 
#include "gumbo.h"


struct string 
{
    char *ptr;
    size_t len;
};
static int gl_flag = 0;

void init_string(struct string*);
size_t writefunc(void*, size_t, size_t, struct string*);
void get_string(struct string*, const char*);
void get_content(const GumboNode*, const char*);
void take_text(GumboNode*);
size_t take_len(const char*, size_t);


int main(int argc, char* argv[])
{

    struct string s;
    init_string(&s);
    get_string(&s, "http://www.estate.am/en");
    
    GumboOutput* output = gumbo_parse_with_options( &kGumboDefaultOptions, s.ptr, s.len );
    
    get_content(output->root, "tbody");    
    gumbo_destroy_output(&kGumboDefaultOptions, output);
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

void take_text(GumboNode* node)
{
    int i;
    const GumboVector* root_children = &node->v.element.children;
    for (i = 0; i < root_children->length; ++i)
    {
        GumboNode* child = root_children->data[i];
        {
            if(child->type == GUMBO_NODE_TEXT)
            {
                if(!strcmp(child->v.text.text, "Today"))
                {
                    puts("\n**************************************************************************\n");
                }
                printf("%s\t", child->v.text.text);
            }
        }
        if(child->type == GUMBO_NODE_ELEMENT)
        {
            take_text(child);
        }
    }
}

void get_content(const GumboNode* root, const char* tag_name)
{
    GumboTag tag;
    if(gl_flag != 0)
        return;
    if(!tag_name || GUMBO_TAG_UNKNOWN == (tag = gumbo_tag_enum(tag_name)) )
    {
        return;
    }

    int i;
    const GumboVector* root_children = &root->v.element.children;
    for (i = 0; i < root_children->length; ++i)
    {
        GumboNode* child = root_children->data[i];
        if(child->v.element.tag == tag && child->v.element.start_pos.line > 0 && child->v.element.start_pos.line < 10000 )
        {
            take_text(child);

            gl_flag++;
        }
        if(child->type == GUMBO_NODE_ELEMENT)
        {
            get_content(child, tag_name);
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

