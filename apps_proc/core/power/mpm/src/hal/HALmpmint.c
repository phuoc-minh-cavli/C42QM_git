/*
===========================================================================

FILE:         HALmpmint.c

DESCRIPTION:  
  This is the generic hardware abstraction layer implementation for the
  MPM interrupt controller.

===========================================================================
             Copyright © 2008-2015 QUALCOMM Technologies Incorporated.
                        All Rights Reserved.
                QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <string.h>
#include <HALmpmint.h>
#include <HALcomdef.h>

#include "HALmpmintInternal.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * Identifier string for this HAL.
 */
#define HAL_MPMINT_VERSION_STRING "HAL_MPMINT_V2"

#define MAKE_MASK(bit_pos) (1 << ((bit_pos) % 32))

#define BIT_SET32(val, bit_pos) (val |  MAKE_MASK(bit_pos))
#define BIT_CLR32(val, bit_pos) (val & ~MAKE_MASK(bit_pos))


/* -------------------------------------------------------------------------
**                         VARIABLE DECLARATIONS
** ------------------------------------------------------------------------- */

/**
 * Register layout object for the master. This will store information 
 * regarding interrupt/gpio configuration (detection, polarity, status) and
 * will be used by RPM to write to actual MPM driver.
 */
HAL_mpmint_PlatformType registerLayout;

/* -----------------------------------------------------------------------
**                         INTERNAL FUNCTIONS
** ----------------------------------------------------------------------- */

/** 
 * Configures the detection and polarity of a given interrupt from the input
 * trigger type. 
 *
 * @note It must be noted that it does not notify RPM about these
 *       changes so it is up to the caller to perform this task.
 *
 * @param nMPM: Interrupt id (at MPM HW) for which we want to change this
 *              configuration.
 * @param eTrigger: New trigger settings for detection and polarity
 *                  Possible Inputs:
 *
 * @Note
 * From MPM HDD, three bits are used as follows for interrupt detection and 
 * polarity. 
 *   
 * 0 00    Level Sensitive Active Low
 * 0 01    Falling Edge Sensitive
 * 0 10    Rising Edge Sensitive
 * 0 11    Dual Edge Sensitive
 * 1 00    Level Sensitive Active High
 * 1 01    Rising Edge Sensitive
 * 1 10    Falling Edge Sensitive
 * 1 11    Dual Edge Sensitive
 * 
 */
static void HAL_mpmint_ConfigDetectionAndPolarity
(
  uint32 nMPM,
  HAL_mpmint_TriggerType eTrigger
)
{
  uint32 nReg;
  uint32 nVal;

  /*
   * Configure detection1 bit
   */
  nReg    = registerLayout.nDetectReg + (sizeof(uint32) * (nMPM / 32));
  nVal    = inpdw(nReg);
  nVal    = HAL_TT_EXTRACT_DET1_BIT(eTrigger)? BIT_SET32(nVal,nMPM): BIT_CLR32(nVal,nMPM);
  outpdw(nReg, nVal);

  /*
   * Configure detection3 bit
   */
  nReg    = registerLayout.nDetect3Reg + (sizeof(uint32) * (nMPM / 32));
  nVal    = inpdw(nReg);
  nVal    = HAL_TT_EXTRACT_DET3_BIT(eTrigger)? BIT_SET32(nVal,nMPM): BIT_CLR32(nVal,nMPM);
  outpdw(nReg, nVal);

  /*
   * Configure the polarity
   */
  nReg = registerLayout.nPolarityReg + (sizeof(uint32) * (nMPM / 32));
  nVal = inpdw(nReg);
  nVal = HAL_TT_EXTRACT_POL_BIT(eTrigger)? BIT_SET32(nVal,nMPM): BIT_CLR32(nVal,nMPM);
  outpdw(nReg, nVal);
}


