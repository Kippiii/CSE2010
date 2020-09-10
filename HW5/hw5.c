/*

  Author: Ian Orzel
  Email: iorzel2019@my.fit.edu
  Course: CSE2010
  Section: 23
  Description: Stores a list of activites with times in which they occur. These are then stored in a skip list so that
  specific activities and be retrieved, deleted, added, and a list of activities between two points can be retrieved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "fakeRandHeight.h"

/*
  Node used to store elements of the skip list
 */
typedef struct Node {
     struct Node *down;
     struct Node *right;
     int time;
     char *activity;
} node_t;

node_t *get(node_t *top_left, int time);
int put(node_t *start, int time, char *activity, node_t **below);
char *remove_node(node_t *top_left, int time);
void submap(node_t *top_left, int time1, int time2);
void add_levels(node_t **top_left, int time, char *activity, int levels);
void remove_levels(node_t **top_left);

int main(int argc, char* argv[]) {
     /*
       Initializing the skip list
      */
     node_t *top_left = (node_t *) malloc(sizeof(node_t));     /* Top left of the skip list */
     top_left->time = INT_MIN;
     top_left->down = NULL;
     top_left->right = (node_t *) malloc(sizeof(node_t));
     top_left->right->time = INT_MAX;
     top_left->right->right = NULL;
     top_left->right->down = NULL;

     FILE *in = fopen(argv[1], "r");                         /* File with the input in it */
     char *command = (char *) malloc(sizeof(char) * 30);    /* Command that is input */

     while (!feof(in)) {
          fscanf(in, "%s", command);

          if (strcmp(command, "AddActivity") == 0) {
               /*
                 Takes in input and adds it to the skip list
                */
               int time;                                              /* Time when activity occured */
               char *activity = (char *) malloc(sizeof(char) * 50);   /* Activity that occurs */
               fscanf(in, "%d%s", &time, activity);
               node_t *below = NULL;                                  /* Node used in the put function */
               int levels = put(top_left, time, activity, &below);    /* The number of levels that need to be added to the top */
               if (levels > -2)
                    add_levels(&top_left, time, activity, levels);

               /*
                 Prints the result of the operation
                */
               printf("%s %08d %s", command, time, activity);
               if (levels == -3) {
                    printf(" ExistingActivityError:%s", below->activity);
                    free(activity);
               }
               printf("\n");
          } else if (strcmp(command, "RemoveActivity") == 0) {
               /*
                 Removes an activity from the skip list
                */
               int time;                                    /* Time when activity occurs */
               fscanf(in, "%d", &time);
               char *activity = remove_node(top_left, time);     /* Activity that was just removed */
               remove_levels(&top_left);
               printf("%s %08d ", command, time);
               /*
                 Activity is null if an activity was not found
                */
               if (activity == NULL) {
                    printf("NoActivityError\n");
               } else {
                    printf("%s\n", activity);
                    free(activity);
               }
          } else if (strcmp(command, "GetActivity") == 0) {
               /*
                 Gets a specific activity from the skip list
                */
               int time;                               /* Time when activity occured */
               fscanf(in, "%d", &time);
               node_t *activity = get(top_left, time); /* THe node of the activity */
               printf("%s %08d ", command, time);
               if (activity != NULL && activity->time == time) {
                    printf("%s\n", activity->activity);
               } else {
                    printf("none\n");
               }
          } else if (strcmp(command, "GetActivitiesBetweenTimes") == 0) {
               /*
                 Uses submap to print the activities between two times
                */
               int start;     /* Time to start from */
               int end;       /* Time to end on */
               fscanf(in, "%d%d", &start, &end);
               printf("%s %08d %08d ", command, start, end);
               submap(top_left, start, end);
          } else if (strcmp(command, "GetActivitiesForOneDay") == 0) {
               /*
                 Generates submap between beginning of date and end of date
                */
               int date; /* Date in which is being analyzed */
               fscanf(in, "%d", &date);
               printf("%s %04d ", command, date);
               submap(top_left, date * 10000, (date + 1) * 10000 - 1);
          } else if (strcmp(command, "GetActivitiesFromEarlierInTheDay") == 0) {
               /*
                 Gets activities from earlier in the day, not including the time in question
                */
               int time;
               fscanf(in, "%d", &time);
               printf("%s %08d ", command, time);
               submap(top_left, (time / 10000) * 10000, time - 1);
          } else if (strcmp(command, "PrintSkipList") == 0) {
               printf("%s\n", command);
               /*
                 Counts the number of levels in the skip list
                */
               int level_count = 0;     /* Number of levels in the skip list */
               for (node_t *left = top_left; left != NULL; left = left->down) {
                    level_count++;
               }

               /*
                 Loops through the skip list row by row
                */
               for (node_t *left = top_left; left != NULL; left = left->down) {
                    level_count--;
                    printf("(S%d) ", level_count);
                    if (left->right->time == INT_MAX) {
                         printf("empty\n");
                         continue;
                    }
                    /*
                      Prints all elements on that level of skip list
                     */
                    for (node_t *tmp = left->right; tmp->time < INT_MAX; tmp = tmp->right) {
                         printf("%08d:%s ", tmp->time, tmp->activity);
                    }
                    printf("\n");
               }
          }
          command[0] = '\0';
     }
     free(command);

     /*
       Moving to the bottom of the spanning tree
      */
     node_t *bottom_left = top_left;
     while (bottom_left->down != NULL) {
          bottom_left = bottom_left->down;
     }

     /*
       Removing all nodes from skip list
      */
     while (bottom_left->right->time < INT_MAX) {
          remove_node(top_left, bottom_left->right->time);
          remove_levels(&top_left);
     }

     /*
       Freeing the final two nodes
      */
     free(top_left->right);
     free(top_left);
}

