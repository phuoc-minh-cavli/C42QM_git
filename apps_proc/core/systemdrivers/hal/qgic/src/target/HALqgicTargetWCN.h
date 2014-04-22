#ifndef HAL_QGIC_TARGET_WCN_H
#define HAL_QGIC_TARGET_WCN_H
/*
===========================================================================

FILE:         HALqgicTargetWCN.h

DESCRIPTION:  
  This is the RIVA processor HWIO definitions for the hardware abstraction
  layer interface for the Qualcomm Generic Interrupt Controller block.

===========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/qgic/src/target/HALqgicTargetWCN.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/08/09   gfr     Renamed GICD_IDR to CCU_QGIC_DI_GICD_CTLR.
01/08/09   gfr     Created.

===========================================================================
             Copyright (c) 2010 Qualcomm Technologies Incorporated.
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
#define GICD_CTLR              CCU_QGIC_DI_GICD_CTLR
#define GICD_TYPER             CCU_QGIC_DI_GICD_TYPER
#define GICD_IIDR              CCU_QGIC_DI_GICD_IIDR
#define GICD_ANSACR            CCU_QGIC_DI_GICD_ANSACR
#define GICD_CGCR              CCU_QGIC_DI_GICD_CGCR
#define GICD_PIDRn             CCU_QGIC_DI_GICD_PIDRn
#define GICD_CIDRn             CCU_QGIC_DI_GICD_CIDRn
#define GICD_SGIR              CCU_QGIC_DI_GICD_SGIR
#define GICD_ISRn              CCU_QGIC_DI_GICD_ISRn
#define GICD_ISENABLERn        CCU_QGIC_DI_GICD_ISENABLERn
#define GICD_ICENABLERn        CCU_QGIC_DI_GICD_ICENABLERn
#define GICD_ISPENDRn          CCU_QGIC_DI_GICD_ISPENDRn
#define GICD_ICPENDRn          CCU_QGIC_DI_GICD_ICPENDRn
#define GICD_IACTIVERn         CCU_QGIC_DI_GICD_IACTIVERn
#define GICD_IPRIORITYRn       CCU_QGIC_DI_GICD_IPRIORITYRn
#define GICD_ITARGETSRn        CCU_QGIC_DI_GICD_ITARGETSRn
#define GICD_ICFGRn            CCU_QGIC_DI_GICD_ICFGRn
#define GICC_CTLR              CCU_QGIC_CI_GICC_CTLR
#define GICC_IIDR              CCU_QGIC_CI_GICC_IIDR
#define GICC_PMR               CCU_QGIC_CI_GICC_PMR
#define GICC_BPR               CCU_QGIC_CI_GICC_BPR
#define GICC_ABPR              CCU_QGIC_CI_GICC_ABPR
#define GICC_IAR               CCU_QGIC_CI_GICC_IAR
#define GICC_EOIR              CCU_QGIC_CI_GICC_EOIR
#define GICC_RPR               CCU_QGIC_CI_GICC_RPR
#define GICC_HPPIR             CCU_QGIC_CI_GICC_HPPIR

#endif /* HAL_QGIC_TARGET_WCN_H */

