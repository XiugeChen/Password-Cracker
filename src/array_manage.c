/*
  array_manage.c

  #### DESCRIPTION ###

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#include <array_manage.h>

BYTE** alloc_2d_byte(int total_len, int element_len) {
  BYTE** array = (BYTE**)malloc(total_len * sizeof(BYTE*));
  for (int i = 0; i < total_len; i++)
    array[i] = (BYTE*)malloc(element_len * sizeof(BYTE));

  return array;
}

void free_2d_byte(BYTE** array, int pwd_len) {
  for (int i = 0; i < pwd_len; i++)
    free(array[i]);
  free(array);
}
