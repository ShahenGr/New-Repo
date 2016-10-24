#include "get_rooms_estate.h"

extern char day[64];


void get_content_est(const GumboNode* root, const char* tag_name, int rnum)
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
                strlen2 = take_len_est(child->v.element.original_tag.data, strlen1);
                print_size = strlen1 - strlen2;
            }
            char pace[print_size];
            strncpy(pace, child->v.element.original_tag.data, print_size);
            char* str[3];
            static int tt = 0;
            static int flag = 0;
            str[tt] = get_data_est(pace, &flag);
            if(str[tt])
            {
                tt++;
            }

            if(tt == 1)
            {
                char* name = correct_str_est(str[0]);
                char* img = correct_str_est(str[0]);
                char room[5];
                sprintf(room, "%d", rnum);
                int res = write_into_db(NULL, name, img, day, room, 2);

                if(res == 0)
                {
                    fprintf(stderr, "\x1b[34m" "**************************************************************************\nestate: A new apartment has written into DB\n" "\x1b[0m");
                }
                else
                {
                    fprintf(stderr, "\x1b[31m" "**************************************************************************\nestate: No apartment written into DB\n" "\x1b[0m");
                }
                free(name);
                free(img);
                tt = 0;
                flag = 0;
                longjmp(env_buffer_est, 1);
            }
                
            }
        }
        if(child->type == GUMBO_NODE_ELEMENT)
        {
            get_content_est(child, tag_name, rnum);
        }
    }



    return;
}

size_t take_len_est(const char* str, size_t len)
{
    int count = 0;
    while(*str++ != '\n')
    {
        count++;
    }

    return len - count;
}

char* get_data_est(char* date, int* flag)
{
   if(*flag == 1)
   {
        return date;               
   }
   if(*flag == 2)
   {
       *flag = 0;
       longjmp(env_buffer_est, 1);
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

char* correct_str_est(char* attr)
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
