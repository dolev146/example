#ifndef MYLIST_H_
#define MYLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

typedef void (*node_function_ptr)(char *, int);

struct nodebefore
{
    char *data;
    struct nodebefore *next;
};
typedef struct nodebefore nodebefore_t;

struct nodedef
{
    char *data;
    nodebefore_t *data_before;
    struct nodedef *next;
    struct nodedef *child;
    int level;
};
typedef struct nodedef node_t;

// insert function
void insert(node_t *insert_node, node_t *parent_node);
// sort all levels
void sort_all_levels();
// print all nodes
void print_all_nodes();

extern node_t *head;

#endif // MYLIST_H