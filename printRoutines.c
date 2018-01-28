
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "printRoutines.h"

// You probably want to create a number of printing routines in this file.
// Put the prototypes in printRoutines.h



/*********************************************************************
   Details on print formatting can be found by reading the man page
   for printf. The formatting rules for the disassembler output are
   given below.  Basically it should be the case that if you take the
   output from your disassembler, remove the initial address
   information, and then take the resulting file and load it into a
   Y86-64 simulator the object code produced be the simulator should
   match what your program read.  (You may have to use a .pos
   directive to indicate the offset to where the code starts.) If the
   simulator reports an assembler error then you need to fix the
   output of your dissassembler so that it is acceptable to the
   simulator.)

   The printing rules are as follows:
   
     1) Each line is to begin with the hex printed value of the
        address followed immediately by a ": ". Leading zeros are to
        be printed for the address, which is 8 bytes long.

     2) After the ": " there are to be 22 characters before the start
        of the printing of the instruction mnemonic. In those 22
        characters the program is to print, left justified, the hex
        representation of the memory values corresponding to the
        assembler instruction and operands that were printed.

     3) The instruction field follows, and is 8 characters long. The
        instruction is to be printed left justified in those 8
        characters (%-8s). All instructions are to be printed in lower
        case.

     4) After the instruction field the first operand is to be
        printed. No extra space is needed, since the 8 characters for
        instructions is enough to leave a space between instruction
        and operands.
 
     5) If there is a second operand then there is to be a comma
        immediately after the first operand (no spaces) and then a
        single space followed by the second operand.

     6) The rules for printing operands are as follows: 

         a) Registers: A register is to be printed with the % sign and
            then its name. (.e.g. %rsp, %rax etc) Register names are
            printed in lower case.
 
         b) All numbers are to be printed in hex with the appropriate
            leading 0x. Leading zeros are to be suppressed. A value of
            zero is to be printed as "0x0". The numbers are assumed to
            be unsigned.

         c) A base displacement address is to be printed as D(reg)
            where the printing of D follows the rules in (b), and the
            printing of reg follows the rules in (a). Note there are
            no spaces between the D and "(" or between reg and the "("
            or ")".
        
         d) An address such as that used by a call or jump is to be
            printed as in (b). For these instructions no "$" is
            required.

         e) A constant (immediate value), such as that used by irmovq
            is to be printed as a number in (b) but with a "$"
            immediately preceeding the 0x without any spaces.
           
     7) The unconditional move instruction is to be printed as rrmovq,
        while the conditional move is to be printed as cmovXX, where
        XX is the condition (e.g., cmovle).

     8) The mnemonics for the instruction are to conform to those
        described in the textbook and lecture slides.

     9) The arguments for the format string in the example printing
        are strictly for illustrating the spacing. You are free to
        construct the output however you want.
 
********************************************************************************/
 
/* This is a function to demonstrate how to do print formatting. It
 * takes the file descriptor the I/O is to be done to. You are not
 * required to use the same type of printf formatting, but you must
 * produce the same result.
 */
int samplePrint(FILE *out) {

  int res = 0;

  unsigned long addr = 0x1016;
  char * r1 = "%rax";
  char * r2 = "%rdx";
  char * inst1 = "rrmovq";
  char * inst2 = "jne";
  char * inst3 = "irmovq";
  char * inst4 = "mrmovq";
  unsigned long destAddr = 8193;
  
  res += fprintf(out, "%016lx: %-22s%-8s%s, %s\n", 
		 addr, "2002", inst1, r1, r2);

  addr += 2;
  res += fprintf(out, "%016lx: %-22s%-8s%#lx\n", 
		 addr, "740120000000000000", inst2, destAddr);

  addr += 9;
  res += fprintf(out, "%016lx: %-22s%-8s$%#lx, %s\n", 
		 addr, "30F21000000000000000", inst3, 16L, r2);

  addr += 10;
  res += fprintf(out, "%016lx: %-22s%-8s%#lx(%s), %s\n", 
		 addr, "50020000010000000000", inst4, 65536L, r2, r1); 
  
  addr += 10;
  res = fprintf(out, "%016lx: %-22s%-8s%s, %s\n", 
		addr, "2020", inst1, r2, r1);

  return res;
}  



