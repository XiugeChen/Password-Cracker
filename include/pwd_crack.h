/*
  pwd_crack.h

  #### DESCRIPTION ####
  Contains all strategies being used in password cracking, including dictionary
  attack and smart/lazy brute force attack.
  Also provide interface "crack_pwd" to call them automatically.

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

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne
*/

#ifndef PWD_CRACK_H
#define PWD_CRACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sha256.h>
#include <hash_manage.h>
#include <common_sub.h>
#include <error_check.h>
#include <array_manage.h>

/**** constant ****/
#define BUFFER_SIZE 10240
// number of bits of each byte
#define NUM_BIT_PER_BYTE 8
#define NUM_SUB 3
static char const * const DICT_FILE_PATH = "./resources/passwords/common_passwords.txt";

// used for brute force search
static const char NUMBER_LIST[10] = "0123456789";
static const char LOWER_CHAR_LIST[26] = "abcdefghijklmnopqrstuvwxyz";
static const char COMMON_CHAR_LIST[62] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char STRAT_CHAR = ' ';
static const char END_CHAR = '~';

/*
  public interface using to call password cracker
  INPUT: hash_file: mandatory, the file contains all SHA256 hash results (32 bytes for each)
        pwd_file: optional, the file contains all passwords in text,
                  if specified cracker will check each password with hash results
        num_guess: optional, the number of guesses will be performed, -1 if no restriction
        pwd_len: optional, the length of password, -1 if no restriction
*/
void crack_pwd(const char* hash_file, const char* pwd_file, const int num_guess,
  const int pwd_len);

// helper functions
/*
  search passwords
  Try flexible dictionary attack and smart brute force attack first since these
  technologies could help the program verify the mostly likely passwords first.
  Then try lazy brute force attack from smaller to larger range
  (check only numbers, only alphabets, numbers and alphabets, then all possible outcomes)
  For detailed description, please refer to README.
*/
void search_pwd(BYTE** hash_result, const int hash_len, const int num_guess,
  const int pwd_len);

/*
  compare password stored inside pwd_file (line by line) with hash results
  stored inside hash_result (32 bytes by 32 bytes)
  Return the number of guessing password stored inside of pwd_file.
*/
void compare_pwd(BYTE** hash_result, const int hash_len, const char* pwd_file);

/*
  Dictionary attack
  Password length has to be specified, it will try to match all the substrings of
  each password in dict_file that has the same length.
  It helps the program to verify those passwords that are more likely to be
  generated by human first, since it is the easiest way for human to memorize
  password by using some meaningful English words.
  For detailed description, please refer to README.
*/
bool dict_attack(BYTE** hash_result, const int hash_len, const int pwd_len,
  int* left_guess, const char* dict_file) ;

/*
  Smart brute force attack, trying differnet expansion of given password by
  substitue num_sub number of chars with their common substitutions, including:
  a. replace character with its common substitution (human convention, like a to @).
  b. Capitalize the character.
  For detailed description, please refer to README.
*/
bool smart_bf_attack(BYTE** hash_result, const int hash_len, const int pwd_len,
  int* left_guess, const char* dict_file, const int num_sub);

/*
  lazy brute force attack (try all possible combination of given list range and
  password length)
  For detailed description, please refer to README.
*/
bool lazy_bf_attack(BYTE** hash_result, const int hash_len, int* left_guess,
  const int pwd_len, const char* range_of_char);

/*
  given a 2D array where contains all possible values of string, brute force
  generate all possible string and try to match the hash results.
*/
bool bf_search_match(BYTE** hash_result, const int hash_len, int* left_guess,
  BYTE** candidate_chars, const int pwd_len, const int sub_limit);

/*
  check whether the given password matches any hash results
  If so, print it out and mark it, so it won't be compare next time.
*/
void check_match(BYTE** hash_result, const int hash_len, BYTE* password);

/*
  get the next line from file descriptor and store it to buffer
*/
bool get_next_line(FILE* fp, BYTE* buffer);

#endif
