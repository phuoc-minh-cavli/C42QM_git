/*!
  @file
  nb1_rrc_ext_api.h

  @brief
  This file describes the API interfaces to NB1 Radio resource Controller (RRC)
*/


/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_rrc_ext_api.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------


===========================================================================*/

#ifndef NB1_RRC_EXT_API_H
#define NB1_RRC_EXT_API_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte_rrc_ext_api.h>
#ifndef FEATURE_DYNAMIC_LOADING_LTE
#include <lte_static_rrc_ext_api.h>
#endif
#include <nb1_cphy_msg.h>
#include <sys.h>
/*===========================================================================

                   EXTERNAL FUNCTIONS

===========================================================================*/

/*Reads the specified NV flag. Read value is populated in item_ptr
  If the NV reading activity is successful, return TRUE, else return FALSE*/
extern boolean nb1_nv_read
(
  nv_items_enum_type nv_item, /* Item to get */
  nv_item_type      *item_ptr /* Pointer where to put the item */
);

#if !defined (FEATURE_DYNAMIC_LOADING_LTE) && !defined (FEATURE_EXTERNAL_PAGING)
extern lte_static_rrc_ext_api_s *nb1_rrc_get_lte_nb1_vtbl
(
  void
);
#endif

extern boolean nb1_rrc_csp_is_psm_succeed(void);

extern  boolean nb1_rrc_csp_is_acq_try_for_psm (void);

extern uint32 nb1_rrc_csp_get_psm_threshhold(void);
/* Return TRUE if NB1 stack is activated and MultiDRB is supported*/
extern boolean nb1_rrc_cap_multi_drb_is_supported(void);
extern nb1_cphy_power_class_e nb1_rrc_get_power_class_per_band(sys_sband_lte_e_type nb1_band);

/* Returns the SNR level for frequency scan */
extern uint8  nb1_rrc_csp_get_fscan_snr_level(void);

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
extern boolean nb1_rrc_plmn_search_get_scan_ref_time
(
  uint64 *scan_ref_time_ptr
);
#endif

#endif /* NB1_RRC_EXT_API_H */

