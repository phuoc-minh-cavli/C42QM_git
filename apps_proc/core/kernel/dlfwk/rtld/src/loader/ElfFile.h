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

#ifndef ELFFILE_H
#define ELFFILE_H



// File Specification **********************************************************
//!
//! @file ElfFile.h
//!
//! @brief ELF File declarations. Converted from C++.
//!
//******************************************************************************

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdio.h>
#include "typedef.h"
#include "exec_elf.h"

//------------------------------------------------------------------------------
// Types
//------------------------------------------------------------------------------
typedef struct fcn_symbols {
  uint32 begAddr;            //!< Beginning address of function
  uint32 endAddr;            //!< End address of function
  char*  szName;             //!< Pointer to function symbol name
  uint32 shndx;              //!< Section index
} TFcnSym;

typedef struct elfcontext {
  FILE*       m_hFile;          //!< File handle for this ELF file
  char*       m_fileData;       //!< Points to ELF file contents (whole file is read in)

  uint16      m_nFileEndian;    //!< ELF file endianness: 1=little, 0=big
  uint16      m_nHostEndian;    //!< Host computer endianness: 1=little, 0=big

  Elf32_Ehdr* m_pElfHdr;        //!< Points to ELF header entry

  Elf32_Shdr* m_pSecHdr;        //!< Points to first section header entry
  uint32      m_nSecHdrs;       //!< Number of section header entries
  char*       m_pSecHdrStrTbl;  //!< Points to section header string table

  Elf32_Sym*  m_pSymTbl;        //!< Points to first symbol table entry
  uint32      m_nSyms;          //!< Number of symbol table entries
  char*       m_pSymStrTbl;     //!< Points to symbol string table
  char*       m_pInterpTbl;     //!< Points to interpreter section

  uint32      m_numPgmSecs;     //!< Number of program code sections
  uint32      m_numFcnSyms;     //!< Number of function symbols

  TFcnSym*    m_fcnSyms;        //!< Address sorted array of function symbols
  char*       m_fcnSymNames;    //!< Buffer containing only function symbol names
} ElfContext;

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Declarations
//------------------------------------------------------------------------------

// Class Specification **********************************************************
//!
//! @brief ELF File functions.
//!
//! This object parses an ELF file and provides query capability for symbols,
//! addresses, etc.
//!
//******************************************************************************

//class ElfFile {
//public:
//   ElfFile(char* szFileName);
//   ~ElfFile();

typedef unsigned int bool;

extern ElfContext* ElfFile(char* name);
extern void DeleteElfFile(ElfContext* c);

extern void DumpAll(ElfContext* c);
extern void DumpElfHdr(ElfContext* c);
extern void DumpPgmHdrTbl(ElfContext* c);
extern void DumpSecHdrTbl(ElfContext* c);
extern void DumpSecData(ElfContext* c);
extern void DumpFcnSyms(ElfContext* c);
extern void DumpFcnSymsSorted(ElfContext* c);
extern uint32 GetNumPgmSecs(ElfContext* c);
extern char*  GetFcnNameForAddr(ElfContext* c, uint32 addr);
extern void LoadProgram(ElfContext* c, unsigned int addr);
extern TFcnSym* GetSymStruct(ElfContext* c, uint32 addr);
extern unsigned int GetEntryAddr(ElfContext* c, uint32 addr);
extern unsigned int GetSymAddr(ElfContext* c, char* sym);
extern bool IsStartOfFcnAddr(ElfContext* c, uint32 addr);
extern void BldFcnSymList(ElfContext* c);
extern void BldPgmSecList(ElfContext* c);
extern void DumpOffset(ElfContext* c, char* ptr);
extern void DumpData(ElfContext* c, char* pdata, size_t num_bytes);
extern uint16 Elf16Value(ElfContext* c, uint16 data);
extern uint32 Elf32Value(ElfContext* c, uint32 data);
extern bool ReadFile(ElfContext* c, char* szFileName);

#define GetExecFD(x) ((x)->m_hFile)
#define GetPhdrAddr(x) ((Elf_Phdr *) &((x)->m_fileData[m_pElfHdr->e_phoff]))
#define GetPhdrSize(x) ((unsigned int) (x)->m_pElfHdr->e_phentsize)
#define GetNumPhdrs(x) ((unsigned int) (x)->m_pElfHdr->e_phnum)
#define GetEntry(x) ((unsigned int) (x)->m_pElfHdr->e_entry)
#define GetInterpreter(x) (x)->m_pInterpTbl
#define GetBase(x) (x)->m_fileData
#define GetNumFcnSyms(x) (x)->m_numFcnSyms
#define GetFcnSym(x,y) &((x)->m_fcnSyms[(y)])
//#define GetEntryAddr(x) ((unsigned int) (x)->m_pSecHdr + (x)->m_pElfHdr->e_entry)
#define PrintInfo(x) { \
   printf("FuncName is %s for addr 0x%08x\n", GetFcnNameForAddr((x), GetEntry((x))), GetEntry((x))); \
   printf("pSecHdr=0x%08x, fileData=0x%08x, entry=0x%08x\n", (unsigned int) (x)->m_pSecHdr, \
           (unsigned int) (x)->m_fileData, (x)->m_pElfHdr->e_entry); }

#endif

