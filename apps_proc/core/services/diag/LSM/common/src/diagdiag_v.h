#ifndef DIAGDIAG_V_H
#define DIAGDIAG_V_H
/*==========================================================================

                      Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of diag packets must be PACK.
  
  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2001-2017, 2020, 2022-2023 by Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.tx/6.0/services/diag/LSM/common/src/diagdiag_v.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/17   gn      Changes to support MMU enablement and command deregistration
03/24/17   gn      Changes to support DIAG delayed reponses on User space
07/20/16   nk      Changes to support ThreadX
08/17/15   xy      Added support for HDLC removal feature
11/25/14   ph      Removed test case DIAGDIAG_STRESS_TEST_ERR_FATAL_ISR.
07/25/14   sa      Added support to send an event to debug diag drops
07/16/14   xy      Update Diag delayed response command to take parameter 
04/24/14   tbg     Fixed issues with extended event, log and F3 pkts 
05/19/14   sa      Added new Query Command.
03/12/14   xy      Add support for STM status
01/29/14   sa      Added support for compression in buffering mode.
01/23/14   xy      Add support for enable/disable STM command
10/14/13   sr      Added support for extended event, log and F3 pkts
10/01/13   xy      Removed code under FEATURE_WINCE
09/20/13   tbg     Added support for Diag over STM 
02/19/13   rh      Added F3 Trace subcommand
12/07/12   is      Include preset_id in set preset_id response
12/06/12   rs      Removed Obsolete command DIAG_RPC_F (100).
10/11/12   is      Support for preset masks
08/24/12   ra      Support for querying Command Registration Tables 
08/22/12   sr      Support for mask retrieval command   
07/25/12   sr      Support max req pkt len commands  
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap.  
01/24/12   is      Support for buffered circular mode
11/04/11   is      Support for buffered threshold mode
03/04/11   is      Support for Diag over SMD-Lite
01/24/11   hm      Query/Save QPST Cookie Commands
12/08/10   hm      New Structure type for Sub-system Loopback Command Resposne
10/26/10   vs      Changes for new encoding protocol
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
07/10/10   vs      Moved definitions of diagpeek/poke to public header
04/20/10   is      Remove support for DIAG_GET_PROPERTY_F, DIAG_PUT_PROPERTY_F,
                   DIAG_GET_PERM_PROPERTY_F, and DIAG_PUT_PERM_PROPERTY_F.
04/05/10   JV      Added test cases for QSR messages in the stress test
03/02/10   JV      New command to enable/disable data aggregation
02/25/10   JV      New command to flush diagbuf.
12/15/09   JV      Mainlining code under the 'DEBUG_DIAG_TEST' feature
12/07/09   cahn    Added Delayed Response to DIAG Health.
11/12/09   cahn    Added variable length log types for DIAG stress test.
11/04/09   cahn    DIAG Health
09/20/09   sg      Added diagdiag_tbl_init prototype
09/15/09   cahn    Added loopback on individual processors.
09/10/09   JV      Added protptype for diagdiag_init().
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/14/09   mad     Featurized includion of customer.h and feature.h.
                   Featured out FEATURE_QUERY pkt/response.
06/11/09   JV      Added command code for Q6 stress test
05/07/09   vk      moved declerations into diagdiag.h that were being referred from 
                   there
01/14/09   vg      Fixed type added during the public and private headerfile
                   split. 
10/03/08   vg      Updated code to use PACK() vs. PACKED
05/17/05   as      Added Dual processor Diag support.
06/15/04   gr      Added support for getting and setting the event mask.
05/18/04   as      Removed support for DIAG_USER_CMD_F & DIAG_PERM_USER_CMD_F
01/07/03   djm     add RPC support for WCDMA_PLT
08/20/02   lad     Moved DIAG_DLOAD_F packet def to dloaddiag.h.
                   Moved DIAG_SERIAL_CHG_F definition to diagcomm_sio.c.
01/28/02   as      Support for DIAG_LOG_ON_DEMAND_F (log on demand).
09/18/01   jal     Support for DIAG_CONTROL_F (mode reset/offline)
08/20/01   jal     Support for Diag packet: DIAG_TS_F (timestamp),
                   DIAG_SPC_F (service programming code), DIAG_DLOAD_F
		   (start downloader), DIAG_OUTP_F/DIAG_OUTPW_F (IO
		   port byte/word output), DIAG_INP_F/DIAG_INPW_F (IO
		   port byte/word input) 
06/27/01   lad     Use of constants in message response packet.
                   Added packet definition for DIAG_LOG_CONFIG_F.  This
                   replaces extended logmask processing.
                   Cleaned up DIAG_STREAMING_CONFIG_F and added subcmd to
                   get diagbuf size.
04/06/01   lad     Added packet definitions for the following:
                   Peek/Poke
                   DIAG_STREAMING_CONFIG_F
                   Debug messages
                   Log services
                   Event services
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "feature.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "log_codes.h"
#include "qw.h"
#include "diag.h"
#include "diagdiag.h"
#include "osal.h"
#include "queue.h"
#include "diagi.h"

/*===========================================================================

PACKET   DIAG_BAD_CMD_F
         DIAG_BAD_PARM_F
         DIAG_BAD_LEN_F
         DIAG_BAD_VOC_F
         DIAG_BAD_MODE_F
         DIAG_BAD_SPC_MODE_F

PURPOSE  Sent by DMSS when it detects an erroneous packet from DM. Errors
         include command code out of bounds, bad length...  Includes the
         first DIAG_MAX_ERR bytes of the offending input packet.
         Also includes when an nv_read/write is attempted before the correct
         SPC has been entered.

============================================================================*/

/* Prototype */
void diagdiag_init (void);
void diagdiag_tbl_init (void);

#define QSHRINK_4_DATABASE_GUID_LENGTH 16

/*! @brief
    This Macro provides the compiler with branch prediction information.

    Calling DIAG_LIKELY(expr) informs the compiler that it is expected that
    expr == 1 <NOTE: expr has to be equal to 1 not greater than 1>.

    Calling DIAG_UNLIKELY(expr) informs the compiler that it is expected
    that expr is going to be 0, hence dont perform prefetching of instructions
    inside.

    @usage
    Use "if (DIAG_LIKELY(a==b))" {} if expression '(a==b)' is expected to be
    TRUE most of the time else use "if (DIAG_UNLIKELY(a==b)"
*/

#define DIAG_LIKELY(x) __builtin_expect((x), 1)
#define DIAG_UNLIKELY(x) __builtin_expect((x), 0)

/* -------------------------------------------------------------------------
** Packet Definitions
** ------------------------------------------------------------------------- */                           


/*===========================================================================

PACKET   DIAG_DIAG_VER_F
PURPOSE  Sent by DM to request the version of the diag

===========================================================================*/
DIAGPKT_REQ_DEFINE(DIAG_DIAG_VER_F)
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_DIAG_VER_F)
  word ver;                    /* diag version */
DIAGPKT_RSP_END
               
/*===========================================================================

PACKET   DIAG_PASSWORD_F
PURPOSE  Sent by external device to enter the Security Password,
         to then allow operations protected by security.  This response 
         indicates whether the correct Password was given or not.

NOTE     If the incorrect password is entered, DIAG will POWER DOWN
         the phone.

===========================================================================*/
#define DIAG_PASSWORD_SIZE 8
DIAGPKT_REQ_DEFINE(DIAG_PASSWORD_F)
  byte password[DIAG_PASSWORD_SIZE];  /* The security password */
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_PASSWORD_F)
  boolean password_ok;        /* TRUE if Security Password entered correctly */
DIAGPKT_RSP_END

/* Logging Services */

/*===========================================================================

PACKET   DIAG_LOG_CONFIG_F
PURPOSE  Sent by the DM to set the equipment ID logging mask in the DMSS.  
         This is necessary to use logging services with MS Equip ID != 1.

!!!Note that the log mask is now sanely ordered LSB to MSB using little endian 
32-bit integer arrays.  This is not the same way the mask was done in 
DIAG_EXT_LOGMASK_F.

TERMINOLOGY:
  'equipment ID' - the 4-bit equipment identifier
  'item ID' - the 12-bit ID that specifies the log item within this equip ID
  'code' - the entire 16-bit log code (contains both equip and item ID)

===========================================================================*/
typedef enum {
  LOG_CONFIG_DISABLE_OP = 0,
  LOG_CONFIG_RETRIEVE_ID_RANGES_OP = 1, 
  LOG_CONFIG_RETRIEVE_VALID_MASK_OP = 2,
  LOG_CONFIG_SET_MASK_OP = 3,
  LOG_CONFIG_GET_LOGMASK_OP = 4
} log_config_command_ops_enum_type;

/* Logging config return status types. 
 * (*) denotes applicable to all commands 
 */
typedef enum {
  LOG_CONFIG_SUCCESS_S = 0,           /* Operation Sucessful */
  LOG_CONFIG_INVALID_EQUIP_ID_S = 1,  /* (*) Specified invalid equipment ID */
  LOG_CONFIG_NO_VALID_MASK_S = 2,     /* Valid mask not available for this ID */
  LOG_CONFIG_INVALID_STREAM_ID = 3,   /* Invalid Stream ID specified */
  LOG_CONFIG_INVALID_PRESET_ID = 4    /* Invalid Preset ID specified */
} log_config_status_enum_type;

