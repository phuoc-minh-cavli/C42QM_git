#ifndef HAL_QGIC_TARGET_MSS_H
#define HAL_QGIC_TARGET_MSS_H
/*
===========================================================================

FILE:         HALqgicTargetMSS.h

DESCRIPTION:  
  This is the modem processor HWIO definitions for the hardware
  abstraction layer interface for the Qualcomm Generic Interrupt Controller
  block.

===========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/qgic/src/target/HALqgicTargetMSS.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/08/09   gfr     Renamed GICD_IDR to GICD_IIDR.
04/02/09   gfr     Created.

===========================================================================
             Copyright (c) 2009 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/


/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/*
 * Macros to map the generic register names to the processor specific
 * ones.
 */
#define GICD_CTLR              MSS_GICD_CTLR
#define GICD_TYPER             MSS_GICD_TYPER
#define GICD_IIDR              MSS_GICD_IIDR
#define GICD_ANSACR            MSS_GICD_ANSACR
#define GICD_CGCR              MSS_GICD_CGCR
#define GICD_PIDRn             MSS_GICD_PIDRn
#define GICD_CIDRn             MSS_GICD_CIDRn
#define GICD_SGIR              MSS_GICD_SGIR
#define GICD_ISRn              MSS_GICD_ISRn
#define GICD_ISENABLERn        MSS_GICD_ISENABLERn
#define GICD_ICENABLERn        MSS_GICD_ICENABLERn
#define GICD_ISPENDRn          MSS_GICD_ISPENDRn
#define GICD_ICPENDRn          MSS_GICD_ICPENDRn
#define GICD_IACTIVERn         MSS_GICD_IACTIVERn
#define GICD_IPRIORITYRn       MSS_GICD_IPRIORITYRn
#define GICD_ITARGETSRn        MSS_GICD_ITARGETSRn
#define GICD_ICFGRn            MSS_GICD_ICFGRn
#define GICC_CTLR              MSS_GICC_CTLR
#define GICC_IIDR              MSS_GICC_IIDR
#define GICC_PMR               MSS_GICC_PMR
#define GICC_BPR               MSS_GICC_BPR
#define GICC_ABPR              MSS_GICC_ABPR
#define GICC_IAR               MSS_GICC_IAR
#define GICC_EOIR              MSS_GICC_EOIR
#define GICC_RPR               MSS_GICC_RPR
#define GICC_HPPIR             MSS_GICC_HPPIR


#endif /* HAL_QGIC_TARGET_MSS_H */

