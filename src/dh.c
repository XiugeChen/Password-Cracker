/*
  dh.c

  #### DESCRIPTION ###
  Project 2
  A program runs the key exchange protocol based on Di􏰆ffie-Hellman key exchange with the server

  for more information, please refer to resources/project_description.pdf

  #### RUNNING COMMAND ####

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

	Code using from other resources:
	1. geeksforgeeks
	2. stack overflow
	3. Lab 5 of COMP30023 at Unimelb
*/

// make popen and pclose work
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// constant
#define BUFFER_SIZE 2048

static char const * const FILE_PATH = "./src/dh.c";
static char const * const USERNAME = "xiugec\n";
static const int USERNAME_LEN = 7;
static char const * const SERVER = "172.26.37.44";
static const int PROT = 7800;
static const unsigned long long G = 15;
static const unsigned long long P = 97;
// number of hexadecimal digits converted from hash result used for b
static const int HASH_NUM = 2;

// decleration
void get_sha256_hash(const char* file_path, char* buffer);
unsigned long long extract_hash(char* buffer);
unsigned long long compute_mod_expo(unsigned long long g, unsigned long long a,
	unsigned long long p);
int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr);
void write_llu_text(const int sockfd, const unsigned long long num);
void add_terminator(char* string);

/*
  main control
  Parameter: number of command line arguments(integer), command line arguments(1D string array)
  Return: execution status(integer)
*/
int main(int argc, char* argv[]) {
	struct sockaddr_in serv_addr;
	int sockfd;
	char buffer[BUFFER_SIZE] = "\0";
	char* cur;
	int n;

	sockfd = setup_client_socket(PROT, SERVER, &serv_addr);

	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}

	// STEP 1: send username to server
	if (write(sockfd, USERNAME, USERNAME_LEN) < 0) {
    perror("write");
    exit(EXIT_FAILURE);
  }

	// get the SHA256 hash of this C source
	get_sha256_hash(FILE_PATH, buffer);
	printf("Hash result: %s\n", buffer);

	unsigned long long b = extract_hash(buffer);

	// compute g^b(mod p) and send it to server as text
	unsigned long long gb_modp = compute_mod_expo(G, b, P);
	write_llu_text(sockfd, gb_modp);

	// recieved g^a(mod p) and convert it to unsigned long long int
	n = read(sockfd, buffer, BUFFER_SIZE);
	if (n < 0) {
		perror("ERROR reading from socket");
		exit(EXIT_FAILURE);
	}
	add_terminator(buffer);
	printf("recieved from server: %s\n", buffer);

	unsigned long long ga_modp = (unsigned long long) strtol(buffer, &cur, 10);

	// compute (g^b)^a (mod p) and send it back to server as text
	unsigned long long gba_modp = compute_mod_expo(ga_modp, b, P);
	write_llu_text(sockfd, gba_modp);

	// recieved succeeded information from server
	n = read(sockfd, buffer, BUFFER_SIZE);
	if (n < 0) {
		perror("ERROR reading from socket");
		exit(EXIT_FAILURE);
	}
	add_terminator(buffer);
	printf("recieved from server: %s\n", buffer);

	/* Close to let server know that we've finished sending our message */
	close(sockfd);

	return 0;
}

/*
	efficiently compute modular exponentiation
	some codes are borrowed from geeksforgeeks.com
	URL: https://www.geeksforgeeks.org/modular-exponentiation-power-in-modular-arithmetic/
*/
unsigned long long compute_mod_expo(unsigned long long g, unsigned long long a,
	unsigned long long p) {

	unsigned long long result = 1;
	printf("g: %llu, a: %llu, p: %llu\n", g, a, p);

	// update g if it is greater than p
  g = g % p;

  while (a > 0) {
    // If a is odd, multiply g with result
    if (a & 1)
      result = (result * g) % p;

    // divide it by 2
    a = a >> 1;
    g = (g * g) % p;
  }

	return result;
}

/*
	extract the first two hexadecimal digits from the hash result
*/
unsigned long long extract_hash(char* buffer) {
	char* cur = buffer;
	char hexi_num[HASH_NUM + 1];

	// trace to the beginning of hash result
	while (*cur != '\0' || *cur != '\n') {
		if (*cur == ' ')
			break;

		cur++;
	}

	// get the first two hexadecimal digits of hash
	strncpy(hexi_num, cur + 1, HASH_NUM);
	hexi_num[HASH_NUM] = '\0';

	unsigned long long b = (unsigned long long) strtol(hexi_num, &cur, 16);

	return b;
}

/*
	store SHA256 hash of specific file into buffer
	some codes are borrowed from stackoverflow.com
	URL: https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
*/
void get_sha256_hash(const char* file_path, char* buffer) {
	FILE* fp;
	char local_buffer[BUFFER_SIZE] = "\0";
	// command
	char command[BUFFER_SIZE] = "openssl sha256 ";
	strcat(command, FILE_PATH);

	fp = popen(command, "r");
	if (fp == NULL) {
    fprintf(stderr, "Fail to run command: %s\n", command);
		printf("Please input hash result by hand: \n");
		char* str = fgets(buffer, BUFFER_SIZE, stdin);
		if (str == NULL) {
			fprintf(stderr, "Read Failure\n");
			exit(EXIT_FAILURE);
		}
  }

	/* Read the output a line at a time - output it. */
  while (fgets(local_buffer, sizeof(local_buffer)-1, fp) != NULL) {
		strcat(buffer, local_buffer);
	}

	pclose(fp);
	return;
}

/*
	Create and return a socket bound to the given port and server
*/
int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr) {
	int sockfd;
	struct hostent* server;

	server = gethostbyname(server_name);
	if (!server) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(EXIT_FAILURE);
	}

	bzero((char*)serv_addr, sizeof(serv_addr));
	serv_addr->sin_family = AF_INET;
	bcopy(server->h_addr_list[0], (char*)&serv_addr->sin_addr.s_addr,
		  server->h_length);
	serv_addr->sin_port = htons(port);

	/* Create socket */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

/*
	convert a long long int to text and write it to server
*/
void write_llu_text(const int sockfd, const unsigned long long num) {
	char local_buffer[BUFFER_SIZE] = "\0";

	sprintf(local_buffer, "%llu\n", num);
	add_terminator(local_buffer);

	if (write(sockfd, local_buffer, strlen(local_buffer)) < 0) {
		perror("write");
    exit(EXIT_FAILURE);
	}
}

/*
	append string terminator to newline char of string
*/
void add_terminator(char* string) {
	char* cur = strstr(string, "\n");

	*(cur + 1) = '\0';
}
