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

void crack_pwd(const char* hash_file, const char* pwd_file, const int num_guess,
  const int pwd_len) {
  BYTE** hash_result = NULL;
  BYTE buffer[BUFFER_SIZE];

  int byte_len = read_hash_file(hash_file, buffer);
  int hash_len = store_hash_result(buffer, byte_len, &hash_result);

  // if there are two arguments, test each of the passwords specified in
  // pwd_file against each of the hashes specified in hash_result
  // and the number of guesses should be set as unlimited (-1)
  if (pwd_file != NULL)
    compare_pwd(hash_result, hash_len, pwd_file);
  // else, generate guesses for hashes specified in hash_result
  // (number of guesses could be either infinite or finite)
  // For detailed description, please refer to README.
  else
    search_pwd(hash_result, hash_len, num_guess, pwd_len);

  free_hash_result(hash_result, hash_len);
}

// helper functions
void search_pwd(BYTE** hash_result, const int hash_len, const int num_guess,
  const int pwd_len) {

  int left_guess = num_guess;

  // try flexible dictionary attack and smart brute force attack first
  if (!dict_attack(hash_result, hash_len, pwd_len, &left_guess, DICT_FILE_PATH))
    return;

  // then try lazy brute force attack
  if (!lazy_bf_attack(hash_result, hash_len, &left_guess, pwd_len))
    return;
}

void compare_pwd(BYTE** hash_result, const int hash_len, const char* pwd_file) {
  FILE* fp;

  fp = fopen(pwd_file, "r");
  if (fp == NULL) {
    perror("No such file");
    exit(EXIT_FAILURE);
  }

  BYTE read_buf[BUFFER_SIZE] = "\0";
  bool has_next_line = false;

  // read password file line by line and test whether it matches hash results
  // if reach the limits of number of guesses, stop
  do {
    has_next_line = get_next_line(fp, read_buf);

    // if there is password being read
    if (*read_buf != '\n' && *read_buf != '\0')
      check_match(hash_result, hash_len, read_buf);

  } while (has_next_line);

  fclose(fp);
}

bool dict_attack(BYTE** hash_result, const int hash_len, const int pwd_len,
  int* left_guess, const char* dict_file) {

    if (pwd_len <= 0) {
      perror("Can't attack without specified password length");
      exit(EXIT_FAILURE);
    }

    FILE* fp;
    fp = fopen(dict_file, "r");
    if (fp == NULL) {
      perror("No such file");
      exit(EXIT_FAILURE);
    }

    BYTE read_buf[BUFFER_SIZE] = "\0";
    BYTE* password = (BYTE*)malloc((pwd_len + 1) * sizeof(BYTE));
    bool has_next_line = false;
    password[pwd_len] = '\0';

    // read dictionary file line by line, extract all substrings with length
    // pwd_len for each password, and then test whether matches hash results.
    // Why: human tend to use substring of name, words... to generate passwords
    // that are easy to memorize)
    // if reach the limits of number of guesses, stop
    do {
      has_next_line = get_next_line(fp, read_buf);

      // if there is password being read
      if (*read_buf != '\n' && *read_buf != '\0') {
        int read_buf_len = strlen((char*)read_buf);

        // only check those passwords have same or greater than specified length
        if (pwd_len <= read_buf_len) {
          // for each start point of substring
          for (int i = 0; i <= read_buf_len - pwd_len; i++) {
            // get the whole substring stored in password
            for (int j = 0; j < pwd_len; j++)
              password[j] = read_buf[i + j];

            // reach the limits of number of guesses
            if (!smart_bf_attack(hash_result, hash_len, password, left_guess)) {
              fclose(fp);
              free(password);
              return false;
            }
          }
        }
        // for passwords have shorter length than specified, fill it with blank
        // spaces and do smart brute force attack
        else {
          strcpy((char*)password, (char*)read_buf);

          for (int i = 0; i < pwd_len - read_buf_len; i++)
            password[read_buf_len + i] = ' ';

          if (!smart_bf_attack(hash_result, hash_len, password, left_guess)) {
            fclose(fp);
            free(password);
            return false;
          }
        }
      }
    } while (has_next_line && *left_guess != 0);

    fclose(fp);
    free(password);
    return true;
}