/**
 * Sets the status of a given input interrupt. 
 *
 * @note It simply writes in shared memory but does not notirfy RPM. It's 
 *       left up to caller.
 *
 *       Also, it simply sets status bit for a given interrupt so if some
 *       pre/post processing is needed, it has to be done by call (e.g.
 *       clearing the interrupt before enabling it).
 *
 * @param nMPM:    Interrupt id (at MPM HW) for which we want to change this
 *                 configuration.
 * @param eStatus: Status to which above interrupt will be set to.
 *                 Possible Inputs:
 *                 - HAL_MPMINT_STATUS_ENABLE
 *                 - HAL_MPMINT_STATUS_DISABLE
 */
static void HAL_mpmint_SetStatus
(
  uint32 nMPM,
  HAL_mpmint_StatusType eStatus
)
{
  uint32 nReg;
  uint32 nVal;

  nReg  = registerLayout.nEnableReg + (sizeof(uint32) * (nMPM / 32));
  nVal  = inpdw(nReg);
  nVal  = (HAL_MPMINT_STATUS_ENABLE == eStatus)? BIT_SET32(nVal,nMPM): BIT_CLR32(nVal,nMPM);
  outpdw(nReg, nVal);  
}

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* ===========================================================================
**  HAL_mpmint_Init
**
** ======================================================================== */

void HAL_mpmint_Init
(
  char **ppszVersion,
  uint8 *master_msg_ram_base_ptr,
  bool32 init_mpm_regs
)
{
  uint32 nMPM;

  // HAL_mpmint_hwio_mmap();

  /* Preparing mpm driver for sending IPC interrupt */
  HAL_mpmint_SetupIPC();

  /* Initializing the registerLayout of shared memory */
  HAL_mpmint_InitializeRegisterLayout(master_msg_ram_base_ptr);

  if( TRUE == init_mpm_regs )
  {
    /*
     * Populate MPM interrupt controller with initial guesses from BSP data.
     * Also, initialize the back-lookup table while we're at it.
     */
    for (nMPM = 0; nMPM < HAL_MPMINT_NUM; nMPM++)
    {
      HAL_mpmint_SetTrigger( nMPM, aInterruptTable[nMPM].eTrigger );
    }
  }

  /*
   * Disable and clear all interrupts
   */
  HAL_mpmint_All_Disable();
  HAL_mpmint_All_Clear();

  /*
   * Fill in return value
   */
  if (ppszVersion != NULL)
  {
    *ppszVersion = (char*)HAL_MPMINT_VERSION_STRING;
  }

} /* END HAL_mpmint_Init */


/* ===========================================================================
**  HAL_mpmint_Reset
**
** ======================================================================== */

void HAL_mpmint_Reset (void)
{
  HAL_mpmint_All_Disable();
  HAL_mpmint_All_Clear();

} /* END HAL_mpmint_Reset */


/* ===========================================================================
**  HAL_mpmint_Save
**
** ======================================================================== */

void HAL_mpmint_Save (void)
{
  /* MPM interrupt controller is always on. */
} /* END HAL_mpmint_Save */


/* ===========================================================================
**  HAL_mpmint_Restore
**
** ======================================================================== */

void HAL_mpmint_Restore (void)
{
  /* MPM interrupt controller is always on. */
} /* END HAL_mpmint_Restore */


/* ===========================================================================
**  HAL_mpmint_Enable
**
** ======================================================================== */

void HAL_mpmint_Enable
(
  uint32 nMPM
)
{
  /* 
   * Enable the interrupt in the MPM register 
   */
  HAL_mpmint_SetStatus( nMPM, HAL_MPMINT_STATUS_ENABLE );

  vmpm_send_interrupt();

} /* END HAL_mpmint_Enable */


/* ===========================================================================
**  HAL_mpmint_Disable
**
** ======================================================================== */

void HAL_mpmint_Disable
(
  uint32 nMPM
)
{
  /*
   * Disable the interrupt in MPM
   */
  HAL_mpmint_SetStatus( nMPM, HAL_MPMINT_STATUS_DISABLE );

  vmpm_send_interrupt();

} /* END HAL_mpmint_Disable */


