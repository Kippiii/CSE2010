/*

  Author: Ian Orzel
  Email: iorzel2019@my.fit.edu
  Course: CSE2010
  Section: 23

  Description of this file: Uses a breadth-first search algorithm in order to control a series of bugs to try to eat
  a player-controlled tron character who is trying to travel to an I/O Tower.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>

/*
  Stores information about a coordinate on the graph
 */
typedef struct {
     int x;    /* x-coordinate */
     int y;    /* y-coordinate */
} coord_t;
coord_t EMPTY = {-1, -1};

/*
  Stores all of the information related to the grid
 */
typedef struct {
     int height;    /* Height of the grid */
     int width;     /* Width of the grid */
     char *grid;    /* The current string representation of the grid */
     coord_t tron;  /* The current coordinate of tron */
     coord_t io;    /* The current coordinate of the I/O Tower */
     int bug_count; /* Number of bugs in the grid */
     coord_t *bugs; /* An array of all of the coordinates of the bugs */
} grid_t;

/*
  Node that stores an element of a heap
 */
typedef struct {
     coord_t coord;
     int value;
     int index;
} node_t;

/*
  Struct that stores a heap
 */
typedef struct {
     node_t **heap;
     int size;
} heap_t;

char directions[] = {'u', 'd', 'l', 'r'};    /* Stores the possible directions */

grid_t generate_grid(char *grid_str, int height, int width);
char get_location(grid_t grid, coord_t coord);
void print_grid(grid_t grid);
coord_t is_valid(grid_t grid, coord_t coord, char move, coord_t except);
int *find_path(grid_t grid);
int equals(coord_t a, coord_t b);
coord_t *find_bug_path(grid_t grid, int bug, int *tron_path);
int left(int index);
int right(int index);
int parent(int index);
void construct_heap(heap_t *heap);
node_t *remove_top(heap_t *heap);
void update_heap(heap_t *heap, int index, int value);
void downheap(heap_t *heap, int node);
void upheap(heap_t *heap, int node);

