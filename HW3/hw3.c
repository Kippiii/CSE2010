/*

  Author: Ian Orzel
  Email: iorzel2019@my.fit.edu
  Course: CSE2010
  Section: 23

  Description of this file: This program takes in information related to the sports, events, and medalists of the olympics.
  Once it knows all of this information, it is stored using a trees and allows the user to enter a number of queries in
  order to gain information about the olympics that took place.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#define BUFFER 1000

/*
  Struct for storing a node in the tree structure
 */
typedef struct Node {
     int level;
     char *data;
     struct Node *sibling;
     struct Node *child;
     struct Node *parent;
} node_t;

/*
  Struct for storing information related to a data point with a string and an int
 */
typedef struct {
     char *name;
     int data;
} data_point_t;

void insertChild(node_t *parentNode, node_t *childNode);
void appendChild(node_t *parentNode, node_t *childNode);
node_t *getChildren(node_t *node);
node_t *getParent(node_t *node);
node_t *find_node(node_t *root, char *str);
int read_line(char *line, FILE *in);
void increment_data_point(data_point_t **data, int *size, char *name);
void generate_medal_freq(node_t *root, data_point_t **medal_person, int *medal_person_legnth, data_point_t **medal_country, int *medal_country_length, data_point_t **gold_medal_person, int *gold_medal_person_length, data_point_t **gold_medal_country, int *gold_medal_country_length);
char *get_country_str(char *athlete);
int comparison(const void *a, const void *b);
void free_tree(node_t *root);

/*
  At the top of each function, describe the function, parameter(s), and return value (if any)
 */

