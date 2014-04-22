/*!
  @file nb1_misc.c


  @brief
  REQUIRED This file has all Miscellaneous routine used by nb1 modules

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_misc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
=============================================================================*/

#ifndef NB1_MISC_H
#define NB1_MISC_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <comdef.h>
#include <IxErrno.h>
#include <sys.h>
#include <nb1_cphy_msg.h>
#include <msgr.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/


  /*8x30 JTAG_ID_1 0x0728 :- MM NB1 Cat2, HSPA+, DOrB, UMTS*/
  #define NB1_RRC_CAP_8x30_CAT2_JTAG_ID_1 0x0728
  /*8x30 JTAG_ID_2 0x0729 :- MM NB1 Cat2, HSPA+, DOrB, UMTS, HDCP*/
  #define NB1_RRC_CAP_8x30_CAT2_JTAG_ID_2 0x0729
  /*8x30 JTAG_ID_3 0x072A :- 3GPP NB1 Cat2, HSPA+, TD-SCDMA, UMTS*/
  #define NB1_RRC_CAP_8x30_CAT2_JTAG_ID_3 0x072A
  /*8x30 JTAG_ID_4 0x072B :- 3GPP NB1 Cat2, HSPA+, TD-SCDMA, UMTS, HDCP*/
  #define NB1_RRC_CAP_8x30_CAT2_JTAG_ID_4 0x072B
  /*! @brief UE Category -2
  */
  #define NB1_UE_CATEGORY2   2


/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/

/*=============================================================================

                   EXTERNAL STRUCTURE/UNION TYPES

=============================================================================*/



/*=============================================================================

                   DATA TYPES

=============================================================================*/

/*! @brief Enumeration of Source Layer
*/
typedef enum
{
  NB1_CLIENT_NONE,          /*!< 0 FOR INTERNAL USE ONLY */
  NB1_ML1_CLIENT,           /*!< 1 */
  NB1_L2_CLIENT,            /*!< 2 */
  NB1_RRC_CLIENT            /*!< 3 */
} nb1_layer_client_id_e;

/*! @brief REQUIRED brief description of this structure typedef
  This type is used for returning ue_category
*/
typedef struct
{
  /*Jtag I.D used for debugging only*/
  uint32 debug_pnId;

  boolean use_cat_2;   /*!< UE category */
} nb1_ue_category_info_s;

#define NB1_COMMON_EFS_REFRESH_NB1_CAT 0x01
#define NB1_COMMON_EFS_REFRESH_CA_DISABLE 0x02
#define NB1_COMMON_EFS_REFRESH_EMBMS_DPD 0x04


typedef struct
{
   /*! Tune-Aways are enabled for DSDS/SRLTE/SGLTE+G/SVLTE+G */
   boolean tuneaway_enabled;
   /*! Conflict checks needed for either TX_BACKOFF or BLANKING*/
   nb1_ml1_coex_dsda_conflict_type_e conflict_type;
   /*! Single Tx is enabled for DSDA device mode*/
   boolean single_tx_enabled;
}nb1_common_msim_crat_features_s;

typedef struct
{
  msgr_hdr_s                          msg_hdr; /*!< Message router header */
  /*! Enabled Common NB1 Features*/
  nb1_common_msim_crat_features_s     common_features;
  /*! Number of Active Sims*/
  sys_modem_dual_standby_pref_e_type  standby_mode;
  /*! SS/DSDS/DSDA/TSTS */
  sys_modem_device_mode_e_type        device_mode;
  /*! SRLTE/SVLTE/SGLTE/Normal */
  nb1_cphy_ue_mode_type_e             cphy_ue_mode;
}nb1_common_msim_crat_feature_req_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  nb1_get_ue_category

===========================================================================*/
/*!
  @brief
   this function will return ue_category

  @return
  ue_category - 2 or 3

*/
/*=========================================================================*/
uint8 nb1_get_ue_category
(
  void
);
/*===========================================================================

  FUNCTION:  nb1_get_current_time

===========================================================================*/
/*!
    @brief
    Get current time in ms since power up

    @details
    Get current time in ms since power up
    This API will be removed once all SW is migrated to use one of the two
    below APIs

    @return
    None
*/
/*=========================================================================*/
uint64 nb1_get_current_time(void);

/*===========================================================================

  FUNCTION:  nb1_get_current_time_since_power_up_ms

===========================================================================*/
/*!
    @brief
    Get current time in ms since power up

    @details
    Get current time in ms since power up

    @return
    None
*/
/*=========================================================================*/
/* Current time since power up in ms */
uint64
nb1_get_current_time_since_power_up_ms(void);

/*===========================================================================

  FUNCTION:  nb1_get_current_time_since_power_up_secs

===========================================================================*/
/*!
    @brief
    Get current time in secs since power up

    @details
    Get current time in secs since power up

    @return
    None
*/
/*=========================================================================*/
/* Current time since power up in secs */
uint32
nb1_get_current_time_since_power_up_secs(void);

/*===========================================================================

  FUNCTION:  nb1_set_current_time

===========================================================================*/
/*!
    @brief
    Set current time in ms since power up

    @details
    Set current time in ms since power up

    @return
    None
*/
/*=========================================================================*/
void nb1_set_current_time(uint64 cur_time);

/*===========================================================================

  FUNCTION:  nb1_is_ca_feature_enabled

===========================================================================*/
/*!
    @brief
    if DALCHIPINFO_ID_MDM9225_1
      then CA is disabled
    else
      CA is Enabled

    @return
      boolean
*/
/*=========================================================================*/
extern boolean nb1_is_ca_feature_enabled
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_allow_efs_refresh

===========================================================================*/
/*!
    @brief
    Allow UE Cat and CAP feature EFS files to be refreshed

    @return
    None
*/
/*=========================================================================*/
extern void nb1_allow_efs_refresh(uint8 efs_refresh_bitmask);


/*===========================================================================

  FUNCTION:  nb1_common_msim_crat_feature_enabled

===========================================================================*/
/*!
    @brief
    Returns struct containing enabled features for multisim/concurrent rat
    configured device

    @details
    Returns struct containing enabled features for multisim/concurrent rat
    configured device

    @return
    None
*/
/*=========================================================================*/
extern nb1_common_msim_crat_features_s nb1_common_msim_crat_feature_enabled(
      nb1_cphy_ue_mode_type_e ue_mode,
      sys_modem_device_mode_e_type device_mode,
      sys_modem_dual_standby_pref_e_type standby_mode);

/*===========================================================================

  FUNCTION:  nb1_is_dpd_feature_enabled

===========================================================================*/
/*!
    @brief
    this function will tell if DPD feature is enabled for eMBMS

    @return
    boolean stating if DPD is enabled or not
*/
/*=========================================================================*/
extern boolean nb1_is_dpd_feature_enabled
(
  void
);

#endif /* NB1_MISC_H */
