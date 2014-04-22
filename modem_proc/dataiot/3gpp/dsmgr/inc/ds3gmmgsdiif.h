#ifndef DS3GMMGSDIIF_H
#define DS3GMMGSDIIF_H
/*===========================================================================
                      DS3GMMGSDIIF.H
DESCRIPTION
This file exposes MMGSDI interface related data structures and APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when       who    what, where, why
--------   ---    ----------------------------------------------------------
30/08/18   PK      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dstaski.h"
#include "dsati.h"

#ifdef FEATURE_MMGSDI

#include "mmgsdilib.h"
#include "mmgsdilib_v.h"
#include "uim_v.h"

#include "mmgsdisessionlib.h"


/*===========================================================================

	PUBLIC DATA DECLARATIONS

===========================================================================*/

#define MAX_DS3G_MMGSDI_SESSIONS 1
#define MAX_DS3G_MMGSDI_NON_PROV_SESSIONS 1

#define DS3G_MMGSDI_INVALID_CLIENT_ID     0x0000000000000000

#define DS3G_MMGSDI_INVALID_SESSION_ID     0x00000000FFFFFFFF


/* The following table records IMSI info reading from USIM  */

#define DS3G_MMGSDI_IMSI_LEN         9
#define DS3G_MMGSDI_MNC_LEN          1
#define DS3G_MMGSDI_ICCID_OFFSET     0

#define DS3G_MMGSDI_ICCID_LEN        10
#define DS3G_MMGSDI_IMSI_M_LEN       10

#define MAX_MMGSDI_BUFFER_LEN  512

#define ACC_CLASS_11_15_MASK  0xF8


/* IMSI info for USIM and SIM APP */
typedef struct 
{
	uint8 imsi_response[DS3G_MMGSDI_IMSI_LEN];
	uint8 mnc_response[DS3G_MMGSDI_MNC_LEN];
	nv_ehrpd_imsi_type imsi_info;
	boolean ready_status;     /* IMSI info is ready for reading */
} ds3g_mmgsdi_imsi_info_type;

/* IMSI_M info for CSIM and RUIM APP */
typedef struct
{
	uint8    imsi_m_response[DS3G_MMGSDI_IMSI_M_LEN];
	boolean  ready_status;
} ds3g_mmgsdi_imsi_m_info_type;

typedef struct 
{
	mmgsdi_client_id_type client_id;
/* Client Id that MMGSDI assigned to DS */
	mmgsdi_session_id_type session_id;
/* Session Id  */
	mmgsdi_session_type_enum_type session_type;
/* Session Type  */
	mmgsdi_app_enum_type   app_type;
/* Application type */
	mmgsdi_slot_id_enum_type  slot;
/* Slot info */
	boolean  is_operational;
/* Received Subscription Ready Event from MMGSDI */
	sys_modem_as_id_e_type    sub_id;
/* Subscription ID assigned to this session type */
	uint8                     iccid_len;
/* Length of ICCID of the card */
	uint8                     iccid[DS3G_MMGSDI_ICCID_LEN];
} ds3g_mmgsdi_info_type;

/* The following events are set when receiving MMGSDI events */

/* IMSI info table */
typedef struct
{
  mmgsdi_session_type_enum_type session_type;
  uint8                         subscription_ready_status;
} ds3g_mmgsdi_subscription_ready_bit_mask_type;


static ds3g_mmgsdi_subscription_ready_bit_mask_type
ds3g_mmgsdi_subscription_ready_status;


