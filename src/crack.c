/*
  crack.c

  #### DESCRIPTION ###
  Project 2
  A project try to crack the passwords of a simple system that has four- and
  six-character passwords. The passwords can contain any ASCII character from 32
  (space) to 126 (∼), but since the passwords are chosen by people, they are not
  arbitrary strings

  Password Description:
  Passwords 1 to 10 are all four characters, located in file "../resources/passwords/pwd4sha256"
  This 􏰁file contains 320 bytes. The 􏰁rst 32 bytes are the SHA256 hash of password 1,
  the second 32 bytes are the hash of password 2 etc..

  Hashes 11 to 30 are all six character passwords, located in file "../resources/passwords/pwd6sha256"
  This 􏰁file contains 640 bytes and followed the same convention as the first file.

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

#include <pwd_crack.h>
#include <error_check.h>

// constant
#define PWD_LEN 4

static char const * const HASH_PWD6_PATH = "./resources/hash_results/pwd6sha256";
static char const * const HASH_PWD4_PATH = "./resources/hash_results/pwd4sha256";

// Declarations
void print_usage();

/*
  main control
  Parameter: number of command line arguments(integer), command line arguments(1D string array)
  Return: execution status(integer)
*/
int main(int argc, char * argv[]) {
  // 0 means infinity number of guesses will be performed
  int num_guess = -1;

  if (!check_args(argc, argv)) {
    print_usage();
    exit(EXIT_FAILURE);
  }

  switch (argc) {
    case 2:
      num_guess = atoi(argv[1]);
    case 1:
      if (PWD_LEN == 4)
        crack_pwd(HASH_PWD4_PATH, NULL, num_guess, 4);
      else if (PWD_LEN == 6)
        crack_pwd(HASH_PWD6_PATH, NULL, num_guess, 6);
      break;
    case 3:
      crack_pwd(argv[2], argv[1], num_guess, 0);
      break;
    default:
      break;
  }

  return 0;
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