int main(int argc, char* argv[]) {
     FILE *dataIn = fopen(argv[1], "r");                    /* File object for taking input about the data for the tree */
     FILE *queryIn = fopen(argv[2], "r");                   /* File object for taking input about the queries for the tree */
     node_t *root = NULL;                                   /* Root of the tree */
     char *line = (char *) malloc(sizeof(char) * BUFFER);   /* Saves a string for the current line of input being read */
     char *line_start = line;                               /* Stores the initial position of the line */
     int leaf_counter = 0;                                  /* Counts how many leaf nodes are in the tree */

     /*
       Creates the root of the tree
      */
     read_line(line, dataIn);
     root = (node_t *) malloc(sizeof(node_t));
     root->level = 0;
     root->data = (char *) malloc(sizeof(char) * 30);
     sscanf(line, "%s", root->data);
     line += strlen(root->data) + 1;

     /*
       Loops through each event that needs to be created
      */
     int line_read = 1;                 /* Used for storing whether the last line was read */
     node_t *parent_node = root;        /* Used for storage the string related to the parent node */

     do {
          char *tmp = (char *) malloc(sizeof(char) * 30);   /* Used for temporary storage of the next event */
          int was_read = sscanf(line, "%s", tmp);           /* Used to store whether the end of the line has been reached */
          while (was_read != EOF) {
               /*
                 Creates a node for each new event and inserts it into the tree
                */
               node_t *new_node = (node_t *) malloc(sizeof(node_t));
               new_node->data = (char *) malloc(sizeof(char) * (strlen(tmp) + 1));
               strcpy(new_node->data, tmp);
               if (parent_node->level != 2) {
                    insertChild(parent_node, new_node);
               } else {
                    appendChild(parent_node, new_node);
                    leaf_counter++;
               }

               /*
                 Reads in the next string for the next event if it exists
                */
               line += strlen(tmp) + 1;
               was_read = sscanf(line, "%s", tmp);
          }

          /*
            Reads the next line and finds the node associated if it exists
           */
          line = line_start;
          line_read = read_line(line, dataIn);
          if (line_read) {
               /*
                 Runs if the line is successfully read from the file
                */
               sscanf(line, "%s", tmp);
               parent_node = find_node(root, tmp);
               assert(parent_node != NULL);
               line += strlen(tmp) + 1;
          }
          free(tmp);
     } while (line_read);
     free(line);

     /*
       Creates four arrays of data points for storage of top medalists/gold medalists by person/country
      */
     data_point_t **medalist_person = (data_point_t **) malloc(sizeof(data_point_t *) * leaf_counter);
     data_point_t **medalist_country = (data_point_t **) malloc(sizeof(data_point_t *) * leaf_counter);
     data_point_t **gold_medalist_person = (data_point_t **) malloc(sizeof(data_point_t *) * leaf_counter);
     data_point_t **gold_medalist_country = (data_point_t **) malloc(sizeof(data_point_t *) * leaf_counter);

     /*
       Stores the current length of each of the following arrays
      */
     int medalist_person_length = 0;
     int medalist_country_length = 0;
     int gold_medalist_person_length = 0;
     int gold_medalist_country_length = 0;

     /*
       Generates the frequency arrays and sorts them
      */
     generate_medal_freq(root, medalist_person, &medalist_person_length, medalist_country, &medalist_country_length, gold_medalist_person, &gold_medalist_person_length, gold_medalist_country, &gold_medalist_country_length);
     qsort((void *) medalist_person, medalist_person_length, sizeof(data_point_t *), comparison);
     qsort((void *) medalist_country, medalist_country_length, sizeof(data_point_t *), comparison);
     qsort((void *) gold_medalist_person, gold_medalist_person_length, sizeof(data_point_t *), comparison);
     qsort((void *) gold_medalist_country, gold_medalist_country_length, sizeof(data_point_t *), comparison);

     /*
       Scans in each command from the input file
      */
     while (!feof(queryIn)) {
          char *command = (char *) malloc(sizeof(char) * 50);    /* Stores a string with the current command */
          fscanf(queryIn, "%s", command);

          if (strcmp(command, "GetEventsBySport") == 0) {
               /*
                 Finds the node associated with the given sport
                */
               char *sport = (char *) malloc(sizeof(char) * 50);
               fscanf(queryIn, "%s", sport);
               node_t *sport_node;
               for (sport_node = root->child; sport_node != NULL; sport_node = sport_node->sibling) {
                    if (strcmp(sport_node->data, sport) == 0) {
                         break;
                    }
               }
               assert(sport_node != NULL);

               /*
                 Prints all of the events associated with the sport
                */
               printf("%s %s", command, sport);
               for (node_t *tmp = sport_node->child; tmp != NULL; tmp = tmp->sibling) {
                    printf(" %s", tmp->data);
               }
               printf("\n");

               free(sport);
          } else if (strcmp(command, "GetWinnersAndCountriesBySportAndEvent") == 0) {
               /*
                 Finds the node associated with the sport and reading input
                */
               char *sport = (char *) malloc(sizeof(char) * 50);
               char *event = (char *) malloc(sizeof(char) * 50);
               fscanf(queryIn, "%s%s", sport, event);
               node_t *sport_node;
               for (sport_node = root->child; sport_node != NULL; sport_node = sport_node->sibling) {
                    if (strcmp(sport_node->data, sport) == 0) {
                         break;
                    }
               }
               assert(sport_node != NULL);

               /*
                 Finds the event from the given sport
                */
               node_t *event_node;
               for (event_node = sport_node->child; event_node != NULL; event_node = event_node->sibling) {
                    if (strcmp(event_node->data, event) == 0) {
                         break;
                    }
               }
               assert(event_node != NULL);

               /*
                 Prints all of the athletes from the given event and sport
                */
               printf("%s %s %s", command, sport, event);
               for (node_t *tmp = event_node->child; tmp != NULL; tmp = tmp->sibling) {
                    printf(" %s", tmp->data);
               }
               printf("\n");

               free(sport);
               free(event);
          } else if (strcmp(command, "GetGoldMedalistAndCountryBySportAndEvent") == 0) {
               /*
                 Finds the node associated with the sport and reading input
                */
               char *sport = (char *) malloc(sizeof(char) * 50);
               char *event = (char *) malloc(sizeof(char) * 50);
               fscanf(queryIn, "%s%s", sport, event);
               node_t *sport_node;
               for (sport_node = root->child; sport_node != NULL; sport_node = sport_node->sibling) {
                    if (strcmp(sport_node->data, sport) == 0) {
                         break;
                    }
               }
               assert(sport_node != NULL);

               /*
                 Finds the event from the given sport
                */
               node_t *event_node;
               for (event_node = sport_node->child; event_node != NULL; event_node = event_node->sibling) {
                    if (strcmp(event_node->data, event) == 0) {
                         break;
                    }
               }
               assert(event_node != NULL);

               /*
                 Prints the given gold medalist
                */
               printf("%s %s %s %s\n", command, sport, event, event_node->child->data);

               free(sport);
               free(event);
          } else if (strcmp(command, "GetAthleteWithMostMedals") == 0) {
               /*
                 Prints all of the top medalists in alphabetical order
                */
               int max = medalist_person[0]->data;
               printf("%s %d", command, max);
               for (int i = 0; i < medalist_person_length && medalist_person[i]->data == max; i++) {
                    /*
                      Prints athlete's name if he/she has the most medals
                    */
                    char *country = get_country_str(medalist_person[i]->name);
                    *(country - 1) = '\0'; //Cheese
                    printf(" %s", medalist_person[i]->name);
                    *(country - 1) = ':';
               }
               printf("\n");
          } else if (strcmp(command, "GetAthleteWithMostGoldMedals") == 0) {
               /*
                 Prints all of the top gold medalists in alphabetical order
                */
               int max = gold_medalist_person[0]->data;
               printf("%s %d", command, max);
               for (int i = 0; i < gold_medalist_person_length && gold_medalist_person[i]->data == max; i++) {
                   /*
                     Prints athlete's name if he/she has the most medals
                   */
                    char *country = get_country_str(gold_medalist_person[i]->name);
                    *(country - 1) = '\0';
                    printf(" %s", gold_medalist_person[i]->name);
                    *(country - 1) = ':';
               }
               printf("\n");
          } else if (strcmp(command, "GetCountryWithMostMedals") == 0) {
               /*
                 Prints all of the countries with the most medals in alphabetical order
                */
               int max = medalist_country[0]->data;
               printf("%s %d", command, max);
               for (int i = 0; i < medalist_country_length && medalist_country[i]->data == max; i++) {
                    printf(" %s", medalist_country[i]->name);
               }
               printf("\n");
          } else if (strcmp(command, "GetCountryWithMostGoldMedals") == 0) {
               /*
                 Prints all of the countries with the most gold medals in alphabetical order
                */
               int max = gold_medalist_country[0]->data;
               printf("%s %d", command, max);
               for (int i = 0; i < gold_medalist_country_length && gold_medalist_country[i]->data == max; i++) {
                    printf(" %s", gold_medalist_country[i]->name);
               }
               printf("\n");
          } else if (strcmp(command, "GetSportAndEventByAthlete") == 0) {
               char *athlete = (char *) malloc(sizeof(char) * 50);    /* Stores a string with the athlete's name */
               fscanf(queryIn, "%s", athlete);
               printf("%s %s", command, athlete);

               /*
                 Three-way for loop that goes through every sport node, event node, and athlete node in alphabetical order
                */
               for (node_t *sport_node = root->child; sport_node != NULL; sport_node = sport_node->sibling) {
                    for (node_t *event_node = sport_node->child; event_node != NULL; event_node = event_node->sibling) {
                         for (node_t *athlete_node = event_node->child; athlete_node != NULL; athlete_node = athlete_node->sibling) {
                              /*
                                Prints the sport and event if the athlete is the one that was inputted
                               */
                              char *country_str = get_country_str(athlete_node->data);
                              *(country_str - 1) = '\0';
                              if (strcmp(athlete, athlete_node->data) == 0) {
                                   printf(" %s:%s", sport_node->data, event_node->data);
                              }
                              *(country_str - 1) = ':';
                         }
                    }
               }
               printf("\n");

               free(athlete);
          }

          free(command);
     }

     /*
       Freeing all of the data points in the frequency arrays
      */
     for (int i = 0; i < medalist_person_length; i++) {
          free(medalist_person[i]);
     }
     for (int i = 0; i < medalist_country_length; i++) {
          free(medalist_country[i]);
     }
     for (int i = 0; i < gold_medalist_person_length; i++) {
          free(gold_medalist_person[i]);
     }
     for (int i = 0; i < gold_medalist_country_length; i++) {
          free(gold_medalist_country[i]);
     }

     free_tree(root);

     fclose(dataIn);
     fclose(queryIn);
}

