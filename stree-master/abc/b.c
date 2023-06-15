#define _XOPEN_SOURCE 600
#include <ftw.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

char *get_perms(mode_t mode)
{
    static char perms[11];
    perms[0] = S_ISDIR(mode) ? 'd' : '-';
    perms[1] = mode & S_IRUSR ? 'r' : '-';
    perms[2] = mode & S_IWUSR ? 'w' : '-';
    perms[3] = mode & S_IXUSR ? 'x' : '-';
    perms[4] = mode & S_IRGRP ? 'r' : '-';
    perms[5] = mode & S_IWGRP ? 'w' : '-';
    perms[6] = mode & S_IXGRP ? 'x' : '-';
    perms[7] = mode & S_IROTH ? 'r' : '-';
    perms[8] = mode & S_IWOTH ? 'w' : '-';
    perms[9] = mode & S_IXOTH ? 'x' : '-';
    perms[10] = '\0';
    return perms;
}

// Linked list node
typedef struct Node
{
    char *data;
    struct Node *next;
} Node;

Node *head = NULL;
// Create a new node
Node *newNode(char *data)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = malloc(strlen(data) + 1);
    strcpy(node->data, data);
    node->next = NULL;
    return node;
}

// Insert a new Node at the sorted position
Node *sortedInsert(Node *head, char *data)
{
    // Special case for head node
    if (head == NULL || strcmp(data, head->data) <= 0)
    {
        Node *temp = newNode(data);
        temp->next = head;
        return temp;
    }

    // Locate the node before point of insertion
    Node *current = head;
    while (current->next != NULL && strcmp(current->next->data, data) < 0)
    {
        current = current->next;
    }

    Node *temp = newNode(data);
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
        printf("%s -> \n", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");
}

static int
dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    if (type == FTW_NS)
    {
        printf("?");
        return 0;
    }
    else
    {
        switch (sbuf->st_mode & S_IFMT)
        {
        case S_IFREG:
            printf("-");
            break;
        case S_IFDIR:
            printf("d");
            break;
        case S_IFCHR:
            printf("c");
            break;
        case S_IFBLK:
            printf("b");
            break;
        case S_IFLNK:
            printf("l");
            break;
        case S_IFIFO:
            printf("p");
            break;
        case S_IFSOCK:
            printf("s");
            break;
        default:
            printf("?");
            break;
        }
    }

    if (type != FTW_NS)
    {
        printf("%7ld ", (long)sbuf->st_ino);
    }
    else
    {
        printf("        ");
    }

    printf(" %*s", 4 * ftwb->level, " ");
    printf("%s\n", &pathname[ftwb->base]);

    head = sortedInsert(head, (char *)&pathname[ftwb->base]);

    return 0;
}

int main(int argc, char *argv[])
{
    int flags = 0;
    if (argc == 1)
    {
        if (nftw(".", dirTree, 10, flags) == -1)
        {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        printList(head);
        exit(EXIT_SUCCESS);
    }

    if (nftw(argv[1], dirTree, 10, flags) == -1)
    {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}