/* ===========================================================================
**  HAL_mpmint_Clear
**
** ======================================================================== */

void HAL_mpmint_Clear
(
  uint32 nMPM
)
{
  uint32 nReg;
  uint32 nVal;

  nReg  = registerLayout.nStatusReg + (sizeof(uint32) * (nMPM / 32));
  nVal  = inpdw(nReg);
  nVal  = BIT_CLR32(nVal,nMPM);
  outpdw(nReg, nVal); 
  
  /* No need to notify the RPM, since it has nothing to do in response to
   * a clear. */
   
} /* END HAL_mpmint_Clear */


/* ===========================================================================
**  HAL_mpmint_All_Enable
**
** ======================================================================== */

void HAL_mpmint_All_Enable( void )
{
  uint32 nNumRegs, nReg, nMask, regIndex;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  /* We could have used memset as used earlier but somehow it results into
   * symbol loading problem. */
  for( regIndex = 0; regIndex < nNumRegs; regIndex++ )
  {
    nReg      = registerLayout.nEnableReg + (sizeof(uint32) * regIndex);
    nMask     = 0xFFFFFFFF;

    outpdw(nReg, nMask);               /* Enable the interrupts */
  }

  vmpm_send_interrupt();

} /* END HAL_mpmint_All_Enable */


/* ===========================================================================
**  HAL_mpmint_All_Disable
**
** ======================================================================== */

void HAL_mpmint_All_Disable( void )
{
  uint32 nNumRegs, nReg, nMask, regIndex;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  /* We could have used memset as used earlier but somehow it results into
   * symbol loading problem. */
  for( regIndex = 0; regIndex < nNumRegs; regIndex++ )
  {
    nReg  = registerLayout.nEnableReg + (sizeof(uint32) * regIndex);
    nMask = 0x0;
    outpdw(nReg, nMask);
  }

  vmpm_send_interrupt();

} /* END HAL_mpmint_Disable */


/* ===========================================================================
**  HAL_mpmint_All_Clear
**
** ======================================================================== */

void HAL_mpmint_All_Clear( void )
{
  uint32 nNumRegs, nReg, nMask, regIndex;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  for( regIndex = 0; regIndex < nNumRegs; regIndex++ )
  {
    nReg  = registerLayout.nStatusReg + (sizeof(uint32) * regIndex);
    nMask = 0x0;
    outpdw(nReg, nMask);
  }

  /* No need to notify the RPM, since it has nothing to do in response to
   * a clear. */

} /* END HAL_mpmint_All_Clear */


/* ===========================================================================
**  HAL_mpmint_GetNumberMasks
**
** ======================================================================== */

void HAL_mpmint_GetNumberMasks
(
  uint32 *pnNumber
)
{

  *pnNumber = (HAL_MPMINT_PHYS_INTS / 32);

} /* END HAL_mpmint_GetNumberMasks */


/* ===========================================================================
**  HAL_mpmint_Mask_Enable
**
** ======================================================================== */

void HAL_mpmint_Mask_Enable
(
  uint32 nMaskIndex,
  uint32 nMask
)
{
  uint32 nReg, nVal, nNumRegs;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  if(nMaskIndex >= nNumRegs)
    return;

  /* 
   * Enable the interrupts in the MPM register 
   */
  nReg = registerLayout.nEnableReg + (sizeof(uint32) * nMaskIndex);
  nVal = inpdw(nReg);
  nVal |= nMask;
  outpdw(nReg, nVal);

  vmpm_send_interrupt();

} /* END HAL_mpmint_Mask_Enable */


/* ===========================================================================
**  HAL_mpmint_Mask_Disable
**
** ======================================================================== */

