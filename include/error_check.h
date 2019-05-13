/*
  error_check.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#ifndef ERROR_CHECK_H
#define ERROR_CHECK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

/*
  validate command line arguments
  Parameter: number of command line arguments(integer), command line arguments(1D string array)
  Return: arguments is valid(bool)
*/
bool check_args(int argc, char* argv[]);

/*
  validate whether a string is a number
  Parameter: string
  Return: true if the string only contains number
*/
bool check_num(const char* str);

/*
  check whether the file is open
*/
void check_file_open(FILE* fp);

/*
  check the length of password is greater than 0
*/
void check_pwd_len(int pwd_len);

#endif
