#ifndef RMNET_DATA_UL_LEGACY_H
#define RMNET_DATA_UL_LEGACY_H
/*===========================================================================

                     PS_DPM . H
DESCRIPTION
Header file describing global DPM config definitions.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/dataiot.mpss/2.0/interface/rmnet/inc/rmnet_data_ul_legacy.h#1 $
  $Author: pwbldsvc $ $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/13    bvd     Initial Version
=========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "dsm.h"
#include "ds_fwk.h"
#include "ds_rmnet_smi.h"
#include "ds_pdn_defs.h"
#include "ps_in.h"

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/**
  @brief  This API sets up legacy wmks.

  @code
  @endcode
*/
void rmnet_data_ul_legacy_init
(
  void
);


/*===========================================================================
FUNCTION RMNET_DATA_UL_LEGACY_INPUT

DESCRIPTION

  This function processes the packet in legacy path.

PARAMETERS
  user_data : pointer to user data
  rx_pkt    : dsm_item_type rx pkt

RETURN VALUE
  TRUE  : if legacy watermark is flow disabled
  FALSE : if legacy watermark is flow enabled

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean rmnet_data_ul_legacy_input
(
  rmnet_smi_info_type     *info,
  dsm_item_type           *rx_pkt,
  ds_fwk_s_type           *fwk_inst
);

#ifdef FEATURE_DATA_QMI_QOS
/**
  @brief  This API processes qos hdr for given pkt

  @param[in] dpm_info_ptr            Ptr to DPM info entry
  @param[in] pkt_ptr                 Ptr to Packet
  @param[in] meta_info_ex_ptr_ptr    Ptr to meta info
  @param[in] qos_hdr                 Ptr to Qos header
  @param[in] ip_ver                  IP version of pkt 
  @param[out]flow_ptr                Effective flow_ptr from 
                                     qos_handle


  @return  TRUE      Atleast one call is flow enabled.
  @return  FALSE     None of call is flow enabled.

  @code
  @endcode
*/
void rmnet_process_qos_hdr
(
  dsm_item_type           *pkt_ptr,
  ds_fwk_s_type           *ds_fwk_inst,
  ip_addr_enum_type     ip_ver,
  boolean                  is_opt,
  ps_tx_meta_info_type    *tx_meta_info_ptr,
  void                    *qos_hdr,
  rmnet_qos_format_e_type  qos_hdr_type,
  ds_flow_type           **flow_ptr
);
#endif /* FEATURE_DATA_QMI_QOS */

#endif /* RMNET_DATA_UL_LEGACY_H */