void HAL_mpmint_Mask_Disable
(
  uint32 nMaskIndex,
  uint32 nMask
)
{
  uint32 nReg, nVal, nNumRegs;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  if(nMaskIndex >= nNumRegs)
    return;

  /* 
   * Disable the interrupts in the MPM register 
   */
  nReg = registerLayout.nEnableReg + (sizeof(uint32) * nMaskIndex);
  nVal = inpdw(nReg);
  nVal &= ~nMask;
  outpdw(nReg, nVal);

  vmpm_send_interrupt();

} /* END HAL_mpmint_Mask_Disable */


/* ===========================================================================
**  HAL_mpmint_Mask_Clear
**
** ======================================================================== */

void HAL_mpmint_Mask_Clear
(
  uint32 nMaskIndex,
  uint32 nMask
)
{
  uint32 nReg, nVal, nNumRegs;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  if(nMaskIndex >= nNumRegs)
    return;

  /* 
   * Clear the interrupts in the MPM register 
   */
  nReg = registerLayout.nStatusReg + (sizeof(uint32) * nMaskIndex);
  nVal = inpdw(nReg);
  nVal &= ~nMask;
  outpdw(nReg, nVal);

  /* No need to notify the RPM, since it has nothing to do in response to
   * a clear. */

} /* END HAL_mpmint_Mask_Clear */


/* ===========================================================================
**  HAL_mpmint_GetPending
**
** ======================================================================== */

void HAL_mpmint_GetPending
(
  uint32 nMaskIndex,
  uint32 *pnMask
)
{
  uint32 nNumRegs, nReg, nEnable;

  HAL_mpmint_GetNumberMasks(&nNumRegs);

  if((nMaskIndex >= nNumRegs) || (!pnMask))
    return;

  nReg = registerLayout.nStatusReg + (sizeof(uint32) * nMaskIndex);
  nEnable = registerLayout.nEnableReg + (sizeof(uint32) * nMaskIndex);
  *pnMask = ( inpdw(nReg) & inpdw(nEnable) );

} /* HAL_mpmint_GetPending */


/* ===========================================================================
**  HAL_mpmint_GetTrigger
**
** ======================================================================== */

void HAL_mpmint_GetTrigger
(
  uint32                  nMPM,
  HAL_mpmint_TriggerType  *peTrigger
)
{
  uint32 nBitPos, nMask, nReg;
  uint32 nDetect1Val, nDetect3Val, nPolarityVal;

  nBitPos = nMPM;
  nMask   = MAKE_MASK(nBitPos);

  nReg    = registerLayout.nDetectReg + (sizeof(uint32) * (nBitPos / 32));
  nDetect1Val = inpdw(nReg) & nMask;

  nReg    = registerLayout.nDetect3Reg + (sizeof(uint32) * (nBitPos / 32));
  nDetect3Val = inpdw(nReg) & nMask;

  nReg = registerLayout.nPolarityReg + (sizeof(uint32) * (nBitPos / 32));
  nPolarityVal = inpdw(nReg) & nMask;

  /*
   * HAL_mpmint_TriggerType is enum'ed to out of POLARITY|DETECT3|DETECT1
   */
  *peTrigger = ( HAL_mpmint_TriggerType )( nPolarityVal << 0x2 | 
                                           nDetect3Val << 0x1 | 
                                           nDetect1Val << 0 );

  /*
   * Update the shadow in case it was incorrect
   */
  aInterruptTable[nMPM].eTrigger = *peTrigger;

} /* HAL_mpmint_GetTrigger */


/* ===========================================================================
**  HAL_mpmint_SetTrigger
**
** ======================================================================== */

void HAL_mpmint_SetTrigger
(
  uint32                   nMPM,
  HAL_mpmint_TriggerType  eTrigger
)
{
  /*
   * Configuring to input detection and polarity.
   */
  HAL_mpmint_ConfigDetectionAndPolarity(nMPM, eTrigger);

  /*
   * Commit the change.
   */
  vmpm_send_interrupt();

  /*
   * Save the new value
   */
  aInterruptTable[nMPM].eTrigger = eTrigger;

} /* END HAL_mpmint_SetTrigger */


/* ===========================================================================
**  HAL_mpmint_GetNumber
**
** ======================================================================== */

