/*
  Author: Phil Chan
  Email: pkc@cs.fit.edu
  Description:

  An implementation of doubly linked lists (DLL) with these operations:

  a.  dll_insert(list, key): insert a key to the list
  b.  dll_delete(list, key): delete a key from the list
  c.  dll_find(list, key): find a key and return the node or NULL
  d.  dll_print(list): print the list

  Keys in a DLL are in an ascending order.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doublyLinkedList.h"

//#define DEBUG



// create and return a node with data and previous/next nodes
DLL_Node* dll_create_node(int data, DLL_Node* prev, DLL_Node* next)
{
  DLL_Node *my_node = (DLL_Node *)malloc(sizeof(DLL_Node));

  if (my_node == NULL)
    {
      printf("malloc failed, memory is full\n");
      exit(-1);
    }

  my_node->data = data;
  my_node->prev = prev;
  my_node->next = next;

  return my_node;
}


// insert a key and maintain the ascending order of DLL
// assuming keys are unique
void dll_insert(DLL *list,  int key)
{
  DLL_Node *new_node = NULL;   // node to be inserted
  
  // empty list
  if (list->head == NULL)
    {
      new_node = dll_create_node(key, NULL, NULL);
      list->head = new_node;
    }

  // insert at head, key is smaller than the first node
  else if (key < list->head->data)
    {
      DLL_Node *first_node = list->head;  // first node of the list

      new_node = dll_create_node(key, NULL, first_node);
      list->head = new_node;
      first_node->prev = new_node;
    }

  // insert in the middle or at the end
  else
    {
      DLL_Node *curr_node = list->head; // current node

      while ( (curr_node->next != NULL) && (key > curr_node->next->data) )
	  curr_node = curr_node->next;

      if (curr_node->next != NULL) // in the middle, between curr_node and curr_node->next
	{
	  new_node = dll_create_node(key, curr_node, curr_node->next);
	  curr_node->next->prev = new_node;
	}
      else // at the end, insert after curr_node
	{
	  new_node = dll_create_node(key, curr_node, NULL);
	}
      curr_node->next = new_node;
    }
}

void dll_print(DLL *list)
{
  DLL_Node *curr_node = list->head; // current node

  if (curr_node == NULL)
    printf("empty list\n");
  else // list is not empty
    {
      do
	{
	  printf("%d ", curr_node->data);
	  curr_node = curr_node->next;
	}
      while (curr_node != NULL);
      printf("\n");
    }
}

  
// find key in list, which is in ascending order
// return node if found, NULL otherwise
DLL_Node* dll_find(DLL *list, int key)
{
  DLL_Node *curr_node = list->head; // current node

  while ( (curr_node != NULL) && (key > curr_node->data) ) // empty list is also checked
    curr_node = curr_node->next;

  if ((curr_node != NULL) && (curr_node->data == key)) // found
    return curr_node;
  else // not found
    return NULL;

}

// delete a node with key and maintain the ascending order of DLL
void dll_delete(DLL *list, int key)
{
  DLL_Node *found_node = dll_find(list, key);  // node found by dll_find()

  if (found_node != NULL)
    {
      if ((found_node->next == NULL) && (found_node->prev == NULL)) // only node
	{
	  list->head = NULL;
        }
      else if (found_node->prev == NULL)  // first node
	{
	  list->head = found_node->next;
	  found_node->next->prev = NULL;
        }
      else if (found_node->next == NULL)  // last node
	{
	  found_node->prev->next = NULL;
        }
      else // middle, in between two nodes
	{
	  found_node->next->prev = found_node->prev;
	  found_node->prev->next = found_node->next;
	}

      free(found_node);
    }
  else // key was not found or list is empty
    printf("dll_delete: %d was not found or list is empty\n", key);
}




// --------------  testing --------------------------------
// uncomment #define DEBUG near the top


#ifdef DEBUG

// test dll_find() and print failures
void test_dll_find(DLL *list, int key)
{
  DLL_Node *found_node = dll_find(list, key);

  if (found_node == NULL || (found_node != NULL && found_node->data != key))
    printf("*** fail: %d ***\n", key);
}

// testing the operations of DLL
void test_dll()
{
  DLL mylist;
  mylist.head = NULL;

  dll_delete(&mylist, 100);  dll_print(&mylist);

  dll_insert(&mylist, 40);  dll_print(&mylist);
  dll_insert(&mylist, 20);  dll_print(&mylist);
  dll_insert(&mylist, 60);  dll_print(&mylist);
  dll_insert(&mylist, 50);  dll_print(&mylist);
  dll_insert(&mylist, 70);  dll_print(&mylist);
  dll_insert(&mylist, 30);  dll_print(&mylist);
  dll_insert(&mylist, 10);  dll_print(&mylist);


  test_dll_find(&mylist, 10);
  test_dll_find(&mylist, 50);
  test_dll_find(&mylist, 70);
  if (dll_find(&mylist, 100) != NULL)
    printf("***fail 100***\n");
	      
  dll_delete(&mylist, 5);  dll_print(&mylist);
  dll_delete(&mylist, 55);  dll_print(&mylist);
  dll_delete(&mylist, 105);  dll_print(&mylist);
  
  dll_delete(&mylist, 10);  dll_print(&mylist);
  dll_delete(&mylist, 70);  dll_print(&mylist);
  dll_delete(&mylist, 40);  dll_print(&mylist);
  dll_delete(&mylist, 20);  dll_print(&mylist);
  dll_delete(&mylist, 60);  dll_print(&mylist);
  dll_delete(&mylist, 50);  dll_print(&mylist);
  dll_delete(&mylist, 30);  dll_print(&mylist);
  
  dll_delete(&mylist, 100);  dll_print(&mylist);
}


int main(int argc, char* argv[])
{
  test_dll();
}
#endif



