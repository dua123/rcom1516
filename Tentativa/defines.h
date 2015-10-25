#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdio.h>
#include <string.h>

char str_hello[16];
char str_world[16];

static const char HELLO_CONST[] = "Hello, ";
static const char WORLD_CONST[] = "World!";

#define TYPE_SET 	1
#define TYPE_DISC 	2
#define TYPE_UA 	3
#define TYPE_RR 	4
#define TYPE_REJ 	5

#define EMISSOR 0
#define RECETOR 1

#define FLAG 	0x7e
#define AE 		0x03
#define AR 		0x01
#define CSET 	0x07
#define CDISC 	0x0b
#define CUA 	0x03
#define BCCR 	0x00
#define BCCE 	0xFF
#define CRR(r) 	((r << 5 )|1)
#define CREJ(r) ((r << 5 )|5)


#endif