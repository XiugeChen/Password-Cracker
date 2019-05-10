/*
  common_sub.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/
#ifndef COMMON_SUB_H
#define COMMON_SUB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"

// constant
#define MAX_SUB 64
#define NUM_RULES 26
static const char CHARS[NUM_RULES] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
	'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
	'x', 'y', 'z'};
static const char SUBS[NUM_RULES][MAX_SUB] = {
	{'A', '@'}, // a
	{'B'}, // b
	{'C'}, // c
	{'D'}, // d
	{'E', '&', '3'}, // e
	{'F'}, // f
	{'G', 'j'}, // g
	{'H'}, // h
	{'I', '1', '|'}, // i
	{'J'}, // j
	{'K'}, // k
	{'L', '1', '|', 'I'}, // l
	{'M'}, // m
	{'N', 'm', 'h'}, // n
	{'O', '0', 'Q'}, // o
	{'P'}, // p
	{'Q'}, // q
	{'R', '2'}, // r
	{'S', '$'}, // s
	{'T', '7'}, // t
	{'U', 'v', 'V'}, // u
	{'V', 'u', 'U'}, // v
	{'W'}, // w
	{'X'}, // x
	{'Y'}, // y
	{'Z'} // z
};

// data structure
typedef struct {
	BYTE c;
	BYTE sub[MAX_SUB];
} COMMON_SUB_MAP;

/*
  initialize the map
*/
void init_sub_map(COMMON_SUB_MAP** map);

/*
  get all common substitution of a char
*/
void get_sub(COMMON_SUB_MAP* map, BYTE c, BYTE* sub);

/*
  free the map
*/
void free_sub_map(COMMON_SUB_MAP* map);

#endif
