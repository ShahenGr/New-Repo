#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "gumbo.h"


void find_tags(const GumboNode*, const char*);
void read_file(FILE* fp, char**, int* );
size_t take_len(const char*, size_t);

int main(int argc, const char** argv) 
{
    if (argc != 3) 
    {
        fprintf(stderr,"Usage: %s <html filename> <tagename>.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char* filename = argv[1];

    FILE* fp = fopen(filename, "r");
    if (!fp) 
    {
        printf("File %s not found!\n", filename);
        exit(EXIT_FAILURE);
    }

    char* input;
    int input_length;
    read_file(fp, &input, &input_length);
    GumboOutput* output = gumbo_parse_with_options( &kGumboDefaultOptions, input, input_length );
    find_tags(output->root, argv[2]);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    free(input);

    return EXIT_SUCCESS;
}

void read_file(FILE* fp, char** output, int* length) 
{
    struct stat filestats;
    int fd = fileno(fp);
    fstat(fd, &filestats);
    *length = filestats.st_size;
    *output = malloc(*length + 1);
    if(!*output)
    {
        perror("Error: malloc");
        exit(EXIT_FAILURE);
    }
    int start = 0;
    int bytes_read;
    while ((bytes_read = fread(*output + start, 1, *length - start, fp))) 
    {
        start += bytes_read;
    }
}

void find_tags(const GumboNode* root, const char* tag_name)
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
                printf("\n******************************************************************************\n %.*s\n", (int)print_size, child->v.element.original_tag.data);
        }
        if(child->type == GUMBO_NODE_ELEMENT)
        {
            find_tags(child, tag_name);
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
