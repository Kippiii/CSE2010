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

char directions[] = {'u', 'd', 'l', 'r'};    /* Stores the possible directions */

grid_t generate_grid(char *grid_str, int height, int width);
char get_location(grid_t grid, coord_t coord);
void print_grid(grid_t grid);
coord_t is_valid(grid_t grid, coord_t coord, char move, coord_t except);
coord_t *find_path(grid_t grid, int bug);
int equals(coord_t a, coord_t b);

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

      /*
        Calculating the best path for the bug
       */
      for (int i = 0; i < grid.bug_count; i++) {
           coord_t *path = find_path(grid, i);
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
          int current = 0;         /* Current index in the path */
          while (path[current].x != grid.tron.x || path[current].y != grid.tron.y) {
               path_length++;
               current++;
          }
          printf("%d", path_length);

          /*
            Print every step of the path
           */
          for (int i = 0; i < path_length + 1; i++) {
               printf(" (%d,%d)", path[i].y, path[i].x);
          }
          printf("\n");

          free(path);
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
  Uses a breadth-first search  algorithm to find the shortest path to Tron
 */
coord_t *find_path(grid_t grid, int bug) {
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
     queue[tail++] = grid.bugs[bug];
     map[grid.bugs[bug].x][grid.bugs[bug].y] = grid.bugs[bug];

     int found = 0; /* Stores whether the node being searched for has been found */
     while (head != tail && !found) {
          coord_t coord = queue[head];  /* Stores the current coord taken from the queue */
          head = (head + 1) % (grid.width*grid.height);

          /*
            Loops through each possible direction
           */
          for (int i = 0; i < 4; i++) {
               coord_t new = is_valid(grid, coord, directions[i], grid.tron);    /* Stores the new coordinate being considered */

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
               if (new.x == grid.tron.x && new.y == grid.tron.y) {
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

     /*
       Checks whether a path has been found
      */
     if (map[grid.tron.x][grid.tron.y].x == -1) {
          return NULL;
     }

     /*
       Counts how long the path is
      */
     int path_length = 1;          /* Saves the length of the path */
     coord_t current = grid.tron;  /* Saves where the current node being viewed is */
     while (current.x != grid.bugs[bug].x || current.y != grid.bugs[bug].y) {
          current = map[current.x][current.y];
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
          current = map[current.x][current.y];
          path[index--] = current;
     }

     return path;
}
