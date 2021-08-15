#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "list.h"

#define NUM_SORT_OPTS 5
#define NUM_SORT_DIRS 2

#define SORT_OPT_SIZE "size"
#define SORT_OPT_ATIME "atime"
#define SORT_OPT_CTIME "ctime"
#define SORT_OPT_MTIME "mtime"
#define SORT_OPT_NAME "name"

#define SORT_DIR_ASC "asc"
#define SORT_DIR_DESC "desc"

char* string_array_to_string(char* array[], int size, char delimiter)
{
    char* output = (char*)malloc(0);

    for (int i = 0; i < size; i++)
    {
        output = (char*)realloc(output, sizeof(char) * (strlen(output) + strlen(array[i]) + 1));
        strcat(output, array[i]);
        strcat(output, &delimiter);
    }

    output[strlen(output) - 1] = '\0';

    return output;
}

int contains(char* array[], int size, char* str)
{
    for (int i = 0; i < size; i++)
    {
        if(strcmp(array[i], str) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char *sort_options[NUM_SORT_OPTS] = { SORT_OPT_SIZE, SORT_OPT_ATIME, SORT_OPT_CTIME, SORT_OPT_MTIME, SORT_OPT_NAME };
    char *sort_directions[NUM_SORT_DIRS] = { SORT_DIR_ASC, SORT_DIR_DESC };

    // ensure params are specified
    if (argc < 4 ||
        !contains(sort_options, NUM_SORT_OPTS, argv[2]) ||
        !contains(sort_directions, NUM_SORT_DIRS, argv[3]) )
    {
        printf("usage: %s <path> <%s> <%s>\n",
                argv[0],
                string_array_to_string(sort_options, NUM_SORT_OPTS, '|'),
                string_array_to_string(sort_directions, NUM_SORT_DIRS, '|'));
        return 1;
    }

    // open specified directory
    DIR *pDir = opendir(argv[1]);
    if (pDir == NULL)
    {
        printf("error while opening '%s'\n", argv[1]);
        return 1;
    }

    // iterate directory entries
    struct dirent *pEntry = readdir(pDir);
    struct Node *pHead = (struct Node*)malloc(sizeof(struct Node));
    struct Node *pCurrent = pHead;

    // ensure path ends with slash
    char slash_char = '/';
    int slash = argv[1][strlen(argv[1]) - 1] == slash_char;
    char* root_path = (char *)malloc(sizeof(char) * (strlen(argv[1]) + slash));
    strcpy(root_path, argv[1]);
    if (!slash)
    {
        strcat(root_path, &slash_char);
    }

    // create linked list out of entries
    while((pEntry = readdir(pDir)) != NULL)
    {
        // generate file path
        char* file_path = (char *)malloc(sizeof(char) * (strlen(root_path) + strlen(pEntry->d_name)));
        strcpy(file_path, root_path);
        strcat(file_path, pEntry->d_name);

        // allocate memory for struct
        pCurrent->next = (struct Node*)malloc(sizeof(struct Node));
        pCurrent = pCurrent->next;

        // copy file data
        pCurrent->name = (char*)malloc(sizeof(char) * strlen(pEntry->d_name));
        strcpy(pCurrent->name, pEntry->d_name);
        pCurrent->stat = (struct stat*)malloc(sizeof(struct stat));
        stat(file_path, pCurrent->stat);
    }
    pHead = pHead->next;
    
    // print entries
    pCurrent = pHead;
    while(pCurrent != NULL)
    {
        printf("%s %ld\n",pCurrent->name, pCurrent->stat->st_size);
        pCurrent = pCurrent->next;
    }

    // close directory and quit
    closedir(pDir);
    return 0;
}