/*
  Adds a child to the parent node based on alphabetical order
 */
void insertChild(node_t *parentNode, node_t *childNode) {
     childNode->parent = parentNode;
     assert(parentNode->level >= 0 && parentNode->level <= 3);
     childNode->level = parentNode->level + 1;
     /*
       Deals with case when the parent does not have a child
      */
     if (parentNode->child == NULL) {
          parentNode->child = childNode;
          return;
     }

     /*
       Checks if child node is lexographically before the current first child node
      */
     if (strcmp(parentNode->child->data, childNode->data) > 0) {
          childNode->sibling = parentNode->child;
          parentNode->child = childNode;
          return;
     }

     /*
       Loops through children until the place for new node to be inserted is found
      */
     node_t *tmp_node;
     for (tmp_node = parentNode->child; tmp_node->sibling != NULL; tmp_node = tmp_node->sibling) {
          if (strcmp(tmp_node->sibling->data, childNode->data) > 0) {
               break;
          }
     }

     /*
       Inserts new node as a sibling to a previous node
      */
     childNode->sibling = tmp_node->sibling;
     tmp_node->sibling = childNode;
}

/*
  Inserts a child from a parent node after the previously added node
 */
void appendChild(node_t *parentNode, node_t *childNode) {
     childNode->parent = parentNode;
     assert(parentNode->level >= 0 && parentNode->level <= 3);
     childNode->level = parentNode->level + 1;
     /*
       Deals with case when the parent does not have a child
      */
     if (parentNode->child == NULL) {
          parentNode->child = childNode;
          return;
     }

     /*
       Adds the node as sibling to the last added child
      */
     node_t *tmp_node;
     for (tmp_node = parentNode->child; tmp_node->sibling != NULL; tmp_node = tmp_node->sibling) {}
     tmp_node->sibling = childNode;
}

