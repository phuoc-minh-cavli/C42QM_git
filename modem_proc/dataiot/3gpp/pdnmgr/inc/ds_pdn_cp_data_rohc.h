#ifndef DS_PDN_CP_DATA_ROHC_H
#define DS_PDN_CP_DATA_ROHC_H

/*===========================================================================
                      DS_PDN_CP_DATA_ROHC.H
DESCRIPTION
This file exposes PDN CP data ROHC through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_cp_data_rohc.h#2 $
  $DateTime: 2021/11/12 04:39:21 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ps_rohc.h"
#include "ds_pdn_psm_hdlr.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Enum of ROHC profiles
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_CP_DATA_ROHC_PROFILE_ID_0X0000 = 0x0000,
  DS_PDN_CP_DATA_ROHC_PROFILE_ID_0X0002 = 0x0001,
  DS_PDN_CP_DATA_ROHC_PROFILE_ID_0X0003 = 0x0002,
  DS_PDN_CP_DATA_ROHC_PROFILE_ID_0X0004 = 0x0004,
  DS_PDN_CP_DATA_ROHC_PROFILE_ID_0X0006 = 0x0008, 
  DS_PDN_CP_DATA_ROHC_PROFILE_ID_0X0102 = 0x0010,
  DS_PDN_CP_DATA_ROHC_PROFILE_ID_0X0103 = 0x0020,
  DS_PDN_CP_DATA_ROHC_PROFILE_ID_0X0104 = 0x0040
} ds_pdn_cp_data_rohc_profile_type;

/*---------------------------------------------------------------------------
  Max Context IDs supported
---------------------------------------------------------------------------*/
#define DS_PDN_CP_DATA_DEF_MAX_CID 15    /* default value for max_cid 
                                             as in RRC spec 25.331 */


#define DS_PDN_CP_DATA_ROHC_MAX_JITTER_CD 60

/* compressor flow default parameters*/
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_WIN_WIDTH 6
#define DS_PDN_CP_DATA_ROHC_COMP_MAX_NUM_ALLOWED_PKT_SIZES 0
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_NUM_UPDATE_IR 15
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_NUM_NON_UPDATES 6
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_SO_IRT 256
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_SO_FOT 128
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_TIMER_COMP FALSE
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_PROF_HINT 0
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_F_MAX_TIME 500
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_EXP_BACKOFF 1
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_TIME_STRIDE 20
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_TS_RECALC TRUE
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_LOSS_OPTION FALSE
#define DS_PDN_CP_DATA_ROHC_COMP_FLOW_P_SN 5
#define DS_PDN_CP_DATA_ROHC_COMP_CONST_IP_ID_HANDLING TRUE

/* decompressor flow default parameters */
#define DS_PDN_CP_DATA_ROHC_DECOMP_FLOW_ACK_ENAB FALSE
#define DS_PDN_CP_DATA_ROHC_DECOMP_FLOW_RD_DEPTH 0
#define DS_PDN_CP_DATA_ROHC_DECOMP_FLOW_TIMER_BASED_DECOMP FALSE
#define DS_PDN_CP_DATA_ROHC_DECOMP_FLOW_LOSS_OPTION_THRES 0
#define DS_PDN_CP_DATA_ROHC_DECOMP_FLOW_P_SN 5

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_ROHC_ALLOC_MEM

DESCRIPTION
  This function allocates memory for ROHC parmeter for the passed bearer
  
PARAMETERS
  Bearer index.

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_cp_data_rohc_alloc_mem
(
  uint8  index
);

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_ROHC_FREE_MEM

DESCRIPTION
  This function frees memory for ROHC parmeter for the passed bearer
  