/* QPST COOKIE COMMAND ERROR CODES */

#ifdef DIAG_FEATURE_QPST_COOKIE
    typedef enum {
        COOKIE_SAVE_SUCCESS = 0,
        COOKIE_READ_SUCCESS = 0,
        COOKIE_SAVE_TO_IRAM_ERROR = 1,
        COOKIE_SAVE_TO_IMEM_ERROR = 1,
        COOKIE_READ_ERROR = 1,
        COOKIE_SAVE_TO_FS_ERROR = 2        
    } qpst_cookie_cmd_err_code_enum_type;
#endif

/* Operation data */
/* DISABLE OP: LOG_CONFIG_DISAPLE_OP -no no supporting data */

/* These member structures are not packed intentionally.  Each data member will 
 * align on a 32-bit boundary.
 */
typedef PACK(struct) {
  uint32 equip_id;

  uint32 last_item;

} log_config_range_type;

typedef PACK(struct) {
  log_config_range_type code_range; /* range of log codes */
  
  byte mask[1]; /* Array of 8 bit masks of size (num_bits + 7) / 8 */
} log_config_mask_type;

/* ID_RANGE_OP  response type */
typedef PACK(struct) {
  uint32 last_item[16]; /* The last item for each of the 16 equip IDs */ 
} log_config_ranges_rsp_type;

/* VALID_MASK_OP request type */
typedef PACK(struct) {
  uint32 equip_id;
} log_config_valid_mask_req_type;

/* VALID_MASK_OP response type */
typedef log_config_mask_type log_config_valid_mask_rsp_type;

/* SET_MASK_OP request type */
typedef log_config_mask_type log_config_set_mask_req_type;

/* GET_MASK_OP response type */
typedef log_config_mask_type log_config_get_mask_rsp_type;

/* SET_MASK_OP response type */
typedef log_config_mask_type log_config_set_mask_rsp_type;

/* This is not packed.  We use uint32 which is always aligned */
typedef PACK(union) {
  /* LOG_CONFIG_DISABLE_OP */
  /* no additional data */

  /* LOG_CONFIG_RETRIEVE_ID_RANGES_OP */
  /* no additional data */
  
  /* LOG_CONFIG_RETRIEVE_VALID_MASK_OP */
  log_config_valid_mask_req_type valid_mask;

  /* LOG_CONFIG_SET_MASK_OP */
  log_config_set_mask_req_type set_mask;

  /* LOG_CONFIG_GET_MASK_OP */
  /* no additional data */

} log_config_op_req_type;

typedef PACK(union) {
  /* LOG_CONFIG_DISABLE_OP */
  /* no additional data */

  /* LOG_CONFIG_RETRIEVE_ID_RANGES_OP */
  log_config_ranges_rsp_type ranges;
  
  /* LOG_CONFIG_RETRIEVE_VALID_MASK_OP */
  log_config_valid_mask_rsp_type valid_mask;

  /* LOG_CONFIG_SET_MASK_OP */
  log_config_set_mask_rsp_type set_mask;

  /* LOG_CONFIG_GET_MASK_OP */
  log_config_get_mask_rsp_type get_mask;

} log_config_op_rsp_type;

#ifdef FEATURE_SSM
typedef PACK (struct)
{
  uint32 start;
  uint32 end;
  uint32 permission;
}
diag_mask_ssm_buffer_type;

#endif

DIAGPKT_REQ_DEFINE(DIAG_LOG_CONFIG_F)

  byte pad[3]; /* Force following items to be on 32-bit boundary */

  uint32 operation;  /* See log_config_command_ops_enum_type */

  uint32 op_data[1]; /* Pointer to operation data */

DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_LOG_CONFIG_F)

  byte pad[3]; /* Force following items to be on 32-bit boundary */

  uint32 operation;  /* See log_config_command_ops_enum_type */

  uint32 status;

  uint32 op_data[1]; /* Pointer to operation data */

DIAGPKT_RSP_END


/* Number of bits in a log mask.
*/
#define DIAG_EXT_LOGMASK_NUM_BITS (LOG_1X_LAST_C & 0x0FFF)

/*Status for mask response*/
#define SUCCESSFUL 0

/* Max # of bytes in a valid log mask.
*/
#define DIAG_EXT_LOGMASK_NUM_BYTES ((DIAG_EXT_LOGMASK_NUM_BITS / 8) + 1)

/*===========================================================================

PACKET   DIAG_CMD_EXT_RSP_TYPE
PURPOSE  Header used for other debug msg, log, and event packets to add
         additional information

===========================================================================*/
typedef PACK(struct)
{
  uint8 cmd_code;       /* cmd code for extended events */
  uint8 version;        /* version of the extended pkt; currently we support version 1 */
  uint16 proc_id;       /* processor ID where the msg is being generated */
  uint32 id;            /* ID passed in by user */
}
diag_cmd_ext_v1_type;

/*===========================================================================

PACKET   DIAG_EVENT_EXT_RSP_TYPE
PURPOSE  Extended event type with support for version and ID fields.

===========================================================================*/
typedef PACK(struct)
{
  diag_cmd_ext_v1_type ext_hdr; /* Extended header */
  uint8 event_cmd_code; /* event cmd code */
  uint16 length;        /* length of the events */
  uint8 events[1];      /* starting address of events */
}
diag_event_ext_rsp_type;

/*===========================================================================

PACKET   DIAG_EXT_LOGMASK_F
PURPOSE  Sent by the DM to set the logging mask in the DMSS.  This is
         necessary for logmasks > 32 bits.

===========================================================================*/
DIAGPKT_REQ_DEFINE(DIAG_EXT_LOGMASK_F)
  word  num_bits;                 /* Number of valid bits */
  byte  mask[DIAG_EXT_LOGMASK_NUM_BYTES]; /* mask to use          */
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_EXT_LOGMASK_F)
  word  num_valid_bits;                     /* Number of valid bits    */
  byte  valid_mask[DIAG_EXT_LOGMASK_NUM_BYTES]; /* mask of valid log codes */
DIAGPKT_RSP_END

/*===========================================================================

PACKET   DIAG_LOG_F
PURPOSE  Encapsulates a log record.

===========================================================================*/

typedef struct
{
  uint8 cmd_code;
  uint8 more;   /* Indicates how many log entries, not including the one 
                   returned with this packet, are queued up in the Mobile
                   Station.  If DIAG_DIAGVER >= 8, this should be set to 0 */
  uint16 len;   /* Indicates the length, in bytes, of the following log entry */
  uint8 log[1]; /* Contains the log entry data. */
}
diag_log_rsp_type;


/* -------------------------------------------------------------------------
** Legacy (but still supported) packet definitions for logging services.
** ------------------------------------------------------------------------- */
/*===========================================================================

PACKET   DIAG_LOGMASK_F
PURPOSE  Sent by the DM to set the 32-bit logging mask in the DMSS.
         Note: this is the legacy logging mask format.

===========================================================================*/
DIAGPKT_REQ_DEFINE(DIAG_LOGMASK_F)
  uint32 mask; /* 32-bit log mask  */
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_LOGMASK_F)
DIAGPKT_RSP_END


/*===========================================================================

PACKET   DIAG_FEATURE_QUERY_F
PURPOSE  Sent by external device to query the phone for a bit mask detailing
         which phone features are turned on.

===========================================================================*/
DIAGPKT_REQ_DEFINE(DIAG_FEATURE_QUERY_F)
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_FEATURE_QUERY_F)
  word feature_mask_size;                 /* Size of the following Mask */
  byte feature_mask[FEATURE_MASK_LENGTH]; /* Space for the largest possible 
                                             feature mask */
DIAGPKT_RSP_END


/*===========================================================================

PACKET   DIAG_EVENT_REPORT_F
PURPOSE  Sent by the DM to configure static event reporting in the DMSS.

===========================================================================*/
/*--------------------------------------
  Special bit flags in the event ID.
--------------------------------------*/
#define EVENT_PAY_LENGTH   0x3
#define EVENT_PAY_TWO_BYTE 0x2
#define EVENT_PAY_ONE_BYTE 0x1
#define EVENT_PAY_NONE     0x0

/* Bitfields may not be ANSI, but all our compilers
** recognize it and *should* optimize it.
** Not that bit-packed structures are only as long as they need to be.
** Even though we call it uint32, it is a 16 bit structure.
*/
typedef struct
{
  uint16 id              : 12;
  uint16 reserved        : 1;
  uint16 payload_len     : 2; /* payload length (0, 1, 2, see payload) */
  uint16 time_trunc_flag : 1;
} event_id_type;

typedef PACK(struct)
{
  uint16 id; /* event_id_type id; */
  qword ts;

} event_type;

/* Used in lieu of event_type if 'time_trunc_flag' is set in event_id_type */
typedef PACK(struct)
{
  uint16 id; /* event_id_type id; */
  uint16 trunc_ts;
} event_trunc_type;

/* The event payload follows the event_type structure */
typedef struct
{
  uint8 length;
  uint8 payload[1]; /* 'length' bytes */
} event_payload_type;

typedef PACK(struct)
{
  uint16 id; /* event_id_type id; */
  qword ts;
  uint32 drop_cnt;
} event_drop_type;

typedef struct
{
  event_id_type id;
  uint16 ts;
  uint32 drop_cnt;
} event_drop_trunc_type;

