/*
===========================================================================

FILE:         HALqgic.c

DESCRIPTION:  
  This is the hardware abstraction layer implementation for the Qualcomm
  Generic Interrupt Controller. This file is primarily used for saving and restoring the QGIc state.

===========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/qgic/src/HALqgicState.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/23/11   an     Created.

===========================================================================
           Copyright (c) 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */


#include <HALhwio.h>
#include <HALqgic.h>
#include "HALqgicRegisters.h"


/* ---------------------------------------------------------------------
**                          DATA DECLARATION
** ----------------------------------------------------------------------- */

#define MAX_QGIC_DETECT_REGISTERS 9
#define MAX_QGIC_ENABLE_REGISTERS 5
#define MAX_QGIC_PRIORITY_REGISTERS 35
#define MAX_QGIC_SECURITY_REGISTERS 5
#define MAX_QGIC_TARGET_REGISTERS 35

/* ---------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */
/*
 * HAL_qgic_StateType
 *
 *  Structure containing basic qgic restoring data.
 *
 *  pDetect: Pointer to the trigger detection type.
 *  pPriority : Pointer to the interrupt priority data.
 *  pEnable: Pointer to the interrupts that  are enabled.
 *  pTarget: Pointer to the the target id for all interrupts.
 *  pSecurity: Pointer to the interrupt security property.
*/
typedef struct
{
  uint32 aDetect[MAX_QGIC_DETECT_REGISTERS];
  uint32 aPriority[MAX_QGIC_PRIORITY_REGISTERS];
  uint32 aEnable[MAX_QGIC_ENABLE_REGISTERS];
  uint32 aTarget[MAX_QGIC_TARGET_REGISTERS];
  uint32 aSecurity[MAX_QGIC_SECURITY_REGISTERS];
  uint32 qgic_distributor_cfg;
  uint32 qgic_cpu_cfg;
} HAL_qgic_StateDataType;


HAL_qgic_StateDataType stHalQgicState;

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**  HAL_qgic_Save
**
** ======================================================================== */

void HAL_qgic_Save (void)
{
  uint8 nIdx;

  /*
   * Store the interrupt Trigger information.
   */
  for(nIdx=0;nIdx < MAX_QGIC_DETECT_REGISTERS;nIdx++)
  {
    stHalQgicState.aDetect[nIdx] = inpdw(HWIO_ADDRI(GICD_ICFGRn, nIdx));
  }
   
  /*
   * Store the priority information.
   */
  for (nIdx = 0; nIdx < MAX_QGIC_PRIORITY_REGISTERS; nIdx++)
  {
    stHalQgicState.aPriority[nIdx] = inpdw(HWIO_ADDRI(GICD_IPRIORITYRn, nIdx));
  }

  /*
   * Store the interrupts that are enabled.
   */
  for (nIdx = 0; nIdx < MAX_QGIC_ENABLE_REGISTERS; nIdx++)
  {
    stHalQgicState.aEnable[nIdx] = inpdw(HWIO_ADDRI(GICD_ISENABLERn, nIdx));
  }

  /*
   * Store the target value for all interrupts.
   */
  for (nIdx = 0; nIdx < MAX_QGIC_TARGET_REGISTERS; nIdx++)
  {
    stHalQgicState.aTarget[nIdx] = inpdw(HWIO_ADDRI(GICD_ITARGETSRn, nIdx));
  }
  /*
   * Store the interrupt security values.
   */
  for (nIdx = 0; nIdx < MAX_QGIC_SECURITY_REGISTERS; nIdx++)
  {
    stHalQgicState.aSecurity[nIdx] = inpdw(HWIO_ADDRI(GICD_ISRn, nIdx));
  }

  /*
   * Store distributor and CPU interface cfg.
   */
  stHalQgicState.qgic_cpu_cfg = inpdw(HWIO_ADDR(GICC_CTLR));
  stHalQgicState.qgic_distributor_cfg = inpdw(HWIO_ADDR(GICD_CTLR));

} /* END HAL_qgic_Save */


/* ===========================================================================
**  HAL_qgic_Restore
**
** ======================================================================== */

void HAL_qgic_Restore (void)
{
  uint8 nIdx;
  uint32 nAddr;

  /*
   * Write back the interrupt Trigger information.
   */
  for(nIdx=0;nIdx < MAX_QGIC_DETECT_REGISTERS;nIdx++)
  {
    outpdw(HWIO_ADDRI(GICD_ICFGRn, nIdx),stHalQgicState.aDetect[nIdx]);
  }
   
  /*
   * Write back the priority information.
   */
  for (nIdx = 0; nIdx < MAX_QGIC_PRIORITY_REGISTERS; nIdx++)
  {
    outpdw(HWIO_ADDRI(GICD_IPRIORITYRn, nIdx),stHalQgicState.aPriority[nIdx]);
  }

  /*
   * Write back the interrupts that are enabled.
   */
  for (nIdx = 0; nIdx < MAX_QGIC_ENABLE_REGISTERS; nIdx++)
  {
    outpdw(HWIO_ADDRI(GICD_ISENABLERn, nIdx),stHalQgicState.aEnable[nIdx]);
  }

  /*
   * Write back the target value for all interrupts.
   */
  for (nIdx = 0; nIdx < MAX_QGIC_TARGET_REGISTERS; nIdx++)
  {
    outpdw(HWIO_ADDRI(GICD_ITARGETSRn, nIdx),stHalQgicState.aTarget[nIdx]);
  }
 
  /*
   * Write back the interrupt security values.
   */
  for (nIdx = 0; nIdx < MAX_QGIC_SECURITY_REGISTERS; nIdx++)
  {
    outpdw(HWIO_ADDRI(GICD_ISRn, nIdx),stHalQgicState.aSecurity[nIdx]);
  }

  /*
   * Set distributor and CPU interface cfg.
   */
  outpdw(HWIO_ADDR(GICC_CTLR),stHalQgicState.qgic_cpu_cfg);
  outpdw(HWIO_ADDR(GICD_CTLR),stHalQgicState.qgic_distributor_cfg);

  /*
   * Set the mask in hardware.
   */
  HWIO_OUTM(GICC_PMR, 0xFF, (HAL_QGIC_PRIORITY_LOWEST + 1));
} /* END HAL_qgic_Restore */






