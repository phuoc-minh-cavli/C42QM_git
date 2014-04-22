#ifndef HAL_QGIC_TARGET_SMPSS_H
#define HAL_QGIC_TARGET_SMPSS_H
/*
===========================================================================

FILE:         HALqgicTargetSMPSS.h

DESCRIPTION:  
  This is the Scorpion processor HWIO definitions for the hardware
  abstraction layer interface for the Qualcomm Generic Interrupt Controller
  block.

===========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/qgic/src/target/HALqgicTargetSMPSS.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/21/09   gfr     Created.

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
#define GICD_CTLR              SMPSS_GICD_CTLR
#define GICD_TYPER             SMPSS_GICD_TYPER
#define GICD_IIDR              SMPSS_GICD_IIDR
#define GICD_ANSACR            SMPSS_GICD_ANSACR
#define GICD_CGCR              SMPSS_GICD_CGCR
#define GICD_PIDRn             SMPSS_GICD_PIDRn
#define GICD_CIDRn             SMPSS_GICD_CIDRn
#define GICD_SGIR              SMPSS_GICD_SGIR
#define GICD_ISRn              SMPSS_GICD_ISRn
#define GICD_ISENABLERn        SMPSS_GICD_ISENABLERn
#define GICD_ICENABLERn        SMPSS_GICD_ICENABLERn
#define GICD_ISPENDRn          SMPSS_GICD_ISPENDRn
#define GICD_ICPENDRn          SMPSS_GICD_ICPENDRn
#define GICD_IACTIVERn         SMPSS_GICD_IACTIVERn
#define GICD_IPRIORITYRn       SMPSS_GICD_IPRIORITYRn
#define GICD_ITARGETSRn        SMPSS_GICD_ITARGETSRn
#define GICD_ICFGRn            SMPSS_GICD_ICFGRn
#define GICC_CTLR              SMPSS_GICC_CTLR
#define GICC_IIDR              SMPSS_GICC_IIDR
#define GICC_PMR               SMPSS_GICC_PMR
#define GICC_BPR               SMPSS_GICC_BPR
#define GICC_ABPR              SMPSS_GICC_ABPR
#define GICC_IAR               SMPSS_GICC_IAR
#define GICC_EOIR              SMPSS_GICC_EOIR
#define GICC_RPR               SMPSS_GICC_RPR
#define GICC_HPPIR             SMPSS_GICC_HPPIR


#endif /* HAL_QGIC_TARGET_SMPSS_H */

