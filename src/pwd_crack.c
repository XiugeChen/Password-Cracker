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
  int default_guess = -1;

  int byte_len = read_hash_file(hash_file, buffer);
  int hash_len = store_hash_result(buffer, byte_len, &hash_result);

  // if there are two arguments, test each of the passwords specified in
  // pwd_file against each of the hashes specified in hash_result
  // and the number of guesses should be set as unlimited (-1)
  if (pwd_file != NULL)
    compare_pwd(hash_result, hash_len, pwd_file, &default_guess);
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
  dict_attack(hash_result, hash_len, pwd_len, &left_guess, DICT_FILE_PATH);
  if (num_guess == 0)
    return;

  // then try lazy brute force attack
  // lazy_BF_attack(hash_result, hash_len, &left_guess, pwd_len);
}

void compare_pwd(BYTE** hash_result, const int hash_len, const char* pwd_file,
  int* left_guess) {

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
    if (*read_buf != '\n' && *read_buf != '\0') {
      check_match(hash_result, hash_len, read_buf);
      (*left_guess)--;
    }
  } while (has_next_line && *left_guess != 0);

  fclose(fp);
}

void dict_attack(BYTE** hash_result, const int hash_len, const int pwd_len,
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
            for (int j = 0; j < pwd_len; j++) {
              password[j] = read_buf[i + j];
            }

            check_match(hash_result, hash_len, password);

            // reach the limits of number of guesses
            (*left_guess)--;
            if (*left_guess == 0)
              break;

            smart_BF_attack(hash_result, hash_len, password, left_guess);
          }
        }
        // for passwords have shorter length than specified, fill it with blank
        // spaces and do smart brute force attack
        else {
          strcpy((char*)password, (char*)read_buf);

          for (int i = 0; i < pwd_len - read_buf_len; i++) {
            password[read_buf_len + i] = ' ';
          }

          smart_BF_attack(hash_result, hash_len, password, left_guess);
        }
      }
    } while (has_next_line);

    fclose(fp);
    free(password);
}

void smart_BF_attack(BYTE** hash_result, const int hash_len, BYTE* password,
  int* left_guess) {

  BYTE backup_buffer[BUFFER_SIZE];
  BYTE c;
  BYTE sub[MAX_SUB] = "\0";
  COMMON_SUB_MAP* map = NULL;

  init_sub_map(&map);
  strcpy((char*)backup_buffer, (char*)password);

  // for all characters in the password, get its common substitution and replace
  // it, then try to match it
  for (int pwd_i = 0; pwd_i < strlen((char*)password); pwd_i++) {
    c = password[pwd_i];
    get_sub(map, c, sub);

    if (*sub != '\0') {
      for (int sub_i = 0; sub_i < strlen((char*)sub); sub_i++) {
        password[pwd_i] = sub[sub_i];
        check_match(hash_result, hash_len, password);

        // recover the password
        strcpy((char*)password, (char*)backup_buffer);

        // reach the limits of number of guesses
        (*left_guess)--;
        if (*left_guess == 0)
          break;
      }
    }

    *sub = '\0';
  }

  free_sub_map(map);
}

// only support password length 4
void lazy_BF_attack(BYTE** hash_result, const int hash_len, int* left_guess,
  const int pwd_len) {

  // only brute force when the length of passwords has been specified
  if (pwd_len != 4) {
    fprintf(stderr, "Can't brute force on password length other than 4");
    return;
  }

  BYTE hash_buf[SHA256_BLOCK_SIZE];
  BYTE* password = (BYTE*)malloc((pwd_len + 1) * sizeof(BYTE));
  password[pwd_len] = '\0';

  // interval between end char and start char
  int total_interval = END_CHAR - STRAT_CHAR;

  for (int first = 0; first < total_interval; first++) {
    for (int second = 0; second < total_interval; second++) {
      for (int third = 0; third < total_interval; third++) {
        for (int forth = 0; forth< total_interval; forth++) {

          password[0] = STRAT_CHAR + first;
          password[1] = STRAT_CHAR + second;
          password[2] = STRAT_CHAR + third;
          password[3] = STRAT_CHAR + forth;

          generate_sha256(password, hash_buf);

          // compare new hash result stored in hash_buf with all hash results
          for(int i = 0; i < hash_len; i++) {
            if (memcmp(hash_result[i], hash_buf, SHA256_BLOCK_SIZE) == 0)
              printf("%s %d\n", password, i + 1);
          }

          (*left_guess)--;
          if (*left_guess == 0) {
            free(password);
            return;
          }
        }
      }
    }
  }

  free(password);
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
