#ifndef LIST_H
#define LIST_H
#include "../kernel/memory/heap.h"
#include <stdint.h>
#include "memory.h"
#include "../kernel/IDT/time.h"


typedef struct node
{
    struct node* next;
    struct node* prev;
    void* data;
}node;


typedef struct list
{
    node* head;
    node* tail;
    uint32_t size;
}list;

/*
    This function creates a list
    @retruns a pointer to the new list
*/
list* create_list();

/*
    This function creates a node
    @param data the new node data
    @retruns the function retruns the new node
*/
node* create_node(void* data);

/*
    This function creates a new node and insets it at the head
    @param list: the list the new node will be added to
    @param data: the data of the new node
    @retruns a pointer to the new node 
*/
node* insert_head(list* list, void* data);

/*
    This function creates a new node and insets it at the tail
    @param list: the list the new node will be added to
    @param data: the data of the new node
    @retruns a pointer to the new node 
*/
node* insert_tail(list* list, void* data);

/*
    This function picks the node from the head and returns it
    @param list: the doubly linked list
    this function returns the node from the head
*/
node* pop_head(list* list);

/*
    This function picks the node from the tail and returns it 
    @param list: the doubly linked list
    this function returns the node from the tail
*/
node* pop_tail(list* list);

/*
    This function deletes a node from list
    @param list: the list that the node will be deleted from
    @param node: the node that willbe deleted
*/
void delete_node(list* list, node* node);

/*
    This function deletes the node at pos (from the head)
    @param pos: the deleted node pos
    @param list: the list that the node will be deleted from
*/
void delete_node_at_pos(list* list, int pos);

/*
    This function deletes the node by data
    @param data: the data
    @param list: the list that the node will be deleted from
*/
void delete_node_by_data(list* list, void* data);

#endif