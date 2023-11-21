/*!
  @file
  rfc_card_factory.c

  @brief
  This module represents RF configuration file for all supported technologies
  for MDM9x00

*/

/*==============================================================================

Copyright (c) 2010, 2011, 2012, 2013-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfc_jolokia.mpss/2.10/target/mdm9607/src/rfc_card_factory.cpp#1 $

when       who     what, where, why
--------------------------------------------------------------------------------
07/24/15   mmd    Port "Dynamaic memory reclaim" and "Force UE offline if RFC and HW config do not 
                   match " from 8909 target
06/12/14   dr      Jolokia Linker errors fix
04/23/14   sd      Initial Revision

==============================================================================*/
extern "C" {
#include "comdef.h"
#include "rfcommon_core_error.h"
#include "rfc_card.h"
#include "rfc_vreg_mgr_wtr1605_sv.h"
#include "rfc_configurable_test_card_cmn_data.h"
#include "rfc_configurable_test_vreg_mgr.h"
#include "rfcommon_mdsp.h"
#include "rfcommon_msg.h"
#include "mcfg_dynrec.h"
#include "string.h"
} /* extern "C" */
#include "rfc_class.h"

/*==============================================================================

                               DATA DEFINITIONS

==============================================================================*/
/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/
/*----------------------------------------------------------------------------*/
/* This funciton must be provided by the auto-gen card factory */
extern
boolean
rfc_factory
(
  rf_hw_type rf_hw,
  rfc_vreg_mgr** vreg_mgr_pp,
  rfc_common_data** rfc_data_pp
);
/*============================================================================*/
/*!
  @name RF Card Specific Initialization

  @brief
  This section contains all initialization functions which are specific to
  this RF Card.
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Card

  @details
  This function will set up the internal function pointers to control the
  specified RF HW Card.

  @param rf_hw
  RFC Hardware installed

  return
  Success or failure during set card operation.
*/
boolean rfc_set_card (rf_hw_type rf_hw)
{
  static boolean rfc_initialized = FALSE;
  rfc_intf *temp_p = NULL;
  rfc_vreg_mgr *vreg_mgr_p = NULL;
  rfc_common_data *rfc_data = NULL;
  system_clock_enum sysclk = SYS_CLK_XO;
  char  dynrec_card_name[12];
  char  temp_buf[12];
  char *dynrec_card_ptr = dynrec_card_name;

  int   card_reclaimed, n;

  /* Check if the Card is set already */
  if(rfc_initialized)
  {
    return rfc_initialized; /* return */
  }
  
  /* generate the memory id in the form of "rfc_hwid" */
  n = snprintf(temp_buf, sizeof(temp_buf), "rfc_");
  if (( n + 4 ) > sizeof(dynrec_card_name))
  { 
    MSG_FATAL("RFC_HEAD_STRING too long!!!", 0, 0, 0);
  }

  (void)snprintf(dynrec_card_ptr, n+1, "%s", temp_buf);
  dynrec_card_ptr += n;

  n = snprintf(temp_buf, sizeof(temp_buf), "%lu", (uint32)rf_hw);
  (void)snprintf(dynrec_card_ptr, n+1, "%s", temp_buf);

  dynrec_card_ptr = dynrec_card_name;

#ifndef TEST_FRAMEWORK
  /* rfc_memory_allocated using dynamic_load(rf_hw); */
  // if ((n = mcfg_dynrec_load(&dynrec_card_ptr, 1)) != 0)
  // { 
    // MSG_FATAL("RFC_CARD: %s, HWID %d, failed to load!!!", dynrec_card_name, rf_hw, 0);
  // }

  // if ((n = mcfg_dynrec_reclaim(&card_reclaimed)) != 0)
  // { 
    // MSG_FATAL("RFC_CARD_HWID %d, failed to reclaim!!!", rf_hw, 0, 0);
  // }
  // else
  // {
    // RF_MSG_1(RF_ERROR, "RFC_CARD_memory: %d, reclaimed!!!", card_reclaimed);
  // }
#endif

  rfc_factory(rf_hw, &vreg_mgr_p, &rfc_data);

  if (vreg_mgr_p == NULL || rfc_data == NULL)
  {
    RF_MSG_1 ( RF_ERROR, "rfc_set_card: Failed to Create rf_card objects (hwid=%lu)",
               rf_hw );
    rfc_initialized = FALSE;
  }
  else
  {
    temp_p = new rfc_intf(rf_hw, sysclk);
    if (temp_p && temp_p->get_init_status())
    {
      rfc_initialized = TRUE;
    }
    else 
    {
      RF_MSG_1 ( RF_ERROR, "rfc_set_card: Failed to Create rfc_intf object (hwid=%lu)",
               rf_hw );
    }
  }

  return rfc_initialized;
} /* rfc_set_card() */

