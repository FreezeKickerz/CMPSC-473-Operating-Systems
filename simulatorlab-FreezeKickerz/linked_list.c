#include <stdlib.h>
#include "linked_list.h"

// Creates and returns a new list
// If compare is NULL, list_insert just inserts at the head
list_t* list_create(compare_fn compare)
{
    /* IMPLEMENT THIS */
    list_t* list = malloc(sizeof *list);
    if (!list)
    {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    list->compare = compare;
    return list;
}

// Destroys a list
void list_destroy(list_t* list)
{
    /* IMPLEMENT THIS */
    if (list == NULL)
    {
        return;
    }
    list_node_t* cur = list->head;
    while (cur)
    {
        list_node_t* nxt = cur->next;
        free(cur);
        cur = nxt;
    }
    free(list);
}

// Returns head of the list
list_node_t* list_head(list_t* list)
{
    /* IMPLEMENT THIS */
    return list ? list->head : NULL;
}

// Returns tail of the list
list_node_t* list_tail(list_t* list)
{
    /* IMPLEMENT THIS */
    return list ? list->tail : NULL;
}

// Returns next element in the list
list_node_t* list_next(list_node_t* node)
{
    /* IMPLEMENT THIS */
    return node ? node->next : NULL;
}

// Returns prev element in the list
list_node_t* list_prev(list_node_t* node)
{
    /* IMPLEMENT THIS */
    return node ? node->prev : NULL;
}

// Returns end of the list marker
list_node_t* list_end(list_t* list)
{
    /* IMPLEMENT THIS */
    return list ? list->tail : NULL;
}

// Returns data in the given list node
void* list_data(list_node_t* node)
{
    /* IMPLEMENT THIS */
    return node ? node->data : NULL;
}

// Returns the number of elements in the list
size_t list_count(list_t* list)
{
    /* IMPLEMENT THIS */
    if (list == NULL)
    {
        return 0;
    }
    return list->count;
}

// Finds the first node in the list with the given data
// Returns NULL if data could not be found
list_node_t* list_find(list_t* list, void* data)
{
    /* IMPLEMENT THIS */
    if (list == NULL)
    {
        return NULL;
    }
    for (list_node_t* cur = list->head; cur; cur = cur->next)
    {
        if (cur->data == data)
        {
            return cur;
        }
    }
    return NULL;
}

// Inserts a new node in the list with the given data
// Returns new node inserted
list_node_t* list_insert(list_t* list, void* data)
{
    /* IMPLEMENT THIS */
    if (list == NULL) {
        return NULL;
    }

    // allocate and initialize the new node
    list_node_t* new_node = malloc(sizeof *new_node);
    if (new_node == NULL) {
        return NULL;
    }
    new_node->data = data;
    new_node->next = new_node->prev = NULL;

    // if no comparator or empty list, insert at head 
    if (list->compare == NULL || list->head == NULL) {
        new_node->next = list->head;
        if (list->head) {
            list->head->prev = new_node;
        } else {
            // list was empty, so tail must also point here 
            list->tail = new_node;
        }
        list->head = new_node;
    }
    else {
        // find the first node whose data is >= the new data 
        list_node_t* curr = list->head;
        while (curr && list->compare(curr->data, data) < 0) {
            curr = curr->next;
        }

        if (curr == NULL) {
            // reached end append to tail 
            new_node->prev = list->tail;
            list->tail->next = new_node;
            list->tail = new_node;
        }
        else {
            // insert before curr
            new_node->next = curr;
            new_node->prev = curr->prev;
            if (curr->prev) {
                curr->prev->next = new_node;
            } else {
                // curr was head 
                list->head = new_node;
            }
            curr->prev = new_node;
        }
    }

    list->count++;
    return new_node;
}   

// Removes a node from the list and frees the node resources
void list_remove(list_t* list, list_node_t* node)
{
    /* IMPLEMENT THIS */
    if (!list || !node)
    {
        return;
    }

    if (node == list->head)
    {
        list->head = node->next;
    }
    if (node == list->tail)
    {
        list->tail = node->prev;
    }
    if (node->prev)
    {
        node->prev->next = node->next;
    }
    if (node->next)
    {
        node->next->prev = node->prev;
    }

    list->count--;
    node->next = NULL;
    node->prev = NULL;
    free(node);
}
