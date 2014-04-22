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

/*  $NetBSD: map_object.c,v 1.31 2004/10/22 05:39:56 skrll Exp $     */

/*
 * Copyright 1996 John D. Polstra.
 * Copyright 1996 Matt Thomas <matt@3am-software.com>
 * Copyright 2002 Charles M. Hannum <root@ihack.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by John Polstra.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sys_mman.h"
#include "dlfs.h"
#include "rtld.h"

#define NSEGS_MAX 5

void
_rtld_obj_free(Obj_Entry* obj)
{
  Objlist_Entry* elm;

  if (obj->path) {
    free(obj->path);
  }

  while (obj->needed != NULL) {
    Needed_Entry* needed = obj->needed;
    obj->needed = needed->next;
    free(needed);
  }

  while ((elm = SIMPLEQ_FIRST(&obj->dldags)) != NULL) {
    SIMPLEQ_REMOVE_HEAD(&obj->dldags, link);
    free(elm);
  }

  while ((elm = SIMPLEQ_FIRST(&obj->dagmembers)) != NULL) {
    SIMPLEQ_REMOVE_HEAD(&obj->dagmembers, link);
    free(elm);
  }

  free(obj);
}

Obj_Entry*
_rtld_obj_new(void)
{
  Obj_Entry* obj;

  obj = malloc(sizeof(Obj_Entry));
  if(obj == NULL) return NULL;
  memset(obj, 0, sizeof(Obj_Entry));
  SIMPLEQ_INIT(&obj->dldags);
  SIMPLEQ_INIT(&obj->dagmembers);
  return obj;
}

static char *strdup(const char *s){
	char *d = malloc (strlen (s) + 1);   
    if (d == NULL) return NULL;         
    if(strlcpy(d, s, sizeof(d)) >= sizeof(d)) return d;          
    else free(d);

    return NULL;
}

int
_rtld_verify_ehdr(Elf_Ehdr* ehdr, const char* path)
{
  int nErr = 0;

  VERIFYM(nErr, memcmp(ELFMAG, ehdr->e_ident, SELFMAG) == 0 &&
          ehdr->e_ident[EI_CLASS] == ELFCLASS,
          "%s: unrecognized file format", path);
  VERIFYM(nErr, ehdr->e_ident[EI_VERSION] == EV_CURRENT &&
          ehdr->e_version == EV_CURRENT /* &&
        ehdr->e_ident[EI_DATA] == ELFDEFNNAME(MACHDEP_ENDIANNESS) */,
          "%s: unsupported file version", path);
  VERIFYM(nErr, ehdr->e_type == ET_EXEC || ehdr->e_type == ET_DYN,
          "%s: unsupported file type", path);
  VERIFYM(nErr, ehdr->e_phentsize == sizeof(Elf_Phdr),
          "ehdr->e_phentsize != Elf_Phdr");
  VERIFYM(nErr, ehdr->e_phoff + ehdr->e_phnum * sizeof(Elf_Phdr) <= _rtld_pagesz,
          "Elf_Phdr header not found in first page");

  CATCH(nErr) {}
  return nErr;
}

int
_rtld_digest_phdr2(Elf_Phdr* phdr, int nphdr, const char* path, Elf_Phdr** loads, int* nloads,
                   int maxloads, unsigned int* upper_limit, const char** interp,
                   Elf_Dyn** dynamic)
{
  Elf_Phdr* phdr_limit = phdr + nphdr;
  int nErr = 0;

  // Scan the program header entries, and save key information
  for (*nloads = 0,*upper_limit = 0; phdr < phdr_limit; phdr++) {
    switch(phdr->p_type) {
      case PT_INTERP:
        *interp = (void*)phdr->p_vaddr;
        break;

      case PT_LOAD:
        if (*nloads < maxloads) {
          loads[*nloads] = phdr;
        }
        (*nloads)++;
        *upper_limit = ((phdr->p_vaddr + phdr->p_memsz) > *upper_limit) ?
           phdr->p_vaddr + phdr->p_memsz : *upper_limit;
        break;

      case PT_DYNAMIC:
        *dynamic = (void*)phdr->p_vaddr;
        break;
    }
  }

  VERIFYM(nErr, *dynamic, "%s: not dynamically linked", path);
  VERIFYM(nErr, *nloads <= maxloads, "%s: wrong number of segments (%d > %d)",
          path, *nloads, maxloads);

  CATCH(nErr) {}
  return nErr;
}

