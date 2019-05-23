/*
  pwd_crack.c

  #### DESCRIPTION ###
  Contains all strategies being used in password cracking, including dictionary
  attack and smart/lazy brute force attack.
  Also provide interface "crack_pwd" to call them automatically.

  For detailed description please refer to pwd_crack.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#include "pwd_crack.h"

/**** Global Variable ****/
// used for check whether hash results being attacked
int CRACK_HASHS[1024] = {0};
BYTE** GENERATE_PWD = NULL;
int NUM_PWD = 0;
// brute force switch
bool USE_BF = false;

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
  bool keep_execute = true;
  if (num_guess > 0) {
    NUM_PWD = num_guess;
    GENERATE_PWD = alloc_2d_byte(NUM_PWD, pwd_len);
  }

  // try dictionary attack
  if (keep_execute && !dict_attack(hash_result, hash_len, pwd_len, &left_guess, DICT_FILE_PATH))
    keep_execute = false;

  for (int sub_limit = 1; sub_limit <= NUM_SUB; sub_limit++) {
    // then try smart brute force attack (replace each char with its common substitution and upper)
    if (keep_execute && !smart_bf_attack(hash_result, hash_len, pwd_len, &left_guess, DICT_FILE_PATH, sub_limit))
      keep_execute = false;
  }

  // then try lazy brute force attack
  // try brute force on numbers only first
  if (keep_execute && !lazy_bf_attack(hash_result, hash_len, &left_guess, pwd_len, NUMBER_LIST))
    keep_execute = false;

  // then try brute force on lowercase characters only
  if (keep_execute && !lazy_bf_attack(hash_result, hash_len, &left_guess, pwd_len, LOWER_CHAR_LIST))
    keep_execute = false;

  // try brute force on lowercase, uppercase and number characters
  if (keep_execute && !lazy_bf_attack(hash_result, hash_len, &left_guess, pwd_len, COMMON_CHAR_LIST))
    keep_execute = false;

  // Turn on the switch to run BRUTE FORCE SEARCH
  // try brute force on all possible characters
  if (keep_execute && USE_BF && !lazy_bf_attack(hash_result, hash_len, &left_guess, pwd_len, NULL))
    keep_execute = false;

  free_2d_byte(GENERATE_PWD, NUM_PWD);
  return;
}

void compare_pwd(BYTE** hash_result, const int hash_len, const char* pwd_file) {
  FILE* fp;

  fp = fopen(pwd_file, "r");
  check_file_open(fp);

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

    check_pwd_len(pwd_len);

    FILE* fp = fopen(dict_file, "r");
    check_file_open(fp);

    bool has_next_line = false;
    BYTE read_buf[BUFFER_SIZE] = "\0";
    BYTE* password = (BYTE*)malloc((pwd_len + 1) * sizeof(BYTE));
    password[0] = '\0';

    // read dictionary file line by line, extract all substrings with length
    // pwd_len for each password, and then test whether matches hash results.
    // Why: human tend to use substring of name, words... to generate passwords
    // that are easy to memorize
    do {
      has_next_line = get_next_line(fp, read_buf);

      // if there is password being read
      if (*read_buf != '\n' && *read_buf != '\0') {
        int read_buf_len = strlen((char*)read_buf);

        // only check those passwords have same or greater than specified length
        if (pwd_len <= read_buf_len) {
          // get the whole substring stored in password
          for (int j = 0; j < pwd_len; j++)
            password[j] = read_buf[j];
          password[pwd_len] = '\0';

          // output results
          if ((*left_guess) > 0) {
            // if is already being generated
            if (!check_element(GENERATE_PWD, password, NUM_PWD)) {
              printf("%s\n", password);
              if (!add_element(GENERATE_PWD, password, NUM_PWD))
                *left_guess = 0;
              else
                (*left_guess)--;
              // reach the limits of number of guesses
              if (*left_guess == 0)
                break;
            }
          }
          else {
            // check the first choices
            check_match(hash_result, hash_len, password);
          }
        }
      }
    } while (has_next_line && *left_guess != 0);

    fclose(fp);
    free(password);

    if (*left_guess == 0)
      return false;
    return true;
}

bool smart_bf_attack(BYTE** hash_result, const int hash_len, const int pwd_len,
  int* left_guess, const char* dict_file, const int sub_limit) {

  check_pwd_len(pwd_len);

  FILE* fp = fopen(dict_file, "r");
  check_file_open(fp);

  bool has_next_line = false;
  BYTE read_buf[BUFFER_SIZE] = "\0";
  BYTE sub_buf[MAX_SUB] = "\0";
  BYTE* password = (BYTE*)malloc((pwd_len + 1) * sizeof(BYTE));
  password[0] = '\0';

  // NEW
  COMMON_SUB_MAP* map = NULL;

  init_sub_map(&map);

  BYTE** candidate_chars = alloc_2d_byte(pwd_len, MAX_SUB);

  // read dictionary file line by line, extract all substrings with length
  // pwd_len, then try to replace common substitution on different chars
  // Why: human tend to use substitution that easy to remember to generate
  // stronger password.
  do {
    has_next_line = get_next_line(fp, read_buf);

    // if there is password being read
    if (*read_buf != '\n' && *read_buf != '\0') {
      int read_buf_len = strlen((char*)read_buf);

      // only check those passwords have same or greater than specified length
      if (pwd_len <= read_buf_len) {
        // get the whole substring stored in password
        for (int j = 0; j < pwd_len; j++)
          password[j] = read_buf[j];
        password[pwd_len] = '\0';

        int total_size = 1;
        for (int i = 0; i < pwd_len; i++)
          total_size *= 2;

        for (int k = 1; k < total_size; k++) {
          int range = total_size;
          int count = 0;

          // for each possible combination, generate the corresponding passwords
          for (int l = 0; l < pwd_len; l++) {
            int len = 2;
            range /= len;
            // avoid float point exception
            int choice = len > 1 ? (k / range % len) : 0;
            if (choice == 1)
              count++;
          }

          if (count == sub_limit) {
            int range = total_size;
            // for each possible combination, generate the corresponding passwords
            for (int l = 0; l < pwd_len; l++) {
              BYTE c = password[l];
              candidate_chars[l][0] = c;
              candidate_chars[l][1] = '\0';

              int len = 2;
              range /= len;
              // avoid float point exception
              int choice = len > 1 ? (k / range % len) : 0;
              if (choice == 1) {
                get_sub(map, c, sub_buf);

                int buffer_len = strlen((char*)sub_buf);
                sub_buf[buffer_len] = '\0';
                strcat((char*)candidate_chars[l], (char*)sub_buf);
              }
            }

            // try to generate brute force attack on all common substitution of password
            if (!bf_search_match(hash_result, hash_len, left_guess, candidate_chars, pwd_len, sub_limit))
              break;
          }
        }
        if (*left_guess == 0)
          break;
      }
    }
  } while (has_next_line && *left_guess != 0);

  free_sub_map(map);
  free_2d_byte(candidate_chars, pwd_len);
  fclose(fp);
  free(password);

  if (*left_guess == 0)
    return false;
  return true;
}

