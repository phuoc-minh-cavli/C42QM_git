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

// File Specification **********************************************************
//!
//! @file ElfFile.cpp
//!
//! @brief ELF File class implementation.
//!
//******************************************************************************

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "ElfFile.h"

//------------------------------------------------------------------------------
// Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define false (0)
#define true (1)

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------

// Function Specification *****************************************************
//!
//! @brief Compares two function symbol entries by starting address.
//!
//! Returns -1 if function1 address < function2 address.
//! Returns  1 if function1 address > function2 address.
//! Returns  0 if function1 address = function2 address.
//!
// End Function Specification *************************************************

static int CompareFcnSymsByAddr(const void* fcnSym1, const void* fcnSym2)
{
  TFcnSym* sym1 = (TFcnSym*)fcnSym1;
  TFcnSym* sym2 = (TFcnSym*)fcnSym2;

  int rc = 0;

  if (sym1->begAddr < sym2->begAddr) {
    rc = -1;
  } else if (sym1->begAddr > sym2->begAddr) {
    rc = 1;
  }

  return rc;
}

// Function Specification *****************************************************
//!
//! @brief Compares address to see if it resides in the specified function symbol
//!
//! Returns -1 if address < function symbol start address.
//! Returns  1 if address >= function symbol end address.
//! Returns  0 if address is within function symbol address range.
//!
// End Function Specification *************************************************

static int CompareAddrInFcn(const void* address, const void* fcnSym)
{
  uint32* addr = (uint32*)address;
  TFcnSym* sym = (TFcnSym*)fcnSym;

  int rc = 0;

  if (*addr < sym->begAddr) {
    rc = -1;
  } else if (*addr >= sym->endAddr) {
    rc = 1;
  }

  return rc;
}

// Function Specification *****************************************************
//!
//! @brief Construct an ElfFile object and allocate its resources.
//!
// End Function Specification *************************************************

ElfContext* ElfFile(char* szFileName)
{
  ElfContext* c = (ElfContext*)calloc(1, sizeof(ElfContext));

  c->m_hFile = 0;
  c->m_fileData = 0;
  c->m_nFileEndian = 0;
  c->m_nHostEndian = 0;
  c->m_pElfHdr = 0;
  c->m_pSecHdr = 0;
  c->m_pSecHdrStrTbl = 0;
  c->m_pSymTbl = 0;
  c->m_pSymStrTbl = 0;
  c->m_pInterpTbl = 0;
  c->m_numPgmSecs = 0;
  c->m_numFcnSyms = 0;
  c->m_fcnSyms = 0;
  c->m_fcnSymNames = 0;

  if (ReadFile(c, szFileName)) {
    return c;
  }

  return NULL;
}

// Function Specification *****************************************************
//!
//! @brief Destroy this ElfFile object and free its resources.
//!
// End Function Specification *************************************************

void DeleteElfFile(ElfContext* c)
{
  fclose(c->m_hFile);
  if (c->m_fileData) {
    free(c->m_fileData);
  }
}

// Function Specification *****************************************************
//!
//! @brief Decode an ELF 16-bit integer and convert it to the host native format.
//!
// End Function Specification *************************************************

uint16 Elf16Value(ElfContext* c, uint16 data)
{
  return (c->m_nHostEndian != c->m_nFileEndian) ? SWAP16(data) : data;
}

// Function Specification *****************************************************
//!
//! @brief Decode an ELF 32-bit integer and convert it to the host native format.
//!
// End Function Specification *************************************************

uint32 Elf32Value(ElfContext* c, uint32 data)
{
  return (c->m_nHostEndian != c->m_nFileEndian) ? SWAP32(data) : data;
}

// Function Specification *****************************************************
//!
//! @brief Build list of Function symbols.
//!
// End Function Specification *************************************************

void BldFcnSymList(ElfContext* c)
{
  uint32 i;

  c->m_numFcnSyms = 0;
  c->m_fcnSyms = NULL;
  c->m_fcnSymNames = NULL;

  if (c->m_pSymTbl != NULL) {
    // Count number of function symbols and amount of space for all symbol names
    // including null terminator at end of each name.
    int fcnSymNamesLen = 0;
    Elf32_Sym* pSym = c->m_pSymTbl;

    for (i = 0; i < c->m_nSyms; i++) {
      if (ELF32_ST_TYPE(pSym[i].st_info) == STT_FUNC) {
        c->m_numFcnSyms++;
        fcnSymNamesLen += strlen(&c->m_pSymStrTbl[Elf32Value(c, pSym[i].st_name)]) + 1;
      }
    }

    if (c->m_numFcnSyms != 0) {
      // Allocate space for function symbol names and sorted address array
      c->m_fcnSyms = (TFcnSym*)malloc(sizeof(TFcnSym) * c->m_numFcnSyms);
      c->m_fcnSymNames = (char*)malloc(fcnSymNamesLen);

      // Populate the sorted address array and function symbol names
      pSym = c->m_pSymTbl;
      TFcnSym* pFcnSym = c->m_fcnSyms;
      char* fcnSymName = c->m_fcnSymNames;

      for (i = 0; i < c->m_nSyms; i++) {
        if (ELF32_ST_TYPE(pSym[i].st_info) == STT_FUNC) {
          pFcnSym->begAddr = pSym[i].st_value;
          pFcnSym->endAddr = pSym[i].st_value + pSym[i].st_size;
          pFcnSym->szName  = fcnSymName;
          pFcnSym->shndx  = pSym[i].st_shndx;
          strcpy(pFcnSym->szName, &c->m_pSymStrTbl[Elf32Value(c, pSym[i].st_name)]);
          fcnSymName += strlen(pFcnSym->szName) + 1;
          pFcnSym++;
        }
      }

      // Sort the function symbols by address
      qsort(c->m_fcnSyms, c->m_numFcnSyms, sizeof(TFcnSym), CompareFcnSymsByAddr);
    }
  }
}

