#include "list.h"


list* create_list();
node* create_node(void* data);
node* insert_head(list* list, void* data);
node* insert_tail(list* list, void* data);

list* create_list()
{
    list* new_list = (list*)heap_malloc(&g_kernelHeap, sizeof(list));
    new_list->size = 0;
    new_list->tail = NULL;
    new_list->head = NULL;
    return new_list;
}

node* create_node(void* data)
{
    node* new_node = (node*)heap_malloc(&g_kernelHeap, sizeod(node));
    
    // assigning data
    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = NULL;
    return new_node;
}

node* insert_head(list* list, void* data)
{   
    //if list is NUll
    if(list == NULL)
    {
        list = (list*)create_list();
        // if failed to alloc new list
        if(list == NULL)
        {
            return NULL;
        }
    }

    node* new_node = create_node(data);
    // if no more kernel heap space
    if(new_node == NULL)
    {
        retrun NULL;
    }

    // making nodes pointing to each other
    new_node->next = list->head;
    list->head->prev = new_node;

    //  if empry list
    if(list->size == NULL)
    {
        list->tail = new_node;
    }

    list->head = new_node;
    
    // inc list size
    list->size = list->size + 1;

    return new_node;    
}

node* insert_tail(list* list, void* data)
{
    //if list is NUll
    if(list == NULL)
    {
        list = (list*)create_list();
        // if failed to alloc new list
        if(list == NULL)
        {
            return NULL;
        }
    }

    node* new_node = create_node(data);
    // if no more kernel heap space
    if(new_node == NULL)
    {
        retrun NULL;
    }

    // making nodes pointing to each other
    new_node->prev = list->tail;
    list->tail->next = new_node;

    //  if empty list
    if(list->size == NULL)
    {
        list->head = new_node;
    }

    list->tail = new_node;
    
    // inc list size
    list->size = list->size + 1;

    return new_node;    
}