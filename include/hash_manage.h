/*
  hash_manage.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#ifndef HASH_MANAGE_H
#define HASH_MANAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"

/*
  given a plain text of string, generate sha256 result and store it in buffer
*/
void generate_sha256(BYTE* text, BYTE* buffer);

/*
  read all hash results from file into buffer
  return the length of file in bytes
*/
int read_hash_file(const char* hash_file, BYTE* buffer);

/*
  store the hash result in buffer into 2D array, return the number of hash results
*/
int store_hash_result(BYTE* buffer, int byte_len, BYTE*** hash_result);

/*
  free the result being stored in hash_result
*/
void free_hash_result(BYTE** hash_result, int res_len);

#endif
