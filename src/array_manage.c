/*
  array_manage.c

  #### DESCRIPTION ###
  array managment

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#include <array_manage.h>

BYTE** alloc_2d_byte(int total_len, int element_len) {
  BYTE** array = (BYTE**)malloc(total_len * sizeof(BYTE*));
  for (int i = 0; i < total_len; i++) {
    array[i] = (BYTE*)malloc(element_len * sizeof(BYTE));
    array[i][0] = '\0';
  }

  return array;
}

void free_2d_byte(BYTE** array, int len) {
  for (int i = 0; i < len; i++)
    free(array[i]);
  free(array);
}

bool check_element(BYTE** array, BYTE* elem, int len_array) {
  for (int i = 0; i < len_array; i++) {
    if (strcmp((char*)array[i], (char*)elem) == 0)
      return true;
  }
  return false;
}

bool add_element(BYTE** array, BYTE* elem, int len_array) {
  for (int i = 0; i < len_array; i++) {
    if (array[i][0] == '\0') {
      strcpy((char*)array[i], (char*)elem);
      return true;
    }
  }
  return false;
}
