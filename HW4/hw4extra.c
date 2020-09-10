/*

  Author: Ian Orzel
  Email: iorzel2019@my.fit.edu
  Course: CSE2010
  Section: 23

  Description of this file:
  This allows users to input buy and sell orders for products and executes these orders
  based on whether they can be. The highest buy order and lowest sell order are also
  able to be displayed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

#define SIZE 241
#define DEFUNCT 1

/*
  Enum that stores whether the current order is a buy or a sell order
 */
typedef enum {
     SELL = -1, BUY = 1
} order_type_t;

/*
  A struct that stores the information related to an order
 */
typedef struct {
     char *name;
     int time;
     int price;
     int quantity;
     order_type_t order_type;
     int index;
} node_t;

/*
  A struct for storing the heap data structure
 */
typedef struct {
     node_t **nodes;
     int size;
     int max_size;
} heap_t;

int get_parent(int index);
int get_left(int index);
int get_right(int index);
int compare_nodes(heap_t *heap, int node_a, int node_b);
node_t *add_to_heap(heap_t *heap, char *name, int time, int price, int quantity, order_type_t order_type);
void upheap(heap_t *heap, int index);
node_t *remove_from_heap(heap_t *heap);
void downheap(heap_t *heap, int index);
void execute_buy_sell_orders(heap_t *buy_heap, heap_t *sell_heap);
int hash(char *string);
void add_to_table(node_t **table, node_t *node);
node_t *get_from_table(node_t **table, char *name);
void remove_from_table(node_t **table, char *name);
void update_heap(heap_t *heap, int index);

/*
  At the top of each function, describe the function, parameter(s), and return value (if any)
 */