/*
  Gets a certain node from the skip list
 */
node_t *get(node_t *top_left, int time) {
     node_t *tmp = top_left;  /* Stores the node currently at in skip list */
     while (tmp->down != NULL) {
          /*
            Continues right until at node that is equal or greater
           */
          while (tmp->right->time <= time) {
               tmp = tmp->right;
          }
          if (tmp->down != NULL) {
               tmp = tmp->down;
          }
     }
     return tmp;
}

/*
  Adds a new key to the skip list.   NOTE: Adding extra levels done elsewhere!
 */
int put(node_t *start, int time, char *activity, node_t **below) {
     /*
       Start by moving right to the node in question
      */
     node_t *tmp = start;     /* Current node in question (floor) */
     while (tmp->right->time <= time) {
          tmp = tmp->right;
     }

     /*
       Fixing problem with ExistingActivityError
      */
     int levels;    /* Stores the number of levels that still need to be added to the skip list */
     if (tmp->down != NULL) {
          levels = put(tmp->down, time, activity, below);
          if (levels == -3)
               return -3;
     } else {
          if (tmp->time == time) {
               *below = tmp;
               return -3;
          }
          levels = getRandHeight();
     }

     /*
     int levels = (tmp->down != NULL) ? put(tmp->down, time, activity, below) : getRandHeight();

     if ((tmp->down == NULL && tmp->time == time) || levels == -3) {
          if (levels != -3)
               *below = tmp;
          return -3;
     } */

     /*
       Adds node to current level if need be
      */
     if (levels + 1 && levels + 2) {
          node_t *new_node = (node_t *) malloc(sizeof(node_t));  /* New node to be inserted */
          if (*below == NULL) {
               new_node->down = NULL;
               *below = new_node;
          } else {
               new_node->down = *below;
               *below = new_node;
          }
          new_node->right = tmp->right;
          tmp->right = new_node;
          new_node->time = time;
          new_node->activity = activity;
          return levels - 1;
     }
     return -2;
}

/*
  Removes a node that was added at the given time
 */
char *remove_node(node_t *top_left, int time) {
     /*
       Move right until next node is greater or equal
      */
     node_t *tmp = top_left;
     while (tmp->right->time < time) {
          tmp = tmp->right;
     }

     /*
       Continues going down levels recursively until it reaches the end
      */
     char *activity;
     if (tmp->down != NULL) {
          activity = remove_node(tmp->down, time);
     } else {
          if (tmp->right->time != time) {
               return NULL;
          }
          activity = tmp->right->activity;
     }

     /*
       Removes the node from the skip list
      */
     if (activity != NULL && tmp->right->time == time) {
          node_t *to_delete = tmp->right;
          tmp->right = tmp->right->right;
          free(to_delete);
          return activity;
     }

     return activity;
}

/*
  Printing all of the activities between two times
 */
void submap(node_t *top_left, int time1, int time2) {
     /*
       Prints none if no nodes are within the range
      */
     node_t *begin = get(top_left, time1);   /* Stores the first node in the range */
     if (begin->time < time1) {
          begin = begin->right;
     }
     if (begin->time > time2) {
          printf("none\n");
          return;
     }

     /*
       Prints each node within the range
      */
     for(; begin->time <= time2; begin = begin->right) {
          printf("%08d:%s ", begin->time, begin->activity);
     }
     printf("\n");
}

/*
  Adds extra levels needed for the skip list
 */
void add_levels(node_t **top_left, int time, char *activity, int levels) {
     while (levels + 1) {
          /*
            Creates a negative infinity node
           */
          node_t *small = (node_t *) malloc(sizeof(node_t));
          small->time = INT_MIN;
          small->down = *top_left;

          /*
            Creates a new version of the new node
           */
          node_t *new_node = (node_t *) malloc(sizeof(node_t));
          small->right = new_node;
          new_node->down = (*top_left)->right;
          new_node->activity = activity;
          new_node->time = time;

          /*
            Creates a positive infinity node
           */
          node_t *large = (node_t *) malloc(sizeof(node_t));
          new_node->right = large;
          large->down = new_node->down->right;
          large->time = INT_MAX;
          large->right = NULL;

          *top_left = small;

          levels--;
     }

     /*
       Adding an empty top level
      */
     node_t *small = (node_t *) malloc(sizeof(node_t));
     small->time = INT_MIN;
     small->down = *top_left;
     node_t *large = (node_t *) malloc(sizeof(node_t));
     small->right = large;
     large->down = small->down->right->right;
     large->time = INT_MAX;
     large->right = NULL;
     *top_left = small;
}

/*
  Removes extra levels left over by the remove function
 */
void remove_levels(node_t **top_left) {
     /*
       Figures out how many empty levels are in the skip list
      */
     int empty_levels = 0;
     for (node_t *tmp = *top_left; tmp != NULL && tmp->right->time == INT_MAX; tmp = tmp->down) {
          empty_levels++;
     }

     /*
       Removes extra levels except for one
      */
     for (int i = 0; i < empty_levels - 1; i++) {
          node_t *tmp = *top_left;
          *top_left = tmp->down;
          free(tmp->right);
          free(tmp);
     }
}


// put(skiplist, ...)
// uses getRandHeight()
