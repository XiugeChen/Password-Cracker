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

#include "sha256.h"

// constant
#define BUFFER_SIZE 1024
// number of bits of each byte
#define NUM_BIT_PER_BYTE 8

/*
  public interface using to call password cracker
  INPUT: hash_file: mandatory, the file contains all SHA256 hash results (32 bytes for each)
        pwd_file: optional, the file contains all passwords in text,
                  if specified cracker will check each password with hash results
        num_guess: optional, the number of guesses will be performed
*/
void crack_pwd(const char* hash_file, const char* pwd_file, const int num_guess);

// helper functions
/*
  search passwords
*/
void search_pwd(BYTE** hash_result, const int hash_len, const char* pwd_file,
  const int num_guess);
/*
  read hash results file into buffer and return the length of file in bytes
*/
int read_hash_file(const char* hash_file, BYTE* buffer);

/*
  store the hash result in buffer into 2D array, return the number of hash result
*/
int store_hash_result(BYTE* buffer, int byte_len, BYTE*** hash_result);

/*
  free the result being stored in hash_result
*/
void free_hash_result(BYTE** hash_result, int res_len);

#endif