int main(int argc, char* argv[]) {
     FILE *in = fopen(argv[1], "r");

     /*
       Taking in the dimensions as input
      */
      char dimensions[25];         /* Stores the current dimensions as a string */
      fgets(dimensions, 25, in);
      int height;                  /* Stores the height of the grid */
      int width;                   /* Stores the width of the grid */
      sscanf(dimensions, "%d%d", &height, &width);

      /*
        Scanning in the grid from the input
       */
      char temp[width + 4];                                                /* Stores the current data being input */
      char *grid_str = (char *) malloc(sizeof(char) * height * width);     /* Stores the grid as a character array */
      for (int i = 0; i < height; i++) {
           fgets(temp, width + 4, in);
           memmove(grid_str + i * width, temp, width);
      }

      grid_t grid = generate_grid(grid_str, height, width); /* Object storing all information about the grid */

      while (1) {
           print_grid(grid);

           /*
             Checks whether Tron has a valid move
            */
           int can_move = 0;
           for (int i = 0; i < 4; i++) {
                if (!equals(is_valid(grid, grid.tron, directions[i], grid.io), EMPTY)) {
                     can_move = 1;
                     break;
                }
           }
           if (!can_move) {
                printf("No valid moves!\n");
                return 1;
           }

           /*
             Prompts user for the direction to move
            */
           printf("Please enter your move [u(p), d(own), l(elf), or r(ight)]: ");
           char direction;    /* Stores the character of the direction that the tron will move */
           scanf(" %c", &direction);
           coord_t new_coord = is_valid(grid, grid.tron, direction, grid.io);
           while (new_coord.x == -1) {
                printf("INVALID DIRECTION: Please try again: ");
                scanf(" %c", &direction);
                new_coord = is_valid(grid, grid.tron, direction, grid.io);
           }

           /*
             Moves the tron to the new location
            */
           grid.grid[new_coord.y * grid.width + new_coord.x] = 'T';
           grid.grid[grid.tron.y * grid.width + grid.tron.x] = ' ';
           grid.tron = new_coord;

           print_grid(grid);

           if (equals(grid.tron, grid.io)) {
                printf("Tron reaches I/O Tower\n");
                return 1;
           }

           int *tron_path = find_path(grid);

           /*
             Calculating the best path for the bug
            */
           for (int i = 0; i < grid.bug_count; i++) {
                coord_t *path = find_bug_path(grid, i, tron_path);
                printf("Bug %c: ", 'a' + i);

                /*
                  Deals with situation where bug cannot move
                 */
                if (path == NULL) {
                     printf("NO VALID PATH\n");
                     continue;
                }

                /*
                  Prints the first movement direction
                 */
                if (path[0].y - 1 == path[1].y) {
                     printf("u ");
                } else if (path[0].y + 1 == path[1].y) {
                     printf("d ");
                } else if (path[0].x - 1 == path[1].x) {
                     printf("l ");
                } else if (path[0].x + 1 == path[1].x) {
                     printf("r ");
                }

               /*
                 Counting how long the path is
                */
               int path_length = 0;     /* Length of the path */
               int path_weight = 0;
               int current = 0;         /* Current index in the path */
               while (path[current].x != grid.tron.x || path[current].y != grid.tron.y) {
                    path_length++;
                    path_weight += 3 - tron_path[path[current].y * grid.width + path[current].x] - tron_path[path[current + 1].y * grid.width + path[current + 1].x];
                    current++;
               }
               printf("%d", path_weight);

               /*
                 Print every step of the path
                */
               for (int i = 0; i < path_length + 1; i++) {
                    printf(" (%d,%d)", path[i].y, path[i].x);
               }
               printf("\n");

               free(path);

               /*
                Checks whether Tron has been eaten
               */
              if (equals(grid.bugs[i], grid.tron)) {
                   print_grid(grid);
                   printf("A bug is not hungry any more!\n");
                   return 1;
              }
           }
      }

      fclose(in);
      free(grid.grid);
      free(grid.bugs);
}

/*
  Checks whether two coordinates are equal
 */
int equals(coord_t a, coord_t b) {
     return a.x == b.x && a.y == b.y;
}

/*
  Creates a grid struct based on the character array of a grid
 */
grid_t generate_grid(char *grid_str, int height, int width) {
     grid_t grid;   /* The new grid structure */
     grid.grid = grid_str;
     grid.height = height;
     grid.width = width;

     /*
       Determines the number of bugs in the grid
      */
     int bug_count = 0;
     for (int i = 0; i < height*width; i++) {
          if (grid_str[i] >= 'a' && grid_str[i] <= 'z') {
               bug_count++;
          }
     }
     grid.bugs = (coord_t *) malloc(sizeof(coord_t) * bug_count);
     grid.bug_count = bug_count;

     /*
       Finds the locations of the tron, the I/O Tower, and the bugs
      */
     for (int i = 0; i < height*width; i++) {
          coord_t current = {(i % width), (i / width)};     /* Stores the current location as a coord_t object */
          if (grid_str[i] == 'T') {
               grid.tron = current;
          } else if (grid_str[i] == 'I') {
               grid.io = current;
          } else if (grid_str[i] >= 'a' && grid_str[i] <= 'z') {
               grid.bugs[grid_str[i] - 'a'] = current;
          }
     }

     return grid;
}

/*
  Returns the character of a certain location in the grid
 */
char get_location(grid_t grid, coord_t coord) {
     return grid.grid[coord.y * grid.width + coord.x];
}

/*
  Prints the current state of the grid
 */
void print_grid(grid_t grid) {
     /*
       Printing the first line
      */
     printf(" ");
     for (int i = 0; i < grid.width; i++) {
          printf("%d", i % 10);
     }
     printf("\n");

     /*
       Printing the content of the grid
      */
     for (int i = 0; i < grid.height; i++) {
          printf("%d", i % 10);
          for (int j = 0; j < grid.width; j++) {
               printf("%c", grid.grid[i * grid.width + j]);
          }
          printf("\n");
     }
}