void HAL_mpmint_GetNumber
(
  uint32 *pnNumber
)
{

  /* MPM interrupt controller supports HAL_MPMINT_NUM interrupts */
  *pnNumber = HAL_MPMINT_NUM;

} /* END HAL_mpmint_GetNumber */


/* ===========================================================================
**  HAL_mpmint_GetNumberPhysical
**
** ======================================================================== */
void HAL_mpmint_GetNumberPhysical
(
  uint32 *pnNumber
)
{
  /* the correct number is defined by the platform */
  *pnNumber = HAL_MPMINT_PHYS_INTS;
} /* HAL_mpmint_GetNumberPhysical */


/* ===========================================================================
**  HAL_mpmint_GetPhysNumber
**
** ======================================================================== */
bool32 HAL_mpmint_GetPhysNumber
(
  uint32 nMPM,
  uint32 *pnPhys
)
{

  if(HAL_mpmint_IsSupported(nMPM))
  {
    *pnPhys = nMPM;
    return TRUE;
  }

  return FALSE;
} /* HAL_mpmint_GetPhysNumber */


/* ===========================================================================
**  HAL_mpmint_GetEnumNumber
**
** ======================================================================== */
bool32 HAL_mpmint_GetEnumNumber
(
  uint32 nPhys,
  uint32 *pnMPM
)
{
  /* current HAL uses a 1:1 mapping between phys and enum */
  uint32 nMPM = nPhys;

  if(HAL_mpmint_IsSupported(nMPM))
  {
    *pnMPM = nMPM;
    return TRUE;
  }

  return FALSE;
} /* HAL_mpmint_GetEnumNumber */


/* ===========================================================================
**  HAL_mpmint_IsSupported
**
** ======================================================================== */

bool32 HAL_mpmint_IsSupported
(
  uint32 nMPM
)
{
  return ( nMPM < HAL_MPMINT_NUM );
} /* END HAL_mpmint_IsSupported */


/* ===========================================================================
**  HAL_mpmint_IsGpioSupported
**
** ======================================================================== */