bool lazy_bf_attack(BYTE** hash_result, const int hash_len, int* left_guess,
  const int pwd_len, const char* range_of_char) {

  check_pwd_len(pwd_len);

  // the base string that contains the first choice for all possible candidates
  BYTE* base_str = (BYTE*)malloc((pwd_len + 1) * sizeof(BYTE));
  base_str[0] = '\0';

  int num_chars = range_of_char == NULL ? END_CHAR - STRAT_CHAR + 1 : strlen(range_of_char);
  BYTE** candidate_chars = alloc_2d_byte(pwd_len, (num_chars + 1));

  // generate all possible chars
  for (int i = 0; i < pwd_len; i++) {
    for (int j = 0; j < num_chars; j++) {
      candidate_chars[i][j] = range_of_char == NULL ? (STRAT_CHAR + j) : range_of_char[j];
    }
    candidate_chars[i][num_chars] = '\0';
  }

  // generate base string
  for (int i = 0; i < pwd_len; i++)
    base_str[i] = candidate_chars[i][0];
  base_str[pwd_len] = '\0';

  // output results
  if ((*left_guess) > 0) {
    // if is already being generated
    if (!check_element(GENERATE_PWD, base_str, NUM_PWD)) {
      printf("%s\n", base_str);
      if (!add_element(GENERATE_PWD, base_str, NUM_PWD))
        *left_guess = 0;
      else
        (*left_guess)--;
    }
  }
  else {
    // check the first choices
    check_match(hash_result, hash_len, base_str);
  }

  // if reach the limits of number of guesses or brute force generate all other combination
  if (*left_guess == 0 || !bf_search_match(hash_result, hash_len, left_guess, candidate_chars, pwd_len, 0)) {
    free_2d_byte(candidate_chars, pwd_len);
    free(base_str);
    return false;
  }

  free_2d_byte(candidate_chars, pwd_len);
  free(base_str);
  return true;
}

bool bf_search_match(BYTE** hash_result, const int hash_len, int* left_guess,
  BYTE** candidate_chars, const int pwd_len, const int sub_limit) {

  long long int total_size = 1;
  BYTE* new_pwd = (BYTE*)malloc((pwd_len + 1) * sizeof(BYTE));
  new_pwd[0] = '\0';

  // get the total size of possible combinations
  for (int i = 0; i < pwd_len; i++)
    total_size *= strlen((char*)candidate_chars[i]);

  for (long long int i = 1; i < total_size; i++) {
    long long int range = total_size;
    int num_sub = 0;
    // for each possible combination, generate the corresponding passwords
    for (int j = 0; j < pwd_len; j++) {

      int len = strlen((char*)candidate_chars[j]);
      range /= len;
      // avoid float point exception
      int choice = len > 1 ? (i / range % len) : 0;
      if (choice > 0)
        num_sub++;

      new_pwd[j] = candidate_chars[j][choice];
    }
    new_pwd[pwd_len] = '\0';
    if (sub_limit != 0 && num_sub != sub_limit)
      continue;

    // output results
    if ((*left_guess) > 0) {
      // if is already being generated
      if (!check_element(GENERATE_PWD, new_pwd, NUM_PWD)){
        printf("%s\n", new_pwd);
        if (!add_element(GENERATE_PWD, new_pwd, NUM_PWD))
          *left_guess = 0;
        else
          (*left_guess)--;
        // reach the limits of number of guesses
        if (*left_guess == 0)
          break;
      }
    }
    else {
      // check the first choices
      check_match(hash_result, hash_len, new_pwd);
    }
  }

  free(new_pwd);

  if (*left_guess == 0)
    return false;
  return true;
}

void check_match(BYTE** hash_result, const int hash_len, BYTE* password) {
  BYTE hash_buf[SHA256_BLOCK_SIZE];

  generate_sha256(password, hash_buf);
  // compare new hash result stored in hash_buf with all hash results
  for(int i = 0; i < hash_len; i++) {
    if (CRACK_HASHS[i])
      continue;

    if (memcmp(hash_result[i], hash_buf, SHA256_BLOCK_SIZE) == 0) {
      if (strlen((char*)password) == 6)
        printf("%s %d\n", password, i + 11);
      else
        printf("%s %d\n", password, i + 1);
      CRACK_HASHS[i] = 1;
    }
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
