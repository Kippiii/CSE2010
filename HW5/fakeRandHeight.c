/*

  Author: Phil Chan
  Email: pkc@cs.fit.edu
  Description:  

  For easier debugging and testing, this provides a known sequence of
  fake random height for HW5 in CSE2010.  The sequence simulates each
  successive higher level is half as likely to be used in
  put(skiplist, ...).

  The sequence has 31 numbers: 0 occurs 16 times, 1 occurs 8 times, 2
  occurs 4 times, 3 occurs 2 times, and 4 occurs 1 time.  Each number
  is returned by getRandHeight() in the following order and the
  numbers are recycled if getRandHeight() is called more than 31
  times.

                                1 1 1 1 1 1
  index:    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5

  sequence: 0 1 0 2 0 1 0 3 0 1 0 2 0 1 0 4

            1 1 1 1 2 2 2 2 2 2 2 2 2 2 3
  index:    6 7 8 9 0 1 2 3 4 5 6 7 8 9 0

  sequence: 0 1 0 2 0 1 0 3 0 1 0 2 0 1 0  

 */




#include "fakeRandHeight.h"

// return the next number in the sequence, recycle the sequence if needed
int getRandHeight()
{
  static int height[] = {0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, // sequence of height
                         0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};
  
  static int count=0;  // number of times getRandHeight() has been called 

  return height[count++ % 31];
}

