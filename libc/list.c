#include "list.h"

extern Heap g_kernelHeap;

list* create_list();
node* create_node(void* data);
node* insert_head(list* list, void* data);
node* insert_tail(list* list, void* data);
node* pop_head(list* list);
node* pop_tail(list* list);
void delete_node(list* list, node* node);
void delete_node_at_pos(list* list, int pos);

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
    node* new_node = (node*)heap_malloc(&g_kernelHeap, sizeof(node));
    
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
        list = create_list();
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
        return  NULL;
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
        list = create_list();
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
        return NULL;
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

node* pop_head(list* list)
{
    node* head_node;

    // if list not null
    if(list)
    {
        head_node = list->head;
        
        // if only one node
        if(list->size == 1)
        {
            list->head = NULL;
            list->tail = NULL;
        }

        // more than one node
        else
        {
            list->head = head_node->next;
        }

        // resizing the list size
        list->size--;
    }

    return head_node;
}

node* pop_tail(list* list)
{
    node* tail_node;

    // if list not null
    if(list)
    {
        tail_node = list->tail;
        
        // if only one node
        if(list->size == 1)
        {
            list->head = NULL;
            list->tail = NULL;
        }

        // more than one node
        else
        {
            list->tail = tail_node->prev;
        }

        // resizing the list size
        list->size--;
    }

    return tail_node;
}

void delete_node(list* list, node* node)
{
    // invalid values
    if(list == NULL || node == NULL)
    {
        return;
    }   

    // if node isn't head
    if(list->head != node)
    {
        node->prev->next = node->next;
    }

    // if node isn't tail 
    if(list->tail != node)
    {
        node->next->prev = node->prev;
    }


    // if deleted node is the head
    if(list->head == node)
    {
        list->head = node->next;
    }

    // if deleted node is the tail
    if(list->tail == node)
    {
        list->tail = node->prev;
    }

    // deleting node
    memset(node, 0, sizeof(node));
    heap_free(&g_kernelHeap, node);
    node = NULL;

}

void delete_node_at_pos(list* list, int pos)
{
    int i =0;
    // invalid arguments
    if(pos >= list->size || list == NULL || pos <= 0)
    {
        return;
    }
    node* curr_node = list->head;

    // oing through the nodes
    while(i < pos && curr_node != NULL)
    {
        curr_node = curr_node->next;
        i++;
    }

    // deleting the curr node
    if(curr_node != NULL)
    {
        delete_node(list, curr_node);
    }
}