/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ElfFile.h"

unsigned int INTERP_LOAD_ADDR = 0xd0000000;

extern unsigned int __dispatch_table__;

void sys_writecreg(void) { } // ASGASG Not in library...

int main(int argc, char** argv)
{
  unsigned long long stack[4096];
  unsigned int* sp = (unsigned int*)&stack[3000];
  unsigned int* sp2 = sp;
  char* interpreter;
  char* defaultInterpreter = "./ld_hexagon_elf.so";
  AuxInfo* auxvec;
  ElfContext* elf;
  ElfContext* interp;
  int (*prog)(unsigned int, unsigned int, void*, unsigned int);
  unsigned int ndx = 0, i;
  int ret;
  unsigned int* envp;
  unsigned int saved_sp;
  int last = 0;

  if (argc < 2) {
    printf("Usage: %s <executable>\n", argv[0]);
    exit(-1);
  }

  while (last < 2) {
    INTERP_LOAD_ADDR = last ? 0x90000000 : 0xd0000000;
    sp = (unsigned int*)&stack[last ? 2000 : 3000];
    sp2 = sp;
    elf = ElfFile(argv[1]);

    if (elf == NULL) {
      printf("Cannot open elf file\n");
      exit(-2);
    }

    interpreter = GetInterpreter(elf);

    if (!interpreter) {
#if 0
      unsigned int evb = elf->GetSymAddr(".EventVectors");
      printf(".EventVectors = 0x%08x\n", evb);
      //prog = (int (*)(unsigned int, unsigned int)) ((unsigned int) elf->GetEntry());
      //printf("Regular program - entry at 0x%08x\n", (unsigned int) elf->GetEntry());
      prog = (int ( *)(unsigned int, unsigned int))((unsigned int) elf->GetSymAddr("hexagon_pre_main"));
      printf("Regular program - entry at 0x%08x\n", (unsigned int) elf->GetSymAddr("hexagon_pre_main"));
      elf->PrintInfo();
      elf->LoadProgram((unsigned int) elf->GetEntry());
      //asm volatile ("evb = %0;" : : "r" (evb) : "r0");
      return prog(0, 0);
      //return 0;
#endif
      interpreter = defaultInterpreter;
    }

    printf("Using interpreter '%s'\n", interpreter);
    interp = ElfFile(interpreter);

    sp += 2;
    *sp++ = argc - 1;
    *sp++ = (Elf32_Word)argv[1];

    for (i = 2; i < argc; i++) {
      *sp++ = (Elf32_Word)argv[i];
    }

    *sp++ = 0;

    envp = sp;      // Point to address of array of environment pointers
#ifdef OLD_GETENV
    *sp++ = (Elf32_Word)"LD_PRELOAD=/prj/qct/sunray-austin/scratch/sgibbons/Hexagon_ldso/Tools/gnu/hexagon/lib/G0/libstandalone.so\0LD_BIND_NOW=yes\0LD_LIBRARY_PATH=.:/prj/qct/sunray-austin/scratch/sgibbons/Hexagon_ldso/Tools/gnu/hexagon/lib/G0:\0";
#else
    *sp++ = (Elf32_Word)"LD_PRELOAD=/prj/qct/coredev/qdsp6v3/austin/scratch/users/sgibbons/Tools64/LDSO/hexframe/12332/src/tools/dinkumware/libc/build/Linux/lib_pic/G0/libstandalone.so";
    *sp++ = (Elf32_Word)"LD_BIND_NOW=yes";
    *sp++ = (Elf32_Word)"LD_LIBRARY_PATH=.:/prj/qct/sunray-austin/scratch/sgibbons/Hexagon_ldso/Tools/gnu/hexagon/lib/G0:";
#endif
    *sp++ = 0;

#if 0
    printf("exec FD = 0x%08x\n", elf->GetExecFD());
    printf("There are %d phdrs of size %d at address 0x%08x\n", elf->GetNumPhdrs(), elf->GetPhdrSize(), elf->GetPhdrAddr());
    printf("Program entry point is 0x%08x\n", elf->GetEntry());
    printf("Interpreter is: '%s'\n", elf->GetInterpreter());
    printf("Interpreter base is 0x%08x\n", interp->GetBase());
    printf("Interpreter entry is 0x%08x\n", interp->GetEntry());
#endif

    auxvec = (AuxInfo*)sp;
#if 1
    auxvec[ndx].a_type = AT_EXECFD;
    auxvec[ndx++].a_v = (Elf32_Word)fileno((FILE*)GetExecFD(elf));
    //auxvec[ndx++].a_v = 0;  // means main prog already loaded
#endif
#if 0
    auxvec[ndx].a_type = AT_PHDR;
    auxvec[ndx++].a_v = (Elf32_Word) GetPhdrAddr(elf);
    auxvec[ndx].a_type = AT_PHENT;
    auxvec[ndx++].a_v = (Elf32_Word) GetPhdrSize(elf);
    auxvec[ndx].a_type = AT_PHNUM;
    auxvec[ndx++].a_v = (Elf32_Word) GetNumPhdrs(elf);
    auxvec[ndx].a_type = AT_ENTRY;
    auxvec[ndx++].a_v = (Elf32_Word) GetEntry(elf);
#endif
    auxvec[ndx].a_type = AT_BASE;
    auxvec[ndx++].a_v = (Elf32_Word)GetBase(interp);
    printf("pAUX_base gets 0x%08x\n", (Elf32_Word)GetBase(interp));
    auxvec[ndx].a_type = AT_PAGESZ;
    auxvec[ndx++].a_v = 4096;
    auxvec[ndx].a_type = AT_NULL;
    auxvec[ndx++].a_v = 0;

    printf("Loading the interpreter at 0x%08x\n", INTERP_LOAD_ADDR);
    LoadProgram(interp, INTERP_LOAD_ADDR);

    prog = (int (*)(unsigned int, unsigned int, void*, unsigned int))((unsigned int)GetEntry(interp) + INTERP_LOAD_ADDR);
    printf("Calling the interpreter at address 0x%08x\n", (unsigned int)GetEntry(interp) + INTERP_LOAD_ADDR);
#ifdef STRAIGHT_CALL
    ret = prog((unsigned int)sp2, INTERP_LOAD_ADDR, envp, (unsigned int)&__dispatch_table__);
#else
    __asm__ volatile ("memw(%1) = sp; sp = %2; callr %3; sp = memw(%1); %0 = r0" : "=r"(ret) : "r"(&saved_sp), "r"(&sp2[2]), "r"(prog) :
                      "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "memory");
#endif
    fflush(stdout);

    DeleteElfFile(interp);
    DeleteElfFile(elf);

    argv[1] = "test2.elf";
    last++;
    memset((void*)INTERP_LOAD_ADDR, 0x1f, 0x1000000);
    memset((void*)0x80000000, 0x1f, 0x1000000);
  }

  return ret;
}

