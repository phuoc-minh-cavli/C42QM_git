#ifndef _POLICYMAN_RF_H_
#define _POLICYMAN_RF_H_

/**
  @file policyman_rf.h

  @brief  RAT and RF band information
*/

/*
    Copyright (c) 2013-2017 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_rf.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "sd_v.h"
#include "policyman_i.h"
#include "policyman_diag.h"



typedef struct
{
  uint32              ratMask;
  sys_band_mask_type  gwBands;
  sys_lte_band_mask_e_type  lteBands;
  sys_band_mask_type  tdsBands;
} policyman_rf_device_info_t;


typedef struct policyman_rf_info_t  policyman_rf_info_t;

#if defined FEATURE_DISABLE_DYNAMIC_POLICIES
typedef struct
{
  POLICYMAN_ITEM;

  sys_band_mask_type  gwBands;
  sys_lte_band_mask_e_type  lteBands;
  sys_band_mask_type  tdsBands;
}rf_bands_item_t;



/*-------- policyman_state_rf_init --------*/
/**
@brief

@param

@return

*/
void
policyman_state_rf_init(
  void
  );

#else

/*-------- policyman_rf_init --------*/
/**
@brief

@param

@return

*/
boolean
policyman_rf_init(
  policyman_rf_info_t **ppRfInfo
  );
#endif

/*-------- policyman_rf_deinit --------*/
void
policyman_rf_deinit(
#if defined FEATURE_DISABLE_DYNAMIC_POLICIES
  void
#else
  policyman_rf_info_t *pRfInfo
#endif
  );

/*-------- policyman_get_hardware_bands_all_techs --------*/
void
policyman_get_hardware_bands_all_techs(
  sys_band_mask_type *pGwBands,
  sys_lte_band_mask_e_type *pLteBands,
  sys_band_mask_type *pTdsBands
  );


/*-------- policyman_rf_bands_item_new --------*/
/**
@brief

@param

@return

*/
policyman_item_t * policyman_rf_bands_item_new(
  sys_band_mask_type  gwBands,
  sys_lte_band_mask_e_type  lteBands,
  sys_band_mask_type  tdsBands
);

/*-------- policyman_rf_band_list_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t
policyman_rf_band_list_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );



/*-------- policyman_rf_bands_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t
policyman_rf_bands_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );

/*-------- policyman_rf_bands_if_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t
policyman_rf_bands_if_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );


/*=============================================================================
  Methods for the RF band configuration item
=============================================================================*/

/*-------- policyman_rf_bands_get_default_from_efs --------*/
/**
@brief

@param

@return

*/
policyman_item_t *
policyman_rf_bands_get_default_from_efs(
  sys_modem_as_id_e_type subs_id
  );


/*-------- policyman_rf_bands_get_default --------*/
/**
@brief

@param

@return

*/
policyman_item_t *
policyman_rf_bands_get_default(
  sys_modem_as_id_e_type subsId
  );


/*-------- policyman_rf_bands_compare --------*/
/**
@brief

@param

@return

*/
boolean
policyman_rf_bands_compare(
  policyman_item_t  *pi1,
  policyman_item_t  *pi2
  );


/*-------- policyman_rf_print_bands --------*/
/**
@brief

@param

@return

*/
void
policyman_rf_print_bands(
  sys_band_mask_type  gwBands,
  sys_lte_band_mask_e_type  lteBands,
  sys_band_mask_type  tdsBands
  );


/*-------- policyman_rf_bands_display --------*/
/**
@brief

@param

@return

*/
void
policyman_rf_bands_display(
  policyman_item_t  *pItem
  );



/*=============================================================================
  Accessor functions to read from configuration items
=============================================================================*/

/*-------- policyman_rf_get_device_info --------*/
/**
@brief

@param

@return

*/
boolean
policyman_rf_get_device_info(
  policyman_rf_info_t const   *pRfInfo,
  size_t                      device,
  policyman_rf_device_info_t  *pDeviceInfo
  );


/*-------- policyman_rf_get_overall_info --------*/
/**
@brief

@param

@return

*/
policyman_rf_device_info_t *
policyman_rf_get_overall_info(
  void
  );


/*-------- policyman_rf_get_overall_rat_mask --------*/
uint32
policyman_rf_get_overall_rat_mask(
  void
  );


/*-------- policyman_get_bands_for_ueMode --------*/
/**
@brief

@param

@return

*/
rf_bands_item_t *
policyman_get_bands_for_ueMode(
   sys_ue_mode_e_type current_ue_mode,
   policyman_policy_t *pPolicy
   );


/*-------- policyman_get_bands_item_for_named_bands --------*/
rf_bands_item_t *
policyman_get_bands_item_for_named_bands(
  rf_bands_t  *pBands
  );

/*-------- policyman_get_bands_for_named_object --------*/
void
policyman_get_bands_for_named_object(
  rf_bands_t         *pBands,
  rf_bands_t         *pGetBands
);


/*-------- policyman_rf_bands_filter_based_on_rat --------*/
void
policyman_rf_bands_filter_based_on_rat(
  rf_bands_item_t  *prfb,
  uint32           ratMask
);

/*-------- policyman_rf_band_list_read --------*/
boolean
policyman_rf_band_list_read(
  policyman_xml_element_t const *pElem,
  rf_bands_t                    *pBands
  );


/*-------- policyman_rf_bands_fill_subs_diag_log --------*/
void policyman_rf_bands_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
);

/*-----------------------------------------------------------------------------
  Band masks used in policyman_rf.c and also in unit tests.
-----------------------------------------------------------------------------*/


#define ALL_CDMA_BANDS        \
  (                       \
    SD_SS_BAND_PREF_BC0   \
  | SD_SS_BAND_PREF_BC1   \
  | SD_SS_BAND_PREF_BC3   \
  | SD_SS_BAND_PREF_BC4   \
  | SD_SS_BAND_PREF_BC5   \
  | SD_SS_BAND_PREF_BC6   \
  | SD_SS_BAND_PREF_BC7   \
  | SD_SS_BAND_PREF_BC8   \
  | SD_SS_BAND_PREF_BC9   \
  | SD_SS_BAND_PREF_BC10  \
  | SD_SS_BAND_PREF_BC11  \
  | SD_SS_BAND_PREF_BC12  \
  | SD_SS_BAND_PREF_BC14  \
  | SD_SS_BAND_PREF_BC15  \
  | SD_SS_BAND_PREF_BC16  \
  | SD_SS_BAND_PREF_BC17  \
  | SD_SS_BAND_PREF_BC18  \
  | SD_SS_BAND_PREF_BC19  \
  )

#define ALL_HDR_BANDS   ALL_CDMA_BANDS
#define ALL_GSM_BANDS   SD_SS_BAND_PREF_GSM_ANY
#define ALL_WCDMA_BANDS SD_SS_BAND_PREF_WCDMA_ANY
#define ALL_LTE_BANDS   SD_SS_BAND_PREF_LTE_ANY
#define ALL_TDS_BANDS   SD_SS_BAND_PREF_TDS_ANY

#endif /* _POLICYMAN_RF_H_ */
