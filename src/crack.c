/*
  crack.c

  #### DESCRIPTION ###
  Project 2
  A project try to crack the passwords of a simple system that has four- and
  six-character passwords. The passwords can contain any ASCII character from 32
  (space) to 126 (∼), but since the passwords are chosen by people, they are not
  arbitrary strings

  Password Description:
  Passwords 1 to 10 are all four characters, located in file resources/passwords/pwd4sha256
  This 􏰁le contains 320 bytes. The 􏰁rst 32 bytes are the SHA256 hash of password 1,
  the second 32 bytes are the hash of password 2 etc..
  Hashes 11 to 30 are for six character passwords,

  for more information, please refer to resources/project_description.pdf

  #### RUNNING COMMAND ####
  Run cracker:
  1: ./crack
  2: ./crack <numGuess>
  3: ./crack <pwdFile> <hashFile>
  where
  numGuess: speci􏰁es how many guesses it should produce
  pwdFile: fi􏰁lename of a list of passwords (one per line)
  hashFile: fi􏰁lename of a list of SHA256 hashes (in groups of 32 bytes, with no newline characters)

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "pwd_crack.h"

// constant
static char const * const HASH_FILE_PATH = "../resources/passwords/pwd4sha256";
// static char const * const HASH_FILE_PATH = "../resources/passwords/pwd6sha256";

// Declarations
bool check_args(int argc, char * argv[]);
bool check_num(const char* str);
void print_usage();

/*
  main control
  Parameter: number of command line arguments(integer), command line arguments(1D string array)
  Return: execution status(integer)
*/
int main(int argc, char * argv[]) {
  // 0 means infinity number of guesses will be performed
  int num_guess = 0;

  if (!check_args(argc, argv))
    exit(EXIT_FAILURE);

  switch (argc) {
    case 1:
      crack_pwd(HASH_FILE_PATH, NULL, num_guess);
      break;
    case 2:
      num_guess = atoi(argv[1]);
      crack_pwd(HASH_FILE_PATH, NULL, num_guess);
      break;
    case 3:
      crack_pwd(argv[2], argv[1], num_guess);
      break;
    default:
      break;
  }

  return 0;
}

/*
  validate command line arguments
  Parameter: number of command line arguments(integer), command line arguments(1D string array)
  Return: arguments is valid(bool)
*/
bool check_args(int argc, char* argv[]) {
  if (argc > 3) {
      print_usage();
      return false;
  }

  if (argc == 2)
    return check_num(argv[1]);

  return true;
}

/*
  validate whether a string is a number
  Parameter: string
  Return: true if the string only contains number
*/
bool check_num(const char* str) {
  int i = 0;

  while (str[i] != '\0') {
    if (!isdigit(str[i])) {
      print_usage();
      return false;
    }

    i++;
  }

  return true;
}

/*
  print the correct usage in stderr
*/
void print_usage() {
  fprintf(stderr, "Wrong configuration, please follow the usage\n");
  fprintf(stderr, "usage 1: ./crack\n");
  fprintf(stderr, "usage 2: ./crack <numGuess>\n");
  fprintf(stderr, "usage 3: ./crack <pwdFile> <hashFile>\n");
}