// Function Specification *****************************************************
//!
//! @brief Build a list of sections containing program data/text.
//!
// End Function Specification *************************************************

void BldPgmSecList(ElfContext* c)
{
}

// Function Specification *****************************************************
//!
//! @brief Dump Fcn Symbols
//!
// End Function Specification *************************************************

void DumpFcnSyms(ElfContext* c)
{
  uint32 i;

  // Walk through symbol table dumping function symbols
  if (c->m_pSymTbl != NULL) {
    Elf32_Sym* pSym = c->m_pSymTbl;

    for (i = 0; i < c->m_nSyms; i++) {
      // Determine if this symbol is a function symbol
      if (ELF32_ST_TYPE(pSym[i].st_info) == STT_FUNC) {

        uint32 start_addr = pSym[i].st_value;
        uint32 end_addr = pSym[i].st_value + pSym[i].st_size;
        char* fcn_name =  &c->m_pSymStrTbl[Elf32Value(c, pSym[i].st_name)];
        printf("0x%08x-0x%08x %s\n", start_addr, end_addr, fcn_name);
      }
    }
  }
}

// Function Specification *****************************************************
//!
//! @brief Dump Fcn Symbols sorted by starting address.
//!
// End Function Specification *************************************************

void DumpFcnSymsSorted(ElfContext* c)
{
  uint32 i;

  if ((c->m_numFcnSyms > 0) && (c->m_fcnSyms != NULL) && (c->m_fcnSymNames != NULL)) {
    TFcnSym* fcnSym = c->m_fcnSyms;

    for (i = 0; i < c->m_numFcnSyms; i++) {
      printf("0x%08x-0x%08x %s\n", fcnSym->begAddr, fcnSym->endAddr, fcnSym->szName);
      fcnSym++;
    }
  }
}

// Function Specification *****************************************************
//!
//! @brief Return the function name for the specified address.
//!
//! @param addr Address to look up
//!
//! @retval 'fcn'         Null terminated function name string
//! @retval 'spontaneous' If valid address but no function symbol found
//! @retval NULL          If invalid address
//!
//// End Function Specification *************************************************

char* GetFcnNameForAddr(ElfContext* c, uint32 addr)
{
  if ((c->m_numFcnSyms > 0) && (c->m_fcnSyms != NULL) && (c->m_fcnSymNames != NULL)) {
    TFcnSym* fcnSym = (TFcnSym*)bsearch((const void*)&addr, (const void*)c->m_fcnSyms,
                                        c->m_numFcnSyms, sizeof(TFcnSym), CompareAddrInFcn);
    if (fcnSym != NULL) {
      return fcnSym->szName;
    }
  }

  return (char*)"spontaneous";
}

unsigned int GetSymAddr(ElfContext* c, char* name)
{
  TFcnSym* fsym = c->m_fcnSyms;
  uint32 i;

  for (i = 0; i < c->m_numFcnSyms; i++) {
    if (!strcmp(name, fsym->szName)) {
      return fsym->begAddr;
    }
    fsym++;
  }

  return 0;
}

TFcnSym* GetSymStruct(ElfContext* c, uint32 addr)
{
  if ((c->m_numFcnSyms > 0) && (c->m_fcnSyms != NULL) && (c->m_fcnSymNames != NULL)) {
    TFcnSym* fcnSym = (TFcnSym*)bsearch((const void*)&addr, (const void*)c->m_fcnSyms,
                                        c->m_numFcnSyms, sizeof(TFcnSym), CompareAddrInFcn);
    if (fcnSym != NULL) {
      return fcnSym;
    }
  }

  return NULL;
}

unsigned int GetEntryAddr(ElfContext* c, uint32 addr)
{
  TFcnSym* fcnSym = GetSymStruct(c, addr);
  if (fcnSym == NULL) return 0;

  printf("** m_fileData=0x%08x, shndx=%d, off=0x%08x\n", (uint32)c->m_fileData, fcnSym->shndx, (uint32)c->m_pSecHdr[fcnSym->shndx].sh_offset);
  printf("** addr=0x%08x\n", addr);

  return addr + (uint32)c->m_fileData + (uint32)c->m_pSecHdr[fcnSym->shndx].sh_offset;
}