typedef struct
{
  uint8 cmd_code;
  uint8 enable;
//  uint16  watermark;   /* Maximum size (in bytes) of a event report         */ 
//  uint16  stale_timer; /* Time (in ms) to allow event buffer to accumulate  */

} event_cfg_req_type;

typedef PACK(struct)
{
  uint8  cmd_code;
  uint16 length;

} event_cfg_rsp_type;

typedef PACK(struct)
{
  uint8  cmd_code;
  uint16 length;    /* Number of bytes to follow */
  uint8  events[1]; /* Series of 'event_type' structures, 'length' bytes long */
} event_rpt_type;

/* DUAL-MASK SUPPORT STRUCTURES  */

/* DUAL MASK EVENT STRUCTURES */

typedef PACK(struct)
{
  byte cmd_code;
  byte subsys_id;
  uint16 subsys_cmd_code;
  byte cmd_version;
  byte subcmd;
  PACK(union)
  {
    byte stream_id;
    uint8 preset_id;
  } id;
} event_ext_rpt_ctrl_type;

typedef PACK(struct)
{
  event_ext_rpt_ctrl_type header;
  byte rsvd;
  uint16 numbits_to_set;
  byte mask[1];
} event_ext_rpt_ctrl_set_mask_req_type;

typedef PACK(struct)
{
  event_ext_rpt_ctrl_type header;
  byte config;
} event_ext_rpt_ctrl_set_all_req_type;

typedef PACK(struct)
{
  event_ext_rpt_ctrl_type header;
  byte error_code;
  uint16 numbits;
  byte mask[1];
} event_ext_rpt_ctrl_rsp_type;

typedef PACK(struct)
{
  event_ext_rpt_ctrl_type header;
  byte config;
  byte error_code;
} event_ext_set_all_rsp_type;

/* DUAL MASK LOG STRUCTURES */

typedef PACK(struct)
{
  byte cmd_code;
  byte subsys_id;
  uint16 subsys_cmd_code;
  byte cmd_version;
  byte opcode;
} ext_config_header;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte rsvd;
} log_ext_config_disable_op_req;

typedef ext_config_header log_ext_config_id_ranges_req;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte rsvd;
    log_config_range_type code_range;
    byte mask[1];
} log_ext_config_set_mask_op_req;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte rsvd;
    uint32 equip_id;
} log_ext_config_get_mask_op_req;

typedef PACK(union)
{
    ext_config_header header;
    log_ext_config_disable_op_req disable_op;
    log_ext_config_id_ranges_req id_ranges;
    log_ext_config_set_mask_op_req set_mask;
    log_ext_config_get_mask_op_req get_mask;
} log_ext_config_req_type;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte status;
} log_ext_config_disable_op_rsp;

typedef PACK(struct)
{
   diagpkt_subsys_header_type header;
   uint32 equip_id;
   byte stream_id;
} log_get_local_mask_req_type;

typedef PACK(struct) 
{
   diagpkt_subsys_header_type header;
   uint32 equip_id;
   byte stream_id;
   uint8 status;
   byte log_mask[1];
} log_get_local_mask_rsp_type;

typedef PACK(struct)
{
    ext_config_header header;
    byte status;
    byte rsvd;
    uint32 last_item[16];
} log_ext_config_id_ranges_rsp;

typedef  PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte status;
    log_config_range_type code_range;
    byte mask[1];
} log_ext_config_set_mask_rsp;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte status;
    log_config_range_type code_range;
    byte mask[1];
} log_ext_config_get_mask_rsp;

typedef PACK(union)
{
    ext_config_header header;
    log_ext_config_disable_op_rsp disable_op;
    log_ext_config_id_ranges_rsp id_ranges;
    log_ext_config_set_mask_rsp set_mask;
    log_ext_config_get_mask_rsp get_mask;
} log_ext_config_rsp_type;

/* DUAL MASK MSG STRUCTURES */

typedef ext_config_header msg_get_ssid_ranges_adv_req_type;

typedef PACK(struct)
{
  ext_config_header header;
  byte status;
  byte rsvd;
  uint32 range_cnt;
  PACK(struct)
  {
    uint16 ssid_first;
    uint16 ssid_last;
  }
  ssid_ranges[1];   
} msg_get_ssid_ranges_adv_rsp_type;

typedef PACK(struct)
{
    ext_config_header header;
    uint16 ssid_first;
    uint16 ssid_last;
} msg_get_mask_adv_req_type;

typedef PACK(struct)
{
    ext_config_header header;
    uint16 ssid_first;
    uint16 ssid_last;
    byte status;
    byte rsvd;
    uint32 bld_mask[1];
} msg_get_mask_adv_rsp_type;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte rsvd;
    uint16 ssid_first;
    uint16 ssid_last;
} msg_get_rt_mask_adv_req_type;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte status;
    uint16 ssid_first;
    uint16 ssid_last;
    uint32 rt_mask[1];
} msg_get_rt_mask_adv_rsp_type;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte rsvd;
    uint16 ssid_first;
    uint16 ssid_last;
    uint32 rt_mask[1];
} msg_set_rt_mask_adv_req_type;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte status;
    uint16 ssid_first;
    uint16 ssid_last;
    uint32 rt_mask[1];
} msg_set_rt_mask_adv_rsp_type;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte rsvd;
    uint32 rt_mask;
} msg_set_all_masks_adv_req_type;

typedef PACK(struct)
{
    ext_config_header header;
    PACK(union)
    {
      byte stream_id;
      uint8 preset_id;
    } id;
    byte status;
    uint32 rt_mask;
} msg_set_all_masks_adv_rsp_type;

/* End DUAL-MASK structures */

typedef PACK(struct)
{
  diagpkt_subsys_header_type header; /* Sub System header */
  uint16 ssid_start;	/*!< Start of subsystem ID range */
  uint16 ssid_end;      /*!< End of subsystem ID range */
  byte stream_id;     /*Stream id*/
}
msg_get_local_mask_req_type;

typedef struct
{
  diagpkt_subsys_header_type header; /* Sub System header */
  uint16 ssid_start;	/*!< Start of subsystem ID range */
  uint16 ssid_end;      /*!< End of subsystem ID range */
   byte stream_id;     /*Stream id*/
  uint8 status;
  uint16 padding;
  uint32 msg_mask[1];
}
 msg_get_local_mask_rsp_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type header; /* Sub System header */
}
msg_get_local_ssid_range_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;  /* Sub System header */
  uint8 status;			/*!< Status of operation */
  uint32 range_cnt;		/*!< number of Subsystem ID ranges that follow, in ssid_ranges[] */

  struct
  {
    uint16 ssid_first;		/*!< First Subsystem ID for range */
    uint16 ssid_last;		/*!< Last Subsystem ID for range */
  }
  ssid_ranges[1];           /*!< SSID range array has range_cnt number of members */
}
msg_get_local_ssid_range_rsp_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;  /* Sub System header */
}
log_get_local_equipid_range_req_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;  /* Sub System header */
  byte status;
  uint32 equipid_ranges[16];
}
log_get_local_equipid_range_rsp_type;

/*=====Diag Drop Debug Event Related ===*/
#define DIAG_GUEST_OS_PD 0

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  uint8 per_threshold_drop;
  uint8 per_step_drop;
}diag_drop_event_param_req_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  uint8 per_threshold_drop;
  uint8 per_step_drop;
}diag_drop_event_param_rsp_type;

typedef enum
{
  DIAG_DROP = 1,
  DIAG_FLOW_CTRL
}diag_drop_event_report_src;

typedef PACK(struct) 
{
  uint32 drop_log_cnt;
  uint32 alloc_log_cnt;
  uint32 drop_f3_cnt;
  uint32 alloc_f3_cnt;
  uint32 flow_ctrl_cnt_smd;
  uint32 flow_ctrl_cnt_sio;
  uint32 flow_ctrl_cnt_dci;
  uint8 drop_cnt_per;
  uint16 proc_id; /* Proc ID*/
  uint32 pd_id; /* Process Domain ID */
  diag_drop_event_report_src event_source; /* Condition in which the event was reported*/
} EVENT_DIAG_DROP_DBG_PAYLOAD_INFO_type;

/*===========================================================================

PACKET   DIAG_STREAMING_CONFIG_F
PURPOSE  Sent by the DM to configure and tweak streaming diag output services.

===========================================================================*/
typedef enum {
  DIAG_READ_NICE_C  = 0, /* Read "Nice" values for LOG and MSG services */
  DIAG_WRITE_NICE_C = 1, /* Write "Nice" values for LOG and MSG services */
  DIAG_READ_PRI_C   = 2, /* Read "priority" values for LOG and MSG services */
  DIAG_WRITE_PRI_C  = 3, /* Write "priority" values for LOG and MSG services */
  DIAG_BUF_SIZE_C   = 4  /* Return size (in bytes) of output buffer */

} diag_streaming_config_subcommand_enum_type;

typedef PACK(struct) {
    uint16 code; /* MSG_LVL value or Log code */

    int16  val;
  
} diagpkt_streaming_config_entry_type;

typedef PACK(struct) {

  /* Number of entries in the following array */
  uint8 num_entries;

  /* # of log codes + 5 Message Levels is the number max $ in this array */
  diagpkt_streaming_config_entry_type entry[1];

} diagpkt_streaming_config_entry_list_type;