/*
  Checks whether a move from a given coordinate is valid
 */
coord_t is_valid(grid_t grid, coord_t coord, char move, coord_t except) {
     coord_t new_coord;  /* Stores the new location that is being moved to */
     /*
       Gets the new coordinate based on the coordinate given by the move
      */
     switch (move) {
          case 'u': case 'U':
               new_coord.x = coord.x;
               new_coord.y = coord.y - 1;
               break;
          case 'd': case 'D':
               new_coord.x = coord.x;
               new_coord.y = coord.y + 1;
               break;
          case 'l': case 'L':
               new_coord.x = coord.x - 1;
               new_coord.y = coord.y;
               break;
          case 'r': case 'R':
               new_coord.x = coord.x + 1;
               new_coord.y = coord.y;
               break;
          default:
               return EMPTY;
     }

     /*
       Checks whether the x-coordinate is out of bounds
      */
     if (new_coord.x < 0 || new_coord.x >= grid.width) {
          return EMPTY;
     }

     /*
       Checks whether the y-coordinate is out of bounds
      */
     if (new_coord.y < 0 || new_coord.y >= grid.height) {
          return EMPTY;
     }

     /*
       Checks if we are at exception
      */
     if (equals(new_coord, except)) {
          return new_coord;
     }

     /*
       Checks whether the new location is empty
      */
     if (!isspace(get_location(grid, new_coord))) {
          return EMPTY;
     }

     return new_coord;
}

/*
  Uses a breadth-first search  algorithm to find the shortest path for the Tron to the IO tower
 */
int *find_path(grid_t grid) {
     coord_t queue[grid.width*grid.height];       /* Queue for storing coords for BFS */
     int head = 0;                                /* The index of the head of the queue */
     int tail = 0;                                /* The index of the tail of the queue */
     coord_t map[grid.width][grid.height];        /* Stores information about parent nodes and whether a node has been used */

     /*
       Initializing the data in the coordinate map
      */
     for (int i = 0; i < grid.width; i++) {
          for (int j = 0; j < grid.height; j++) {
               map[i][j] = EMPTY;
          }
     }

     /*
       Adding infromation about the initial node
      */
     queue[tail++] = grid.tron;
     map[grid.tron.x][grid.tron.y] = grid.tron;

     int found = 0; /* Stores whether the node being searched for has been found */
     while (head != tail && !found) {
          coord_t coord = queue[head];  /* Stores the current coord taken from the queue */
          head = (head + 1) % (grid.width*grid.height);

          /*
            Loops through each possible direction
           */
          for (int i = 0; i < 4; i++) {
               coord_t new = is_valid(grid, coord, directions[i], grid.io);    /* Stores the new coordinate being considered */

               if (new.x == -1) {
                    continue;
               }
               if (map[new.x][new.y].x != -1) {
                    continue;
               }

               map[new.x][new.y] = coord;

               /*
                 Runs if the tron has been found
                */
               if (equals(new, grid.io)) {
                    found = 1;
                    break;
               }

               /*
                 Adds the new node to the queue
                */
               queue[tail] = new;
               tail = (tail + 1) % (grid.width*grid.height);
          }
     }

     int *result = (int *) calloc(sizeof(int), grid.width * grid.height);

     /*
       Checks whether a path has been found
      */
     if (map[grid.io.x][grid.io.y].x == -1) {
          return result;
     }

     /*
       Counts how long the path is
      */
     int path_length = 1;          /* Saves the length of the path */
     coord_t current = grid.io;    /* Saves where the current node being viewed is */
     while (current.x != grid.tron.x || current.y != grid.tron.y) {
          current = map[current.x][current.y];
          path_length++;
     }

     /*
       Turns the path into an array
      */
     coord_t *path = (coord_t *) malloc(sizeof(coord_t) * path_length);    /* Stores the path */
     int index = path_length - 1;                                          /* Stores the next index to insert into the path */
     path[index--] = grid.io;
     current = grid.io;
     while (current.x != grid.tron.x || current.y != grid.tron.y) {
          current = map[current.x][current.y];
          path[index--] = current;
     }

     for (int i = 0; i < path_length; i++) {
          result[path[i].y * grid.width + path[i].x] = 1;
     }

     return result;
}

