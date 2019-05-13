/*
  crack.c

  #### DESCRIPTION ###
  Project 2 for COMP30023 Computer System at University of Melbourne

  A project try to crack the passwords of a simple system that has four- and
  six-character passwords. The passwords can contain any ASCII character from 32
  (space) to 126 (∼), but since the passwords are chosen by people, they are not
  arbitrary strings

  Password Description:
  Passwords 1 to 10 are all four characters, located in file "./resources/hash_results/pwd4sha256"
  This 􏰁file contains 320 bytes. The rest 32 bytes are the SHA256 hash of password 1,
  the second 32 bytes are the hash of password 2 etc..

  Hashes 11 to 30 are all six character passwords, located in file "./resources/hash_results/pwd6sha256"
  This 􏰁file contains 640 bytes and followed the same convention as the first file.

  Several password breaking/generating technologies are being used here sequentially,
  the order of execution and detailed description of each methods are listed below:
  1. **dictionary attack**:

  This is the first approach will be used for attacking, because people are most likely
  to use passwords that are real daily-used words in English since they are the
  easiest to memorize.

  Some (9743) of common passwords are provided from COMP30023, stored in "./resources/passwords/common_passwords.txt".

  When performing dictionary attack, password length has to be specified,
  the program will go through all frequently used passwords stored in common_passwords.text
  and also check all the substrings of each password that has the specific length.

  2. **smart brute force attack**:

  Second approach, since some people are aware of insecurity of real-word password
  but don't want to come up random passwords that are hard to remember, so they
  used some substitution to "create" stronger passwords. Therefore, replacing
  passwords obtained from "common_passwords.text" with common substitutions will
  also generate passwords are more likely to be used.

  Smart brute force basically does the expansion on dictionary words from "common_passwords.text"
  that combines the following rules:

  a. According to human common substitution convention, replace some character
  with its substitution, like n <-> m, i <-> |, r <-> 2 and so on.

  All common substitution rules are stored at: "./resources/passwords/common_substitution".

  b. Capitalize the first character.

  3. **Lazy brute force attack**:

  Lazy brute force is all about try all possible combinations of given char set,
  although it certainly is not as efficient as previous approach, there are still
  some tricks and strategies could be used:

  According to the analysis on common passwords in "common_passwords.text", most people
  like use a set of all numbers like birthdate, or a set of all lowercase alphabets
  like meaningful information, or a combination of alphabets and number as passwords.
  Therefore, instead of going through all possible combinations (including '~' ...),
  it is definitely more efficient to try a smaller range that might contain most of
  unrevealed passwords (ie. numbers and alphabets) first. So the execution sequence
  of lazy brute force attack is listed below:

  a. brute force on char set that only contains number.

  b. brute force on char set that only contains lowercase chars.

  c. brute force on char set that contains lowercase chars, numbers and uppercase chars.

  d. Lastly, try all possible combinations.

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
#define PWD_LEN 6

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
