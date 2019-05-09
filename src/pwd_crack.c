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

  // if there are two arguments, test each of the passwords specified in
  // pwd_file against each of the hashes specified in hash_result
  if (pwd_file != NULL)
    compare_pwd(hash_result, hash_len, pwd_file);
  // else, generate guesses for hashes specified in hash_result
  // (number of guesses could be either infinite or finite)
  else
    search_pwd(hash_result, hash_len, num_guess);

  free_hash_result(hash_result, hash_len);
}

// helper functions
void search_pwd(BYTE** hash_result, const int hash_len, const int num_guess) {
  // TEST
  BYTE test[] = {"xiug"};
  BYTE buf[SHA256_BLOCK_SIZE];

  generate_sha256(test, buf);

  for(int i = 0; i < hash_len; i++) {
    if (memcmp(hash_result[i], buf, SHA256_BLOCK_SIZE) == 0)
      printf("xiug %d\n", i + 1);
  }
}

void compare_pwd(BYTE** hash_result, const int hash_len, const char* pwd_file) {
  FILE* fp;
  BYTE hash_buf[SHA256_BLOCK_SIZE];
  BYTE read_buf[BUFFER_SIZE] = "\0";
  bool has_next_line = false;

  fp = fopen(pwd_file, "r");
  if (fp == NULL) {
    perror("No such file");
    exit(EXIT_FAILURE);
  }

  // read password file line by line and test whether its in hash results
  do {
    has_next_line = get_next_line(fp, read_buf);

    if (*read_buf != '\n' && *read_buf != '\0') {
      generate_sha256(read_buf, hash_buf);

      // compare new hash result stored in hash_buf with all hash results
      for(int i = 0; i < hash_len; i++) {
        if (memcmp(hash_result[i], hash_buf, SHA256_BLOCK_SIZE) == 0)
          printf("%s %d\n", read_buf, i + 1);
      }
    }
  } while (has_next_line);

  fclose(fp);
}

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

bool get_next_line(FILE* fp, BYTE* buffer) {
  char ch = fgetc(fp);
  int count = 0;

  while (ch != '\n' && ch != EOF) {
    buffer[count] = ch;

    ch = fgetc(fp);
    count++;
  }

  buffer[count] = '\0';

  // return false if nothing being read but reach the end of string
  if (ch == EOF)
    return false;

  return true;
}
