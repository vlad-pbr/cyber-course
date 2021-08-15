#include <sys/stat.h>

struct Node
{
    char* name;
    struct stat* stat;
    struct Node* next;
};