bool32 HAL_mpmint_IsGpioSupported
(
  uint32 nGPIO,
  uint32 *pnMPM
)
{
  uint32 nMPM;

  /*
   * Check to see if this GPIO is supported as a wakeup, and if so, 
   * return the MPM interrupt that it corresponds to.
   */ 
  for ( nMPM = 0; nMPM < HAL_MPMINT_NUM; nMPM++ )
  {
    if ( aInterruptTable[nMPM].gpio == nGPIO )
    {
      break;
    }
  }

  if ( nMPM < HAL_MPMINT_NUM )
  {
    *pnMPM = nMPM;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/* ===========================================================================
**  HAL_mpmint_IsGpio
**
** ======================================================================== */

bool32 HAL_mpmint_IsGpio
(
  uint32 nMPM,
  uint32 *pnGPIO
)
{
  bool32 retval = FALSE;

  if ( aInterruptTable[nMPM].gpio != HAL_MPMINT_INVALID_GPIO )
  {
    *pnGPIO = aInterruptTable[nMPM].gpio;
    retval = TRUE;
  }

  return retval;
} /* HAL_mpmint_IsGpio */


/* ===========================================================================
**  HAL_mpmint_IsPending
**
** ======================================================================== */

bool32 HAL_mpmint_IsPending
(
  uint32 nMPM
)
{
  uint32 nBitPos, nMask, nStatusReg, nEnableReg, nRegOffset;
  bool32 bPending = FALSE;

  nBitPos    = nMPM;
  nMask      = MAKE_MASK(nBitPos);
  nRegOffset = sizeof(uint32) * (nBitPos / 32);
  nStatusReg = registerLayout.nStatusReg + nRegOffset;
  nEnableReg = registerLayout.nEnableReg + nRegOffset;

  if (inpdw(nStatusReg) & inpdw(nEnableReg) & nMask)
  {
    bPending = TRUE;
  }

  /*
   * Return if we are set
   */
  return bPending;

} /* END HAL_mpmint_IsPending */


/* ===========================================================================
**  HAL_mpmint_IsEnabled
**
** ======================================================================== */

bool32 HAL_mpmint_IsEnabled
(
  uint32 nMPM
)
{
  uint32 nBitPos, nMask, nReg;
  bool32 bEnabled = FALSE;

  nBitPos = nMPM;
  nMask   = MAKE_MASK(nBitPos);
  nReg    = registerLayout.nEnableReg + (sizeof(uint32) * (nBitPos / 32));

  if (inpdw(nReg) & nMask)
  {
    bEnabled = TRUE;
  }

  /*
   * Return if we are set
   */
  return bEnabled;

} /* END HAL_mpmint_IsEnabled */


/* ===========================================================================
**  HAL_mpmint_IsClearBefore
**
** ======================================================================== */

bool32 HAL_mpmint_IsClearBefore
(
  uint32 nMPM
)
{   
  /*
   * Return TRUE if this interrupt clear before.  If not, then it is
   * assumed to be clear after.
   */
  if ( aInterruptTable[nMPM].eTrigger == HAL_MPMINT_TRIGGER_LOW ||
       aInterruptTable[nMPM].eTrigger == HAL_MPMINT_TRIGGER_HIGH )
  {
    return FALSE;
  }
  else
  {
    /* All edge detect interrupts are also CLEAR BEFORE */
    return TRUE;
  }
}


/* ========================================================================
**  HAL_mpmint_setup_interrupts
**
** ========================================================================*/
void HAL_mpmint_setup_interrupts  
( 
  HAL_mpmint_ConfigInfoType *intrs,
  uint32 intrs_count 
)
{
  uint32 iter, nMPM;
  HAL_mpmint_TriggerType eTrigger;
  HAL_mpmint_StatusType  eStatus;

  for( iter = 0; iter < intrs_count; iter++ )
  {
    nMPM     = intrs[iter].mpm_hw_int_id;
    eTrigger = (HAL_mpmint_TriggerType)intrs[iter].trigger;
    eStatus  = (HAL_mpmint_StatusType)intrs[iter].status;

    /* Setting up the detection and polarity */
    HAL_mpmint_ConfigDetectionAndPolarity(nMPM, eTrigger);

    /* Save new value in the table */
    aInterruptTable[nMPM].eTrigger = eTrigger;

    /* Updating interrupt status */
    HAL_mpmint_SetStatus(nMPM, eStatus);

  }

  /* Notifying RPM about this update */
  vmpm_send_interrupt();

}

/* ========================================================================
**  HAL_mpmint_InitializeRegisterLayout
**
**  This function must be called with appropriate address passed to it 
**  during early initialization (i.e. before configuring any interrupt/gpio).
** ========================================================================*/
void HAL_mpmint_InitializeRegisterLayout(uint8 *msg_ram_base_ptr)
{
  uint32 nRegs, offset, vmpm_wakeup_addr, vmpm_config_base_addr;

  HAL_mpmint_GetNumberMasks(&nRegs);
  offset = nRegs * sizeof(uint32);

  vmpm_wakeup_addr = (uint32)msg_ram_base_ptr;      /* 64 bit Wakeup address */
  vmpm_config_base_addr = vmpm_wakeup_addr + 0x8;   /* Config base address */

  registerLayout.WakeupReg     = vmpm_wakeup_addr;
  registerLayout.nEnableReg    = vmpm_config_base_addr;
  registerLayout.nDetectReg    = vmpm_config_base_addr + offset * 1;
  registerLayout.nDetect3Reg   = vmpm_config_base_addr + offset * 2;
  registerLayout.nPolarityReg  = vmpm_config_base_addr + offset * 3;
  registerLayout.nStatusReg    = vmpm_config_base_addr + offset * 4;
  registerLayout.nEnableRegPad = 0;
  registerLayout.nClearReg     = 0;
}