typedef enum
{
	DS3G_MMGSDI_IDLE_EVENT = 0,
/* Initilized value */
	DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT = 1,
/* Received MMGSDI_SUBSCRIPTION_READY_EVT */
	DS3G_MMGSDI_REFRESH_VOTING_EVENT  = 2,
/* Received MMGSDI_REFRESH_EVT and stage is MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT */
/* ISIM voting feature is interested at this event */
	DS3G_MMGSDI_OK_REFRESH_EVENT = 3,
/* Received MMGSDI_REFRESH_EVT and stage is MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN */
	DS3G_MMGSDI_REFRESH_START_EVENT = 4,
/* Received MMGSDI_REFRESH_EVT and stage is MMGSDI_REFRESH_STAGE_START */
	DS3G_MMGSDI_REFRESH_COMPLETE_EVENT = 5,
/* this event is received when SIM card's refresh 
		 is triggered */  
	DS3G_MMGSDI_SIM_READ_EVENT = 6,
/* When receiving  MMGSDI_SUBSCRIPTION_READY_EVT, DS will send request to check
		 CDMA service table. DS will set this event when receiving the call back of
		 mmgsdi_session_read_transparent */
	DS3G_MMGSDI_SIM_WRITE_EVENT = 7,
/* After reading info from CDMA service table. DS will set update
		 CDMA operation table, this event is set when receiving the call back of
		 mmgsdi_session_write_transparent */
	DS3G_MMGSDI_SRV_AVAIL_EVENT = 8,
/* this event is received when receiving the callback of
		 mmgsdi_session_is_service_available */
	DS3G_MMGSDI_IMSI_INFO_EVENT = 9,
/* this event is received when receiving the callback of
		 imsi info */
	DS3G_MMGSDI_MNC_INFO_EVENT = 10,
/* this event is received when receiving the callback of
		 mnc info */
	DS3G_MMGSDI_CLIENT_ID_EVENT = 11,
/* this event is received when receiving the callback of
		 mmgsdi_client_id_and_evt_reg */
	DS3G_MMGSDI_ACL_INFO_EVENT = 12,
/* this event is received when receiving the callback of
		 acl info */
	DS3G_MMGSDI_ACL_ENABLED_EVENT = 13,
/* this event is received when receiving the callback of
		 mmgsdi_session_get_app_capabilities */
	DS3G_MMGSDI_CARD_ERROR_EVENT = 14,
/* this event is received when SIM card is removed */
	DS3G_MMGSDI_SESSION_CHANGED_EVENT = 15,
/* this event is received when session is changed */
	DS3G_MMGSDI_ISIM_REFRESH_START_EVENT = 16,
/* this event is received when ISIM refresh is started */
	DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_EVENT = 17,
/* this event is received when ISIM refresh is complete */
	DS3G_MMGSDI_EFACC_READ_CNF_EVENT  = 18,
/* this event is received when callback of EFacc Read */
	DS3G_MMGSDI_ICCID_INFO_EVENT = 19,
/* this event is received when receiving the callback of */
/* iccid info */
	DS3G_MMGSDI_IMSI_M_INFO_EVENT = 20
/* this event is received when receiving the callback of */
} ds3g_mmgsdi_event_enum_type;

typedef struct
{
  ds3g_mmgsdi_event_enum_type           event_type;
  mmgsdi_session_id_type                session_id;
  mmgsdi_app_enum_type                  app_type;
  byte                                  data[MAX_MMGSDI_BUFFER_LEN];
  mmgsdi_slot_id_enum_type              slot;
  mmgsdi_refresh_source_enum_type         source;
  mmgsdi_session_changed_evt_info_type  session_changed_info;
  mmgsdi_refresh_mode_enum_type           refresh_mode;
  mmgsdi_refresh_enforcement_policy_type  refresh_enforcement_policy;
  union
  {
    ds_at_mmgsdi_event_type     mmgsdi_event_info;
    ds_at_mmgsdi_sim_info_type  mmgsdi_sim_info;
  }cmd_info;
} ds3g_mmgsdi_event_info_type;


typedef struct
{
	boolean                       is_operational;
	mmgsdi_session_id_type        session_id;
	mmgsdi_session_type_enum_type session_type;
	int32                         channel_id;
	mmgsdi_slot_id_enum_type      slot_id;
	mmgsdi_aid_type               app_info;
} ds3g_mmgsdi_non_prov_isim_session_info_type;



