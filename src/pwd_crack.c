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

  // try dictionary attack
  if (!dict_attack(hash_result, hash_len, pwd_len, &left_guess, DICT_FILE_PATH))
    return;

  // then try smart brute force attack (replace each char with its common substitution and uppercase)
  if (!smart_bf_attack(hash_result, hash_len, pwd_len, &left_guess, DICT_FILE_PATH, NUM_SUB))
    return;

  // then try lazy brute force attack
  // try brute force on numbers only first
  if (!lazy_bf_attack(hash_result, hash_len, &left_guess, pwd_len, NUMBER_LIST))
    return;

  // then try brute force on lowercase characters only
  if (!lazy_bf_attack(hash_result, hash_len, &left_guess, pwd_len, LOWER_CHAR_LIST))
    return;

  // try brute force on lowercase, uppercase and number characters
  if (!lazy_bf_attack(hash_result, hash_len, &left_guess, pwd_len, COMMON_CHAR_LIST))
    return;

  // try brute force on lowercase, uppercase and number characters
  if (!lazy_bf_attack(hash_result, hash_len, &left_guess, pwd_len, NULL))
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
          // for each start point of substring
          for (int i = 0; i <= read_buf_len - pwd_len; i++) {
            // get the whole substring stored in password
            for (int j = 0; j < pwd_len; j++)
              password[j] = read_buf[i + j];
            password[pwd_len] = '\0';

            // check the first choices
            check_match(hash_result, hash_len, password);

            // reach the limits of number of guesses
            (*left_guess)--;
            if (*left_guess == 0)
              break;
          }
        }
        // for passwords have shorter length than specified, fill it with blanks
        else {
          strcpy((char*)password, (char*)read_buf);

          for (int i = 0; i < pwd_len - read_buf_len; i++)
            password[read_buf_len + i] = ' ';

          // check the first choices
          check_match(hash_result, hash_len, password);

          // reach the limits of number of guesses
          (*left_guess)--;
          if (*left_guess == 0)
            break;
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
  int* left_guess, const char* dict_file, const int num_sub) {

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
        // for each start point of substring
        for (int i = 0; i <= read_buf_len - pwd_len; i++) {
          // get the whole substring stored in password
          for (int j = 0; j < pwd_len; j++)
            password[j] = read_buf[i + j];
          password[pwd_len] = '\0';

          int total_size = floor(pow(2, pwd_len));

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

            if (count == num_sub) {
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
              if (!bf_search_match(hash_result, hash_len, left_guess, candidate_chars, pwd_len))
                break;
            }
          }
        }
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

  // check the first choices
  check_match(hash_result, hash_len, base_str);

  (*left_guess)--;

  // if reach the limits of number of guesses or brute force generate all other combination
  if (*left_guess == 0 || !bf_search_match(hash_result, hash_len, left_guess, candidate_chars, pwd_len)) {
    free_2d_byte(candidate_chars, pwd_len);
    free(base_str);
    return false;
  }

  free_2d_byte(candidate_chars, pwd_len);
  free(base_str);
  return true;
}

bool bf_search_match(BYTE** hash_result, const int hash_len, int* left_guess,
  BYTE** candidate_chars, const int pwd_len) {

  long long int total_size = 1;
  BYTE* new_pwd = (BYTE*)malloc((pwd_len + 1) * sizeof(BYTE));
  new_pwd[0] = '\0';

  // get the total size of possible combinations
  for (int i = 0; i < pwd_len; i++)
    total_size *= strlen((char*)candidate_chars[i]);

  for (long long int i = 1; i < total_size; i++) {
    long long int range = total_size;

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
    if (*left_guess == 0)
      break;
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
