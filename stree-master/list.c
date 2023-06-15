#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

node_t *head = NULL;

void insert(node_t *insert_node, node_t *parent_node)
{
    if (parent_node == NULL)
    {
        // insert at head of top-level list
        insert_node->next = head;
        head = insert_node;
    }
    else
    {
        // insert at head of child list
        insert_node->next = parent_node->child;
        parent_node->child = insert_node;
    }
}

void sort_list(node_t **head_ref)
{
    node_t *current = *head_ref, *index = NULL;
    char *temp_data;
    int temp_level;

    if (*head_ref == NULL)
    {
        return;
    }

    while (current != NULL)
    {
        index = current->next;

        while (index != NULL)
        {
            if (strcmp(current->data, index->data) > 0)
            {
                temp_data = current->data;
                current->data = index->data;
                index->data = temp_data;

                temp_level = current->level;
                current->level = index->level;
                index->level = temp_level;
            }
            index = index->next;
        }
        current = current->next;
    }
}

void sort_all_levels()
{
    node_t *current = head;

    while (current != NULL)
    {
        sort_list(&(current->child));
        current = current->next;
    }

    sort_list(&head);
}

void print_list(node_t *head)
{
    node_t *current = head;
    while (current != NULL)
    {
        printf("%s\n", current->data);
        if (current->child != NULL)
        {
            print_list(current->child);
        }
        current = current->next;
    }
}

void print_all_nodes()
{
    print_list(head);
}