/*==========================================================================

PACKET   DIAG_TS_F

PURPOSE  Sent from the DM to the DMSS to request the IS-95/IS-2000 time.

============================================================================*/
DIAGPKT_REQ_DEFINE(DIAG_TS_F)

  /* It's just the command code */
  
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_TS_F)
  
  qword ts;        /* Time Stamp */

DIAGPKT_RSP_END


/*==========================================================================

PACKET   DIAG_OUTP_F

PURPOSE  Request sent from the DM to the DMSS to send a byte to an
         IO port

============================================================================*/
DIAGPKT_REQ_DEFINE( DIAG_OUTP_F )

  word port;                   /* number of port to output to */
  byte data;                   /* data to write to port */

DIAGPKT_REQ_END

DIAGPKT_DEFINE_RSP_AS_REQ( DIAG_OUTP_F )
  

/*==========================================================================

PACKET   DIAG_OUTPW_F

PURPOSE  Request sent from the DM to the DMSS to send a 16-bit word to an
         IO port

============================================================================*/
DIAGPKT_REQ_DEFINE( DIAG_OUTPW_F )

  word port;                   /* number of port to output to */
  word data;                   /* data to write to port */

DIAGPKT_REQ_END

DIAGPKT_DEFINE_RSP_AS_REQ( DIAG_OUTPW_F )

  

/*==========================================================================

PACKET   DIAG_INP_F

PURPOSE  Request sent from the DM to the DMSS to read a byte to an
         IO port

============================================================================*/
DIAGPKT_REQ_DEFINE( DIAG_INP_F )

  word port;                   /* number of port to output to */

DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE( DIAG_INP_F )

  word port;                   /* number of port to output to */
  byte data;                   /* data to write to port */

DIAGPKT_REQ_END


/*==========================================================================

PACKET   DIAG_INPW_F

PURPOSE  Request sent from the DM to the DMSS to read a 16-bit word from an
         IO port

============================================================================*/
DIAGPKT_REQ_DEFINE( DIAG_INPW_F )

  word port;                   /* number of port to output to */

DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE( DIAG_INPW_F )

  word port;                   /* number of port to output to */
  word data;                   /* data to write to port */

DIAGPKT_REQ_END



/*==========================================================================

PACKET   DIAG_LOG_ON_DEMAND_F

PURPOSE  Request sent from the user to register a function pointer and 
         log_code with the diagnostic service for every log that needs 
         logging on demand support.
         
============================================================================*/
DIAGPKT_REQ_DEFINE( DIAG_LOG_ON_DEMAND_F )

   uint16 log_code;             /* The log_code to be sent */
   
DIAGPKT_REQ_END


DIAGPKT_RSP_DEFINE( DIAG_LOG_ON_DEMAND_F )

  uint16 log_code;             /* The log_code sent */
  uint8  status;               /* status returned from the function pointer */

DIAGPKT_RSP_END

/* Diagnostic extensions */
/*
** The maximum number of properties and callback functions. These are not
** used to determine the size of any data structure; they are used merely
** to guard against infinite loops caused by corruption of the callback
** and properties tables.
*/
#define DIAG_MAX_NUM_PROPS 20
#define DIAG_MAX_NUM_FUNCS 20

typedef void (*diag_cb_func_type) (
                                   unsigned char  *data_ptr,
                                   unsigned short  data_len,
                                   unsigned char  *rsp_ptr,
                                   unsigned short *rsp_len_ptr
);

typedef struct
{
   char             *name;
   diag_cb_func_type address;
} diag_cb_struct_type;

typedef struct
{
   char *name;
   void *address;
} diag_prop_struct_type;

typedef enum
{
  DIAG_EXTN_INVALID_GUID = 1,
  DIAG_EXTN_INVALID_SIZE,
  DIAG_EXTN_INVALID_ADDRESS,
  DIAG_EXTN_INVALID_NAME,
  DIAG_EXTN_INVALID_DATA
} diag_extn_err_type;

/* Need to use PACKED not PACK() */
PACKED typedef unsigned long diag_guid_type[ 4 ];

/*===========================================================================

PACKET   DIAG_GET_GUID_F

PURPOSE  Sent by the DM to retrieve the GUID (globally unique identifier)
         for the current build. This is stored during the build process.

===========================================================================*/
DIAGPKT_REQ_DEFINE(DIAG_GET_GUID_F)
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE(DIAG_GET_GUID_F)
  diag_guid_type guid;               /* Globally unique identifier  */
DIAGPKT_RSP_END

#define DIAG_MAX_PROPERTY_NAME_SIZE 40

// Required DIAG Health Definitions
#define MAX_VALUE_UINT16                65525U
#define MAX_VALUE_UINT32                4294967295UL   // Overflow
#define DIAGBUF_ALLOC_REQUEST_OTHER     0x0
#define DIAGBUF_ALLOC_REQUEST_LOG       0x1
#define DIAGBUF_ALLOC_REQUEST_MSG       0x2

/*-------------------------------------------------------------------------------------------------------
  Command Codes between the Diagnostic Monitor and the mobile. These command 
  codes are used for stress testing.
  
  These are 2-byte subsystem cmd codes, therefore the max value can be 65535.
---------------------------------------------------------------------------------------------------------*/
#define DIAGDIAG_SUBSYS_ID_APP_OFFSET          0x0200 /* 512 */
#define DIAGDIAG_SUBSYS_ID_MODEM_OFFSET        0x0800 /* 2048 */
#define DIAGDIAG_SUBSYS_ID_Q6_OFFSET           0x0E00 /* 3584 */
#define DIAGDIAG_SUBSYS_ID_RIVA_OFFSET         0x1400 /* 5120 */

#define DIAGDIAG_START_STRESS_TEST_MODEM_F     0x0000 /* 0 */
#define DIAGDIAG_CMD_REQUEST_F                 0x0001 /* 1 */
#define DIAGDIAG_ADD_EVENT_LISTENER_F          0x0002 /* 2 */
#define DIAGDIAG_REMOVE_EVENT_LISTENER_F       0x0003 /* 3 */
#define DIAGDIAG_ADD_LOG_LISTENER_F            0x0004 /* 4 */
#define DIAGDIAG_REMOVE_LOG_LISTENER_F         0x0005 /* 5 */
#define DIAGDIAG_START_STRESS_TEST_APPS_F      0x0006 /* 6 */
#define DIAGDIAG_START_STRESS_TEST_QDSP_F      0x0007 /* 7 */
#define DIAGDIAG_START_STRESS_TEST_RIVA_F      0x0008 /* 8 */
#define DIAGDIAG_ADD_F3_LISTENER_F             0x0009 /* 9 */
#define DIAGDIAG_REMOVE_F3_LISTENER_F          0x000A /* 10 */
#define DIAGDIAG_ADD_EVENT_EXT_LISTENER_F      0x000B /* 11 */        
#define DIAGDIAG_REMOVE_EVENT_EXT_LISTENER_F   0x000C /* 12 */                
#define DIAGDIAG_ADD_LOG_EXT_LISTENER_F        0x000D /* 13 */           
#define DIAGDIAG_REMOVE_LOG_EXT_LISTENER_F     0x000E /* 14 */  
#define DIAGDIAG_ADD_F3_EXT_LISTENER_F         0x000F /* 15 */        
#define DIAGDIAG_REMOVE_F3_EXT_LISTENER_F      0x0010 /* 16 */
#define DIAGDIAG_REMOVE_EVENT_EXTP_LISTENER_F  0x0011 /* 17 */     
#define DIAGDIAG_REMOVE_LOG_EXTP_LISTENER_F    0x0012 /* 18 */

// Processor Loopback Command Codes
#define DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_APP        0x0028 /* 40 */
#define DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_MODEM      0x0029 /* 41 */
#define DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_Q6         0x002A /* 42 */

