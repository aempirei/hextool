/* hextool
 * Simple Intel HEX file tool
 *
 * Copyright(c) 2012 256 LLC
 * Written by Christopher Abad
 * 20 GOTO 10
 *
 * email: aempirei@gmail.com aempirei@256.bz
 * http://www.256.bz/ http://www.twentygoto10.com/
 * git: git@github.com:aempirei/hextool.git
 * aim: ambientempire
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>

void usage(const char *);
void hextool(FILE *, unsigned long);
void hexline(unsigned char *, unsigned long, const char *, int);

int main(int argc, char **argv) {

	FILE *fp = stdin;

	unsigned long size;

	if(argc != 2) {
		usage(*argv);
		exit(EXIT_FAILURE);
	}

	size = strtoul(argv[1], NULL, 0);

	if(size == ULONG_MAX && errno != 0) { 
		usage(*argv);
		exit(EXIT_FAILURE);
	}

	hextool(fp, size);

	exit(EXIT_SUCCESS);
}

void usage(const char *arg0) {
	fprintf(stderr, "\nusage: %s size\n\n", arg0);
}

int isvalid(const char *p) {

	if(*p++ != ':')
		return 0;

	while(isxdigit(*p))
		p++;

	if(*p == '\r')
		p++;

	if(*p++ != '\n')
		return 0;

	if(*p++ != '\0')
		return 0;

	return 1;
}


int hex(char ch) {
	if(ch >= '0' && ch <= '9')
		return ch - '0';
	if(ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	if(ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	return -1;
}

#define HEX(a,p) (a)=(a)*16+hex(p)
#define HEXS(a,p,s) do{ (a)=0; for(int i = 0; i < s; i++) HEX(a,*(p)++); }while(0)

void hexline(unsigned char *buffer, unsigned long size, const char *line, int lineno) {

	unsigned long count;
	unsigned long address;
	unsigned int type;
	unsigned int checksum;
	unsigned int sum;

	const char *p = line + 1;

	HEXS(count,p,2);
	HEXS(address,p,4);
	HEXS(type,p,2);

	if(strlen(line) != 13 + count * 2) {
		fprintf(stderr, "ERROR: unexpected count field on line %d: %s\n", lineno, line);
		exit(EXIT_FAILURE);
	}

	sum = count + address + type;

	for(unsigned int n = 0; n < count; n++) {

		unsigned int byte;

		HEXS(byte,p,2);

		sum += byte;

		if(type == 0x00)
			if(address + n < size)
				buffer[address + n] = byte;
	}

	HEXS(checksum,p,2);

	sum = -sum & 0xff;

	if(checksum != sum) {
		fprintf(stderr, "ERROR: invalid checksum on line %d: %s\n", lineno, line);
		exit(EXIT_FAILURE);
	}
}

void hextool(FILE *fp, unsigned long size) {
	
	unsigned char *buffer;

	buffer = (unsigned char *)malloc(size);

	memset(buffer, 0, size);

	for(int lineno = 1;; lineno++) {
		
		char line[256];

		fgets(line, sizeof(line) - 1, fp);

		if(feof(fp))
			break;

		if(strlen(line) < 12) {
			fprintf(stderr, "ERROR: unexpectedly short input at line %d: %s\n", lineno, line);
			exit(EXIT_FAILURE);
		}

		if(!isvalid(line)) {
			fprintf(stderr, "ERROR: invalid .HEX input at line %d: %s\n", lineno, line);
			exit(EXIT_FAILURE);
		}

		hexline(buffer, size, line, lineno);
	}

	fwrite(buffer, size, 1, stdout);

	free(buffer);
}