/*
 * Given a set of ELF protection flags, return the corresponding protection
 * flags for MMAP.
 */
static int
protflags(int elfflags)
{
  int prot = 0;

  if (elfflags & PF_R) {
    prot |= PROT_READ;
  }
  if (elfflags & PF_W) {
    prot |= PROT_WRITE;
  }
  if (elfflags & PF_X) {
    prot |= PROT_EXEC;
  }

  return prot;
}

/*
 * Map a shared object into memory.  The argument is a file descriptor,
 * which must be open on the object and positioned at its beginning.
 *
 * The return value is a pointer to a newly-allocated Obj_Entry structure
 * for the shared object.  Returns NULL on failure.
 */
Obj_Entry*
_rtld_map_object(const char* path, int fd, const struct dlfs_stat* sb)
{
  Obj_Entry* obj = 0;
  Elf_Ehdr* ehdr = 0;
  Elf_Phdr* loads[NSEGS_MAX];
  int nloads;
  unsigned int upper_limit;
  caddr_t mapbase = MAP_FAILED;
  size_t mapsize = 0;
  int nErr = 0;

  VERIFYM(nErr, 0 != (obj = _rtld_obj_new()), "_rtld_obj_new failed: %s", path);

  // only file based loading supported
  VERIFY(nErr, sb);

  // We rely on the program header being in the first page.  This is
  // not strictly required by the ABI specification, but it seems to
  // always true in practice.  And, it simplifies things considerably
  VERIFYM(nErr, 0 != (ehdr = malloc(_rtld_pagesz)), "malloc failed (0x%X B) %s",
          _rtld_pagesz, path);
  {
    int nread = 0;
    VERIFYM(nErr, _rtld_pagesz == (nread = dlfs_read(fd, (char*)ehdr, _rtld_pagesz)),
            "read failed (0x%X of 0x%X B read) %s", nread, _rtld_pagesz, path);
  }

  VERIFY(nErr, 0 == _rtld_verify_ehdr(ehdr, path));
  VERIFY(nErr, 0 == _rtld_digest_phdr2((Elf_Phdr*)((caddr_t)ehdr + ehdr->e_phoff),
                                       ehdr->e_phnum, path, loads, &nloads, NSEGS_MAX,
                                       &upper_limit, &obj->interp, &obj->dynamic));

  {
    // Get a chunk big enough for everything and read entire file
    int file_size = sb->st_size;

    mapsize = file_size > upper_limit ? round_up(file_size) : upper_limit;
    VERIFYM(nErr, MAP_FAILED != (mapbase = sys_mmap((void*)0, mapsize, PROT_READ | PROT_WRITE, 0, -1, 0)),
            "failed to mmap (0x%X B) %s", mapsize, path);
    FARF(HIGH, "mapped  [%p - %p] (0x%X B)", mapbase, mapbase + mapsize, mapsize);

	dlfs_seek(fd, 0);
    //VERIFYM(nErr, 0 == dlfs_seek(fd, 0, SEEK_SET), "lseek failed (0x%X B) %s", 0, path);
    VERIFYM(nErr, file_size == dlfs_read(fd, (void*)mapbase, file_size),
            "read failed (0x%X B) %s", file_size, path);

    if (_rtld_vtbl.verify) {
      VERIFYM(nErr, 0 == _rtld_vtbl.verify((const unsigned char*)mapbase, file_size),
              "signature verification failed (0x%X B) %s", file_size, path);
    } else {
      FARF(ERROR, "!! verification ignored, no function specified !!");
    }
  }

  {
    // seup memory view
    int load;
    Elf64_Addr last_seg_end_vaddr = 0;

    for (load = 0; load < nloads; load++) {
      Elf64_Addr vaddr = loads[load]->p_vaddr;
      Elf64_Off offset = loads[load]->p_offset;
      Elf64_Xword filesz = loads[load]->p_filesz;
      Elf64_Xword memsz = loads[load]->p_memsz;
      caddr_t addr = mapbase + vaddr;
      last_seg_end_vaddr = (Elf64_Addr)addr + memsz;

      if (offset != vaddr) {
        int i;
        // to conserve space, file view converted to memory view in place,
        // bail if any moves will overwrite a not yet moved segment
        for (i = load + 1; i < nloads; i++) 
        {
          int c1, c2, c3, c4;
          c1 = vaddr < loads[i]->p_offset ? 1:0;
          c2 = vaddr >= (loads[i]->p_offset + loads[i]->p_filesz)? 1: 0;
          c3 = vaddr + memsz < loads[i]->p_offset ? 1:0;
          c4 = vaddr + memsz >= (loads[i]->p_offset + loads[i]->p_filesz) ? 1:0;
          printf("c1: %d, c2: %d, c3:%d, c4:%d\n", c1, c2, c3, c4); 
          VERIFYM(nErr,
                  (vaddr < loads[i]->p_offset ||
                   vaddr >= (loads[i]->p_offset + loads[i]->p_filesz)) &&
                  (vaddr + memsz < loads[i]->p_offset ||
                   vaddr + memsz >= (loads[i]->p_offset + loads[i]->p_filesz)),
                  "Unsupported: overlapping segments %d %d %s", load, i, path);
        }
        printf("moving segment %d from %#X to %#x\n", load, (mapbase +offset), addr);
        if((Elf64_Addr)addr <= last_seg_end_vaddr)
        {
          printf("Overlap moving segment %d\n", load);
        }
        FARF(HIGH, "move    %p -> %p (0x%llX B)",
             mapbase + offset, addr, filesz);
        memmove((void*)addr, (void*)(mapbase + offset), filesz);
      }

      if (memsz > filesz) {
        FARF(HIGH, "zero    [%p - %p] (0x%llX B)",
             addr + filesz, addr + memsz, memsz - filesz);
        memset((void*)(addr + filesz), 0, memsz - filesz);
      }

      //printf("segment %d resides from %#x to %#x\n", load, addr, addr + memsz);
      FARF(HIGH, "protect [%p - %p] (0x%llX B) %c%c%c",
           addr, addr + memsz, memsz,
           loads[load]->p_flags & PF_R ? 'r' : '.',
           loads[load]->p_flags & PF_W ? 'w' : '.',
           loads[load]->p_flags & PF_X ? 'x' : '.');

      //VERIFYM(nErr, 0 == sys_mprotect(addr, memsz, protflags(loads[load]->p_flags)),
              //"Cannot protect segment %d %s", load, path);

      if (loads[load]->p_flags & PF_X) {
        // remember text section, will mprotect again during reloc
        // TODO assuming only one executable region containing code
        // TODO assuming text region filesz == memsz
        obj->textbase = addr;
        obj->textsize = filesz;
      }
    }

    //Set appropriate permissions
    for (load = 0; load < nloads ; load++) 
    {
      Elf64_Addr vaddr = loads[load]->p_vaddr;
      Elf64_Xword memsz = loads[load]->p_memsz;
      caddr_t addr = mapbase + vaddr;

      FARF(HIGH, "protect [%p - %p] (0x%llX B) %c%c%c",
           addr, addr + memsz, memsz,
           loads[load]->p_flags & PF_R ? 'r' : '.',
           loads[load]->p_flags & PF_W ? 'w' : '.',
           loads[load]->p_flags & PF_X ? 'x' : '.');

      printf("segment %d from %#x to %#x, protection: %d \n", load, addr, addr + memsz, protflags(loads[load]->p_flags));
      VERIFYM(nErr, 0 == sys_mprotect(addr, memsz, protflags(loads[load]->p_flags)),
              "Cannot protect segment %d %s", load, path);

    }


  }

  obj->path = strdup(path);
  obj->pathlen = strlen(path);
  obj->isdynamic = ehdr->e_type == ET_DYN;
  obj->vaddrbase = 0;
  obj->mapbase = mapbase;
  obj->mapsize = mapsize;
  obj->relocbase = mapbase;
  obj->dynamic = (Elf_Dyn*)(obj->relocbase + (Elf_Addr)obj->dynamic);
  obj->entry = ehdr->e_entry ? (caddr_t)(obj->relocbase + ehdr->e_entry) : 0;
  obj->interp = obj->interp ? (const char*)(obj->relocbase + (Elf_Addr)obj->interp) : 0;;
  // If these are both 0 then we must be running on windows so
  // create artificial inode/dev using the size and createtion time
  if (sb->st_dev == 0 && sb->st_ino == 0) {
    obj->ino = (ino_t)sb->st_size;
    obj->dev = (dev_t)sb->st_ctime;
  } else {
    obj->dev = sb->st_dev;
    obj->ino = sb->st_ino;
  }

  CATCH(nErr) {
    if (mapbase != MAP_FAILED) {
      sys_munmap(mapbase, mapsize);
    }

    if (obj) {
      _rtld_obj_free(obj);
      obj = 0;
    }
  }

  if (ehdr) {
    free(ehdr);
  }

  return obj;
}