// DIAG Health on APPs
#define DIAGDIAG_RESET_DROP_COUNT_LOG_APP      0x002B /* 43 */      
#define DIAGDIAG_GET_DROP_COUNT_LOG_APP        0x002C /* 44 */ 
#define DIAGDIAG_RESET_DROP_COUNT_EVENT_APP    0x002D /* 45 */
#define DIAGDIAG_GET_DROP_COUNT_EVENT_APP      0x002E /* 46 */
#define DIAGDIAG_RESET_DROP_COUNT_F3_APP       0x002F /* 47 */
#define DIAGDIAG_GET_DROP_COUNT_F3_APP         0x0030 /* 48 */
#define DIAGDIAG_GET_ALLOC_COUNT_LOG_APP       0x0031 /* 49 */
#define DIAGDIAG_GET_ALLOC_COUNT_EVENT_APP     0x0032 /* 50 */
#define DIAGDIAG_GET_ALLOC_COUNT_F3_APP        0x0033 /* 51 */
#define DIAGDIAG_RESET_DROP_COUNT_DELAY_APP    0x0046 /* 70 */
#define DIAGDIAG_GET_DROP_COUNT_DELAY_APP      0x0047 /* 71 */
#define DIAGDIAG_GET_ALLOC_COUNT_DELAY_APP     0x0048 /* 72 */
#define DIAGDIAG_GET_FLOW_CTRL_COUNT_APP       (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0000) /* 0x0200 =  512 */
#define DIAGDIAG_RESET_FLOW_CTRL_COUNT_APP     (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0001) /* 0x0201 =  513 */
#define DIAGDIAG_GET_DSM_CHAINED_COUNT_APP     (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0002) /* 0x0202 =  514 */
#define DIAGDIAG_RESET_DSM_CHAINED_COUNT_APP   (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0003) /* 0x0203 =  515 */
#define DIAGDIAG_QUERY_ENABLE_APP              (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0004) /* 0x0204 =  516 */
#define DIAGDIAG_TX_MODE_CONFIG_APP            (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0005) /* 0x0205 =  517 */
#define DIAGDIAG_GET_CMD_REG_TBL_APPS          (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0006) /* 0x0206 =  518 */
#define DIAGDIAG_EVENT_MASK_RETRIEVAL_APP      (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0007) /* 0x0207 =  519 */
#define DIAGDIAG_MSG_MASK_RETRIEVAL_APP        (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0008) /* 0x0208 =  520 */
#define DIAGDIAG_LOG_MASK_RETRIEVAL_APP        (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0009) /* 0x0209 =  521 */
#define DIAGDIAG_RETRIEVE_SSID_RANGE_APP       (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x000A) /* 0x020A =  522 */
#define DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_APP   (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x000B) /* 0x020B =  523 */
#define DIAGDIAG_SET_PRESET_ID_APP             (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x000C) /* 0x020C =  524 */
#define DIAGDIAG_GET_PRESET_ID_APP             (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x000D) /* 0x020D =  525 */
#define DIAG_F3_TRACE_SET_CONFIG_APP           (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x000E) /* 0x020E =  526 */
#define DIAGDIAG_QSHRINK_4_FILELIST_APP        (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x000F) /* 0x020F =  527 */
#define DIAGDIAG_QSHRINK_4_OPEN_APP            (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0010) /* 0x0210 =  528 */
#define DIAGDIAG_QSHRINK_4_READ_APP            (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0011) /* 0x0211 =  529 */
#define DIAGDIAG_QSHRINK_4_CLOSE_APP           (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0012) /* 0x0212 =  530 */
#define DIAGDIAG_QSHRINK_4_DATABASE_ID_APP     (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0013) /* 0x0213 =  531 */
#define DIAGDIAG_STM_CONTROL                   (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0014) /* 0x0214 =  532 */
#define DIAGDIAG_STRESS_TEST_DELAYED_RSP_APP   (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0015) /* 0x0215 =  533 */
#define DIAGDIAG_DROP_THRESHOLD_CONFIG_APP     (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0016) /*0x0216 = 534*/
#define DIAGDIAG_GET_DROP_PER_APP              (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0017) /*0x0217 = 535*/
#define DIAGDIAG_HDLC_DISABLE_APP              (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0018) /* 0x0218 =  536 */
#define DIAGDIAG_FLASH_LOGGING_TEST            (DIAGDIAG_SUBSYS_ID_APP_OFFSET + 0x0033) /* 0x0233 =  563 */

// DIAG Health on MODEM                                                
#define DIAGDIAG_RESET_DROP_COUNT_LOG_MODEM    0x0034 /* 52 */
#define DIAGDIAG_GET_DROP_COUNT_LOG_MODEM      0x0035 /* 53 */
#define DIAGDIAG_RESET_DROP_COUNT_EVENT_MODEM  0x0036 /* 54 */
#define DIAGDIAG_GET_DROP_COUNT_EVENT_MODEM    0x0037 /* 55 */
#define DIAGDIAG_RESET_DROP_COUNT_F3_MODEM     0x0038 /* 56 */
#define DIAGDIAG_GET_DROP_COUNT_F3_MODEM       0x0039 /* 57 */
#define DIAGDIAG_GET_ALLOC_COUNT_LOG_MODEM     0x003A /* 58 */
#define DIAGDIAG_GET_ALLOC_COUNT_EVENT_MODEM   0x003B /* 59 */
#define DIAGDIAG_GET_ALLOC_COUNT_F3_MODEM      0x003C /* 60 */
#define DIAGDIAG_RESET_DROP_COUNT_DELAY_MODEM  0x0050 /* 80 */
#define DIAGDIAG_GET_DROP_COUNT_DELAY_MODEM    0x0051 /* 81 */
#define DIAGDIAG_GET_ALLOC_COUNT_DELAY_MODEM   0x0052 /* 82 */
#define DIAGDIAG_GET_FLOW_CTRL_COUNT_MODEM     (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0000) /* 0x0800 = 2048 */
#define DIAGDIAG_RESET_FLOW_CTRL_COUNT_MODEM   (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0001) /* 0x0801 = 2049 */
#define DIAGDIAG_GET_DSM_CHAINED_COUNT_MODEM   (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0002) /* 0x0802 = 2050 */
#define DIAGDIAG_RESET_DSM_CHAINED_COUNT_MODEM (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0003) /* 0x0803 = 2051 */
#define DIAGDIAG_QUERY_ENABLE_MODEM            (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0004) /* 0x0804 = 2052 */
#define DIAGDIAG_EVENT_MASK_RETRIEVAL_MODEM    (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0005) /* 0x0805 = 2053 */
#define DIAGDIAG_MSG_MASK_RETRIEVAL_MODEM      (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0006) /* 0x0806 = 2054 */
#define DIAGDIAG_LOG_MASK_RETRIEVAL_MODEM      (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0007) /* 0x0807 = 2055 */
#define DIAGDIAG_RETRIEVE_SSID_RANGE_MODEM     (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0008) /* 0x0808 = 2056 */
#define DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_MODEM (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0009) /* 0x0809 = 2057 */
#define DIAGDIAG_GET_CMD_REG_TBL_MODEM         (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x000A) /* 0x080A = 2058 */
#define DIAG_F3_TRACE_SET_CONFIG_MODEM_F       (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x000B) /* 0x080B = 2059 */
#define DIAGDIAG_DROP_THRESHOLD_CONFIG_MODEM   (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x000E) /* 0x080E = 2062 */
#define DIAGDIAG_GET_DROP_PER_MODEM          (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x000F) /* 0x080F = 2063 */
#define DIAGDIAG_STRESS_TEST_DELAYED_RSP_MODEM (DIAGDIAG_SUBSYS_ID_MODEM_OFFSET + 0x0011) /* 0x0811 = 2065 */


// DIAG Health on QDSP6                                                
#define DIAGDIAG_RESET_DROP_COUNT_LOG_Q6       0x003D /* 61 */
#define DIAGDIAG_GET_DROP_COUNT_LOG_Q6         0x003E /* 62 */
#define DIAGDIAG_RESET_DROP_COUNT_EVENT_Q6     0x003F /* 63 */
#define DIAGDIAG_GET_DROP_COUNT_EVENT_Q6       0x0040 /* 64 */
#define DIAGDIAG_RESET_DROP_COUNT_F3_Q6        0x0041 /* 65 */
#define DIAGDIAG_GET_DROP_COUNT_F3_Q6          0x0042 /* 66 */
#define DIAGDIAG_GET_ALLOC_COUNT_LOG_Q6        0x0043 /* 67 */
#define DIAGDIAG_GET_ALLOC_COUNT_EVENT_Q6      0x0044 /* 68 */
#define DIAGDIAG_GET_ALLOC_COUNT_F3_Q6         0x0045 /* 69 */
#ifdef DIAG_FEATURE_QPST_COOKIE
    #define DIAG_SAVE_QPST_COOKIE              0x0053 /* 83 */ 
    #define DIAG_QUERY_QPST_COOKIE             0x0054 /* 84 */
#endif
#define DIAGDIAG_GET_MAX_REQ_PKT_LEN           0X0055 /* 85 */
#define DIAGDIAG_RESET_DROP_COUNT_DELAY_Q6     0x005A /* 90 */
#define DIAGDIAG_GET_DROP_COUNT_DELAY_Q6       0x005B /* 91 */
#define DIAGDIAG_GET_ALLOC_COUNT_DELAY_Q6      0x005C /* 92 */
#define DIAGDIAG_GET_FLOW_CTRL_COUNT_Q6        (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0000) /* 0x0E00 = 3584 */
#define DIAGDIAG_RESET_FLOW_CTRL_COUNT_Q6      (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0001) /* 0x0E01 = 3585 */
#define DIAGDIAG_GET_DSM_CHAINED_COUNT_Q6      (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0002) /* 0x0E02 = 3586 */
#define DIAGDIAG_RESET_DSM_CHAINED_COUNT_Q6    (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0003) /* 0x0E03 = 3587 */
#define DIAG_CHANGE_TRHESHOLD_Q6               (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0004) /* 0x0E04 = 3588 */
#define DIAGDIAG_GET_CMD_REG_TBL_Q6            (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0005) /* 0x0E05 = 3589 */
#define DIAGDIAG_EVENT_MASK_RETRIEVAL_Q6       (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0006) /* 0x0E06 = 3590 */
#define DIAGDIAG_MSG_MASK_RETRIEVAL_Q6         (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0007) /* 0x0E07 = 3591 */
#define DIAGDIAG_LOG_MASK_RETRIEVAL_Q6         (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0008) /* 0x0E08 = 3592 */
#define DIAGDIAG_RETRIEVE_SSID_RANGE_Q6        (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x0009) /* 0x0E09 = 3593 */
#define DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_Q6    (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x000A) /* 0x0E0A = 3594 */
#define DIAG_F3_TRACE_SET_CONFIG_Q6            (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x000B) /* 0x0E0B = 3595 */
#define DIAGDIAG_STRESS_TEST_DELAYED_RSP_Q6    (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x000D) /* 0x0E0D = 3597 */
#define DIAGDIAG_DROP_THRESHOLD_CONFIG_Q6      (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x000E) /* 0x0E0E = 3598 */
#define DIAGDIAG_GET_DROP_PER_Q6               (DIAGDIAG_SUBSYS_ID_Q6_OFFSET + 0x000F) /* 0x0E0F = 3599 */

