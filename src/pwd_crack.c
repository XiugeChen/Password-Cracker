/*
  pwd_crack.c

  #### DESCRIPTION ###


  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#include "pwd_crack.h"
#include "sha256.h"

void crack_pwd(const char* hash_file, const char* pwd_file, const int num_guess) {
  unsigned char** hash_result = NULL;
  unsigned char buffer[BUFFER_SIZE];

  read_hash_file(hash_file, buffer);
  int res_len = store_hash_result(buffer, hash_result);

  free_hash_result(hash_result, res_len);

  
}


// helper functions
void read_hash_file(const char* hash_file, unsigned char* buffer) {
  FILE *fp;

  fp = fopen(hash_file, "rb");
  if (fp == NULL) {
    perror("No such file");
    exit(EXIT_FAILURE);
  }

  // get the file length and return back to the beginning
  fseek(fp, 0, SEEK_END);
  int filelen = ftell(fp);
  rewind(fp);

  fread(buffer, sizeof(unsigned char), filelen, fp);

  // append string terminator char at the end of string
  int end_index = filelen / NUM_BIT_PER_BYTE;
  buffer[end_index] = '\0';

  fclose(fp);
}

int store_hash_result(unsigned char* buffer, unsigned char** hash_result) {
  unsigned char* cur = buffer;
  // get the number of hash results
  int num_hash = strlen((char*)buffer) * NUM_BIT_PER_BYTE / SHA256_BLOCK_SIZE;

  // allocate memory for hash results array
  hash_result = (unsigned char**) malloc(num_hash * sizeof(unsigned char*));
  for (int i = 0; i < num_hash; i++) {
    hash_result[i] = (unsigned char*) malloc(SHA256_BLOCK_SIZE * sizeof(unsigned char));

    // store next SHA256_BLOCK_SIZE number of bytes from buffer into hash_result
    for (int j = 0; j < SHA256_BLOCK_SIZE / NUM_BIT_PER_BYTE; j++) {
      hash_result[i][j] = *cur;
      cur++;
    }
  }

  return num_hash;
}

void free_hash_result(unsigned char** hash_result, int res_len) {
  if (hash_result != NULL) {
    for (int i = 0; i < res_len; i++) {
      if (hash_result[i] != NULL)
        free(hash_result[i]);
    }
    free(hash_result);
  }
}