void LoadProgram(ElfContext* c, unsigned int addr)
{
  unsigned int va, fsz, msz;
  char* srcaddr, *destaddr;
  Elf32_Phdr* pPgmHdr = (Elf32_Phdr*)&c->m_fileData[c->m_pElfHdr->e_phoff];
  uint32 i;

  for (i = 0; i < c->m_pElfHdr->e_phnum; i++) {

    if ((pPgmHdr->p_type == PT_LOAD) && (pPgmHdr->p_memsz != 0)) {
      va = pPgmHdr->p_vaddr;
      fsz = pPgmHdr->p_filesz;
      msz = pPgmHdr->p_memsz;
      srcaddr = (char*)&c->m_fileData[pPgmHdr->p_offset];
      destaddr = (char*)(va + addr);

      memcpy(destaddr, srcaddr, fsz);

      printf("Loading segment @0x%08x, size=%d\n", (unsigned int)destaddr, fsz);

      if (msz > fsz) {
        printf("Zeroing 0x%08x through 0x%08x\n", (unsigned int)&destaddr[fsz], (unsigned int)&destaddr[msz - fsz - 1]);
        memset((void*)&destaddr[fsz], 0, msz - fsz);
      }
    }

    pPgmHdr++;
  }
}

// Function Specification *****************************************************
//!
//! @brief Return 'true' if specified address is the first address of a function.
//! Otherwise, return 'false'.
//!
// End Function Specification *************************************************

bool IsStartOfFcnAddr(ElfContext* c, uint32 addr)
{
  if ((c->m_numFcnSyms > 0) && (c->m_fcnSyms != NULL) && (c->m_fcnSymNames != NULL)) {
    TFcnSym* fcnSym = (TFcnSym*)bsearch((const void*)&addr, (const void*)c->m_fcnSyms,
                                        c->m_numFcnSyms, sizeof(TFcnSym), CompareAddrInFcn);
    if (fcnSym != NULL) {
      return (fcnSym->begAddr == addr);
    }
  }

  return false;
}

// Function Specification *****************************************************
//!
//! @brief Read ELF file and set up tracking info to key items
//!
// End Function Specification *************************************************

bool ReadFile(ElfContext* c, char* szFileName)
{
  // Open the file, seek to end, record position (size of file) and malloc space
  long nFileSize = 0;
  uint32  nTestEndian = 1, i;
  char szBuf[1024];

  // Determine host computer endianness
  c->m_nHostEndian = (*((char*)&nTestEndian) == 1) ? ELFDATALSB : ELFDATAMSB;

  if ((c->m_hFile = fopen(szFileName, "rb")) == NULL) {
    sprintf(szBuf, "Error: Unable to open file %s\n", szFileName);
    return false;
  }
  if (fseek(c->m_hFile, 0, SEEK_END) != 0) {
    sprintf(szBuf, "Error: File corrupt %s\n", szFileName);
    fclose(c->m_hFile);
    return false;
  }

  if ((nFileSize = ftell(c->m_hFile)) == -1L) {
    sprintf(szBuf, "Error: File corrupt %s\n", szFileName);
    fclose(c->m_hFile);
    return false;
  }

  if ((c->m_fileData = (char*)malloc(nFileSize)) == NULL) {
    sprintf(szBuf, "Error: Unable to allocate enough memory\n");
    fclose(c->m_hFile);
    return false;
  }

  if (fseek(c->m_hFile, 0, SEEK_SET) != 0) {
    sprintf(szBuf, "Error: File corrupt %s\n", szFileName);
    fclose(c->m_hFile);
    return false;
  }

  if (fread(c->m_fileData, nFileSize, 1, c->m_hFile) != 1) {
    sprintf(szBuf, "Error: Unable to read file %s\n", szFileName);
    fclose(c->m_hFile);
    return false;
  }

  // Set up pointer ELF header
  c->m_pElfHdr = (Elf32_Ehdr*)c->m_fileData;

  // Verify that this is a valid ELF file
  if ((c->m_pElfHdr->e_ident[EI_MAG0] != ELFMAG0) ||
      (c->m_pElfHdr->e_ident[EI_MAG1] != ELFMAG1) ||
      (c->m_pElfHdr->e_ident[EI_MAG2] != ELFMAG2) ||
      (c->m_pElfHdr->e_ident[EI_MAG3] != ELFMAG3)) {
    sprintf(szBuf, "Error: Object file is not ELF format: %s\n", szFileName);
    fclose(c->m_hFile);
    return false;
  }

  // Determine ELF file endianness
  c->m_nFileEndian = c->m_pElfHdr->e_ident[EI_DATA];

  // Set up Section Header info:
  //   pointer to first entry, number of entries, string table pointer
  c->m_pSecHdr = (Elf32_Shdr*)(c->m_fileData + (Elf32Value(c, c->m_pElfHdr->e_shoff)));
  c->m_nSecHdrs = Elf16Value(c, c->m_pElfHdr->e_shnum);
  c->m_pSecHdrStrTbl = (char*)(c->m_fileData + Elf32Value(c, c->m_pSecHdr[Elf16Value(c, c->m_pElfHdr->e_shstrndx)].sh_offset));

  // Set up Symbol Table info:
  //   pointer to first entry, number of symbols, symbol string table pointer
  Elf32_Shdr* pSecHdr = c->m_pSecHdr;

  for (i = 0; i < c->m_nSecHdrs; i++) {
    if (strcmp(&c->m_pSecHdrStrTbl[Elf32Value(c, pSecHdr->sh_name)], ".symtab") == 0) {
      c->m_pSymTbl = (Elf32_Sym*)(c->m_fileData + Elf32Value(c, pSecHdr->sh_offset));
      c->m_nSyms = Elf32Value(c, pSecHdr->sh_size) / sizeof(Elf32_Sym);
    }

    if (strcmp(&c->m_pSecHdrStrTbl[Elf32Value(c, pSecHdr->sh_name)], ".interp") == 0) {
      c->m_pInterpTbl = (char*)(c->m_fileData + Elf32Value(c, pSecHdr->sh_offset));
    }

    if (strcmp(&c->m_pSecHdrStrTbl[Elf32Value(c, pSecHdr->sh_name)], ".strtab") == 0) {
      c->m_pSymStrTbl = (char*)(c->m_fileData + Elf32Value(c, pSecHdr->sh_offset));
    }

    //printf("Section %s (%d) starts at addr 0x%08x\n", &c->m_pSecHdrStrTbl[Elf32Value(c, pSecHdr->sh_name)], i, (char*) (c->m_fileData + Elf32Value(c, pSecHdr->sh_offset)));

    pSecHdr++;
  }

  BldFcnSymList(c);

  return true;
}

