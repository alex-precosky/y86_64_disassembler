/* This file contains the prototypes and constants needed to use the
   routines defined in printRoutines.c
*/

#ifndef _PRINTROUTINES_H_
#define _PRINTROUTINES_H_

#include <stdio.h>

int samplePrint(FILE *);

void print_instruction(FILE* outputFile, long address, unsigned char icode, 
		       unsigned char ifun, unsigned char rA, unsigned char rB, 
		       unsigned long D, int num_instruction_bytes, unsigned char* instruction_bytes);
int check_valid_instruction(unsigned char icode, unsigned char ifun);


// icodes
#define HALT  0
#define NOP   1
#define RRMOV 2
#define IRMOV 3
#define RMMOV 4
#define MRMOV 5
#define MATH  6
#define JUMP  7
#define CALL  8
#define RET   9
#define PUSH  10
#define POP   11

// ifuns
#define RRMOVQ 0
#define CMOVLE 1
#define CMOVL  2
#define CMOVE  3
#define CMOVNE 4
#define CMOVGE 5
#define CMOVG  6

#define JMP 0
#define JLE 1
#define JL  2
#define JE  3
#define JNE 4
#define JGE 5
#define JG  6

// math ifuns
#define ADD 0
#define SUB 1
#define AND 2
#define XOR 3

#endif /* PRINTROUTINES */
