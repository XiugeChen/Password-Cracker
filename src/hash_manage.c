/*
  hash_manage.c

  #### DESCRIPTION ###


  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/
#include "hash_manage.h"

void generate_sha256(BYTE* text, BYTE* buffer) {
  SHA256_CTX ctx;

  sha256_init(&ctx);
  sha256_update(&ctx, text, strlen((char*)text));
  sha256_final(&ctx, buffer);
}

int read_hash_file(const char* hash_file, BYTE* buffer) {
  FILE* fp;

  fp = fopen(hash_file, "rb");
  if (fp == NULL) {
    perror("No such file");
    exit(EXIT_FAILURE);
  }

  // get the file length and return back to the beginning
  fseek(fp, 0, SEEK_END);
  int filelen = ftell(fp);
  rewind(fp);

  int n = fread(buffer, sizeof(BYTE), filelen, fp);

  // append string terminator char at the end of string
  buffer[n] = '\0';

  fclose(fp);

  return n;
}

int store_hash_result(BYTE* buffer, int byte_len, BYTE*** hash_result) {
  BYTE* cur = buffer;

  // get the number of hash results
  int num_hash = byte_len / SHA256_BLOCK_SIZE;

  // allocate memory for hash results array
  *hash_result = (BYTE**) malloc(num_hash * sizeof(BYTE*));
  for (int i = 0; i < num_hash; i++) {
    (*hash_result)[i] = (BYTE*) malloc(SHA256_BLOCK_SIZE * sizeof(BYTE));

    // store next SHA256_BLOCK_SIZE number of bytes from buffer into hash_result
    for (int j = 0; j < SHA256_BLOCK_SIZE; j++) {
      (*hash_result)[i][j] = *cur;
      cur++;
    }
  }

  return num_hash;
}

void free_hash_result(BYTE** hash_result, int res_len) {
  if (hash_result != NULL) {
    for (int i = 0; i < res_len; i++) {
      if (hash_result[i] != NULL)
        free(hash_result[i]);
    }
    free(hash_result);
  }
}
