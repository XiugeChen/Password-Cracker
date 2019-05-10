/*
  common_sub.c

  #### DESCRIPTION ###
  common substitution of single character

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/
#include "common_sub.h"

void init_sub_map(COMMON_SUB_MAP** map) {
  if (*map != NULL)
    free_sub_map(*map);

  *map = (COMMON_SUB_MAP*)malloc(NUM_RULES * sizeof(COMMON_SUB_MAP));

  for (int i = 0; i < NUM_RULES; i++) {
    (*map)[i].c = CHARS[i];
    strcpy((char*)(*map)[i].sub, SUBS[i]);
  }
}

void get_sub(COMMON_SUB_MAP* map, BYTE c, BYTE* sub) {
  if (map == NULL)
    return;

  for (int i = 0; i < NUM_RULES; i++) {
    if (map[i].c == c)
      strcpy((char*)sub, (char*)map[i].sub);
  }
}

void free_sub_map(COMMON_SUB_MAP* map) {
  free(map);
}
