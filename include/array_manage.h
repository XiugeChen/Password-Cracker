/*
  array_manage.h

  #### DESCRIPTION ###
  array managment

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#ifndef ARRAY_MANAGE_H
#define ARRAY_MANAGE_H

#include <stdlib.h>
#include <sha256.h>
#include <stdbool.h>
#include <string.h>

BYTE** alloc_2d_byte(int total_len, int element_len);

/*
  free 2d BYTE array
*/
void free_2d_byte(BYTE** array, int len);

/*
  check whether an element is already in array
*/
bool check_element(BYTE** array, BYTE* elem, int len_array);

/*
  add element to array
*/
bool add_element(BYTE** array, BYTE* elem, int len_array);

#endif