void print_register_name(char* dest, unsigned char register_code)
{
  switch(register_code)
    {
      case 0:
	strcpy(dest, "%rax");
	break;
      case 1:
	strcpy(dest, "%rcx");
	break;
      case 2:
	strcpy(dest, "%rdx");
	break;
      case 3:
	strcpy(dest, "%rbx");
	break;
      case 4:
	strcpy(dest, "%rsp");
	break;
      case 5:
	strcpy(dest, "%rbp");
	break;
      case 6:
	strcpy(dest, "%rsi");
	break;
      case 7:
	strcpy(dest, "%rdi");
	break;
      case 8:
	strcpy(dest, "%r8");
	break;
      case 9:
	strcpy(dest, "%r9");
	break;
      case 10:
	strcpy(dest, "%r10");
	break;
      case 11:
	strcpy(dest, "%r11");
	break;
      case 12:
	strcpy(dest, "%r12");
	break;
      case 13:
	strcpy(dest, "%r13");
	break;
      case 14:
	strcpy(dest, "%r14");
	break;
      default:
	strcpy(dest, "%r-1");
	break;
    }


}

// returns 1 if valid
int check_valid_instruction(unsigned char icode, unsigned char ifun)
{
  switch(icode)
    {
      case HALT:
      case NOP:
      case IRMOV:
      case RMMOV:
      case MRMOV:
      case CALL:
      case RET:
      case PUSH:
      case POP:
	if(ifun != 0)
	  return 0;
	else
	  return 1;
	break;

      case RRMOV:
      case MATH:
      case JUMP:
	if(ifun > 6)
	  return 0;
	else
	  return 1;
	break;

      default:
	return 0;
    }
}
  