/*
  Finds the optimal path for a bug
 */
coord_t *find_bug_path(grid_t grid, int bug, int *tron_path) {
     /*
       Initializing the heap
      */
     heap_t *heap = (heap_t *) malloc(sizeof(heap_t));
     heap->size = 0;
     node_t **map = (node_t **) malloc(sizeof(coord_t *) * grid.width * grid.height);
     heap->heap = (node_t **) malloc(sizeof(coord_t *) * grid.width * grid.height);
     for (int i = 0; i < grid.width * grid.height; i++) {
          node_t *current = (node_t *) malloc(sizeof(coord_t));
          current->coord.x = i % grid.width;
          current->coord.y = i / grid.width;
          current->index = i;
          current->value = equals(current->coord, grid.bugs[bug]) ? 0 : INT_MAX;
          heap->heap[i] = current;
          heap->size++;
          map[i] = current;
     }

     /*
       Initializing Dijstra's
      */
     coord_t visited[grid.width][grid.height];
     for (int i = 0; i < grid.width * grid.height; i++) {
          visited[i % grid.width][i / grid.width] = EMPTY;
     }
     visited[grid.bugs[bug].x][grid.bugs[bug].y] = grid.bugs[bug];
     construct_heap(heap);
     int found = 0;
     while (heap->size > 0 && !found) {
          node_t *current = remove_top(heap);

          /*
            Checks whether the tron has been found
           */
          if (equals(current->coord, grid.tron)) {
               found = 1;
               break;
          }

          /*
            Checks if tron cannot be found from the current location
           */
          if (current->value == INT_MAX) {
               for (int i = 0; i < heap->size; i++) {
                    if (heap->heap[i]->value != INT_MAX) {
                         //printf("%d, %d, %d\n", i, heap->heap[i]->value, heap->heap[i]->coord.y * grid.width + heap->heap[i]->coord.x);
                    }
               }
               break;
          }

          /*
            Loops through every possible direction that the bug can move
           */
          for (int i = 0; i < 4; i++) {
               coord_t new = is_valid(grid, current->coord, directions[i], grid.tron);    /* Stores the new coordinate being considered */

               if (new.x == -1) {
                    continue;
               }
               if (map[new.y * grid.width + new.x]->index == -1) {
                    continue;
               }

               /*
                 Checks whether distance needs to be updated and updates it
                */
               int weight = 3 - tron_path[current->coord.y * grid.width + current->coord.x] - tron_path[new.y * grid.width + new.x];
               if (map[current->coord.y * grid.width + current->coord.x]->value + weight < map[new.y * grid.width + new.x]->value) {
                    update_heap(heap, map[new.y * grid.width + new.x]->index, map[current->coord.y * grid.width + current->coord.x]->value + weight);
                    visited[new.x][new.y] = current->coord;
               }
          }
     }

     /*
       Returns null if no path was found
      */
     if (!found) {
          return NULL;
     }

     /*
       Counts how long the path is
      */
     int path_length = 1;          /* Saves the length of the path */
     coord_t current = grid.tron;  /* Saves where the current node being viewed is */
     while (current.x != grid.bugs[bug].x || current.y != grid.bugs[bug].y) {
          current = visited[current.x][current.y];
          path_length++;
     }

     /*
       Turns the path into an array
      */
     coord_t *path = (coord_t *) malloc(sizeof(coord_t) * path_length);    /* Stores the path */
     int index = path_length - 1;                                          /* Stores the next index to insert into the path */
     path[index--] = grid.tron;
     current = grid.tron;
     while (current.x != grid.bugs[bug].x || current.y != grid.bugs[bug].y) {
          current = visited[current.x][current.y];
          path[index--] = current;
     }

     free(heap->heap);
     free(map);
     free(heap);

     grid.grid[grid.bugs[bug].y * grid.width + grid.bugs[bug].x] = ' ';
     grid.grid[path[1].y * grid.width + path[1].x] = 'a' + bug;
     grid.bugs[bug] = path[1];

     return path;
}