/*
  Returns the node object for the linked list of children
 */
node_t *getChildren(node_t *node) {
     return node->child;
}

/*
  Returns the node object associated with the parent of the current node
 */
node_t *getParent(node_t *node) {
     return node->parent;
}

/*
  Searches tree for a node with the given string
 */
node_t *find_node(node_t *root, char *str) {
     assert(root != NULL);
     /*s
       Returns root if the given string is equal to the data in the root
      */
     if (strcmp(root->data, str) == 0) {
          return root;
     }

     /*
       Loops through all the children of the node and searches the subtrees for the given string
      */
     node_t *tmp_node;
     for (tmp_node = root->child; tmp_node != NULL; tmp_node = tmp_node->sibling) {
          node_t *found_node = find_node(tmp_node, str);
          if (found_node != NULL) {
               return found_node;
          }
     }
     return NULL;
}

/*
  Reads the next line of input from the input stream. Returns false if EOF is reached
 */
int read_line(char *line, FILE *in) {
     char *new = fgets(line, BUFFER, in);
     if (new == NULL) {
          return 0;
     }
     while (isspace(line[strlen(line) - 1])) {
          line[strlen(line) - 1] = '\0';
     }
     return 1;
}

/*
  Finds a data point with the proper string for an array and increments the data. Creates new point if it doesn't exist.
 */
void increment_data_point(data_point_t **data, int *size, char *name) {
     /*
       Loops through each data point in the array and increments it if it is found
      */
     for (int i = 0; i < *size; i++) {
          assert(data[i] != NULL);
          if (strcmp(data[i]->name, name) == 0) {
               (data[i]->data)++;
          }
     }

     /*
       Creates a new data point after it discovers that one does not exist
      */
     data_point_t *new = (data_point_t *) malloc(sizeof(data_point_t)); /* Stores a pointer to the new data point */
     new->name = name;
     new->data = 1;
     data[*size] = new;
     (*size)++;
}

/*
  Generates the frequency arrays for the medal information related to the leaf nodes
 */
void generate_medal_freq(node_t *root, data_point_t **medal_person, int *medal_person_length, data_point_t **medal_country, int *medal_country_length, data_point_t **gold_medal_person, int *gold_medal_person_length, data_point_t **gold_medal_country, int *gold_medal_country_length) {
     /*
       Explores all child nodes if current node is not a the parent to leafs
      */
     if (root->level != 2) {
          for (node_t *tmp = root->child; tmp != NULL; tmp = tmp->sibling) {
               generate_medal_freq(tmp, medal_person, medal_person_length, medal_country, medal_country_length, gold_medal_person, gold_medal_person_length, gold_medal_country, gold_medal_country_length);
          }
     } else {
          if (root->child != NULL) {
               /*
                 Increments the data on gold medalists for first child
                */
               //It is assumed here that athetes represent one country and cannot represent many countries, which is true in the olympics
               increment_data_point(gold_medal_person, gold_medal_person_length, root->child->data);
               increment_data_point(gold_medal_country, gold_medal_country_length, get_country_str(root->child->data));

               /*
                 Increments data for medalists for every child
                */
               for (node_t *tmp = root->child; tmp != NULL; tmp = tmp->sibling) {
                    increment_data_point(medal_person, medal_person_length, tmp->data);
                    increment_data_point(medal_country, medal_country_length, get_country_str(tmp->data));
               }
          }
     }
}

/*
  Returns the string associated with the country of an athlete
 */
char *get_country_str(char *athlete) {
     /*
       Loops through string until colon is found, which indicates the country str is after it
      */
     for (int i = 0; i < strlen(athlete); i++) {
          if (athlete[i] == ':') {
               return athlete + i + 1;
          }
     }
     return NULL;
}

/*
  Comparison used for sorting algorithm
 */
int comparison(const void *a, const void *b) {
     data_point_t **dpa = ((data_point_t **) a);
     data_point_t **dpb = ((data_point_t **) b);
     int tmp = (*dpb)->data - (*dpa)->data;
     if (tmp == 0) {
          return strcmp((*dpa)->name, (*dpb)->name);
     }
     return tmp;
}

/*
  Frees every node in the tree
 */
void free_tree(node_t *root) {
     if (root == NULL) {
          return;
     }

     /*
       Frees every subtree connected to the root
      */
     for (node_t *tmp = root->child; tmp != NULL; tmp = tmp->sibling) {
          free_tree(tmp);
     }

     free(root->data);
     free(root);
}
