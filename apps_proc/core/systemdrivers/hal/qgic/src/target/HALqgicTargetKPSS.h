#ifndef HAL_QGIC_TARGET_SCSS_H
#define HAL_QGIC_TARGET_SCSS_H
/*
===========================================================================

FILE:         HALqgicTargetSCSS.h

DESCRIPTION:  
  This is the Scorpion processor HWIO definitions for the hardware
  abstraction layer interface for the Qualcomm Generic Interrupt Controller
  block.

===========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/qgic/src/target/HALqgicTargetKPSS.h#1 $

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
#define GICD_CTLR              APCS_GICD_CTLR
#define GICD_TYPER             APCS_GICD_TYPER
#define GICD_IIDR              APCS_GICD_IIDR
#define GICD_ANSACR            APCS_GICD_ANSACR
#define GICD_CGCR              APCS_GICD_CGCR
#define GICD_PIDRn             APCS_GICD_PIDRn
#define GICD_CIDRn             APCS_GICD_CIDRn
#define GICD_SGIR              APCS_GICD_SGIR
#define GICD_ISRn              APCS_GICD_ISRn
#define GICD_ISENABLERn        APCS_GICD_ISENABLERn
#define GICD_ICENABLERn        APCS_GICD_ICENABLERn
#define GICD_ISPENDRn          APCS_GICD_ISPENDRn
#define GICD_ICPENDRn          APCS_GICD_ICPENDRn
#define GICD_IPRIORITYRn       APCS_GICD_IPRIORITYRn
#define GICD_ITARGETSRn        APCS_GICD_ITARGETSRn
#define GICD_ICFGRn            APCS_GICD_ICFGRn
#define GICC_CTLR              APCS_GICC_CTLR
#define GICC_IIDR              APCS_GICC_IIDR
#define GICC_PMR               APCS_GICC_PMR
#define GICC_BPR               APCS_GICC_BPR
#define GICC_ABPR              APCS_GICC_ABPR
#define GICC_IAR               APCS_GICC_IAR
#define GICC_EOIR              APCS_GICC_EOIR
#define GICC_RPR               APCS_GICC_RPR
#define GICC_HPPIR             APCS_GICC_HPPIR

#ifdef HWIO_APCS_GICD_ISACTIVERn_ADDR
  #define GICD_IACTIVERn         APCS_GICD_ISACTIVERn
#else
  #define GICD_IACTIVERn         APCS_GICD_IACTIVERn
#endif

#ifdef HWIO_APCS_GICC_CTLR_ACKCTL_BMSK
  #define HWIO_APCS_GICC_CTLR_S_ACK_BMSK  HWIO_APCS_GICC_CTLR_ACKCTL_BMSK 
  #define HWIO_APCS_GICC_CTLR_S_ACK_SHFT  HWIO_APCS_GICC_CTLR_ACKCTL_SHFT
#endif

#endif /* HAL_QGIC_TARGET_KPSS_H */