int main(int argc, char* argv[]) {
     FILE *in = fopen(argv[1], "r");                              /* Stores the file where the input is being read from */
     heap_t *buy_orders = (heap_t *) malloc(sizeof(heap_t));      /* The heap for storing buy orders */
     heap_t *sell_orders = (heap_t *) malloc(sizeof(heap_t));     /* The heap for storing sell orders */
     node_t **table = (node_t **) malloc(SIZE * sizeof(node_t *));/* Hash table for storing the nodes in the heap */

     /*
       Setting some of the default values for the two heaps
      */
     buy_orders->max_size = 50;
     sell_orders->max_size = 50;
     buy_orders->size = 0;
     sell_orders->size = 0;
     buy_orders->nodes = (node_t **) malloc(sizeof(node_t *) * buy_orders->max_size);
     sell_orders->nodes = (node_t **) malloc(sizeof(node_t *) * sell_orders->max_size);

     /*
       Loops through input file until there is no more input
      */
     while (!feof(in)) {
          char *command = (char *) malloc(sizeof(char) * 50);    /* Stores the string for the next given command */
          fscanf(in, "%s", command);
          if (strcmp(command, "EnterBuyOrder") == 0) {
               /*
                 Reading in the variables for the buy order
                */
               int time;                                         /* Stores the time of the transaction */
               char *buyer = (char *) malloc(sizeof(char) * 50); /* Stores the buyer's name in the transaction */
               double price_double;                              /* Stores the double form of the price */
               int quantity;                                     /* Stores the quantity of the items that the buyer wants */
               fscanf(in, "%d%s%lf%d", &time, buyer, &price_double, &quantity);
               int price = (int)(price_double * 100 + 0.5);            /* Stores the price of the product as an integer */

               /*
                 Reprints the command, adds the buy order to the heap, and then checks if transactions can occur
                */
               printf("%s %06d %s %.2f %d\n", command, time, buyer, price_double, quantity);
               node_t *new_node = add_to_heap(buy_orders, buyer, time, price, quantity, BUY);
               add_to_table(table, new_node);
               execute_buy_sell_orders(buy_orders, sell_orders);
          } else if (strcmp(command, "EnterSellOrder") == 0) {
               /*
                 Reading in the variables for the sell order
                */
               int time;                                              /*Stores the time of the transaction */
               char *seller = (char *) malloc(sizeof(char) * 50);     /* Stores the name of the seller */
               double price_double;                                   /* Stores the price the seller wants as a double */
               int quantity;                                          /* Stores the number of items the seller has to sell */
               fscanf(in, "%d%s%lf%d", &time, seller, &price_double, &quantity);
               int price = (int)(price_double * 100 + 0.5);                 /* Stores the price of the product as an integer */

               /*
                 Reprints the command, adds the sell order to the heap, and checks if a new transaction can occur
                */
               printf("%s %06d %s %.2f %d\n", command, time, seller, price_double, quantity);
               node_t *new_node = add_to_heap(sell_orders, seller, time, price, quantity, SELL);
               add_to_table(table, new_node);
               execute_buy_sell_orders(buy_orders, sell_orders);
          } else if (strcmp(command, "DisplayHighestBuyOrder") == 0) {
               /*
                 Reading in the variables with the command and printing the results
                */
               int time; /* Stores the time of the request */
               fscanf(in, "%d", &time);
               printf("%s %06d", command, time);
               if (buy_orders->size != 0) {
                    printf(" %s %06d %.2f %d\n", buy_orders->nodes[0]->name, buy_orders->nodes[0]->time, buy_orders->nodes[0]->price * buy_orders->nodes[0]->order_type / 100.0, buy_orders->nodes[0]->quantity);
               } else {
                    printf("\n");
               }
          } else if (strcmp(command, "DisplayLowestSellOrder") == 0) {
               /*
                 Reading in the variables with the command and printing the results
                */
               int time; /* Stores the time of the request */
               fscanf(in, "%d", &time);
               printf("%s %06d", command, time);
               if (sell_orders->size != 0) {
                    printf(" %s %06d %.2f %d\n", sell_orders->nodes[0]->name, sell_orders->nodes[0]->time, sell_orders->nodes[0]->price * sell_orders->nodes[0]->order_type / 100.0, sell_orders->nodes[0]->quantity);
               } else {
                    printf("\n");
               }
          } else if (strcmp(command, "ChangeBuyOrder") == 0) {
               /*
                 Reading in variables for the command
                */
               int time;                                         /* Time of the request */
               char *name = (char *) malloc(sizeof(char) * 100); /* Name of the buyer */
               double price_double;                              /* Price of the product in double form */
               int quantity;                                     /* Number of items willing to buy */
               fscanf(in, "%d%s%lf%d", &time, name, &price_double, &quantity);
               int price = (int)(price_double * 100 + 0.5);            /* Stores the integer value of the price */

               /*
                 Finds the buyer in the heap and then updates their information
                */
               node_t *node = get_from_table(table, name);       /* Stores the node associated with the buyer */
               if (node != NULL) {
                    node->time = time;
                    node->price = price * node->order_type;
                    node->quantity = quantity;
                    update_heap(buy_orders, node->index);
               }

               printf("%s %d %s %.2f %d %s\n", command, time, name, price_double, quantity, (node == NULL) ? "noBuyerError" : "");
               execute_buy_sell_orders(buy_orders, sell_orders);
          } else if (strcmp(command, "ChangeSellOrder") == 0) {
               /*
                 Reading in variables for the command
                */
                int time;                                         /* Time of the request */
                char *name = (char *) malloc(sizeof(char) * 100); /* Name of the buyer */
                double price_double;                              /* Price of the product in double form */
                int quantity;                                     /* Number of items willing to buy */
                fscanf(in, "%d%s%lf%d", &time, name, &price_double, &quantity);
                int price = (int)(price_double * 100 + 0.5);            /* Stores the integer value of the price */

                /*
                  Finds the seller and updates their information
                 */
                node_t *node = get_from_table(table, name);
                if (node != NULL) {
                     node->time = time;
                     node->price = price * node->order_type;
                     node->quantity = quantity;
                     update_heap(sell_orders, node->index);
                }

                printf("%s %d %s %.2f %d %s\n", command, time, name, price_double, quantity, (node == NULL) ? "noSellerError" : "");
                execute_buy_sell_orders(buy_orders, sell_orders);
          } else if (strcmp(command, "CancelBuyOrder") == 0) {
               /*
                 Reading in the variables with the command
                */
               int time;                                         /* Time of the cancellation */
               char *name = (char *) malloc(sizeof(char) * 100); /* Name of the buyer */
               fscanf(in, "%d%s", &time, name);

               /*
                 Finds node from table, removes it from heap, and updates the heap
                */
               node_t *node = get_from_table(table, name);
               if (node != NULL) {
                    buy_orders->nodes[node->index] = buy_orders->nodes[--buy_orders->size];
                    buy_orders->nodes[node->index]->index = node->index;
                    update_heap(buy_orders, node->index);
                    free(node);
               }

               printf("%s %d %s %s\n", command, time, name, (node == NULL) ? "noBuyerError" : "");
          } else if (strcmp(command, "CancelSellOrder") == 0) {
               /*
                 Reading in the variables with the command
                */
               int time;                                         /* Time of the cancellation */
               char *name = (char *) malloc(sizeof(char) * 100); /* Name of the buyer */
               fscanf(in, "%d%s", &time, name);

               /*
                 Finds node from sellers, removes it, and updates the heap
                */
               node_t *node = get_from_table(table, name);
               if (node != NULL) {
                    sell_orders->nodes[node->index] = sell_orders->nodes[--sell_orders->size];
                    sell_orders->nodes[node->index]->index = node->index;
                    update_heap(sell_orders, node->index);
                    free(node);
               }
               printf("%s %d %s %s\n", command, time, name, (node == NULL) ? "noSellerError" : "");
          }
          free(command);
     }

     /*
       Freeing everything in the heap
      */
     while (buy_orders->size > 0) {
          node_t *tmp = remove_from_heap(buy_orders);
          free(tmp->name);
          free(tmp);
     }
     while (sell_orders->size > 0) {
          node_t *tmp = remove_from_heap(sell_orders);
          free(tmp->name);
          free(tmp);
     }
     free(buy_orders->nodes);
     free(sell_orders->nodes);
     free(buy_orders);
     free(sell_orders);

     fclose(in);
}