// Function Specification *****************************************************
//!
//! @brief Dump Offset of object from beginning of ELF file
//!
// End Function Specification *************************************************

void DumpOffset(ElfContext* c, char* ptr)
{
  printf("%08x ", (uint32)(ptr - c->m_fileData));
}

// Function Specification *****************************************************
//!
//! @brief Dump Data supplied data bytes (1 to 4 bytes only)
//!
// End Function Specification *************************************************

void DumpData(ElfContext* c, char* pdata, size_t num_bytes)
{
  size_t i;

  for (i = 0; i < num_bytes; i++) {
    printf("%02x", (unsigned char)*pdata);
    pdata++;
  }

  for (i = num_bytes; i < 4; i++) {
    printf("  ");
  }

  printf(" ");
}

// Function Specification *****************************************************
//!
//! @brief Dump ELF Header
//!
// End Function Specification *************************************************

void DumpElfHdr(ElfContext* c)
{
  Elf32_Ehdr* pElfHdr = c->m_pElfHdr;
  uint32 i;

  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_MAG0]);   printf("  /----------------------------------------\n");
  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_MAG0]);   printf(" / ELF Header\n");
  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_MAG0]);   printf("/----------------------------------------\n");

  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_MAG0]);
  DumpData(c,   (char*)&pElfHdr->e_ident[EI_MAG0],
           sizeof(pElfHdr->e_ident[EI_MAG0]));
  printf("e_ident[EI_MAG0] (should be 0x7f)\n");

  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_MAG1]);
  DumpData(c,   (char*)&pElfHdr->e_ident[EI_MAG1],
           sizeof(pElfHdr->e_ident[EI_MAG1]));
  printf("e_ident[EI_MAG1] '%c' (should be 'E')\n",
         pElfHdr->e_ident[EI_MAG1]);

  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_MAG2]);
  DumpData(c,   (char*)&pElfHdr->e_ident[EI_MAG2],
           sizeof(pElfHdr->e_ident[EI_MAG2]));
  printf("e_ident[EI_MAG2] '%c' (should be 'L')\n",
         pElfHdr->e_ident[EI_MAG2]);

  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_MAG3]);
  DumpData(c,   (char*)&pElfHdr->e_ident[EI_MAG3],
           sizeof(pElfHdr->e_ident[EI_MAG3]));
  printf("e_ident[EI_MAG3] '%c' (should be 'F')\n",
         pElfHdr->e_ident[EI_MAG3]);

  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_CLASS]);
  DumpData(c,   (char*)&pElfHdr->e_ident[EI_CLASS],
           sizeof(pElfHdr->e_ident[EI_CLASS]));
  printf("e_ident[EI_CLASS] (%d) 0=Invalid, 1=32-bit objects, 2=64-bit objects\n",
         pElfHdr->e_ident[EI_CLASS]);

  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_DATA]);
  DumpData(c,   (char*)&pElfHdr->e_ident[EI_DATA],
           sizeof(pElfHdr->e_ident[EI_DATA]));
  printf("e_ident[EI_DATA] (%d) 0=Invalid, 1=Little endian, 2=Big endian\n",
         pElfHdr->e_ident[EI_DATA]);

  DumpOffset(c, (char*)&pElfHdr->e_ident[EI_VERSION]);
  DumpData(c,   (char*)&pElfHdr->e_ident[EI_VERSION],
           sizeof(pElfHdr->e_ident[EI_VERSION]));
  printf("e_ident[EI_VERSION] (%d)\n",
         pElfHdr->e_ident[EI_VERSION]);

  for (i = EI_PAD; i < EI_NIDENT; i++) {
    DumpOffset(c, (char*)&pElfHdr->e_ident[i]);
    DumpData(c,   (char*)&pElfHdr->e_ident[i],
             sizeof(pElfHdr->e_ident[i]));
    printf("e_ident[EI_PAD%d] (%d)\n",
           i - EI_PAD, pElfHdr->e_ident[i]);
  }

  DumpOffset(c, (char*)&pElfHdr->e_type);
  DumpData(c,   (char*)&pElfHdr->e_type,
           sizeof(pElfHdr->e_type));
  printf("e_type (%d) 0=No type, 1=Relocatable, 2=Executable, 3=Shared object, 4=Core, 0xff00=Proc specific, 0xffff=Proc specific\n",
         pElfHdr->e_type);

  DumpOffset(c, (char*)&pElfHdr->e_machine);
  DumpData(c,   (char*)&pElfHdr->e_machine,
           sizeof(pElfHdr->e_machine));
  printf("e_machine   Machine Architecture (%d) QDSP6=0xa4\n",
         pElfHdr->e_machine);

  DumpOffset(c, (char*)&pElfHdr->e_version);
  DumpData(c,   (char*)&pElfHdr->e_version,
           sizeof(pElfHdr->e_version));
  printf("e_version   Object file version (%d) should be 1\n",
         pElfHdr->e_version);

  DumpOffset(c, (char*)&pElfHdr->e_entry);
  DumpData(c,   (char*)&pElfHdr->e_entry,
           sizeof(pElfHdr->e_entry));
  printf("e_entry     Virtual address to start system (0x%08x)\n",
         pElfHdr->e_entry);

  DumpOffset(c, (char*)&pElfHdr->e_phoff);
  DumpData(c,   (char*)&pElfHdr->e_phoff,
           sizeof(pElfHdr->e_phoff));
  printf("e_phoff     Offset of Program Header Table (0x%08x)\n",
         pElfHdr->e_phoff);

  DumpOffset(c, (char*)&pElfHdr->e_shoff);
  DumpData(c,   (char*)&pElfHdr->e_shoff,
           sizeof(pElfHdr->e_shoff));
  printf("e_shoff     Offset of Section Header Table (0x%08x)\n",
         pElfHdr->e_shoff);

  DumpOffset(c, (char*)&pElfHdr->e_flags);
  DumpData(c,   (char*)&pElfHdr->e_flags,
           sizeof(pElfHdr->e_flags));
  printf("e_flags     Processor specific flags (%d)\n",
         pElfHdr->e_flags);

  DumpOffset(c, (char*)&pElfHdr->e_ehsize);
  DumpData(c,   (char*)&pElfHdr->e_ehsize,
           sizeof(pElfHdr->e_ehsize));
  printf("e_ehsize    ELF Header size in bytes (%d)\n",
         pElfHdr->e_ehsize);

  DumpOffset(c, (char*)&pElfHdr->e_phentsize);
  DumpData(c,   (char*)&pElfHdr->e_phentsize,
           sizeof(pElfHdr->e_phentsize));
  printf("e_phentsize Size in bytes of one Program Header Table entry (%d)\n",
         pElfHdr->e_phentsize);

  DumpOffset(c, (char*)&pElfHdr->e_phnum);
  DumpData(c,   (char*)&pElfHdr->e_phnum,
           sizeof(pElfHdr->e_phnum));
  printf("e_phnum     Number of Program Header Table Entries (%d)\n",
         pElfHdr->e_phnum);

  DumpOffset(c, (char*)&pElfHdr->e_shentsize);
  DumpData(c,   (char*)&pElfHdr->e_shentsize,
           sizeof(pElfHdr->e_shentsize));
  printf("e_shentsize Size in bytes of one Section Header Table entry (%d)\n",
         pElfHdr->e_shentsize);

  DumpOffset(c, (char*)&pElfHdr->e_shnum);
  DumpData(c,   (char*)&pElfHdr->e_shnum,
           sizeof(pElfHdr->e_shnum));
  printf("e_shnum     Number of Section Header Table Entries (%d)\n",
         pElfHdr->e_shnum);

  DumpOffset(c, (char*)&pElfHdr->e_shstrndx);
  DumpData(c,   (char*)&pElfHdr->e_shstrndx,
           sizeof(pElfHdr->e_shstrndx));
  printf("e_shstrndx  Section Header Table index of section name string table (%d)\n",
         pElfHdr->e_shstrndx);
}

