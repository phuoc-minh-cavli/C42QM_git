/** @file

  This file contains the definination for host controller memory management routines.

  Portions copyright (c) 2010 - 2011, 2015, 2019 Qualcomm Technologies, Inc. All rights reserved.
  Copyright (c) 2007 - 2010, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 07/07/15   ck       Enable 4 USB port support and fix PHY reset steps
 04/27/15   ck       Add initial Hawker support
 03/30/11   sniedzie Port changes for:
                     - Support for embedded transaction translator.
                     - Cache flushing workaround.
                     - Put PHY in host mode on reset.
 03/30/11   sniedzie Integrated from MdeModulePkg for QCOM changes.

=============================================================================*/

#ifndef _EFI_EHCI_MEM_H_
#define _EFI_EHCI_MEM_H_

#define USB_HC_BIT(a)                  ((UINTN)(1 << (a)))

#define USB_HC_BIT_IS_SET(Data, Bit)   \
          ((BOOLEAN)(((Data) & USB_HC_BIT(Bit)) == USB_HC_BIT(Bit)))

#define USB_HC_HIGH_32BIT(Addr64)    \
          ((UINT32)(RShiftU64((UINTN)(Addr64), 32) & 0XFFFFFFFF))

typedef struct _USBHC_MEM_BLOCK USBHC_MEM_BLOCK;
struct _USBHC_MEM_BLOCK {
  UINT8                   *Bits;    // Bit array to record which unit is allocated
  UINTN                   BitsLen;
  UINT8                   *Buf;
  UINT8                   *BufHost;
  UINTN                   BufLen;   // Memory size in bytes
  VOID                    *Mapping;
  USBHC_MEM_BLOCK         *Next;
};

//
// USBHC_MEM_POOL is used to manage the memory used by USB
// host controller. EHCI requires the control memory and transfer
// data to be on the same 4G memory.
//
typedef struct _USBHC_MEM_POOL {
  EFI_PCI_IO_PROTOCOL     *PciIo;
  BOOLEAN                 Check4G;
  UINT32                  Which4G;
  USBHC_MEM_BLOCK         *Head;
} USBHC_MEM_POOL;

//
// Memory allocation unit, must be 2^n, n>4
//
#define USBHC_MEM_UNIT           64

#define USBHC_MEM_UNIT_MASK      (USBHC_MEM_UNIT - 1)

// CK-DF Decrease the default pages since IMEM is limited resources
#define USBHC_MEM_DEFAULT_PAGES  16

#define USBHC_MEM_ROUND(Len)  (((Len) + USBHC_MEM_UNIT_MASK) & (~USBHC_MEM_UNIT_MASK))

//
// Advance the byte and bit to the next bit, adjust byte accordingly.
//
#define NEXT_BIT(Byte, Bit)   \
          do {                \
            (Bit)++;          \
            if ((Bit) > 7) {  \
              (Byte)++;       \
              (Bit) = 0;      \
            }                 \
          } while (0)



/**
  Initialize the memory management pool for the host controller.

  @param  PciIo               The PciIo that can be used to access the host controller.
  @param  Check4G             Whether the host controller requires allocated memory
                              from one 4G address space.
  @param  Which4G             The 4G memory area each memory allocated should be from.

  @retval EFI_SUCCESS         The memory pool is initialized.
  @retval EFI_OUT_OF_RESOURCE Fail to init the memory pool.

**/
USBHC_MEM_POOL *
UsbHcInitMemPool (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN BOOLEAN              Check4G,
  IN UINT32               Which4G
  );


/**
  Release the memory management pool.

  @param   Pool               The USB memory pool to free.

  @retval EFI_SUCCESS       The memory pool is freed.
  @retval EFI_DEVICE_ERROR  Failed to free the memory pool.

**/
EFI_STATUS
UsbHcFreeMemPool (
  IN USBHC_MEM_POOL       *Pool
  );


/**
  Allocate some memory from the host controller's memory pool
  which can be used to communicate with host controller.

  @param  Pool  The host controller's memory pool.
  @param  Size  Size of the memory to allocate.

  @return The allocated memory or NULL.

**/
VOID *
UsbHcAllocateMem (
  IN  USBHC_MEM_POOL      *Pool,
  IN  UINTN               Size
  );


/**
  Free the allocated memory back to the memory pool.

  @param  Pool  The memory pool of the host controller.
  @param  Mem   The memory to free.
  @param  Size  The size of the memory to free.

**/
VOID
UsbHcFreeMem (
  IN USBHC_MEM_POOL       *Pool,
  IN VOID                 *Mem,
  IN UINTN                Size
  );

/**
  Calculate the corresponding pci bus address according to the Mem parameter.

  @param  Pool           The memory pool of the host controller.
  @param  Mem            The pointer to host memory.
  @param  Size           The size of the memory region.

  @return the pci memory address
**/
EFI_PHYSICAL_ADDRESS
UsbHcGetPciAddressForHostMem (
  IN USBHC_MEM_POOL       *Pool,
  IN VOID                 *Mem,
  IN UINTN                Size
  );

#endif
