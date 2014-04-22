#ifndef HAL_QGIC_TARGET_RPM_H
#define HAL_QGIC_TARGET_RPM_H
/*
===========================================================================

FILE:         HALqgicTargetRPM.h

DESCRIPTION:  
  This is the RPM processor HWIO definitions for the hardware abstraction
  layer interface for the Qualcomm Generic Interrupt Controller block.

===========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/qgic/src/target/HALqgicTargetRPM.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/08/09   gfr     Renamed GICD_IDR to GICD_IIDR.
01/08/09   gfr     Created.

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
#define GICD_CTLR              RPM_GICD_CTLR
#define GICD_TYPER             RPM_GICD_TYPER
#define GICD_IIDR              RPM_GICD_IIDR
#define GICD_ANSACR            RPM_GICD_ANSACR
#define GICD_CGCR              RPM_GICD_CGCR
#define GICD_PIDRn             RPM_GICD_PIDRn
#define GICD_CIDRn             RPM_GICD_CIDRn
#define GICD_SGIR              RPM_GICD_SGIR
#define GICD_ISRn              RPM_GICD_ISRn
#define GICD_ISENABLERn        RPM_GICD_ISENABLERn
#define GICD_ICENABLERn        RPM_GICD_ICENABLERn
#define GICD_ISPENDRn          RPM_GICD_ISPENDRn
#define GICD_ICPENDRn          RPM_GICD_ICPENDRn
#define GICD_IACTIVERn         RPM_GICD_IACTIVERn
#define GICD_IPRIORITYRn       RPM_GICD_IPRIORITYRn
#define GICD_ITARGETSRn        RPM_GICD_ITARGETSRn
#define GICD_ICFGRn            RPM_GICD_ICFGRn
#define GICC_CTLR              RPM_GICC_CTLR
#define GICC_IIDR              RPM_GICC_IIDR
#define GICC_PMR               RPM_GICC_PMR
#define GICC_BPR               RPM_GICC_BPR
#define GICC_ABPR              RPM_GICC_ABPR
#define GICC_IAR               RPM_GICC_IAR
#define GICC_EOIR              RPM_GICC_EOIR
#define GICC_RPR               RPM_GICC_RPR
#define GICC_HPPIR             RPM_GICC_HPPIR


#endif /* HAL_QGIC_TARGET_RPM_H */