// Function Specification *****************************************************
//!
//! @brief Dump Program Header Table
//!
// End Function Specification *************************************************

void DumpPgmHdrTbl(ElfContext* c)
{
  Elf32_Phdr* pPgmHdr = (Elf32_Phdr*)&c->m_fileData[c->m_pElfHdr->e_phoff];
  uint32 i;

  for (i = 0; i < c->m_pElfHdr->e_phnum; i++) {

    DumpOffset(c, (char*)&pPgmHdr->p_type);   printf("  /----------------------------------------\n");
    DumpOffset(c, (char*)&pPgmHdr->p_type);   printf(" / Program Header Entry %-2d \n", i + 1);
    DumpOffset(c, (char*)&pPgmHdr->p_type);   printf("/----------------------------------------\n");

    DumpOffset(c, (char*)&pPgmHdr->p_type);
    DumpData(c,   (char*)&pPgmHdr->p_type,
             sizeof(pPgmHdr->p_type));
    printf("p_type   (%d)\n",
           pPgmHdr->p_type);

    DumpOffset(c, (char*)&pPgmHdr->p_offset);
    DumpData(c,   (char*)&pPgmHdr->p_offset,
             sizeof(pPgmHdr->p_offset));
    printf("p_offset (0x%x)\n",
           pPgmHdr->p_offset);

    DumpOffset(c, (char*)&pPgmHdr->p_vaddr);
    DumpData(c,   (char*)&pPgmHdr->p_vaddr,
             sizeof(pPgmHdr->p_vaddr));
    printf("p_vaddr  (0x%08x)\n",
           pPgmHdr->p_vaddr);

    DumpOffset(c, (char*)&pPgmHdr->p_paddr);
    DumpData(c,   (char*)&pPgmHdr->p_paddr,
             sizeof(pPgmHdr->p_paddr));
    printf("p_paddr  (0x%08x)\n",
           pPgmHdr->p_paddr);

    DumpOffset(c, (char*)&pPgmHdr->p_filesz);
    DumpData(c,   (char*)&pPgmHdr->p_filesz,
             sizeof(pPgmHdr->p_filesz));
    printf("p_filesz (%d)\n",
           pPgmHdr->p_filesz);

    DumpOffset(c, (char*)&pPgmHdr->p_memsz);
    DumpData(c,   (char*)&pPgmHdr->p_memsz,
             sizeof(pPgmHdr->p_memsz));
    printf("p_memsz  (%d)\n",
           pPgmHdr->p_memsz);

    DumpOffset(c, (char*)&pPgmHdr->p_flags);
    DumpData(c,   (char*)&pPgmHdr->p_flags,
             sizeof(pPgmHdr->p_flags));
    printf("p_flags  (%d)\n",
           pPgmHdr->p_flags);

    DumpOffset(c, (char*)&pPgmHdr->p_align);
    DumpData(c,   (char*)&pPgmHdr->p_align,
             sizeof(pPgmHdr->p_align));
    printf("p_align  (%d)\n",
           pPgmHdr->p_align);

    pPgmHdr++;
  }
}