/*
  Returns the parent index given the index of the current node
 */
int get_parent(int index) {
     return (index % 2 == 0) ? (index - 2) / 2 : (index - 1) / 2;
}

/*
  Returns the index of the left node given a node
 */
int get_left(int index) {
     return 2 * index + 1;
}

/*
  Returns the index of the right node given a node
 */
int get_right(int index) {
     return 2 * index + 2;
}

/*
  Compares two nodes in a heap and returns a number based on that comparison
 */
int compare_nodes(heap_t *heap, int node_a, int node_b) {
     if (heap->nodes[node_a]->price == heap->nodes[node_b]->price) {
          return heap->nodes[node_a]->time - heap->nodes[node_b]->time;
     }
     return (heap->nodes[node_b]->price) - (heap->nodes[node_a]->price);
}

/*
  Adds a new node to the last node of the heap and then runs upheap on it
 */
node_t *add_to_heap(heap_t *heap, char *name, int time, int price, int quantity, order_type_t order_type) {
     /*
       Creates a new node for the heap
      */
     node_t * new_node = (node_t *) malloc(sizeof(node_t));
     new_node->name = name;
     new_node->time = time;
     new_node->price = price * order_type;
     new_node->quantity = quantity;
     new_node->order_type = order_type;

     /*
       Reallocates the heap array if the heap is not big enough to account for the number of elements
      */
     if (heap->size >= heap->max_size) {
          heap->max_size *= 2;
          heap->nodes = realloc(heap->nodes, sizeof(node_t *) * heap->max_size);
     }

     /*
       Places the new node in the heap and then runs upheap
      */
     new_node->index = heap->size;
     heap->nodes[heap->size++] = new_node;
     upheap(heap, heap->size - 1);
     return new_node;
}

/*
  Checks whether the node needs to be swapped with its parent through the heap in order to keep it a heap
 */
void upheap(heap_t *heap, int index) {
     /*
       Loop that stops when the root has been reached
      */
     while (index != 0) {
          int parent = get_parent(index);
          if (compare_nodes(heap, index, parent) < 0) {
               /*
                 Swaps nodes if new node is larger than its parent
                */
               node_t *tmp = heap->nodes[index];
               heap->nodes[index] = heap->nodes[parent];
               heap->nodes[parent] = tmp;
               heap->nodes[index]->index = index;
               heap->nodes[parent]->index = parent;
          } else {
               break;
          }
     }
}

/*
  Removes the top node of the heap and returns this node
 */
node_t *remove_from_heap(heap_t *heap) {
     node_t *top = heap->nodes[0];     /* Stores the current top node of the heap to be returned at the end */

     /*
       Sets the last node to the top and runs downheap
      */
     heap->nodes[0] = heap->nodes[--heap->size];
     heap->nodes[0]->index = 0;
     downheap(heap, 0);
     return top;
}

/*
  Pushes a node downward through heap from top while ensuring that it remains a heap
 */
