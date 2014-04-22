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

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/qgic/src/target/HALqgicTargetSCSS.h#1 $

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
#define GICD_CTLR              SCSS_GICD_CTLR
#define GICD_TYPER             SCSS_GICD_TYPER
#define GICD_IIDR              SCSS_GICD_IIDR
#define GICD_ANSACR            SCSS_GICD_ANSACR
#define GICD_CGCR              SCSS_GICD_CGCR
#define GICD_PIDRn             SCSS_GICD_PIDRn
#define GICD_CIDRn             SCSS_GICD_CIDRn
#define GICD_SGIR              SCSS_GICD_SGIR
#define GICD_ISRn              SCSS_GICD_ISRn
#define GICD_ISENABLERn        SCSS_GICD_ISENABLERn
#define GICD_ICENABLERn        SCSS_GICD_ICENABLERn
#define GICD_ISPENDRn          SCSS_GICD_ISPENDRn
#define GICD_ICPENDRn          SCSS_GICD_ICPENDRn
#define GICD_IACTIVERn         SCSS_GICD_IACTIVERn
#define GICD_IPRIORITYRn       SCSS_GICD_IPRIORITYRn
#define GICD_ITARGETSRn        SCSS_GICD_ITARGETSRn
#define GICD_ICFGRn            SCSS_GICD_ICFGRn
#define GICC_CTLR              SCSS_GICC_CTLR
#define GICC_IIDR              SCSS_GICC_IIDR
#define GICC_PMR               SCSS_GICC_PMR
#define GICC_BPR               SCSS_GICC_BPR
#define GICC_ABPR              SCSS_GICC_ABPR
#define GICC_IAR               SCSS_GICC_IAR
#define GICC_EOIR              SCSS_GICC_EOIR
#define GICC_RPR               SCSS_GICC_RPR
#define GICC_HPPIR             SCSS_GICC_HPPIR


#endif /* HAL_QGIC_TARGET_SCSS_H */