// Function Specification *****************************************************
//!
//! @brief Dump Section Header Table
//!
// End Function Specification *************************************************

void DumpSecHdrTbl(ElfContext* c)
{
  Elf32_Shdr* pSecHdr = (Elf32_Shdr*)&c->m_fileData[c->m_pElfHdr->e_shoff];
  uint32 i;

  for (i = 0; i < c->m_pElfHdr->e_shnum; i++) {

    DumpOffset(c, (char*)&pSecHdr->sh_name);   printf("  /----------------------------------------\n");
    DumpOffset(c, (char*)&pSecHdr->sh_name);   printf(" / Section Header Entry %-2d \n", i + 1);
    DumpOffset(c, (char*)&pSecHdr->sh_name);   printf("/----------------------------------------\n");

    DumpOffset(c, (char*)&pSecHdr->sh_name);
    DumpData(c,   (char*)&pSecHdr->sh_name,
             sizeof(pSecHdr->sh_name));
    printf("sh_name      (Index=%d) '%s'\n",
           pSecHdr->sh_name, &c->m_pSecHdrStrTbl[pSecHdr->sh_name]);

    DumpOffset(c, (char*)&pSecHdr->sh_type);
    DumpData(c,   (char*)&pSecHdr->sh_type,
             sizeof(pSecHdr->sh_type));
    printf("sh_type      (%s)\n",
           pSecHdr->sh_type == SHT_NULL     ? "NULL"    :
                                              pSecHdr->sh_type == SHT_PROGBITS ? "PROGBITS" :
                                                                                 pSecHdr->sh_type == SHT_SYMTAB   ? "SYMTAB"  :
                                                                                                                    pSecHdr->sh_type == SHT_STRTAB   ? "STRTAB"  :
                                                                                                                                                       pSecHdr->sh_type == SHT_RELA     ? "RELA"    :
                                                                                                                                                                                          pSecHdr->sh_type == SHT_HASH     ? "HASH"    :
                                                                                                                                                                                                                             pSecHdr->sh_type == SHT_DYNAMIC  ? "DYNAMIC" :
                                                                                                                                                                                                                                                                pSecHdr->sh_type == SHT_NOTE     ? "NOTE"    :
                                                                                                                                                                                                                                                                                                   pSecHdr->sh_type == SHT_NOBITS   ? "NOBITS"  :
                                                                                                                                                                                                                                                                                                                                      pSecHdr->sh_type == SHT_REL      ? "REL"     :
                                                                                                                                                                                                                                                                                                                                                                         pSecHdr->sh_type == SHT_SHLIB    ? "SHLIB "  :
                                                                                                                                                                                                                                                                                                                                                                                                            pSecHdr->sh_type == SHT_DYNSYM   ? "DYNSYM"  :
                                                                                                                                                                                                                                                                                                                                                                                                                                               pSecHdr->sh_type == SHT_LOPROC   ? "LOPROC"  :
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  pSecHdr->sh_type == SHT_HIPROC   ? "HIPROC"  :
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     pSecHdr->sh_type == SHT_LOUSER   ? "LOUSER"  :
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        pSecHdr->sh_type == SHT_HIUSER   ? "HIUSER"  : "");

    DumpOffset(c, (char*)&pSecHdr->sh_flags);
    DumpData(c,   (char*)&pSecHdr->sh_flags,
             sizeof(pSecHdr->sh_flags));
    printf("sh_flags     (%s %s %s)\n",
           pSecHdr->sh_flags & SHF_WRITE ? "WRITE" : "",
           pSecHdr->sh_flags & SHF_ALLOC ? "ALLOC" : "",
           pSecHdr->sh_flags & SHF_EXECINSTR ? "EXECINSTR" : "");

    DumpOffset(c, (char*)&pSecHdr->sh_addr);
    DumpData(c,   (char*)&pSecHdr->sh_addr,
             sizeof(pSecHdr->sh_addr));
    printf("sh_addr      (%d)\n",
           pSecHdr->sh_addr);

    DumpOffset(c, (char*)&pSecHdr->sh_offset);
    DumpData(c,   (char*)&pSecHdr->sh_offset,
             sizeof(pSecHdr->sh_offset));
    printf("sh_offset    (0x%x)\n",
           pSecHdr->sh_offset);

    DumpOffset(c, (char*)&pSecHdr->sh_size);
    DumpData(c,   (char*)&pSecHdr->sh_size,
             sizeof(pSecHdr->sh_size));
    printf("sh_size      (%d)\n",
           pSecHdr->sh_size);

    DumpOffset(c, (char*)&pSecHdr->sh_link);
    DumpData(c,   (char*)&pSecHdr->sh_link,
             sizeof(pSecHdr->sh_link));
    printf("sh_link      (%d)\n",
           pSecHdr->sh_link);

    DumpOffset(c, (char*)&pSecHdr->sh_info);
    DumpData(c,   (char*)&pSecHdr->sh_info,
             sizeof(pSecHdr->sh_info));
    printf("sh_info      (%d)\n",
           pSecHdr->sh_info);

    DumpOffset(c, (char*)&pSecHdr->sh_addralign);
    DumpData(c,   (char*)&pSecHdr->sh_addralign,
             sizeof(pSecHdr->sh_addralign));
    printf("sh_addralign (%d)\n",
           pSecHdr->sh_addralign);

    DumpOffset(c, (char*)&pSecHdr->sh_entsize);
    DumpData(c,   (char*)&pSecHdr->sh_entsize,
             sizeof(pSecHdr->sh_entsize));
    printf("sh_entsize   (%d)\n",
           pSecHdr->sh_entsize);

    pSecHdr++;
  }
}

