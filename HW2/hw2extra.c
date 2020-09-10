/*

  Author: Ian Orzel
  Email: iorzel2019@my.fit.edu
  Course: CSE2010
  Section: 23

  Description of this file: This program takes a list of classes with possible times for that class as the input.
  With this information, the program generates the best possible schedule while prioritizing the number of classes,
  then the priority of classes, and then the priority of times. Once the best schedule has been generated, all of
  the classes that are in the schedule are printed along with a list of all of the classes that could not be added.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/*
  Structure for storing the information related to a class object
 */
typedef struct {
     char *name;         /* Stores a pointer to the string with the name of the class */
     char *time;         /* Stores a pointer to the string with the time a class is taking place */
     int class_priority; /* Stores the number representing the priority given to this particular class */
     int time_priority;  /* Stores the number representing the priority given to this particular time of the class */
} class_t;

/*
  Structure for storing a schedule and also for storing all the class times for a given class initially
 */
typedef struct {
     int n;               /* Stores the number of classes that are currently in this schedule */
     class_t **classes;   /* Stores a pointer to an array of pointers to the classes associated with this schedule */
} schedule_t;

void generate_schedules(schedule_t **classes, int class_count, schedule_t **best_schedule);
int increment_array(schedule_t **classes, int class_count, int class_list[]);
int compare_schedules(schedule_t *s1, schedule_t *s2);
int can_add_class(schedule_t schedule, class_t *class);
int strip_string(char *str);

/*
  At the top of each function, describe the function, parameter(s), and return value (if any)
 */

int main(int argc, char* argv[])
{
  /*
    description of declared variables, with meaningful names
   */
   FILE *in = fopen(argv[1], "r");                                         /* Stores the file object for the text file to be read from */
   schedule_t *best_schedule = (schedule_t *) malloc(sizeof(schedule_t));  /* Stores the object for storing the best schedule that can possibly be made */

   /*
     Generates structure for storing the list of possible classes
    */
   int max_length = 20;                                                                   /* Stores the current maximum size of the class list */
   int n = 0;                                                                             /* Stores the current number of classes that have been accounted for */
   schedule_t **class_list = (schedule_t **) malloc(sizeof(schedule_t *) * max_length);   /* Schedule array that will store the schedules where all of the classes will be saved */
   int end_of_file = 0;                                                                   /* Flag that detects when the end of the file has been reached */

   /*
     Loops for every class that is put into the input file
    */
   while (!end_of_file) {
        char *class_str = (char *) malloc(sizeof(char) * 9);     /* Stores the string for the name of the class */
        char *class_get = fgets(class_str, 9, in);               /* Used to check for when EOF is reached */
        int strip = strip_string(class_str);                     /* Flag that stores whether newline or end of file have been reached */

        /* Stops the while loop if EOF is reached and skips lines with a newline character */
        if (class_get == NULL) {
             end_of_file = 1;
        } else if (strip == 0) {
             /*
               Initializes a schedule for the class
              */
             class_list[n] = (schedule_t *) malloc(sizeof(schedule_t));
             class_list[n]->n = 0;
             int class_max_length = 20;
             if (n >= max_length) {
                  /*
                    Reallocs array if it goes over max size
                   */
                  max_length *= 2;
                  class_list = realloc(class_list, max_length);
             }
             class_list[n]->classes = (class_t **) malloc(sizeof(class_t *) * class_max_length);
             n++;
             int class_end_flag = 0;    /* Flag that stores whether there is more information to read in about the class */
             /*
               Loops for every time slot that is in the file for the given class
              */
             while (!class_end_flag) {
                  char *time_str = (char *) malloc(sizeof(char) * 8); /* Stores the time of the class */
                  char *time_get = fgets(time_str, 8, in);            /* Helps check for EOF */
                  int strip_time = strip_string(time_str);            /* Flag that stores whether newline or end of file have been read after getting the time */

                  /*
                    This runs if a valid time has been entered and adds that time to the schedule object
                   */
                  if (strlen(time_str) == 7 || strlen(time_str) == 6) {
                       class_t *current_time = (class_t *) malloc(sizeof(class_t));  /* Stores the object for the current class object that stores the time */
                       current_time->name = class_str;
                       current_time->time = time_str;
                       current_time->class_priority = n;
                       current_time->time_priority = class_list[n-1]->n + 1;
                       if (class_list[n-1]->n >= class_max_length) {
                            /*
                              Reallocs the array of class times
                             */
                            class_max_length *= 2;
                            class_list[n-1]->classes = realloc(class_list[n-1]->classes, class_max_length);
                       }
                       class_list[n-1]->classes[class_list[n-1]->n++] = current_time;

                       /*
                         Deletes leftover space if the length of the time was seven characters
                        */
                        if (strlen(time_str) == 7) {
                            char space[2];
                            char *space_get = fgets(space, 2, in);    /* Used to check for EOF */
                            if (space[0] == '\n') {
                                 strip_time = 1;
                            } else if (space_get == NULL) {
                                 strip_time = 2;
                            }
                       }
                  }

                  /*
                    Checks if the end of the line or the end of the file have been reached and adjusts accordingly
                   */
                  if (strip_time == 1) {
                       class_end_flag = 1;
                  } else if (strip_time == 2 || time_get == NULL) {
                       class_end_flag = 1;
                       end_of_file = 1;
                  }
             }
        }
   }

   /*
     Uses the recursive function in order to generate the best schedule
    */
   generate_schedules(class_list, n, &best_schedule);

   /*
     Prints the course schedule into the output
    */
   printf("---Course schedule---\n");
   for (int i = 0; i < best_schedule->n; i++) {
        printf("%s %s\n", best_schedule->classes[i]->name, best_schedule->classes[i]->time);
   }

   /*
     Prints all information about classes that could not be added to the schedule
    */
   printf("---Courses with a time conflict---\n");
   for (int i = 0, j = 0; i < n; i++) {
        if (strcmp(best_schedule->classes[j]->name, class_list[i]->classes[0]->name) != 0) {
             printf("%s", class_list[i]->classes[0]->name);
             for (int k = 0; k < class_list[i]->n; k++) {
                  printf(" %s", class_list[i]->classes[k]->time);
             }
             printf("\n");
        } else {
             j++;
        }
   }
   if (best_schedule->n == n) {
        printf("None\n");
   }

   /*
     Frees all of the the dynamic memory that has been allocated.
    */
   free(best_schedule->classes);
   free(best_schedule);
   for (int i = 0; i < n; i++) {
        free(class_list[i]->classes[0]->name);
        for (int j = 0; j < class_list[i]->n; j++) {
             free(class_list[i]->classes[j]->time);
             free(class_list[i]->classes[j]);
        }
        free(class_list[i]->classes);
   }
   free(class_list);
}

