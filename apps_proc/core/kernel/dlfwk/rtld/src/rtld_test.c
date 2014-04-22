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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <dlfcn.h>

#include "HAP_debug.h"
#include "sigverify.h"

int tv_sigverify_return = 0;

int SigVerify_AuthenticateELF(const unsigned char *pELF, int cbELF)
{
  // TODO, verify
   if (pELF == 0 || cbELF <= 0) {
     printf("error: pELF %p, cbELF %d\n", pELF, cbELF);
     return 1;
   }
   return tv_sigverify_return;
}

#define NUM_ITER 1

int main(int a, char **c)
{
  char *builtin[] = {(char*)"libc.so", (char*)"libstdc++.so", (char*)"libgcc.so"};
  DL_vtbl vtbl = {sizeof(DL_vtbl), HAP_debug, SigVerify_AuthenticateELF};
  void* h = 0;
  void (*pfn)(void);
  int i;
  int nErr = 0;

  //(void)dlinitex(3, builtin, &vtbl);
  //dldeinit();

  (void)dlinitex(3, builtin, &vtbl);

  printf("===== test_elf_nop.so ===============================\n");
  h = dlopen("test_elf_nop.so", RTLD_NOW);
  if (0 == h) {
    printf("dlopen failed: %s\n", dlerror());
    nErr = 1;
    goto bail;
  }
  pfn = dlsym(h, "nop");
  if (0 == pfn) {
    printf("dlsym failed: %s\n", dlerror());
    nErr = 1;
    goto bail;
  }
  pfn();
  dlclose(h);

  for (i = 0; i < NUM_ITER; i++) {
    printf("===== test_elf_dep1.so ===============================\n");
    h = dlopen("test_elf_dep1.so", RTLD_NOW);
    if (0 == h) {
      printf("dlopen failed: %s\n", dlerror());
      nErr = 1;
      goto bail;
    }
    pfn = dlsym(h, "dep1");
    if (0 == pfn) {
      printf("dlsym failed: %s\n", dlerror());
      nErr = 1;
      goto bail;
    }
    pfn();
    dlclose(h);
  }

  printf("===== test_elf_dep3.so ===============================\n");
  h = dlopen("test_elf_dep3.so", RTLD_NOW);
  if (0 != h) {
    printf("dlopen success even with missing dep4: %s\n", dlerror());
    nErr = 1;
    goto bail;
  }

  printf("===== sig failure ===============================\n");
  tv_sigverify_return = 1;
  h = dlopen("test_elf_nop.so", RTLD_NOW);
  if (0 != h) {
    printf("dlopen success even with sigveriyf failure: %s\n", dlerror());
    nErr = 1;
    goto bail;
  }
  printf("dlopen failed: %s\n", dlerror());


  printf("\ntests passed!\n");

bail:
  if (0 != nErr) {
    return 1;
  }
  return 0;
}

#if 1
// TODO temporary until standalone test gets fixed

/*****************************************************************
# Copyright (c) $Date: 2020/01/30 $ Qualcomm Technologies INCORPORATED.
# All Rights Reserved.
# Modified by Qualcomm Technologies INCORPORATED on $Date: 2020/01/30 $
*****************************************************************/

#include <types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>

void hexagon_code_clean(const uint8_t* addr, size_t nbytes)
{
  /*
  * 32byte cache line size
  * == 5 bits --> 0x1f
  * 32 == 0x1 << 5
  */

  int ntimes = ((nbytes + ((uint32_t)addr & 0x1f) + 0x1f)) >> 5; /* upward align */
  uint32_t readback;

  while (ntimes--) {
    asm volatile("dccleaninva (%0)\n\t"
                 ::"r"(addr));
    asm volatile("syncht \n\t"
                 "icinva (%0)\n\t"
                 "isync\n\t"
                 ::"r"(addr));
    addr += 0x20;
  }
  addr = addr - 0x20;

  asm volatile("%[readback] = memb(%[addr])\n\t"
                 :[readback] "=r" (readback)
                   :[addr]     "r"  (addr));
  asm volatile("dccleaninva (%0)\n\t"
                   ::"r"(addr));
}

_STD_BEGIN
void* sys_mmap(void* start, size_t length, int prot, int flags, int fd, off_t offset)
{
  void* ptr = (void*)0;

  if ((ptr = (void*)memalign(0x1000, length)) == 0) {
    printf("%s:%d: memalign of length = 0x%x failed\n", __func__, __LINE__, length);
    return (void*)0;
  }

  if (fd == -1) {
    memset(ptr, 0, length);
    return ptr;
  }

  if (lseek(fd, offset, SEEK_SET) != 0) {
    printf("%s:%d: fseek failed \n", __func__, __LINE__);
    return (void*)0;
  }
  if (read(fd, ptr, length) != length) {
    printf("%s:%d: fread failed \n", __func__, __LINE__);
    return (void*)0;
  }
  return ptr;
}
_STD_END

_STD_BEGIN
int sys_munmap(void* start, size_t length)
{
  free(start);
  return 0;
}

_STD_END

_STD_BEGIN
int sys_mprotect(void* start, size_t length, int prot)
{
  hexagon_code_clean(start, length);
  return 0;
}

_STD_END

#endif


