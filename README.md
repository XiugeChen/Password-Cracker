## DESCRIPTION

Project 2 for COMP30023 Computer System at University of Melbourne

A project try to crack the passwords of a simple system that has four- and
six-character passwords. The passwords can contain any ASCII character from 32
(space) to 126 (∼), but since the passwords are chosen by people, they are not
arbitrary strings

Password Description:
Passwords 1 to 10 are all four characters, located in file "../resources/passwords/pwd4sha256"
This 􏰁file contains 320 bytes. The rest 32 bytes are the SHA256 hash of password 1,
the second 32 bytes are the hash of password 2 etc..

Hashes 11 to 30 are all six character passwords, located in file "../resources/passwords/pwd6sha256"
This 􏰁file contains 640 bytes and followed the same convention as the first file.

Password breaking/generating technologies being used here are:
1. *flexible dictionary attack*:

Some of common passwords are provided from COMP30023
Others are collected from

When performing dictionary attack, if the password length has been specified,
the program will check all the substrings of each password that has the same
length.

2. *smart brute force attack*:

Within the dictionary attack, if the program fail to match the current stored
password with hash results, then the program will also try smart brute force
attack, which basically does the expansion that combines the following rules:

a. According to human common substitution convention, replace some character
with its substitution, like n <-> m, n <-> h, i <-> |, i <-> 1, r <-> 2 and
so on. All common substitution rules are stored at:
"./resources/passwords/common_substitution".

b. Capitalize the first character.

c. add punctuation at the start or end of password

d. add number at the start or end of password

3. *lazy brute force attack*:

Try all possible combinations, currently only support specified password length 4.

For detailed description please look at resources/project_description.pdf

## EXECUTION COMMAND (Linux)

*To compile*:

`make`

*To execute after compile*:

1. Diffie-Hellman key change client side:

`./dh`

2. password cracker:

a.

`./crack`

b.

`./crack <num_guess>`

where num_guess means the number of guesses will be generated,

c.

`./crack <pwd_file> <hash_file>`

where pwd_file is the file path to the file contains a list of plain text passwords (one per line)
hash_file is the file path to the file contains hash results of passwords (one per 32 bytes)

*To clean up executable and object files and directories*:

`make clean`

## ATTRIBUTION
Xiuge Chen

xiugec@student.unimelb.edu.au

Subject: COMP30023 Computer Systems

University of Melbourne

Some code came from:
1. GitHub

  (a) sha256.h and sha256.c: https://github.com/B-Con/crypto-algorithms/blob/master/sha256_test.c

2. geeksforgeeks:

  (a) part of dh.c: https://www.geeksforgeeks.org/modular-exponentiation-power-in-modular-arithmetic/

3. Stack overflow

  (a) part of dh.c: https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output

4. Lab 5 of COMP30023 at Unimelb
