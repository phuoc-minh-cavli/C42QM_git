/*!
  @file
  nb1_mac_ind.h

  @brief
  This file contains all External indications thrown by MAC layer.


*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_mac_ind.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/21   ve      CR 2869397 : FR68338:Idle mode RACH improvement changes

===========================================================================*/

#ifndef NB1_MAC_IND_H
#define NB1_MAC_IND_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_nb1.h>
#include <msgr_attach.h>

/* Start of L2 internal including */
#include "nb1_mac.h"
#include "a2_diag.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief MAC RACH access problem indication. MAC UL sends to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;           /*!< Message header */
  nb1_mac_access_reason_e             access_reason; /*!< access reason */
  /* Message payload */
} nb1_mac_random_access_problem_ind_msg_s;

/*! @brief Message for MAC RRC DL Data Indication. MAC DL sends to RRC
*/
typedef struct
{
  msgr_hdr_s                          hdr;          /*!< Message header */
  msgr_attach_s                       dsm_attach;   /*!< dsm attachment contained signal message */
  uint16                              phy_cell_id;  /*!< The cell Id of the cell on which data was received */
  nb1_earfcn_t                        freq;         /*!< The frequency of the cell on which data was received */
  nb1_sfn_s                           sfn;          /*!< The system and subframe number */
} nb1_mac_rrc_dl_data_ind_s;

/*
* @brief Parameter struct for  NB1_MAC_A2_DL_PHY_DATA_IND. Using
* in a callback function provided to A2 driver by MAC so that
* when the callback function is invoked, it will send this indication
* to MAC DL
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
} nb1_mac_a2_dl_phy_data_ind_s;

/*
* @brief Parameter struct for  NB1_MAC_RELEASE_RESOURCES_IND. MAC will send
* this indication to RRC when TA timer expired or SR MAX
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
} nb1_mac_release_resources_ind_s;

/*
* @brief Parameter struct for  NB1_MAC_BCAST_DATA_IND. MAC will send
* this indication to RLC when there is some eMBMS data on DL
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
} nb1_mac_bcast_data_ind_s;

/*
* @brief Parameter struct for  NB1_MAC_DUAL_DATA_IND. MAC will send
* this indication to RLC when there is both eMBMS and unicast data on DL
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
} nb1_mac_dual_data_ind_s;

/*
* @brief Parameter struct for  NB1_MAC_MAX_HARQ_RETX_IND. MAC will
* send this indication to RLC when it detects max HARQ retransmission attempt
* ML1 also sends this indication in long tuneaway.
*/
typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
  uint8                          harq_id; /*!< harq ID */
}nb1_mac_max_harq_retx_ind_s;

typedef struct
{
  msgr_hdr_struct_type           hdr; /*!< Message header */
  boolean                        msg5_grant_success; /*!<grant success flag */
}nb1_mac_rrc_msg5_grant_ind_s;


#endif /* NB1_MAC_IND_H */
