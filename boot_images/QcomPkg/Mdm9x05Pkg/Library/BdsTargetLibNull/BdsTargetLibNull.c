#include "Uefi.h"
#include <Library/BdsTargetLib.h>

EFI_STATUS OverrideAblLoadAddr(UINTN *Addr)
{
	
  *Addr = (UINTN) 0x9fa00000;
  // PRE_SIL HACK :  return EFI_UNSUPPORTED;
  return EFI_SUCCESS;
}
