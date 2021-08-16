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

int compare_nodes(struct Node* n1, struct Node* n2, char* sort_option)
{
    int result;

    if (strcmp(sort_option, SORT_OPT_SIZE) == 0)
    {
        result = n1->stat->st_size > n2->stat->st_size;
    }
    else if (strcmp(sort_option, SORT_OPT_ATIME) == 0)
    {
        result = n1->stat->st_atim.tv_nsec < n2->stat->st_atim.tv_nsec;
    }
    else if (strcmp(sort_option, SORT_OPT_CTIME) == 0)
    {
        result = n1->stat->st_ctim.tv_nsec < n2->stat->st_ctim.tv_nsec;
    }
    else if (strcmp(sort_option, SORT_OPT_MTIME) == 0)
    {
        result = n1->stat->st_mtim.tv_nsec < n2->stat->st_mtim.tv_nsec;
    }
    else if (strcmp(sort_option, SORT_OPT_NAME) == 0)
    {
        result = strcmp(n1->name, n2->name);
    }
    else
    {
        return -1;
    }

    return result;
}

void sort_list(struct Node* head, char* sort_option)
{   
    int swap;
    struct Node* pCurrent;
    struct Node* pLeft = NULL;

    // do not sort empty list
    if (!head)
    {
        return;
    }

    do
    {
        pCurrent = head;
        swap = 0;

        while (pCurrent->next != pLeft)
        {
            int compare_result = compare_nodes(pCurrent, pCurrent->next, sort_option);
            if (compare_result > 0)
            {
                
                // swap nodes

                char* name = pCurrent->name;
                struct stat* stat = pCurrent->stat;

                pCurrent->name = (char *)malloc(sizeof(char) * strlen(pCurrent->next->name));
                strcpy(pCurrent->name, pCurrent->next->name);
                pCurrent->stat = pCurrent->next->stat;

                pCurrent->next->name = (char *)malloc(sizeof(char) * strlen(name));
                strcpy(pCurrent->next->name, name);
                pCurrent->next->stat = stat;

                swap++;
            }
            pCurrent = pCurrent->next;
        }
        pLeft = pCurrent;    
    }
    while (swap != 0);
    
}

void reverse_list(struct Node** head)
{
    struct Node* pLast = NULL;
    struct Node* pCurrent = *head;
    struct Node* pNext = NULL;

    while (pCurrent != NULL)
    {
        pNext = pCurrent->next;
        pCurrent->next = pLast;
        pLast = pCurrent;
        pCurrent = pNext;
    }
    *head = pLast;
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

    // sort linked list
    sort_list(pHead, argv[2]);

    // reverse list if descending order
    if (strcmp(argv[3], SORT_DIR_DESC) == 0)
    {
        reverse_list(&pHead);
    }

    // print entries
    for (pCurrent = pHead; pCurrent != NULL; pCurrent = pCurrent->next)
    {
        printf("%s %ld %ld %ld %ld\n",
                pCurrent->name,
                pCurrent->stat->st_size,
                pCurrent->stat->st_atim.tv_nsec,
                pCurrent->stat->st_ctim.tv_nsec,
                pCurrent->stat->st_mtim.tv_nsec);
    }

    // close directory and quit
    closedir(pDir);
    return 0;
}