#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

// a node in DLL
typedef struct dll_node
{
  int             data;
  struct dll_node *prev, *next;
} DLL_Node;


// DLL with only head
// Using a struct allows head to be changed by passing address of DLL,
// instead of passing address of head (double pointer),
//    which might be more confusing
// This also allows more fields to be added later if desired, e.g. tail and length
typedef struct dll
{
  DLL_Node   *head;
} DLL;


void dll_insert(DLL *list,  int key);
void dll_delete(DLL *list,  int key);
DLL_Node* dll_find(DLL *list, int key);
void dll_print(DLL *list);

#endif