/*
  This function generates all possible valid schedules given all of the information about the classes and an empty array for storing the schedules (generates classes with same times)
 */
void generate_schedules(schedule_t **classes, int class_count, schedule_t **best_schedule) {
     int *cur_class_list = (int *) calloc(class_count, sizeof(int));     /* Stores the indexes of the classes for the schedule as numbers */
     /*
       Loops through every possible combination of schedule that can be made
      */
     do {
          schedule_t schedule;               /* Stores the schedule object that stores the currently being made schedule */
          schedule.n = 0;
          schedule.classes = (class_t **) malloc(sizeof(class_t *) * class_count);
          int schedule_made_flag = 1;        /* Flag that shows whether the schedule is able to be made */
          int index = 0;                     /* Stores the current index where the next class gets added to the array */
          /*
            Loops through every class and adds the respective class to the array
           */
          for (int i = 0; i < class_count; i++) {
               if (cur_class_list[i] != classes[i]->n) {
                    if (can_add_class(schedule, classes[i]->classes[cur_class_list[i]])){
                         schedule.classes[index++] = classes[i]->classes[cur_class_list[i]];
                         schedule.n++;
                    } else {
                         schedule_made_flag = 0;
                         break;
                    }
               }
          }

          /*
            Copies the schedule into memory location of best_schedule if it is a better schedule
           */
          if (schedule_made_flag) {
               if (compare_schedules(&schedule, *best_schedule) > 0) {
                    memcpy(*best_schedule, &schedule, sizeof(schedule_t));
                    (*best_schedule)->classes = (class_t **) malloc(sizeof(class_t **) * (*best_schedule)->n);
                    memcpy((*best_schedule)->classes, schedule.classes, sizeof(class_t *) * (*best_schedule)->n);
               }
          }
          free(schedule.classes);
     } while(increment_array(classes, class_count, cur_class_list));
}

/*
  Increments elements in array supposing it is a number. Returns false if it is at its maximum
 */
int increment_array(schedule_t **classes, int class_count, int class_list[]) {
     int carry = 1;      /* Flag that stores whether the next element needs to be incrmented */
     for (int i = 0; i < class_count && carry; i++) {
          class_list[i] = (class_list[i] + 1) % (classes[i]->n + 1);
          if (class_list[i] != 0) {
               carry = 0;
          }
     }
     return !carry;
}

/*
  Compares two schedules to each other to see if one is better than the other
 */
int compare_schedules(schedule_t *s1, schedule_t *s2) {
     /*
       Checks if preference can be determined based on the number of classes
      */
     if (s1->n != s2->n) {
          return s1->n - s2->n;
     } else {
          /*
            Checks if preference can be determined based on classes with the highest priority
           */
          for (int i = 0; i < s1->n; i++) {
               if (s1->classes[i]->class_priority != s2->classes[i]->class_priority) {
                    return s2->classes[i]->class_priority - s1->classes[i]->class_priority;
               }
          }

          /*
            Checks if preference can be determined based on times with the highest priority
           */
          for (int i = 0; i < s1->n; i++) {
               if (s1->classes[i]->time_priority != s2->classes[i]->time_priority) {
                    return s2->classes[i]->time_priority - s1->classes[i]->time_priority;
               }
          }
     }

     return 0;
}

/*
  Checks whether a certain class can be added into a certain schedule
 */
int can_add_class(schedule_t schedule, class_t *class) {
     /*
       Loops through every class in the schedule and checks for time conflict
      */
     for (int i = 0; i < schedule.n; i++) {
          if (strcmp(schedule.classes[i]->time, class->time) == 0) {
               return 0;
          }
     }
     return 1;
}

/*
  Strips the whitespace from a given string and then returns whether it hits a newline or end of file character
 */
int strip_string(char *str) {
     int len = strlen(str);   /* Stores the length of the string being analyzed */
     int return_value = 0;    /* Stores the value that will be returned at the end of the function */
     /*
       Loops through the end of the string in order to eliminate all whitespace and check if the end of the line or the end of the file were reached
      */
     while (isspace(str[len - 1])) {
          if (str[len - 1] == '\n') {
               return_value = 1;
          }
          str[len - 1] = '\0';
     }
     return return_value;
}
