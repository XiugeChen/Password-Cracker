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

void crack_pwd(const char* hash_file, const char* pwd_file, const int num_guess) {
  BYTE** hash_result = NULL;
  BYTE buffer[BUFFER_SIZE];

  int byte_len = read_hash_file(hash_file, buffer);
  int hash_len = store_hash_result(buffer, byte_len, &hash_result);

  search_pwd(hash_result, hash_len, pwd_file, num_guess);

  free_hash_result(hash_result, hash_len);
}

// helper functions
void search_pwd(BYTE** hash_result, const int hash_len, const char* pwd_file,
  const int num_guess) {

  // TEST
  printf("test start!\n");
  SHA256_CTX ctx;
  BYTE test[] = {"xiug"};
  BYTE buf[SHA256_BLOCK_SIZE];

  sha256_init(&ctx);
  sha256_update(&ctx, test, strlen((char*)test));
  sha256_final(&ctx, buf);

  for(int i = 0; i < hash_len; i++) {
    if (memcmp(hash_result[i], buf, SHA256_BLOCK_SIZE) == 0)
      printf("xiug %d\n", i + 1);
  }
}

int read_hash_file(const char* hash_file, BYTE* buffer) {
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

  fread(buffer, sizeof(BYTE), filelen, fp);

  // append string terminator char at the end of string
  buffer[filelen] = '\0';

  fclose(fp);

  return filelen;
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