/* Diag Health on RIVA */
#define DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_RIVA   (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0000) /* 0x1400 = 5120 */
#define DIAGDIAG_RESET_DROP_COUNT_LOG_RIVA          (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0001) /* 0x1401 = 5121 */
#define DIAGDIAG_GET_DROP_COUNT_LOG_RIVA            (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0002) /* 0x1402 = 5122 */
#define DIAGDIAG_RESET_DROP_COUNT_EVENT_RIVA        (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0003) /* 0x1403 = 5123 */
#define DIAGDIAG_GET_DROP_COUNT_EVENT_RIVA          (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0004) /* 0x1404 = 5124 */
#define DIAGDIAG_RESET_DROP_COUNT_F3_RIVA           (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0005) /* 0x1405 = 5125 */
#define DIAGDIAG_GET_DROP_COUNT_F3_RIVA             (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0006) /* 0x1406 = 5126 */
#define DIAGDIAG_GET_ALLOC_COUNT_LOG_RIVA           (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0007) /* 0x1407 = 5127 */
#define DIAGDIAG_GET_ALLOC_COUNT_EVENT_RIVA         (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0008) /* 0x1408 = 5128 */
#define DIAGDIAG_GET_ALLOC_COUNT_F3_RIVA            (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0009) /* 0x1409 = 5129 */
#define DIAGDIAG_RESET_DROP_COUNT_DELAY_RIVA        (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x000A) /* 0x140A = 5130 */
#define DIAGDIAG_GET_DROP_COUNT_DELAY_RIVA          (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x000B) /* 0x140B = 5131 */
#define DIAGDIAG_GET_ALLOC_COUNT_DELAY_RIVA         (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x000C) /* 0x140C = 5132 */
#define DIAGDIAG_GET_DSM_CHAINED_COUNT_RIVA         (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x000D) /* 0x140D = 5133 */
#define DIAGDIAG_RESET_DSM_CHAINED_COUNT_RIVA       (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x000E) /* 0x140E = 5134 */
#define DIAG_CHANGE_THRESHOLD_RIVA                  (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x000F) /* 0x140F = 5135 */
#define DIAGDIAG_GET_CMD_REG_TBL_RIVA               (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0010) /* 0x1410 = 5136 */
#define DIAGDIAG_EVENT_MASK_RETRIEVAL_RIVA          (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0011) /* 0x1411 = 5137 */
#define DIAGDIAG_MSG_MASK_RETRIEVAL_RIVA            (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0012) /* 0x1412 = 5138 */
#define DIAGDIAG_LOG_MASK_RETRIEVAL_RIVA            (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0013) /* 0x1413 = 5139 */
#define DIAGDIAG_RETRIEVE_SSID_RANGE_RIVA           (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0014) /* 0x1414 = 5140 */
#define DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_RIVA       (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0015) /* 0x1415 = 5141 */
#define DIAG_F3_TRACE_SET_CONFIG_RIVA_F             (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0016) /* 0x1416 = 5142 */
#define DIAGDIAG_STRESS_TEST_DELAYED_RSP_RIVA       (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0018) /* 0x1418 = 5144 */
#define DIAGDIAG_DROP_THRESHOLD_CONFIG_RIVA         (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x0019) /* 0x1419 = 5145 */
#define DIAGDIAG_GET_DROP_PER_RIVA                  (DIAGDIAG_SUBSYS_ID_RIVA_OFFSET + 0x001A) /* 0x141A = 5146 */

#define DIAG_FLUSH_BUFFER                      0x005D /* 93 */
#define DIAG_CHANGE_THRESHOLD                  0x005E /* 94 */
#define DIAG_CHANGE_ENC_PROTOCOL               0x005F /* 95 */
#define DIAG_EXT_EVENT_REPORT_CTRL             0x0060 /* 96 */ 
#define DIAG_EXT_MSG_CONFIG_ADV                0x0061 /* 97 */ 
#define DIAG_EXT_LOG_CONFIG                    0x0062 /* 98 */ 

/* NOTE: Subsystem command codes 100-102 are used for the user-level app */

/*==========================================================================

PACKET   DIAGDIAG_STRESS_TEST

PURPOSE  Request sent from the DM to the DMSS to stress test events.

============================================================================*/
typedef enum {
  DIAGDIAG_STRESS_TEST_MSG = 0,
  DIAGDIAG_STRESS_TEST_MSG_1 = 1,
  DIAGDIAG_STRESS_TEST_MSG_2 = 2,
  DIAGDIAG_STRESS_TEST_MSG_3 = 3,
  DIAGDIAG_STRESS_TEST_MSG_4 = 4,
  DIAGDIAG_STRESS_TEST_MSG_5 = 5,
  DIAGDIAG_STRESS_TEST_MSG_6 = 6,
  DIAGDIAG_STRESS_TEST_MSG_STR = 7,
  DIAGDIAG_STRESS_TEST_MSG_PSEUDO_RANDOM = 8,
  DIAGDIAG_STRESS_TEST_MSG_LOW = 9,
  DIAGDIAG_STRESS_TEST_MSG_MED = 10,
  DIAGDIAG_STRESS_TEST_MSG_HIGH = 11,
  DIAGDIAG_STRESS_TEST_MSG_ERROR = 12,
  DIAGDIAG_STRESS_TEST_MSG_FATAL = 13,
  DIAGDIAG_STRESS_TEST_ERR = 14,
  DIAGDIAG_STRESS_TEST_ERR_FATAL = 15,
  DIAGDIAG_STRESS_TEST_LOG = 16,
  DIAGDIAG_STRESS_TEST_EVENT_NO_PAYLOAD = 17,
  DIAGDIAG_STRESS_TEST_EVENT_WITH_PAYLOAD = 18,
  /*19-DIAGDIAG_STRESS_TEST_ERR_FATAL_ISR removed */
  DIAGDIAG_STRESS_TEST_CMD_REQ = 20,
  /* Reserved space up to 39 */
  DIAGDIAG_STRESS_TEST_LOG_64 = 40, 
  DIAGDIAG_STRESS_TEST_LOG_128 = 41,
  DIAGDIAG_STRESS_TEST_LOG_256 = 42,
  /* Log of 512 is test type 16 */
  DIAGDIAG_STRESS_TEST_LOG_1K = 43,
  DIAGDIAG_STRESS_TEST_LOG_2K = 44,
  DIAGDIAG_STRESS_TEST_LOG_4K = 45,
  DIAGDIAG_STRESS_TEST_LOG_6K = 46,
  /* Test cases for extended messages */
  DIAGDIAG_STRESS_TEST_MSG_EXT = 47,
  DIAGDIAG_STRESS_TEST_MSG_1_EXT = 48,
  DIAGDIAG_STRESS_TEST_MSG_2_EXT = 49,
  DIAGDIAG_STRESS_TEST_MSG_3_EXT = 50,
  DIAGDIAG_STRESS_TEST_MSG_4_EXT = 51,
  DIAGDIAG_STRESS_TEST_MSG_9_EXT = 52,
   /* Test cases for extended logs */
  DIAGDIAG_STRESS_TEST_LOG_EXT   = 53,
  DIAGDIAG_STRESS_TEST_LOG_64_EXT = 54,
  DIAGDIAG_STRESS_TEST_LOG_128_EXT = 55,
  DIAGDIAG_STRESS_TEST_LOG_256_EXT = 56,
  DIAGDIAG_STRESS_TEST_LOG_1K_EXT = 57,
  DIAGDIAG_STRESS_TEST_LOG_2K_EXT = 58,
   /* Test cases for Sprintf messages */
  DIAGDIAG_STRESS_TEST_MSG_SPRINTF_1 = 59,
  DIAGDIAG_STRESS_TEST_MSG_SPRINTF_2 = 60,
  DIAGDIAG_STRESS_TEST_MSG_SPRINTF_3 = 61,
  DIAGDIAG_STRESS_TEST_MSG_SPRINTF_4 = 62,
  DIAGDIAG_STRESS_TEST_MSG_SPRINTF_5 = 63,
  /* Test cases for extended logs (contd) */
  DIAGDIAG_STRESS_TEST_LOG_4K_EXT = 64,
  DIAGDIAG_STRESS_TEST_LOG_6K_EXT = 65,
   /* Test cases for extended Sprintf messages */
  DIAGDIAG_STRESS_TEST_MSG_SPRINTF_6_EXT = 66,
  DIAGDIAG_STRESS_TEST_MSG_SPRINTF_7_EXT = 67,
  DIAGDIAG_STRESS_TEST_MSG_SPRINTF_8_EXT = 68,
   /* Test cases for extended events messages */
  DIAGDIAG_STRESS_TEST_EVENT_EXT_NO_PAYLOAD = 69,
  DIAGDIAG_STRESS_TEST_EVENT_EXT_WITH_PAYLOAD = 70,
 /* Shorter logs at 43... */
  DIAGDIAG_STRESS_TEST_LOG_8K5 = 71,
  DIAGDIAG_STRESS_TEST_LOG_SUBMIT_64 = 72,
  DIAGDIAG_STRESS_TEST_LOG_SUBMIT_EXT_64 = 73,
} diag_stress_test_type_enum_type;