bool smart_bf_attack(BYTE** hash_result, const int hash_len, BYTE* password,
  int* left_guess) {

  int pwd_len = strlen((char*)password);
  BYTE buffer[MAX_SUB] = "\0";
  COMMON_SUB_MAP* map = NULL;
  BYTE** candidate_chars = (BYTE**)malloc(pwd_len * sizeof(BYTE*));

  for (int i = 0; i < pwd_len; i++)
    candidate_chars[i] = (BYTE*)malloc(MAX_SUB * sizeof(BYTE));

  init_sub_map(&map);

  // for all characters in the password, get its common substitution
  for (int i = 0; i < pwd_len; i++) {
    BYTE c = password[i];
    get_sub(map, c, buffer);

    int buffer_len = strlen((char*)buffer);

    buffer[buffer_len] = c;
    buffer[buffer_len + 1] = '\0';

    strcpy((char*)candidate_chars[i], (char*)buffer);
  }

  free_sub_map(map);

  // try to generate brute force attack on all common substitution of password
  if (!bf_search_match(hash_result, hash_len, left_guess, candidate_chars, pwd_len)) {
    for (int i = 0; i < pwd_len; i++)
      free(candidate_chars[i]);
    free(candidate_chars);
    return false;
  }

  for (int i = 0; i < pwd_len; i++)
    free(candidate_chars[i]);
  free(candidate_chars);
  return true;
}

// only support password length 4
bool lazy_bf_attack(BYTE** hash_result, const int hash_len, int* left_guess,
  const int pwd_len) {

  int num_chars = END_CHAR - STRAT_CHAR + 1;
  BYTE** candidate_chars = (BYTE**)malloc(pwd_len * sizeof(BYTE*));

  for (int i = 0; i < pwd_len; i++)
    candidate_chars[i] = (BYTE*)malloc((num_chars + 1) * sizeof(BYTE));

  // generate all possible chars
  for (int i = 0; i < pwd_len; i++) {
    for (int j = 0; j < num_chars; j++) {
      candidate_chars[i][j] = STRAT_CHAR + j;
    }
    candidate_chars[i][num_chars] = '\0';
  }

  // try to generate brute force attack on all common substitution of password
  if (!bf_search_match(hash_result, hash_len, left_guess, candidate_chars, pwd_len)) {
    for (int i = 0; i < pwd_len; i++)
      free(candidate_chars[i]);
    free(candidate_chars);
    return false;
  }

  for (int i = 0; i < pwd_len; i++)
    free(candidate_chars[i]);
  free(candidate_chars);
  return true;
}

bool bf_search_match(BYTE** hash_result, const int hash_len, int* left_guess,
  BYTE** candidate_chars, const int pwd_len) {

  int total_size = 1;
  BYTE* new_pwd = (BYTE*)malloc((pwd_len + 1) * sizeof(BYTE));

  // get the total size of possible combinations
  for (int i = 0; i < pwd_len; i++)
    total_size *= strlen((char*)candidate_chars[i]);

  for (int i = 0; i < total_size; i++) {
    int range = total_size;

    // for each possible combination, generate the corresponding passwords
    for (int j = 0; j < pwd_len; j++) {
      int len = strlen((char*)candidate_chars[j]);
      range /= len;
      // avoid float point exception
      int choice = len > 1 ? (i / range % len) : 0;
      new_pwd[j] = candidate_chars[j][choice];
    }
    new_pwd[pwd_len] = '\0';

    check_match(hash_result, hash_len, new_pwd);

    // reach the limits of number of guesses
    (*left_guess)--;
    if (*left_guess == 0) {
      free(new_pwd);
      return false;
    }
  }

  free(new_pwd);
  return true;
}

void check_match(BYTE** hash_result, const int hash_len, BYTE* password) {
  BYTE hash_buf[SHA256_BLOCK_SIZE];

  generate_sha256(password, hash_buf);
  // compare new hash result stored in hash_buf with all hash results
  for(int i = 0; i < hash_len; i++) {
    if (memcmp(hash_result[i], hash_buf, SHA256_BLOCK_SIZE) == 0)
      printf("%s %d\n", password, i + 1);
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
