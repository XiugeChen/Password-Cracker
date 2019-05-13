/*
  error_check.c

  #### DESCRIPTION ###
  check common errors in program

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#include <error_check.h>

void check_file_open(FILE* fp) {
  if (fp == NULL) {
    perror("No such file");
    exit(EXIT_FAILURE);
  }
}

void check_pwd_len(int pwd_len) {
  if (pwd_len <= 0) {
    perror("Can't attack without specified password length");
    exit(EXIT_FAILURE);
  }
}

bool check_args(int argc, char* argv[]) {
  if (argc > 3)
      return false;

  if (argc == 2)
    return check_num(argv[1]);

  return true;
}

bool check_num(const char* str) {
  int i = 0;

  while (str[i] != '\0') {
    if (!isdigit(str[i]))
      return false;

    i++;
  }

  return true;
}