void print_instruction(FILE* outputFile, long address, unsigned char icode, unsigned char ifun, 
		       unsigned char rA, unsigned char rB, unsigned long D, 
		       int num_instruction_bytes, unsigned char* instruction_bytes)
{
  char opcode_str[10];
  static int last_icode = -1;

  if(num_instruction_bytes == 0)
    return;

  printf("printing instruction of %d bytes\n\n", num_instruction_bytes);

  // make strings for rA and rB
  char rA_str[10];
  char rB_str[10];

  print_register_name(rA_str, rA);
  print_register_name(rB_str, rB);


  // print the instruction bytes into a hex string
  char instruction_bytes_str[21];
  for(int i = 0; i < num_instruction_bytes; i++)
    {
      char byte_str[3];
      sprintf(byte_str, "%02X", instruction_bytes[i]);
      instruction_bytes_str[2*i] = byte_str[0];
      instruction_bytes_str[2*i+1] = byte_str[1];
    }
  instruction_bytes_str[2*num_instruction_bytes] = '\0';



  switch(icode)
    {
      case HALT:
	strcpy(opcode_str, "halt");
	break;
      case RET:
	strcpy(opcode_str, "ret");
	break;
      case NOP:
	strcpy(opcode_str, "nop");
	break;
      case IRMOV:
	strcpy(opcode_str, "irmovq");
	break;
      case MRMOV:
	strcpy(opcode_str, "mrmovq");
	break;
      case RMMOV:
	strcpy(opcode_str, "rmmovq");
	break;
      case CALL:
	strcpy(opcode_str, "call");
	break;
      case PUSH:
	strcpy(opcode_str, "pushq");
	break;
      case POP:
	strcpy(opcode_str, "popq");
	break;
      case RRMOV:
	switch(ifun)
	  {
	    case RRMOVQ:
	      strcpy(opcode_str, "rrmovq");
	      break;
	    case CMOVLE:
	      strcpy(opcode_str, "cmovle");
	      break;
	    case CMOVL:
	      strcpy(opcode_str, "cmovl");
	      break;
	    case CMOVE:
	      strcpy(opcode_str, "cmove");
	      break;
	    case CMOVNE:
	      strcpy(opcode_str, "cmovne");
	      break;
	    case CMOVGE:
	      strcpy(opcode_str, "cmovge");
	      break;
	    case CMOVG:
	      strcpy(opcode_str, "cmovg");
	      break;	   
	  }
	break;
      case JUMP:
	switch(ifun)
	  {
	    case JMP:
	      strcpy(opcode_str, "jmp");
	      break;
	    case JLE:
	      strcpy(opcode_str, "jle");
	      break;
	    case JL:
	      strcpy(opcode_str, "jl");
	      break;
	    case JE:
	      strcpy(opcode_str, "je");
	      break;
	    case JNE:
	      strcpy(opcode_str, "jne");
	      break;
	    case JGE:
	      strcpy(opcode_str, "jge");
	      break;
	    case JG:
	      strcpy(opcode_str, "jg");
	      break;	   
	  }

	break;
      case MATH:
	switch(ifun)
	  {
	    case ADD:
	      strcpy(opcode_str, "addq");
	      break;
	    case SUB:
	      strcpy(opcode_str, "subq");
	      break;
	    case AND:
	      strcpy(opcode_str, "andq");
	      break;
	    case XOR:
	      strcpy(opcode_str, "xorq");
	      break;
	    default:
	      strcpy(opcode_str, "err");
	      break;
	  }
	break;
      default:
	strcpy(opcode_str, "err");
	break;

    }


  int is_valid_instruction = check_valid_instruction(icode, ifun);
  
  if(is_valid_instruction)
    {
      switch(icode)
	{
	case RRMOV:
	case MATH:
	  fprintf(outputFile, "%016lx: %-22s%-8s%s, %s\n", 
		  address, instruction_bytes_str, opcode_str, rA_str, rB_str);
	  break;
	case IRMOV:
	  fprintf(outputFile, "%016lx: %-22s%-8s$%#lx, %s\n", 
		  address, instruction_bytes_str, opcode_str, D, rB_str);       
	  break;
	case MRMOV:
	  fprintf(outputFile, "%016lx: %-22s%-8s%#lx(%s), %s\n", 
		  address, instruction_bytes_str, opcode_str, D, rB_str, rA_str); 
	  break;
	case RMMOV:
	  fprintf(outputFile, "%016lx: %-22s%-8s%s,%#lx(%s)\n", 
		  address, instruction_bytes_str, opcode_str, rA_str, D, rB_str); 
	  break;
	case JUMP:
	case CALL:
	  fprintf(outputFile, "%016lx: %-22s%-8s%#lx\n", 
		  address, instruction_bytes_str, opcode_str, D);
	  break;
	case RET:
	case NOP:
	  fprintf(outputFile, "%016lx: %-22s%-8s\n", 
		  address, instruction_bytes_str, opcode_str);
	  break;
	case HALT:
	  if( last_icode != HALT)
	    fprintf(outputFile, "%016lx: %-22s%-8s\n", 
		    address, instruction_bytes_str, opcode_str);
	  break;
	case PUSH:
	case POP:
	  fprintf(outputFile, "%016lx: %-22s%-8s%s\n", 
		  address, instruction_bytes_str, opcode_str, rA_str);
	  
	  break;

	default:
	  fprintf(outputFile, "not supported icode: %d\n", icode);
	}
    }
  else // invalid instruction
    {
      fprintf(outputFile, "%016lx: %-22s%-8s%#lX\n",
	      address, instruction_bytes_str, ".quad", *(uint64_t*)instruction_bytes);
    }



  last_icode = icode;

}
