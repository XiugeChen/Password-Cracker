/*
  pwd_crack.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#ifndef PWD_CRACK_H
#define PWD_CRACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constant
#define BUFFER_SIZE 1024
// number of bits of each byte
#define NUM_BIT_PER_BYTE 8

/*
  interface
*/
void crack_pwd(const char* hash_file, const char* pwd_file, const int num_guess);

// helper functions
/*

*/
void read_hash_file(const char* hash_file, unsigned char* buffer);

/*
  store the hash result in buffer into 2D array
*/
int store_hash_result(unsigned char* buffer, unsigned char** hash_result);

/*
  free the result being stored in hash_result
*/
void free_hash_result(unsigned char** hash_result, int res_len);

#endif
