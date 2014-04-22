/**
 * @file ddr_sdi_wrapper.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/DDRTargetLib/ddr_sdi_wrapper.c#2 $
$DateTime: 2019/02/08 18:06:59 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/17/17   din     Updated DDR dload cookie
07/20/15   tw      updated sanity ddr test to check across interleave
02/20/15   tw      Initial version.
================================================================================
                   Copyright 2015 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
//#include "ddrss_init_sdi.h"
#include <stdbool.h>
#include "DALSys.h"
#include "ddr_hwio.h"
#include "ddr_drivers.h"

/*==============================================================================
                                  MACROS
==============================================================================*/

/*==============================================================================
                                  DATA
==============================================================================*/
//DDR_STRUCT *ddrsns_share_data_sdi = (DDR_STRUCT *)DDR_GLOBAL_STRUCT_DATARAM_ADDR;
//static boolean ddr_init_status = FALSE;
boolean Clock_ConfigureDDR( void );
bool icb_sdi_restore( void );

/* ============================================================================
**  Function : ddr_sdi_init
** ============================================================================
*/
/*!
*   @brief
*   This function is called to reinitialize ddr in SDI context. 
*
*   @details
*   This function is called to reinitialize ddr in SDI context. 
*   lite library for both clock and bus will be called first to
*   prepare for data access on the ddr, then sdi version of ddr driver
*   will be called to take ddr out from self refresh.
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/


void ddr_sdi_init(void)
{
  /* initialize clock for ddr */
  Clock_ConfigureDDR();				
  
  /* initialize bus driver */
  //icb_sdi_restore();	
  
  ddr_restore_from_wdog_reset(DDR_XO_SPEED_IN_KHZ);
             
}

void ddr_sdi_exit(void)
{
  /*DDR Enter Self refresh after sdi exit */
   ddr_restore_from_sdi_exit(DDR_XO_SPEED_IN_KHZ);
}