void downheap(heap_t *heap, int index) {
     /*
       Loops until the bottom of the heap has been reached
      */
     while (index < heap->size) {
          int left = (get_left(index) < heap->size) ? heap->nodes[get_left(index)]->price : INT_MIN;     /* Stores the price of the left-most node */
          int right = (get_right(index) < heap->size) ? heap->nodes[get_right(index)]->price : INT_MIN;  /* Stores the price of the right-most node */

          if (heap->nodes[index]->price > left && heap->nodes[index]->price > right) {
               /* The heap must be a heap */
               break;
          } else if (left > right) {
               /*
                 Swaps the left node with the root if it is bigger than the right node
                */
               node_t *tmp = heap->nodes[get_left(index)];
               heap->nodes[get_left(index)] = heap->nodes[index];
               heap->nodes[index] = tmp;
               heap->nodes[get_left(index)]->index = get_left(index);
               heap->nodes[index]->index = index;
               index = get_left(index);
          } else {
               /*
                 Swaps the right node with the root if it is bigger than the left node
                */
               node_t *tmp = heap->nodes[get_right(index)];
               heap->nodes[get_right(index)] = heap->nodes[index];
               heap->nodes[index] = tmp;
               heap->nodes[get_left(index)]->index = get_left(index);
               heap->nodes[index]->index = index;
               index = get_right(index);
          }
     }
}

/*
  Checks whether any purchases can occur and then does them if possible
 */
void execute_buy_sell_orders(heap_t *buy_heap, heap_t *sell_heap) {
     /*
       Loops until no sell order is less than or equal to a buy order
      */
     while (buy_heap->size != 0 && sell_heap->size != 0 && (sell_heap->nodes[0]->price * sell_heap->nodes[0]->order_type) <= (buy_heap->nodes[0]->price * buy_heap->nodes[0]->order_type)) {
          int sell_price = sell_heap->nodes[0]->price * sell_heap->nodes[0]->order_type;       /* Stores the price of the current seller */
          int buy_price = buy_heap->nodes[0]->price * buy_heap->nodes[0]->order_type;          /* Stores the price of the current buyer */
          int price = (sell_price == buy_price) ? sell_price : (sell_price + buy_price) / 2;   /* Stores the price of the transaction */
          int sell_quantity = sell_heap->nodes[0]->quantity;                                   /* Stores the number of items the seller can sell */
          int buy_quantity = buy_heap->nodes[0]->quantity;                                     /* Stores the number of items the buyer can buy */
          int quantity = sell_quantity < buy_quantity ? sell_quantity : buy_quantity;          /* Stores the quantity for this transaction */

          /*
            Printing information related to the pending transaction
           */
          printf("ExecuteBuySellOrders %.2f %d\n", price / 100.0, quantity);
          printf("Buyer: %s %d\n", buy_heap->nodes[0]->name, buy_quantity - quantity);
          printf("Seller: %s %d\n", sell_heap->nodes[0]->name, sell_quantity - quantity);

          /*
            Updating quantities and removing proper items from their heap
           */
          buy_heap->nodes[0]->quantity -= quantity;
          sell_heap->nodes[0]->quantity -= quantity;
          if (buy_heap->nodes[0]->quantity == 0) {
               free(remove_from_heap(buy_heap));
          }
          if (sell_heap->nodes[0]->quantity == 0) {
               free(remove_from_heap(sell_heap));
          }
     }
}

/*
  Returns a smaller version of the time integer that is unique for every time
 */
int hash(char *string) {
     int num = 0;
     for (char *c = string; *c != '\0'; c++) {
          num += *c - 'A';
     }
     return num;
}

/*
  Adds a new node to the hash table
 */
void add_to_table(node_t **table, node_t *node) {
     int index = hash(node->name) % SIZE;
     while (table[index] != NULL) {
          index = (index + 1) % SIZE;
     }
     table[index] = node;
}

/*
  Gets the node stored in the hash table
 */
node_t *get_from_table(node_t **table, char *name) {
     int index = hash(name) % SIZE;
     while (table[index] != NULL && (table[index] == DEFUNCT || strcmp(table[index]->name, name) != 0)) {
          index++;
     }
     return table[index];
}

/*
  Removes a node that is stored in the hash table
 */
void remove_from_table(node_t **table, char *name) {
     int index = hash(name) % SIZE;
     while (table[index] != NULL && (table[index] == DEFUNCT || strcmp(table[index]->name, name) != 0)) {
          index++;
     }
     if (table[index] != NULL) {
          table[index] = DEFUNCT;
     }
}

/*
  Updates a heap when a node is updated in the middle
 */
void update_heap(heap_t *heap, int index) {
     upheap(heap, index);
     downheap(heap, index);
}