typedef enum {
  EXPLICIT_PRI = 0, /* Priority given by the user */
  RELATIVE_PRI = 1  /* relative priority is relative to DIAG_PRI */
} diag_stress_pri_type_enum_type;

typedef struct {
  uint8 test_type;  /* Decides if it is log, msg or event*/
  uint8 pri_type;   /* External or relative priority */
  int16 pri;        /* Priority at which the task is created */
} diag_task_priority_type;

typedef struct {
  diag_task_priority_type priority; /* refer to diag_task_priority_type above*/
  int num_iterations; /* the number of times the test_type should be called	*/
  int sleep_duration; /* Sleep time in milliseconds */	
  int num_iterations_before_sleep; /*After NUM_ITERATIONS_BEFORE_SLEEP  iterations, sleep */
} diag_per_task_test_info;

typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  int num_tasks;                     /* Number of tasks, to be started */
  diag_per_task_test_info test[1];   /* Place holder for per task info */
} DIAGDIAG_STRESS_TEST_req_type;

typedef DIAGDIAG_STRESS_TEST_req_type DIAGDIAG_STRESS_TEST_rsp_type;

typedef struct {
  uint16 pri_type;   /* External or relative priority */
  int16  pri;        /* Priority at which the task is created */
} diag_priority_type;


/*==========================================================================

PACKET   DIAGDIAG_CMD_REQUEST

PURPOSE  Request sent from the DM to the DMSS to test command request.
============================================================================*/

typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  uint32 length;                     /* length of the packet */
  diag_priority_type priority;
  byte req[64];                      /* Packet sent */
} diagdiag_cmd_req_type;

typedef diagdiag_cmd_req_type diagdiag_cmd_rsp_type;


/*==========================================================================

PACKET   DIAGDIAG_LOG_EVENT_LISTENER

PURPOSE  Request sent from the DM to the DMSS to test log and event listeners.
		 This type is used for adding and removing log and event listeners.
============================================================================*/

typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  uint32 length;
  uint32 id;      /* log code or event id */
  uint32 param;   /* Will be printed by debug message */
} diag_log_event_listener_req_type;

typedef diag_log_event_listener_req_type diag_log_event_listener_rsp_type;

/* Functions required for DIAG Health */
void diagbuf_reset_drop_count_log( void );
uint32 diagbuf_get_drop_count_log( void );
void event_reset_drop_count_event( void );
uint32 event_get_drop_count_event( void );
void diagbuf_reset_drop_count_f3( void );
uint32 diagbuf_get_drop_count_f3( void );
uint32 diagbuf_get_alloc_count_log( void );
uint32 event_get_alloc_count_event( void );
uint32 diagbuf_get_alloc_count_f3( void );
void diagpkt_reset_drop_count_delay( void );
uint32 diagpkt_get_drop_count_delay( void );
uint32 diagpkt_get_alloc_count_delay( void );

/*===========================================================================
FUNCTION DIAGBUF_GET_DROP_PERCENTAGE
DESCRIPTION
    This function returns the percentage diag drop count 
===========================================================================*/
uint8 diagbuf_get_drop_percentage(void);

/*===========================================================================

FUNCTION DIAG_DROP_EVENT_REPORT

DESCRIPTION
  This function generates an event whenever the total drop count exceeds 
  a defined threshold and then further at a defined periodicity.
  
PARAMETERS
  Parameter send by the diag idle processing while
  calling this call back.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
===========================================================================*/
void diag_drop_event_report (void *param);

/* Structure for Sub-system Loopback Command Resposne */
typedef PACK(struct) {
    diagpkt_subsys_header_type header;
    qword ts;                 /* Timestamp 8-bytes long*/  
}diag_subsys_loopback_rsp_type;

/* Structures required for DIAG Health */
typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  PACK(union){
    uint32 drop_count;          /* Also used for alloc_count and DSM chaining count */
    uint8 per_drop_count;
  }count;
} diag_health_response_type;

/* Structures for QPST COOKIE commands */
#ifdef DIAG_FEATURE_QPST_COOKIE

typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  byte cmd_version;
  byte cookie_length;
  byte cookie[1];
} diag_save_qpst_cookie_req_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  byte cmd_version;
  byte error_code;
} diag_save_qpst_cookie_rsp_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  byte cmd_version;
} diag_query_qpst_cookie_req_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  byte cmd_version;
  byte error_code;
  byte cookie_length;
  byte cookie[1];
} diag_query_qpst_cookie_rsp_type;

#endif  /* DIAG_FEATURE_QPST_COOKIE */

/* Structures and defines for buffering modes */
#define DIAG_TX_MODE_CONFIG_SET_MODE              0
#define DIAG_TX_MODE_CONFIG_GET_MODE              1
#define DIAG_TX_MODE_CONFIG_SET_PARAM_HIGH_WM     2
#define DIAG_TX_MODE_CONFIG_GET_PARAM_HIGH_WM     3
#define DIAG_TX_MODE_CONFIG_SET_PARAM_LOW_WM      4
#define DIAG_TX_MODE_CONFIG_GET_PARAM_LOW_WM      5
#define DIAG_TX_MODE_CONFIG_DRAIN                 6
#define DIAG_TX_MODE_CONFIG_GET_BYTE_DROPPED_CNT       7
#define DIAG_TX_MODE_CONFIG_QUERY         8
#define DIAG_TX_MODE_CONFIG_RESET_DROPPED_CNT          9

/* Structure for User Registration Table Entry */
typedef PACK(struct)
{
  word cmd_code_lo;  /* Minimum Command code value */
  word cmd_code_hi;  /* Maximum Command code value */
} diagdiag_ctrl_msg_user_tbl_entry_type;

/* Structure for Command Registration Table Reponse Entry */
typedef PACK(struct)
{
  uint16 cmd_code;  /* Commad Code (defined in diagcmd.h) */
  word subsysid;    /* Subsystem ID if Diagnostics Subsytem Dispatch is used by client */
  uint16 proc_id;   /* Processor ID */
  word cmd_count;   /* Number of entries in the table */
  diagdiag_ctrl_msg_user_tbl_entry_type usr_cmds[1];  /* Client's Packet Dispatch Table entries */
} diagdiag_ctrl_msg_reg_type;

/* Structure for Registration Table Retrieval Request */
typedef PACK(struct)
{
  diagpkt_subsys_header_type header;  
  uint8 proc_id;   /* Defines the processor whose reg cmds we are querying */
} diagdiag_get_reg_tbl_req_type;

/* Structure for Registration Table Retrieval Immediate Response */
typedef PACK(struct)
{
  diagpkt_subsys_header_v2_type header;
  uint8 proc_id;    /* Defines the processor whose reg cmds we are querying */
  uint8 cmd_count;  /* Total number of commands registered */
} diagdiag_get_reg_tbl_imm_rsp_type;

/* Structure for Registration Table Retrieval Delayed Reponse */
typedef PACK(struct)
{
  diagpkt_subsys_header_v2_type header;
  diagdiag_ctrl_msg_reg_type reg_cmds[1];   /* The command registration entries */
} diagdiag_get_reg_tbl_delayed_rsp_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  uint8 op_code;
  PACK(union)
  {
    uint8 mode;       /* mode */  
    uint8 wm_value;  /* high_wm, low_wm */ 
  } data;
} diagcomm_tx_mode_config_req_type;

typedef PACK(struct) {
  uint8 per_compression_buf_used; /* Percentage Compression Buffer Full */
  uint32 byte_to_compress_cnt; 
} diagcomm_query_rsp_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  uint8 op_code;
  PACK(union)
  {
    uint32 byte_dropped_cnt;
    diagcomm_query_rsp_type query;
    uint32 byte_alloc_cnt;
    uint8 wm_value;          /* high_wm, low_wm */
    uint8 mode;               /* mode */  
  } data;
} diagcomm_tx_mode_config_rsp_type;

/* Structures for preset mask */
typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  uint8 preset_id;
} diag_set_preset_id_req_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  uint8 preset_id;
  uint8 error;
} diag_set_preset_id_rsp_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type header;
} diag_get_preset_id_req_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  uint8 preset_id;
} diag_get_preset_id_rsp_type;

/*=====================================================================*/
#define DIAG_HEALTH_FLOW_CTRL_VER 1
typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  uint8 version;              /* Version of this rsp pkt */
  uint32 count[2];            /* Used for flow control count */
} diag_health_get_flow_ctrl_rsp_type;

#define DIAG_HEALTH_DSM_CHAINED_VER 1
typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  uint8 version;              /* Version of this rsp pkt */
  uint32 count;               /* Used for DSM chained count */
  uint32 tot_count;           /* Used for total DSM count */
} diag_health_ver_rsp_type;

#ifdef DIAG_MAX_REQ_LEN_RSP_SEND
/* Structure for Get Max Request Packet Length CMD (DIAGDIAG_GET_MAX_REQ_PKT_LEN) */
typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  uint32 max_req_len;                   /* Maximum length of Diag REQ packet */
} diag_get_max_req_pkt_len_rsp_type;
#endif /* DIAG_MAX_REQ_LEN_RSP_SEND */

