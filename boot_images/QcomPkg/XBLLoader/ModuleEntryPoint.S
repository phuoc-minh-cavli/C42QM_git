//------------------------------------------------------------------------------ 
//
//  Copyright (c) 2014, Qualcomm Technologies Inc. All rights reserved.
//  Portions Copyright (c) 2011-2013, ARM Limited. All rights reserved.
//
//  This program and the accompanying materials
//  are licensed and made available under the terms and conditions of the BSD License
//  which accompanies this distribution.  The full text of the license may be found at
//  http://opensource.org/licenses/bsd-license.php
//
//  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//------------------------------------------------------------------------------

//=============================================================================
//                              EDIT HISTORY
//
//
// when       who     what, where, why
// --------   ---     ---------------------------------------------------------
// 05/30/14   kedara   Initial revision
//
//============================================================================


//


.text
.align 3

GCC_ASM_IMPORT (sbl1_entry)
GCC_ASM_EXPORT (_ModuleEntryPoint)

  
_ModuleEntryPoint:
  b sbl1_entry

 
dead:  
  b dead                      /* We should never get here */