/*
  Gets the left node of a binary tree
 */
int left(int index) {
     return index * 2 + 1;
}

/*
  Gets the right node of a binary tree
 */
int right(int index) {
     return index * 2 + 2;
}

/*
  Finding the parent node of a certain node
 */
int parent(int index) {
     return index == 0 ? 0 : (index - 1) / 2;
}

/*
  Runs bottom-up heap construction on an array
 */
void construct_heap(heap_t *heap) {
     for (int i = (heap->size - 1) / 2 - 1; i >= 0; i--) {
          downheap(heap, i);
     }
}

/*
  Removes and returns the top node of the heap
 */
node_t *remove_top(heap_t *heap) {
     assert(heap->size > 0);
     node_t *top = heap->heap[0];
     heap->heap[0] = heap->heap[--heap->size];
     heap->heap[0]->index = 0;
     downheap(heap, 0);
     top->index = -1;
     return top;
}

/*
  Updates a node in the heap to a new value
 */
void update_heap(heap_t *heap, int index, int value) {
     heap->heap[index]->value = value;
     if (heap->heap[index]->value < heap->heap[parent(index)]->value) {
          upheap(heap, index);
     } else {
          downheap(heap, index);
     }
}

/*
  Runs downheap on the heap from a starting node
 */
void downheap(heap_t *heap, int node) {
     /*
       Checks whether we are out of the bounds of the heap
      */
     if (node >= heap->size) {
          return;
     }

     int left_index = left(node) >= heap->size ? node : left(node);
     int right_index = right(node) >= heap->size ? node : right(node);

     /*
       Checks whether this subtree is already a valid heap
      */
     if (heap->heap[node]->value <= heap->heap[left_index]->value && heap->heap[node]->value <= heap->heap[right_index]->value) {
          return;
     }

     assert(left_index != node || right_index != node);

     /*
       Swaps the proper nodes of the heap and then calls downheap on where the node has been placed
      */
     if (heap->heap[left_index]->value < heap->heap[right_index]->value) {
          node_t *temp = heap->heap[left_index];
          heap->heap[left_index] = heap->heap[node];
          heap->heap[node] = temp;
          heap->heap[left_index]->index = left_index;
          heap->heap[node]->index = node;
          downheap(heap, left_index);
     } else {
          node_t *temp = heap->heap[right_index];
          heap->heap[right_index] = heap->heap[node];
          heap->heap[node] = temp;
          heap->heap[right_index]->index = right_index;
          heap->heap[node]->index = node;
          downheap(heap, right_index);
     }
}

/*
  Runs upheap on the node from a starting node
 */
void upheap(heap_t *heap, int node) {
     /*
       Checks if we are out of bounds in the heap
      */
     if (node <= 0) {
          return;
     }

     /*
       Checks whether heap is currently valid
      */
     if (heap->heap[node]->value >= heap->heap[parent(node)]->value) {
          return;
     }

     /*
       Swaps current node with parent and then runs upheap on that
      */
     node_t *temp = heap->heap[parent(node)];
     heap->heap[parent(node)] = heap->heap[node];
     heap->heap[node] = temp;
     heap->heap[parent(node)]->index = parent(node);
     heap->heap[node]->index = node;
     upheap(heap, parent(node));
}