/*==========================================================================

PACKET   DIAG_EVENT_MASK_GET_F

PURPOSE  Request sent from the DM to the DMSS to retrieve the event mask.
============================================================================*/

typedef struct {
  diagpkt_header_type header;
  uint8 pad;
  uint16 reserved;
} event_mask_get_req_type;

typedef struct {
  diagpkt_header_type header;
  uint8 error_code;
  uint16 reserved;
  uint16 numbits;         /* number of bits in the mask           */
  unsigned char mask[1];  /* size of this field = (numbits + 7)/8 */
} event_mask_get_rsp_type;


/*==========================================================================
 
PACKET   DIAG_EVENT_MASK_RETRIEVAL_F

PURPOSE  Request sent from the DM to the DMSS to retrieve 
the event mask from peripheral
============================================================================*/

typedef PACK(struct) 
{
 diagpkt_subsys_header_type header;
 byte stream_id;
}  event_get_local_mask_req_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  byte stream_id;
  uint8 status;
  uint16 numbits;         /* number of bits in the mask           */
  unsigned char mask[1];  /* size of this field = (numbits + 7)/8 */
}  event_get_local_mask_rsp_type;

/*==========================================================================

PACKET   DIAG_EVENT_MASK_SET_F

PURPOSE  Request sent from the DM to the DMSS to set the event mask.
============================================================================*/

typedef struct {
  diagpkt_header_type header;
  uint8 pad;
  uint16 reserved;
  uint16 numbits;         /* number of bits in the mask           */
  unsigned char mask[1];  /* size of this field = (numbits + 7)/8 */
} event_mask_set_req_type;

typedef struct {
  diagpkt_header_type header;
  uint8 error_code;
  uint16 reserved;
  uint16 numbits;         /* number of bits in the mask           */
  unsigned char mask[1];  /* size of this field = (numbits + 7)/8 */
} event_mask_set_rsp_type;

typedef struct {
  diagpkt_subsys_header_type header;
  uint16 num_rsp;
}diagdiag_delayed_rsp_test_req_type;

typedef struct {
  diagpkt_subsys_header_type header;
  uint32 status;
  uint16 delayed_rsp_id;
  uint16 response_cnt;
}diagdiag_delayed_rsp_test_rsp_type;

typedef struct {
  diagpkt_subsys_header_type header;
} diagdiag_query_enable_req_type;

typedef struct {
  diagpkt_subsys_header_type header;
  uint32 ballot_mask;
  uint32 cx_state;
  uint8  enable_report;
} diagdiag_query_enable_rsp_type;

typedef struct
{
  diagpkt_subsys_header_type header;
  uint8 enable_aggregation;
  uint8 reserved1;
  uint16 reserved2;
} diagdiag_change_threshold_req_type;


typedef struct
{
  diagpkt_subsys_header_type header;
  uint8 disable_hdlc;
} diagdiag_change_enc_protocol_rsp_type;

typedef struct
{
  diagpkt_subsys_header_type header;
  uint8 disable_hdlc;
  uint8 reserved1;
  uint16 reserved2;
} diagdiag_change_enc_protocol_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;
  uint8 framing_version;
} diagdiag_hdlc_disable_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;
  uint8 framing_version;
  uint8 result;
} diagdiag_hdlc_disable_rsp_type;

#if defined(DIAG_FLASH_LOGGING_SUPPORT)
typedef struct
{
  diagpkt_subsys_header_type header;
  uint32 num_iterations;
  uint32 iteration_duration_msecs;
} diagdiag_flash_logging_test_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;
  uint32 num_iterations;
  uint32 iteration_duration_msecs;
  uint8  result;
} diagdiag_flash_logging_test_rsp_type;
#endif

#define DIAG_HDLC_DISABLE_SUCCESS 0
#define DIAG_HDLC_DISABLE_FAILURE 1

#define DIAG_HDLC_DISABLE_VERSION 1

typedef enum {
  HDLC_DIABLED_ERR_RECOVERY_INIT = 0,
  HDLC_DIABLED_ERR_RECOVERY_START = 1, 
  HDLC_DIABLED_ERR_RECOVERY_FOUND_VALID = 2,
} hdlc_disabled_err_recovery_status_enum_type;

#define DIAG_STM_VER_2   2
#define DIAG_STM_DISABLE 0
#define DIAG_STM_ENABLE  1
#define DIAG_STM_QUERY   2
#define DIAG_STM_PROC_MASK_MODEM 0x01
#define DIAG_STM_PROC_MASK_ADSP  0x02
#define DIAG_STM_PROC_MASK_WCNSS 0x04
#define DIAG_STM_PROC_MASK_APPS  0x08

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  uint8 version;
  uint8 proc_mask;
  uint8 stm_command; 
} diag_stm_control_req_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  uint8 version;
  uint8 proc_mask;
  uint8 stm_command;
  uint8 is_support_mask;
  uint8 stm_status_mask;
} diag_stm_control_rsp_type;

typedef struct
{
  diagpkt_subsys_header_type header;
} diagdiag_change_threshold_rsp_type;

/* Error codes for the above two packets.
 */
#define EVENT_MASK_GENERAL_FAILURE 1
#define EVENT_MASK_ARRAY_TOO_SMALL 2
#define EVENT_MASK_ARRAY_TOO_BIG   3



extern osal_mutex_arg_t diagdiag_memop_tbl_mutex;

/*===========================================================================
FUNCTION DIAGPKT_GET_REG_CMD_COUNT

DESCRIPTION  
   This function is used to get the number of command codes /command handlers
   registered for a given processor
 
RETURNS 
   The number of entries in the master or slave table
============================================================================*/ 
uint8 diagpkt_get_reg_cmd_count(void);

/*===========================================================================
FUNCTION DIAGPKT_GET_TBL_PKT_LENGTH

DESCRIPTION  
   This procedure returns the total number of bytes needed to store the
   registration table information -- used to send the registration table
   as a response to Registration Table Retrieval Commands
 
RETURNS 
   The total size (in bytes) that should be allocated in diagbuf to
   accomodate the response for registration command retrieval request
============================================================================*/ 
uint32 diagpkt_get_reg_tbl_pkt_length(void);

/*===========================================================================
FUNCTION DIAGPKT_GET_REGISTRATION_TABLE

DESCRIPTION  
   This function is called by DIAGDIAG_GET_REG_TABLE to query the processors's
   registration table. It sends one or more delayed responses containing the
   information about the commands registered in the processor's
   master or slave registration table
 
============================================================================*/ 
void diagpkt_get_registration_table(void);

/*===========================================================================
FUNCTION DIAGDIAG_IS_VALID_REG_TBL_CMD

DESCRIPTION
  This procedure is used to validate the incoming command (and parameters)
  for retreiving the registration tables.
 
  Also assigns values to is_mater ( denotes whether it is a master proc )
  and the port_id (for querying slave table)

============================================================================*/
boolean diagdiag_is_valid_reg_tbl_cmd(void);

#define DIAGPKT_HDR_PATTERN (0xDEADD00DU)
#define DIAGPKT_OVERRUN_PATTERN (0xDEADU)

typedef struct
{
  uint8 command_code;
  uint8 subsys_id;
  uint16 subsys_cmd_code;
}
diagpkt_subsys_hdr_type;

/* Local structure definitions */
typedef struct
{
  uint8 command_code;
  uint8 subsys_id;
  uint16 subsys_cmd_code;
  uint32 status;
  uint16 delayed_rsp_id;
  uint16 rsp_cnt; 
}
diagpkt_subsys_hdr_type_v2;

/* Local structure definitions */
typedef struct
{
  uint8 command_code;
}
diagpkt_hdr_type;


typedef struct
{
  q_link_type qlink;

  diag_cmd_rsp rsp_func; /* If !NULL, this is called in lieu of comm layer */
  void *rsp_func_param;

  diagpkt_rsp_type rsp; /* see diagi.h */
}
diagpkt_q_type;

void * diag_rsp_malloc (uint32 num_bytes);

extern int diagpkt_user_space_tbl_initialized;
extern osal_mutex_arg_t diagpkt_tbl_reg_lock;
	
uint16 diagpkt_get_delayed_rsp_id(void);

void* diagpkt_user_space_commit (void * pkt, uint32 rsp_len, uint32 rsp_flag);

PACK(void *) diagpkt_delay_alloc(diagpkt_cmd_code_type code, unsigned int length);

boolean diagpkt_user_space_tbl_init (void);

void diagpkt_process_user_space_command (diagpkt_user_table_entry_type *tbl_entry, void *req_pkt, uint16 pkt_len, uint8 index);

/*===========================================================================

FUNCTION qsr_v4_get_address_table_index

DESCRIPTION
  Binary search on address table to get the index pointed to by the function
  
DEPENDENCIES
  The table is expected to have limited entries which are unmapped in QShrink 4.0
  message sections.
 
PARAMETERS 
  search_address - The address that need to be searched from the qshrink 4 table
 
RETURN 
  uint16 - index in the address table
===========================================================================*/

uint16 qsr_v4_get_address_table_index (const uint32 search_address);


#endif /*DIAGDIAG_V_H*/

