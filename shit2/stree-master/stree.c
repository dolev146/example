/* nftw_dir_tree.c
   Demonstrate the use of nftw(3). Walk though the directory tree specified
   on the command line (or the current working directory if no directory
   is specified on the command line), displaying an indented hierarchy
   of files in the tree. For each file, display:
      * a letter indicating the file type (using the same letters as "ls -l")
      * a string indicating the file type, as supplied by nftw()
      * the file's i-node number.
*/
#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <sys/types.h> /* Type definitions used by many programs */
#include <stdio.h>     /* Standard I/O functions */
#include <stdlib.h>    /* Prototypes of commonly used library functions,
                             plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>    /* Prototypes for many system calls */
#include <errno.h>     /* Declares errno and defines error constants */
#include <string.h>    /* Commonly used string-handling functions */
#include <sys/stat.h>
#include <pwd.h>
#include <strings.h>
#include <grp.h>
char *gid_to_name(gid_t uid);
char modestr[11];
char *uid_to_name(uid_t uid);
char *gid_to_name(gid_t gid);
void mod_to_letters(int mode, char str[]);
int dir = 0;
int counter = 0;
int files = 0;
int num = 0;
char *strr;

// Linked list node
typedef struct Node
{
    char *data;
    char *extra_info;
    int level; // Level of the node
    struct Node *next;
} Node;

Node *head = NULL;

// Create a new node
Node *newNode(char *data, int level, char *extra_info)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = malloc(strlen(data) + 1);
    node->level = level;
    node->extra_info = malloc(strlen(extra_info) + 1);
    strcpy(node->extra_info, extra_info);
    strcpy(node->data, data);
    node->next = NULL;
    return node;
}

// Insert a new Node at the sorted position
Node *sortedInsert(Node *head, char *data, int level, char *extra_info)
{
    // Special case for head node
    if (head == NULL || strcasecmp(data, head->data) <= 0)
    {
        Node *temp = newNode(data, level, extra_info);

        temp->next = head;
        return temp;
    }

    // Locate the node before point of insertion
    Node *current = head;
    while (current->next != NULL && strcasecmp(current->next->data, data) < 0)
    {
        current = current->next;
    }
    Node *temp = newNode(data, level, extra_info);
    temp->next = current->next;
    current->next = temp;
    return head;
}

// Print the linked list
void printList(Node *head)
{
    Node *temp = head;
    while (temp != NULL)
    {
        // printf("%s", temp->extra_info);
        printf("%s\n", temp->data);
        temp = temp->next;
    }
}

static int /* Callback function called by ftw() */
dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{

    struct stat info;

    if (type != FTW_NS && stat(pathname, &info) != -1)
    {
        if (num > 1)
        {
            mod_to_letters(info.st_mode, modestr);

            if (strstr(strr, "/") != NULL && counter == 0 && modestr[0] == 'd')
            { /// handle the path/path argument
                counter++;
                printf("%s\n", strr);
                return 0;
            }
            if (modestr[0] != 'd' && counter == 0)
            {
                printf("%s [error opening dir]\n", strr);
                counter++;
                return 0;
            }
        }
        if ((strcmp(pathname + (ftwb->base), ".") == 0) || (strcmp(pathname + (ftwb->base), pathname) == 0))
        {
            printf("%s\n", &pathname[ftwb->base]);
            counter++;

            return 0;
        }

        char extra_info[1024];
        bzero(extra_info, 1024);

        if (ftwb->level == 1)
        {
            counter++;
            strcat(extra_info, "├──");
        }

        else
        {
            counter++;
            // insert to extra info
            strcat(extra_info, "│   ");
            int i = 0;
            while (i < ftwb->level - 1)
            {
                strcat(extra_info, "│   ");
                i++;
            }
            strcat(extra_info, "└──");
        }

        mod_to_letters(info.st_mode, modestr);
        printf("[%s", modestr);
        // insert to extra info
        strcat(extra_info, "[");
        strcat(extra_info, modestr);
        printf(" %s   %s               %ld]   ", uid_to_name(info.st_uid), gid_to_name(info.st_uid), info.st_size);
        strcat(extra_info, " ");
        strcat(extra_info, uid_to_name(info.st_uid));
        strcat(extra_info, " ");
        strcat(extra_info, gid_to_name(info.st_uid));
        strcat(extra_info, " ");
        char size[1024];
        sprintf(size, "%ld", info.st_size);
        strcat(extra_info, size);
        strcat(extra_info, "]   ");
        head = sortedInsert(head, (char *)&pathname[ftwb->base], ftwb->level, extra_info);
        bzero(extra_info, 1024);
        
        if (modestr[0] == 'd')
        {
            dir++;
        }
        else
        {
            files++;
        }
    }
    else
    {
        printf("---");
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int flags = 0;
    char *path = ".";
    if (argc == 1)
    {
        num = argc;
        if (nftw(path, dirTree, 10, flags) == -1)
        {
            fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        num = argc;
        for (int i = 1; i < argc; i++)
        {
            strr = argv[i];
            if (nftw(argv[i], dirTree, 10, flags) == -1)
            {
                printf("%s [error opening dir]\n", strr);
            }
        }
    }
    if (argc)
    {

        printList(head);
        printf("\n%d directories, %d files\n", dir, files);
        exit(EXIT_SUCCESS);
    }
}

void mod_to_letters(int mode, char str[])
{
    strcpy(str, "----------");
    if (S_ISDIR(mode))
        str[0] = 'd';
    if (S_ISCHR(mode))
        str[0] = 'c';
    if (S_ISBLK(mode))
        str[0] = 'b';
    if (mode & S_IRUSR)
        str[1] = 'r';
    if (mode & S_IWUSR)
        str[2] = 'w';
    if (mode & S_IXUSR)
        str[3] = 'x';

    if (mode & S_IRGRP)
        str[4] = 'r';
    if (mode & S_IWGRP)
        str[5] = 'w';
    if (mode & S_IXGRP)
        str[6] = 'x';

    if (mode & S_IROTH)
        str[7] = 'r';
    if (mode & S_IWOTH)
        str[8] = 'w';
    if (mode & S_IXOTH)
        str[9] = 'x';
}
char *uid_to_name(uid_t uid)
{
    return getpwuid(uid)->pw_name;
}
char *gid_to_name(gid_t gid)
{
    struct group *getgrgid(), *grp_ptr;
    static char numstr[10];
    if ((grp_ptr = getgrgid(gid)) == NULL)
    {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    else
    {
        return grp_ptr->gr_name;
    }
}
