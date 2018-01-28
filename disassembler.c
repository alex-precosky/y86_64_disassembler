#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "printRoutines.h"

#define ERROR_RETURN -1
#define SUCCESS 0

int read_instruction(FILE* machineCode, unsigned int* icode, unsigned int *ifun, 
		      unsigned int *rA, unsigned int *rB, unsigned long* D, int* num_bytes_read, unsigned char* instruction_bytes);
int fpeek(FILE *stream);

int main(int argc, char **argv) {
  
  FILE *machineCode, *outputFile;
  long currAddr = 0; 

  // Verify that the command line has an appropriate number
  // of arguments

  if (argc < 3 || argc > 4) {
    printf("Usage: %s InputFilename OutputFilename [startingOffset]\n", argv[0]);
    return ERROR_RETURN;
  }

  // First argument is the file to read, attempt to open it 
  // for reading and verify that the open did occur.
  machineCode = fopen(argv[1], "rb");

  if (machineCode == NULL) {
    printf("Failed to open %s: %s\n", argv[1], strerror(errno));
    return ERROR_RETURN;
  }

  // Second argument is the file to write, attempt to open it 
  // for writing and verify that the open did occur.
  outputFile = fopen(argv[2], "w");

  if (outputFile == NULL) {
    printf("Failed to open %s: %s\n", argv[2], strerror(errno));
    fclose(machineCode);
    return ERROR_RETURN;
  }

  // If there is a 3rd argument present it is an offset so
  // convert it to a value. 
  if (4 == argc) {
    // See man page for strtol() as to why we check for errors by examining errno
    errno = 0;
    currAddr = strtol(argv[3], NULL, 0);
    if (errno != 0) {
      perror("Invalid offset on command line");
      fclose(machineCode);
      fclose(outputFile);
      return ERROR_RETURN;
    }
  }

  printf("Opened %s, starting offset 0x%lX\n", argv[1], currAddr);
  printf("Saving output to %s\n", argv[2]);


  // Your code starts here.
  int read_address = 0;
  unsigned int icode, ifun, rA, rB;
  unsigned long D;
  int num_bytes_read;
  unsigned char instruction_bytes[10];

  // skip bytes until a non-zero value is read
  while( fpeek(machineCode) == 0 )
    {
      fgetc(machineCode);
      read_address++;
    }

  while(1)
    {
      int eof = read_instruction(machineCode, &icode, &ifun, &rA, &rB, &D, &num_bytes_read, instruction_bytes);
      print_instruction(outputFile, read_address, icode, ifun, rA, rB, D, num_bytes_read, instruction_bytes);
      read_address += num_bytes_read;

      if(eof != 0)
	break;
    }


  fclose(machineCode);
  fclose(outputFile);
  return SUCCESS;
}

// peek at a letter in the file without removing it from the file stream
int fpeek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}


// return 1 if end of file was reached
// 0 otherwise
int read_instruction(FILE* machineCode, unsigned int* icode, unsigned int *ifun, 
    unsigned int *rA, unsigned int *rB, unsigned long* D, int* num_bytes_read, unsigned char* instruction_bytes)
{
  char buf[10];
  *num_bytes_read = 0;
  int i;

  i = fread(buf, 1, 1, machineCode);
  *num_bytes_read += i;
  if(i == 0) // eof
    return 1;

  memcpy(instruction_bytes, buf, 1);
  instruction_bytes += 1;

  *icode = (buf[0] >> 4) & 0x0F;
  *ifun = buf[0] &= 0x0F;

  printf("Reading Icode: %u\n", *icode);
  printf("Reading Ifun: %u\n", *ifun);

  int is_valid_instruction = check_valid_instruction(*icode, *ifun);
  printf("Is valid: %d\n", is_valid_instruction);

  // read rA, rB
  if((*icode == RRMOV || *icode == IRMOV || *icode == RMMOV || *icode == MRMOV || *icode == MATH || *icode == PUSH || *icode == POP) && is_valid_instruction == 1)
    {
      i = fread(buf, 1, 1, machineCode);
      *num_bytes_read += i;
      if(i == 0) // eof
	return 1;

      memcpy(instruction_bytes, buf, 1);
      instruction_bytes += 1;

      *rA = (buf[0] >> 4) & 0x0F;
      *rB = buf[0] &= 0x0F;
      printf("Reading rA: %d\n", *rA);
      printf("Reading rB: %d\n", *rB);
    }


  // read the V, D, or Dest bytes
  if((*icode == IRMOV || *icode == RMMOV || *icode == MRMOV || *icode == JUMP || *icode == CALL) && is_valid_instruction == 1 )
    {
      i = fread(buf, 1, 8, machineCode);
      *num_bytes_read += i;
      if(i != 8) // eof
	return 1;

      memcpy(instruction_bytes, buf, 8);
      instruction_bytes += 8;

      memcpy(D, buf, 8);
    }
  
  // read the rest of the bytes for an invalid instruction
  
  if(is_valid_instruction == 0)
    {
      i = fread(buf, 1, 7, machineCode);
      *num_bytes_read += i;
      if( i != 7)
	return 1;
      
      memcpy(instruction_bytes, buf, 7);
      instruction_bytes += 7;

      memcpy(D, buf, 7);

    }

  return 0;  // not eof
 
}