// Function Specification *****************************************************
//!
//! @brief Dump Section Data
//!
// End Function Specification *************************************************

void DumpSecData(ElfContext* c)
{
  //-------------------------------------------------------------------------
  // Dump Section Data
  //-------------------------------------------------------------------------
  Elf32_Shdr* pSecHdr = (Elf32_Shdr*)&c->m_fileData[c->m_pElfHdr->e_shoff];
  uint32 i;

  for (i = 0; i < c->m_pElfHdr->e_shnum; i++) {

    char* pdata = (char*)&c->m_fileData[pSecHdr->sh_offset];
    Elf32_Word size = pSecHdr->sh_size;
    if (pSecHdr->sh_type == SHT_NOBITS) {
      size = 0;
    }

    if (pdata != (char*)c->m_pElfHdr) {

      DumpOffset(c, pdata);   printf("/%2da----------------------------------------\n", i + 1);
      DumpOffset(c, pdata);   printf("/%2db Section %-2d '%s' Data (%d bytes)\n", i + 1, i + 1, &c->m_pSecHdrStrTbl[pSecHdr->sh_name], size);
      DumpOffset(c, pdata);   printf("/%2dc----------------------------------------\n", i + 1);

      // Dump Symbol table entries if section is .symtab
      if (strcmp(&c->m_pSecHdrStrTbl[pSecHdr->sh_name], ".symtab") == 0) {
        Elf32_Sym* pSym = (Elf32_Sym*)pdata;

        Elf32_Word j = 1;

        while (size > 0) {
          DumpOffset(c, (char*)&pSym->st_name);   printf("  /----------------------------------------\n");
          DumpOffset(c, (char*)&pSym->st_name);   printf(" / Symbol Table Entry %-2d \n", j);
          DumpOffset(c, (char*)&pSym->st_name);   printf("/----------------------------------------\n");

          DumpOffset(c, (char*)&pSym->st_name);
          DumpData(c,   (char*)&pSym->st_name,
                   sizeof(pSym->st_name));
          printf("st_name   (%d) String Table Index '%s'\n",
                 pSym->st_name,
                 &c->m_pSymStrTbl[pSym->st_name]);

          DumpOffset(c, (char*)&pSym->st_value);
          DumpData(c,   (char*)&pSym->st_value,
                   sizeof(pSym->st_value));
          printf("st_value  (0x%08x, %d) Absolute value, address etc\n",
                 pSym->st_value,
                 pSym->st_value);

          DumpOffset(c, (char*)&pSym->st_size);
          DumpData(c,   (char*)&pSym->st_size,
                   sizeof(pSym->st_size));
          printf("st_size   (%d) Size of object in bytes\n",
                 pSym->st_size);

          DumpOffset(c, (char*)&pSym->st_info);
          DumpData(c,   (char*)&pSym->st_info,
                   sizeof(pSym->st_info));
          printf("st_info   [7:4]=%d Binding=%s, [3:0]=%d Type=%s\n",
                 ELF32_ST_BIND(pSym->st_info),
                 ELF32_ST_BIND(pSym->st_info) == STB_LOCAL  ? "LOCAL " :
                                                              ELF32_ST_BIND(pSym->st_info) == STB_GLOBAL ? "GLOBAL" :
                                                                                                           ELF32_ST_BIND(pSym->st_info) == STB_WEAK   ? "WEAK  " :
                                                                                                                                                        ELF32_ST_BIND(pSym->st_info) == STB_LOPROC ? "LOPROC" :
                                                                                                                                                                                                     ELF32_ST_BIND(pSym->st_info) == STB_HIPROC ? "HIPROC" : "???   ",
                 ELF32_ST_TYPE(pSym->st_info),
                 ELF32_ST_TYPE(pSym->st_info) == STT_NOTYPE  ? "NOTYPE " :
                                                               ELF32_ST_TYPE(pSym->st_info) == STT_OBJECT  ? "OBJECT " :
                                                                                                             ELF32_ST_TYPE(pSym->st_info) == STT_FUNC    ? "FUNC   " :
                                                                                                                                                           ELF32_ST_TYPE(pSym->st_info) == STT_SECTION ? "SECTION" :
                                                                                                                                                                                                         ELF32_ST_TYPE(pSym->st_info) == STT_FILE    ? "FILE   " :
                                                                                                                                                                                                                                                       ELF32_ST_TYPE(pSym->st_info) == STT_LOPROC  ? "LOPROC " :
                                                                                                                                                                                                                                                                                                     ELF32_ST_TYPE(pSym->st_info) == STT_HIPROC  ? "HIPROC " : "???    ");

          DumpOffset(c, (char*)&pSym->st_other);
          DumpData(c,   (char*)&pSym->st_other,
                   sizeof(pSym->st_other));
          printf("st_other  (%d) should be 0\n",
                 pSym->st_other);

          DumpOffset(c, (char*)&pSym->st_shndx);
          DumpData(c,   (char*)&pSym->st_shndx,
                   sizeof(pSym->st_shndx));
          printf("st_shndx  (%d) Section Header Table Index '%s'\n",
                 pSym->st_shndx,
                 (pSym->st_shndx < c->m_pElfHdr->e_shnum) ?
                 &c->m_pSecHdrStrTbl[((Elf32_Shdr*)&c->m_fileData[c->m_pElfHdr->e_shoff])[pSym->st_shndx].sh_name]
                 : "???");

          j++;
          pSym++;
          size -= sizeof(Elf32_Sym);
        }
      } else {
        // Dump raw data bytes (16 bytes per line) with ascii output
        while (size > 0) {
          Elf32_Word j;

          DumpOffset(c, pdata);

          for (j = 0; j < 16; j++) {
            if (j < size) {
              printf("%02x ", (unsigned char)pdata[j]);
            } else {
              printf("   ");
            }
          }

          printf("|");
          for (j = 0; j < 16; j++) {
            if (j < size) {
              printf("%c", isprint((unsigned char)pdata[j]) ? (unsigned char)pdata[j] : '.');
            } else {
              printf(" ");
            }
          }
          printf("|\n");

          if (size > 16) {
            size -= 16;
          } else {
            size = 0;
          }

          pdata += 16;
        }
      }
    }

    pSecHdr++;
  }
}

// Function Specification *****************************************************
//!
//! @brief Dump entire ELF file contents (ELF hdr, Pgm Hdrs, Sec hdrs, Data)
//!
// End Function Specification *************************************************

void DumpAll(ElfContext* c)
{
  DumpElfHdr(c);
  DumpPgmHdrTbl(c);
  DumpSecHdrTbl(c);
  DumpSecData(c);
}