/*===========================================================================

	PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*==============================================================================
FUNCTION  DS3G_MMGSDI_VALIDATE_CLIENT_ID

DESCRIPTION

  Validate provided client id with client id getting from MMGSDI interface. 
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Valid client id
  FALSE: Invalid client id

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_validate_client_id
(
 mmgsdi_client_id_type client_id
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_APP_TYPE_FROM_SESSION_ID

DESCRIPTION
  This function returns the app_type based on session_id.

PARAMETERS
  session_id : mmgsdi session id.

RETURN VALUE
  app_type : app_type corresponding to the given session_id.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_app_enum_type ds3g_mmgsdi_get_app_type_from_session_id
(
  mmgsdi_session_id_type  session_id
);

/*===========================================================================
FUNCTION      DS3G_MMGSDI_INIT

DESCRIPTION
  This Function registers DS manager as a client of MMGSDI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_init
(
 void
);


/*===========================================================================
FUNCTION DS3G_MMGSDI_PROCESS_CMD

DESCRIPTION
  A command processing function is called based on the type of command received.
  This function should be called by the top-level task dispatcher whenever
  any MMGSDI command is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_mmgsdi_process_cmd
(
 ds_command_type* cmd_ptr
);

/*==============================================================================
FUNCTION DS3G_MMGSDI_CLIENT_EVT_REG_CB

DESCRIPTION
  This function handles the MMGSDI client event registration callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void ds3g_mmgsdi_client_evt_reg_cb
(
 const mmgsdi_event_data_type *event
);

/*===========================================================================
FUNCTION DS3G_MMGSDIF_READ_NAS_CONFIG_FILE
 
DESCRIPTION
  This function reads the NAS Config File from USIM and fetches the 
  Tag Value coressponding to the Tag Id specified in the input
 
PARAMETERS 
  1. Tag Id
  2. Output ptr to write the Tag Value
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  TRUE if Tag Value can be read successfully. 
  FALSE otherwise.
 
SIDE EFFECTS 
  None   
===========================================================================*/
boolean ds3g_mmgsdi_read_nas_config_file
(
  ds3g_mmgsdi_tag_e_type    tag_id,
  void                      *tag_value_p
);

/*===========================================================================
FUNCTION DS3G_MMGSDIF_READ_NAS_CONFIG_TAG
 
DESCRIPTION
  This function takes a byte stream and fetches Tag Value
  coressponding to the Tag Id from the byte stream
 
PARAMETERS 
  1. Byte stream
  2. Length of the byte stream
  3. Tag Id
  4. Output ptr to write the Tag Value
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  TRUE if Tag Value can be read successfully. 
  FALSE otherwise.
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds3g_mmgsdi_read_nas_config_tag
(
  uint8                  *input_file_stream,
  uint8                   input_file_length,
  ds3g_mmgsdi_tag_e_type  tag_id,
  void                   *output_tag_value_p
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_EFACC_CLASS
  
DESCRIPTION
  Returns true if any of the bits from b4-b8 is set in the byte 1 
  i.e 11 -15 as shown below. 
  
  Byte 1:
  b8 b7 b6 b5 b4 b3 b2 b1
  15 14 13 12 11 10 09 08 Number of the ACC (except for bit b3)
  
  if any of the bits from 11-15 are enabled then EFACC flag is set to true.
  Spec reference: TS 3GPP: 31.102
  Byte 2:
  b8 b7 b6 b5 b4 b3 b2 b1
  07 06 05 04 03 02 01 00 Number of the ACC
 
  In addition this returns the EFACC CLASS value
  
PARAMETERS
  Pointer to retrieve EFACC class
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE  - if bits b4-b8 are set in the byte 1 of ACC
  FALSE - otherwise
  
SIDE EFFECTS
  None.
============================================================================*/
boolean ds3g_mmgsdi_get_efacc_class
(
  uint32                *efacc_class
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_IS_SUBS_READY

DESCRIPTION
  Function to determine if subscription ready has been received

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If suscription ready was received
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_is_subs_ready
(
  void
);

/*===========================================================================

FUNCTION DS3G_GET_MMGSDI_INFO_HNDL

DESCRIPTION
  This function returns ds3g_mmgsdi_info_handler

DEPENDENCIES
  None

RETURN VALUE
  ds3g_mmgsdi_info_handler

SIDE EFFECTS
  None

===========================================================================*/

ds3g_mmgsdi_info_type * ds3g_get_mmgsdi_info_hndl
(
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMSI_DATA()

DESCRIPTION
  Passed cached IMSI data to client.

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMSI data
  TRUE: Read IMSI data successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_imsi_data
(
  nv_ehrpd_imsi_type*    imsi_data_ptr
);

/*===========================================================================
FUNCTION  DS3G_MMGSDI_MNC_INFO_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_MNC_INFO_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_mnc_info_event_process
(
  void
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_SESSION_ID

DESCRIPTION
  Get session id getting from MMGSDI interface

DEPENDENCIES
  None.

RETURN VALUE
  session id

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3g_mmgsdi_get_session_id
(
  mmgsdi_session_id_type         *session_id_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_READ_REFRESH_VOTE_OK_NV

DESCRIPTION    Utility to read from NV item - REFRESH VOTE OK

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_mmgsdi_read_refresh_vote_ok_nv 
(
   void
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */


#endif /* FEATURE_MMGSDI */
#endif /* DS3GMMGSDIIF_H */