PARAMETERS
  Bearer index.

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_cp_data_rohc_free_mem
(
  uint8  index
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_rohc_alloc

DESCRIPTION
  This function is used to allocate a ROHC compressor and decompressor 
  instance.

PARAMETERS
  Bearer Instance
  Header Compression Configuration

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_rohc_alloc
(
  int            bearer_instance,
  hc_config_T   *hc_config_p
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_rohc_free

DESCRIPTION
  This function is used to free the compressor and decompressor instance.

PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_rohc_free
(
 uint8 bearer_instance
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_rohc_compress

DESCRIPTION
  This function is used to compress the Uplink Packet

PARAMETERS
  Bearer Instance,
  Pointer to the Pointer to the packet

DEPENDENCIES
  None.

RETURN VALUE  
  0, if success
 -1 otherwise

SIDE EFFECTS
  None.

===========================================================================*/
int ds_pdn_cp_data_rohc_compress
(
  int          bearer_instance,
  dsm_item_type **pkt_pp
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_rohc_decompress

DESCRIPTION
  This function is used to decompress the Downlink Packet

PARAMETERS
  Bearer Instance,
  Pointer to the Pointer to the packet
  Pointer to the pointer to the feedback packet that should be sent to NW

DEPENDENCIES
  None.

RETURN VALUE  
  0, if success
 -1  otherwise

SIDE EFFECTS
  None.

===========================================================================*/
int ds_pdn_cp_data_rohc_decompress
(
  int             bearer_instance,
  dsm_item_type **pkt_pp,
  dsm_item_type **fb_pp
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_rohc_is_enabled_for_bearer

DESCRIPTION
  This function checks whether ROHC is enabled for the given bearer.

PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if enabled

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_cp_data_rohc_is_enabled_for_bearer
(
  int bearer_instance
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_tau_complete_ind_hdlr

DESCRIPTION
  This function handles the TAU complete indication from NAS, which would
  provide the header compression status for each bearer.

PARAMETERS
  TAU complete message from NAS

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if enabled

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_tau_complete_ind_hdlr
(
  emm_tau_complete_ind_type  *payload_ptr
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_rohc_get_comp_hdl

DESCRIPTION
  This function is used to fetch the compressor handle for the given
  bearer instance

PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE  
  ROHC Handle

SIDE EFFECTS
  None.

===========================================================================*/
rohc_handle_type* ds_pdn_cp_data_rohc_get_comp_hdl
(
  int bearer_inst
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_set_rohc_is_enabled_for_bearer

DESCRIPTION
  This function sets the ROHC is enabled flag for the given bearer instance.

PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_set_rohc_is_enabled_for_bearer
(
  int     bearer_instance,
  boolean rohc_is_enabled
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_rohc_process_call_end

DESCRIPTION
  This function processes the CALL END information to cleanup the ROHC state.

PARAMETERS
  Bearer Instance
  Subs Id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_rohc_process_call_end
(
 uint8            bearer_inst
);

/*===========================================================================
FUNCTION ds_pdn_cp_data_init_rohc_params

DESCRIPTION
  This function is used to initilize the ROHC parameters

PARAMETERS
  Compressor Channel
  Compressor Flow
  Decompressor Channel
  Decompressor Flow

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_init_rohc_params
(
  int                                bearer_index,
  rohc_params_comp_channel_s_type   *comp_channel_p, /*!< compressor ch parameters*/
  rohc_params_comp_flow_s_type      *comp_flow_p, /*!< compressor flow parameters*/
  rohc_params_decomp_channel_s_type *decomp_channel_p, /*!< decompressor ch parameters*/
  rohc_params_decomp_flow_s_type    *decomp_flow_p /*!< decompressor flow parameters*/
);

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_ROHC_PDM_SAVE_CONTEXT
DESCRIPTION
  

PARAMETERS
  psm_info_p

DEPENDENCIES
  None.

RETURN VALUE  
  

SIDE EFFECTS
  None.

===========================================================================*/
void  ds_pdn_cp_data_rohc_psm_save_context
(
  ds_pdn_psm_rohc_info_s               *psm_rohc_info_p,
  uint8                                bearer_instance
);

#endif /* DS_PDN_CP_DATA_ROHC_H */
