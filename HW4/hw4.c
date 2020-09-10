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
void add_to_heap(heap_t *heap, char *name, int time, int price, int quantity, order_type_t order_type);
void upheap(heap_t *heap, int index);
node_t *remove_from_heap(heap_t *heap);
void downheap(heap_t *heap);
void execute_buy_sell_orders(heap_t *buy_heap, heap_t *sell_heap);
int get_time(int orig_time);

/*
  At the top of each function, describe the function, parameter(s), and return value (if any)
 */

int main(int argc, char* argv[]) {
     FILE *in = fopen(argv[1], "r");                             /* Stores the file where the input is being read from */
     heap_t *buy_orders = (heap_t *) malloc(sizeof(heap_t));     /* The heap for storing buy orders */
     heap_t *sell_orders = (heap_t *) malloc(sizeof(heap_t));    /* The heap for storing sell orders */

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
               int price = (int) (price_double * 100 + 0.5);     /* Stores the price of the product as an integer */

               /*
                 Reprints the command, adds the buy order to the heap, and then checks if transactions can occur
                */
               printf("%s %06d %s %.2f %d\n", command, time, buyer, price_double, quantity);
               add_to_heap(buy_orders, buyer, time, price, quantity, BUY);
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
               int price = (int)(price_double * 100 + 0.5);            /* Stores the price of the product as an integer */

               /*
                 Reprints the command, adds the sell order to the heap, and checks if a new transaction can occur
                */
               printf("%s %06d %s %.2f %d\n", command, time, seller, price_double, quantity);
               add_to_heap(sell_orders, seller, time, price, quantity, SELL);
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
void add_to_heap(heap_t *heap, char *name, int time, int price, int quantity, order_type_t order_type) {
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
     if (heap->size + 1 >= heap->max_size) {
          heap->max_size *= 2;
          heap->nodes = realloc(heap->nodes, sizeof(node_t *) * heap->max_size);
     }

     /*
       Places the new node in the heap and then runs upheap
      */
     heap->nodes[heap->size++] = new_node;
     upheap(heap, heap->size - 1);
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
     downheap(heap);
     return top;
}

/*
  Pushes a node downward through heap from top while ensuring that it remains a heap
 */
void downheap(heap_t *heap) {
     int index = 0; /* Stores the index where the node currently is in the heap */
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
               index = get_left(index);
          } else {
               /*
                 Swaps the right node with the root if it is bigger than the left node
                */
               node_t *tmp = heap->nodes[get_right(index)];
               heap->nodes[get_right(index)] = heap->nodes[index];
               heap->nodes[index] = tmp;
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
          int t = sell_heap->nodes[0]->quantity;                                   /* Stores the number of items the seller can sell */
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
int get_time(int orig_time) {
     return (orig_time % 100) + (orig_time % 10000 / 100) * 60 + (orig_time % 1000000 / 10000) * 60 * 60;
}
