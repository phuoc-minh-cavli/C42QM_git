/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Mobile Diagnostic Message Services

General Description
  All the declarations, definitions, and routines necessary to support
  reporting messages for errors and debugging.

Initialzation and Sequencing Requirements


Copyright (c) 2010, 2012-2016, 2020 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                           Edit History

$Header: //components/rel/core.tx/6.0/services/diag/DCM/rtos/src/msg_api.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/16   nk      Changes to support ThreadX
04/20/16   ph      Support for QShrink 2.0
01/29/15   ph      Fix KW errors.
01/08/15   rs      Added check for msg_service_init.
06/25/14   rs      Fixed issues with runtime update of msg mask table in BT mask centralization.
10/14/13   sr      Added support for extended event, log and F3 pkts
10/01/13   xy      Removed code under FEATURE_WINCE
09/20/13   tbg     Added support for Diag over STM
03/25/13   ph      Corrected msg_sprintf to use strlcpy().
12/07/12   is      Warnings fixes
11/07/12   is      Support for preset masks
07/10/12   ra      F3 Mask Check Optimization
12/20/10   mad     Changes for dual diag debug message mask
10/29/10   vs      Moved msg_get_time to diagtarget.h.
                   Other changes for debugtools for lock-less implementation
09/28/10   sg      Moved diag_time_get to diag_cfg.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
08/18/10   vs      Merged in changes from Tim to add QXDM masking for f3_save
08/09/10   sg      Removed std_strlprintf() wrapper function calls
06/30/10   sg      Inclusion of err.h
03/29/10   sg      created file

============================================================================*/
#include "customer.h"
#include "comdef.h"

#include <stringl/stringl.h>  /*For usage of strlcpy() */
#include <stdarg.h>     /* support for variable argument lists */

#include "msg.h"        /* Message typedefs and prototypes */
#include "msg_qsr.h"

#include "diagcmd.h"    /* diag command codes */
#include "diagbuf_v.h"  /* diag outpout buffer allocation
                                 manager. */
#include "qw.h"         /* definition of qword */
#include "err.h"
#include <stdio.h>
#include <stdarg.h>  /* Definitions va_start, va_end, etc */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
#include "diag_f3_tracei.h"
#endif /* FEATURE_SAVE_DEBUG_TRACE */

#include "diagtarget.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

#include "diagi_v.h"
#include "diagdiag_v.h"
#include "msgi.h"
#include "diag_cfg.h"
#include "diag_stm.h"

#include <stdlib.h>

#ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
#include "wcdmadiagif.h"
#endif

  /* Number of bits in the frame number that is inserted into the timestamp,
     ** and the mask to extract these bits.
   */
#define MSG_FN_NUM_BITS 10
#define MSG_FN_MASK 0x3ff


#ifndef MSG_FMT_STR_ARG_SIZE
#define MSG_FMT_STR_ARG_SIZE 100 /* 100 is guess */
#endif
#define MSG_V2_CHECK_IS_HASH(X) ((((uint32)(X)) & 0x03) != 0)
extern uint8 diag_cur_preset_id; /* from diag.c */
extern uint32 msg_dropped;  /* number of dropped messages */
extern uint32 msg_drop_delta;   /* number of dropped messages */
extern boolean msg_service_init;
extern uint16 msg_stream_ssid_tbl_mask[DIAG_MAX_STREAM_ID][DIAG_MAX_PRESET_ID];

/* QShrink 4 parameters referenced from msg.c */
extern char* qsr_4_0_msg_aligned_start_address;
extern char* qsr_4_0_msg_unaligned_start_address;
extern char* qsr_4_0_msg_end_address;
extern uint32 msg_v4_ss_num_aligned_entries;
extern msg_v4_ss_aam_table_type msg_v4_ss_aam_table;
extern uint32 msg_v4_ss_num_unaligned_entries;
extern msg_v4_ss_uam_table_type msg_v4_ss_uam_table;
extern uint32 msg_v4_ss_num_table_entries; 
extern msg_v4_ssid_table_type msg_v4_ss_table;
extern const uint8 msg_v4_dbase_guid[QSHRINK_4_DATABASE_GUID_LENGTH];

static inline void qsr_v4_copy_args(uint8 dst[], uint8 src[], uint8 length);
static uint8 qsr_v4_check_bytes(uint8 arg_bytes, uint32 arg);


typedef struct
{
  diag_cmd_ext_v1_type ext_hdr;
  msg_ext_type msg;
}msg_sprintf_ext_desc_type;

static void * msg_send_prep (const msg_const_type * const_blk, unsigned int num_args,
                    uint64 timestamp, boolean ts_valid, byte stream_id, uint32 version, uint32 id);

#ifdef FEATURE_DIAG_STM
static void *
diag_stm_msg_prep(const msg_const_type * const_blk, unsigned int num_args, uint32 *len, uint32 version, uint32 id);
#endif /* FEATURE_DIAG_STM */

static msg_qsr_store_type *
qsr_msg_send_prep (const msg_qsr_const_type * const_blk,unsigned int num_args, uint32 isHash, byte stream_id);


static uint32 qsr_msg_dropped;  /* number of dropped messages */
static uint32 qsr_msg_drop_delta;   /* number of dropped messages */

#ifdef FEATURE_DIAG_STM

/* STREAM_STM and STM enabled: Need to allocate and send over STM immediately */
/* If TRUE, packet is needed for STM and should be malloc'd from STM pool */
/* (This might be in addition to allocating from the diagbuf if needed for listeners or other streams) */


/* NOTE: For both MSG_STM and MSG_DIAGBUF, calling functions must pass in the value
   'flag' indicatign if diag STM is enabled or not. Each function should manage this
   value to ensure persistence through-out the function and avoid accessing the
   global variable diag_stm_enabled when possible. */

#define MSG_STM(s,flag) (((s & DIAG_STREAM_STM) && (flag)) ? TRUE : FALSE)


/* Used to compute packet size needed for QSR packet */
#define STM_QSR_EXT_MSG_PKT_SIZE(numargs) \
  sizeof(msg_hdr_type) + sizeof(msg_qsr_const_type) + (4*(numargs))

/* Used to compute packet size needed for non-QSR packet */
#define STM_EXT_MSG_PKT_SIZE(fmt_len,num_args) \
  sizeof(msg_hdr_type) + sizeof(msg_desc_type) + (fmt_len) + (4*(num_args))

#endif

/* STREAM_STM and STM not enabled, or stm_enabled with a stream other than STREAM_STM */
/* If TRUE, packet needs to be allocated in diagbuf */

#define MSG_DIAGBUF(s,flag) (((!(flag) && s) || ((flag) && (s & ~DIAG_STREAM_STM))) ? TRUE : FALSE)

extern uint8 msg_enabled_mask; /* Bit mask to check if F3 masks, listeners and Extended Listeners are set */
extern uint32 msg_v4_ss_num_unaligned_entries;
extern msg_v4_ss_uam_table_type msg_v4_ss_uam_table;

#if defined (FEATURE_SAVE_DEBUG_TRACE)
#define MAX_VAR_ARGS 9

extern osal_mutex_arg_t         diag_debug_record_buf_cs;
static void msg_save_trace
(
  const void * constant_data_ptr,
  msg_arg_type * args,
  uint8 nargs,
  void * msg,
  uint32 type,
  uint8 version,
  void* param,
  uint8 arg_bytes);

/*===========================================================================

FUNCTION MSG_SAVE_TRACE

DESCRIPTION
  Extracts arguments based on the constant_data_ptr type and saves it to the
  circular buffer.  If timestamp-saving option is checked, it will be saved
  to the circular buffer as well.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes copied to the buffer.

SIDE EFFECTS
  None

===========================================================================*/
static void
msg_save_trace
(
  const void * constant_data_ptr,
  msg_arg_type * args,
  uint8 nargs,
  void * msg,
  uint32 type,
  uint8 version,
  void* param,
  uint8 arg_bytes
)
{
  /* Is DIAG services initialized? */
  if (!diag_services_initialized)
    return;

  err_f3_save_msg(constant_data_ptr, args, nargs, msg, type, version, param, arg_bytes);

  return;
}



/*===========================================================================

FUNCTION MSG_SAVE_3

DESCRIPTION
  Saves the F3 message to the RAM buffer.  Also called from the ERR_FATAL
  macro.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void msg_save_3(const msg_const_type* const_blk,
                uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3,
                msg_ext_store_type* msg)
{
    msg_arg_type f3_args[3];

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;

    msg_save_trace((void*)const_blk, f3_args, 3, (void*)msg, DIAG_F3_TRACE_STANDARD, 0, NULL, 0);
    return;
}
#endif /* FEATURE_SAVE_DEBUG_TRACE */


void
msg_get_time( qword * ts)
{
  /* Get timestamp for this message ASAP */
  diag_time_get (*ts);


  {
    /* Get the frame number (either SFN or CFN, depending on the
    ** state of the mobile), extract its least significant 10 bits
    ** and add these bits to the timestamp.
    */
    uint32 frame_number = 0;

    #ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
    frame_number = wl1_get_frame_number_for_diag() & MSG_FN_MASK;
    #endif

    (*ts)[0] >>= MSG_FN_NUM_BITS;
    (*ts)[0] <<= MSG_FN_NUM_BITS;
    (*ts)[0] |= frame_number;
  }


  return;
}

typedef union
{
    unsigned long  ts[2];
    uint64 timestamp;
}msg_timestamp;

/*===========================================================================

FUNCTION MSG_STREAM_CHECK

DESCRIPTION
  This helper function performs a check to see which streams a given message
  is enabled on. In cases where STM is enabled, a LISTENER stream bit is also
  set if listeners are enabled.

DEPENDENCIES

===========================================================================*/
static byte msg_stream_check(const msg_desc_type *const_desc)
{
  uint32 rt_mask = 0;
  boolean valid_ssid = FALSE;
  byte stream_id = 0;
  #ifdef DIAG_STREAM_2_MASK_CHECK
  uint32 rt_mask_ctrl = 0;
  boolean valid_ssid_ctrl = FALSE;
  #endif

  if(const_desc != NULL)
  {
    /* Check the runtime mask */
    valid_ssid =
        msg_get_ssid_masks_adv (MSG_MASK_RT, const_desc->ss_id,
            const_desc->ss_id, &rt_mask, DIAG_STREAM_1, diag_cur_preset_id,FALSE);
    if((valid_ssid && (const_desc->ss_mask & rt_mask)))
        stream_id |= DIAG_STREAM_1;

    #ifdef DIAG_STREAM_2_MASK_CHECK
        valid_ssid_ctrl = msg_get_ssid_masks_adv (MSG_MASK_RT, const_desc->ss_id,
            const_desc->ss_id, &rt_mask_ctrl, DIAG_STREAM_2, DIAG_DCI_MASK_1,FALSE);
        if((valid_ssid_ctrl && (const_desc->ss_mask & rt_mask_ctrl)))
            stream_id |= DIAG_STREAM_2;
    #endif
  }
  return stream_id;
}

/*===========================================================================

FUNCTION MSG_SEND_PREP

DESCRIPTION
  This message performs the common operations used by all messages.
  It allocates, fills in all data except arguments, and returns a pointer
  to the allocated message buffer.  It also handles message statisitics.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

PARAMETERS
 const_blk  static constant constructed using the XX_MSG_CONST macro
 num_args   Number of arguments
 timestamp  This value of timestamp will be used if ts_valid is set to TRUE
 ts_valid   Indicates whether the timestamp is valid or not
 version    0: if the message being generated is in msg_ext_store_type type.
            1: if the message being generated is in msg_send_multi_client_type type.
 id         ID passed in by user if version = 1;

===========================================================================*/
static void *
msg_send_prep (const msg_const_type * const_blk, unsigned int num_args,
              uint64 timestamp, boolean ts_valid, byte stream_id, uint32 version, uint32 id)
{
  msg_timestamp temp_timestamp;
  msg_ext_store_type *msg = NULL;
  unsigned int rsp_len = 0;
  msg_send_multi_client_type *msg_ext = NULL;

  if (stream_id)
  {
    /*-----------------------------------------------------------------------
      Get a pointer to a buffer.  If it's a NULL pointer, there aren't any
      free buffers.  No effort is made to save higher priority messages in
      favor of lower priority messages, as message flooding hides enough
      details to make the usable margin on such a feature too small for
      implementation.
    -----------------------------------------------------------------------*/
    rsp_len = FPOS(msg_ext_store_type, args) + num_args * FSIZ(msg_ext_store_type,
                                                                 args[0]);

   if (version == DIAGBUF_VERSION_1)
    {
      rsp_len += FPOS(msg_send_multi_client_type, msg);
    }

    msg = (void *)diagbuf_alloc(DIAG_EXT_MSG_TERSE_F, rsp_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, 0);

    if (msg)
    {
        diagbuf_write_stream_id (msg,stream_id);
    }
    else
    {
        msg_drop_delta++;
        msg_dropped++;        /* count up one more ignored message */
        return NULL;
    }

    if (version == DIAGBUF_VERSION_1)
    {
        /*Fill out the header for DSDA msg pkts*/
        msg_ext = (msg_send_multi_client_type*) msg;
        msg_ext->cmd_code = DIAG_CMD_EXT_F;
        msg_ext->version = DIAGBUF_VERSION_1;
        msg_ext->id = id;
        msg_ext->proc_id = DIAG_MY_PROC_ID;
        msg = &(msg_ext->msg);
        msg->hdr.cmd_code = DIAG_EXT_MSG_TERSE_F;
    }
    else
    {
        msg_ext = NULL;
    }

    if (msg)
    {
        if(ts_valid) /* client timestamp is valid, copy that into the header */
        {
            temp_timestamp.timestamp = timestamp;
            msg->hdr.ts[0] =(unsigned long) temp_timestamp.ts[0];
            msg->hdr.ts[1] = (unsigned long)temp_timestamp.ts[1];
        }
        else
        {
            /*lint -save -e{545} Suspicious use of & */
            msg_get_time((qword*)&(msg->hdr.ts));
            /*lint -restore */
        }
#ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
        msg->hdr.ts_type = MSG_TS_TYPE_GW;
#else
        msg->hdr.ts_type = MSG_TS_TYPE_CDMA_FULL;
#endif

        msg->hdr.num_args = (uint8)num_args;

        msg->hdr.drop_cnt = (uint8) ((msg_drop_delta > 255) ? 255 : msg_drop_delta);

        msg_drop_delta = 0;   /* Reset delta drop count */

        /*-----------------------------------------------------------------------
            Set the pointer to the constant blk, to be expanded in DIAG context.
        -----------------------------------------------------------------------*/
        msg->const_data_ptr = const_blk;
        if (msg_ext)
        {
          return msg_ext;
        }
        else
        {
            return msg;
        }
    }
  }
  return NULL;
}               /* msg_send_prep() */

/*===========================================================================

FUNCTION MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send (const msg_const_type * const_blk )
{
    msg_send_ext (const_blk, 0, NULL);
}               /* msg_send() */

/*===========================================================================
DESCRIPTION
  This function is similar to MSG_SEND function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).
===========================================================================*/
void
msg_send_ext (const msg_const_type * const_blk, uint32 version, void* parameter)
{
  msg_ext_store_type *msg = NULL;
  msg_multi_client_store_type *msg_ext =NULL;
  const unsigned int num_args = 0;  /* # of message arguments */
  uint32 *id_ptr = NULL;
  uint32 id = 0;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
        byte stream_id = 0;
        switch(version)
        {
            case DIAGBUF_VERSION_0:
            {
                id=0;
                break;
            }
            case DIAGBUF_VERSION_1:
            {
                if (parameter == NULL)
                {
                    return;
                }
                id_ptr = (uint32 *)parameter;
                id = *id_ptr;
                break;
            }
        }
        stream_id = msg_stream_check(&const_blk->desc);
#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_msg_prep(const_blk, num_args, &pkt_length, version, id);

            if (stm_msg)
            {
                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

                /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
                stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif
        if MSG_DIAGBUF(stream_id,stm_enabled)
        {
            void* msg_ptr;
            msg_ptr = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, version, id);

            if (msg_ptr)
            {
                if(version == DIAGBUF_VERSION_1)
                {
                    msg_ext= (msg_multi_client_store_type*) msg_ptr;
                }
                else
                {
                    msg= (msg_ext_store_type*)msg_ptr;
                }
                diagbuf_commit (msg_ptr);
            }
        }
    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)
    if(version==DIAGBUF_VERSION_1)
    {
        msg_save_trace((void *)const_blk, NULL, 0, (void *)msg_ext, DIAG_F3_TRACE_STANDARD,0,NULL,0);
    }
    else
    {
        msg_save_trace((void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_STANDARD,0,NULL,0);
    }


#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send() */


/*===========================================================================

FUNCTION MSG_SEND_TS

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with timestamp as user supplied arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_ts (const msg_const_type * const_blk, uint64 timestamp)
{
  msg_send_ts_ext(const_blk, 0, NULL, timestamp);
}               /* msg_send_ts() */

/*===========================================================================

FUNCTION MSG_SEND_TS_EXT

DESCRIPTION
  This function is similar to msg_send_ts function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  This functions enqueues such a message with timestamp as user supplied arguments.

  Do not call directly; use macro MSG_SEND_TS_EXT defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_ts_ext (const msg_const_type * const_blk, uint32 version, void *parameter, uint64 timestamp)
{
  msg_ext_store_type *msg = NULL;
  msg_multi_client_store_type *msg_ext =NULL;
  const unsigned int num_args = 0;  /* # of message arguments */
  uint32 *id_ptr = NULL;
  uint32 id = 0;
  byte stream_id = 0;
  void *msg_ptr=NULL;

  if ((msg_enabled_mask == 0) || (msg_service_init != TRUE))
  {
    return;
  }

  switch (version)
  {
     case DIAGBUF_VERSION_0:
       id = 0;
       break;
     case DIAGBUF_VERSION_1:
       if (parameter == NULL)
       {
         return;
       }
       id_ptr = (uint32 *)parameter;
       id = *id_ptr;
       break;
     default:
       return;
  }

    stream_id = msg_stream_check(&const_blk->desc);

    msg_ptr = msg_send_prep(const_blk, num_args, timestamp, TRUE, stream_id, version, id);

    if (msg_ptr)
    {
        if(version == DIAGBUF_VERSION_1)
        {
            msg_ext= (msg_multi_client_store_type*) msg_ptr;
        }
        else
        {
            msg= (msg_ext_store_type*)msg_ptr;
        }

        diagbuf_commit(msg_ptr);
    }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

    if(version==DIAGBUF_VERSION_1)
    {
        msg_save_trace((void *)const_blk, NULL, 0, (void *)msg_ext, DIAG_F3_TRACE_STANDARD,(uint8)version, parameter, 0);
    }
    else
    {
        msg_save_trace((void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_STANDARD,0,NULL,0);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */

}               /* msg_send_ts_ext() */
/*===========================================================================

FUNCTION MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_1 (const msg_const_type * const_blk, uint32 xx_arg1)
{
    msg_send_1_ext(const_blk, 0, NULL, xx_arg1 );
}               /* void msg_send_1() */

/*===========================================================================
DESCRIPTION
  This function is similar to MSG_SEND_1 function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).
===========================================================================*/
void
msg_send_1_ext (const msg_const_type * const_blk, uint32 version, void* parameter, uint32 xx_arg1)
{
    msg_ext_store_type *msg = NULL;
    msg_multi_client_store_type *msg_ext =NULL;
    uint32 *args = NULL;
    const unsigned int num_args = 1;  /* # of message arguments */
    uint32 id = 0;
    uint32 *id_ptr = NULL;
    byte stream_id = 0;
    boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
    msg_arg_type    f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
        switch(version)
        {
            case DIAGBUF_VERSION_0:
            {
                id=0;
                break;
            }
            case DIAGBUF_VERSION_1:
            {
                if (parameter == NULL)
                {
                    return;
                }
                id_ptr = (uint32 *)parameter;
                id = *id_ptr;
                break;
            }
        }

        stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_msg_prep(const_blk, num_args, &pkt_length, version, id);

            if (stm_msg)
            {

                if(version == DIAGBUF_VERSION_1)
                {
                    args = (uint32*) ((byte*)stm_msg + FPOS(msg_multi_client_store_type, msg)+ sizeof(msg_hdr_type) +sizeof(msg_desc_type));
                }
                else
                {
                    args = (uint32*) ((byte*)stm_msg + sizeof(msg_hdr_type) +sizeof(msg_desc_type));
                }

                args[0] = xx_arg1;

                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

                /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
                stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif
        if MSG_DIAGBUF(stream_id,stm_enabled)
        {
            void* msg_ptr;
            msg_ptr = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, version, id);

            if (msg_ptr)
            {
                if(version == DIAGBUF_VERSION_1)
                {
                    msg_ext= (msg_multi_client_store_type*) msg_ptr;

                    args = msg_ext->msg.args;
                }
                else
                {
                    msg= (msg_ext_store_type*)msg_ptr;

                    args = msg->args;
                }
                /*-----------------------------------------------------------------------
                    Store the arguments in the buffer.
                -----------------------------------------------------------------------*/
                args[0] = xx_arg1;

                diagbuf_commit (msg_ptr);

            }
        }
    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;

    if(version==DIAGBUF_VERSION_1)
    {
        msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg_ext, DIAG_F3_TRACE_STANDARD,(uint8)version,parameter,0);
    }
    else
    {
        msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_STANDARD,0,NULL,0);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}

/*===========================================================================

FUNCTION MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_2 (const msg_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
    msg_send_2_ext(const_blk, 0, NULL, xx_arg1, xx_arg2 );
}
/*===========================================================================
DESCRIPTION
  This function is similar to MSG_SEND_2 function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).
===========================================================================*/

void
msg_send_2_ext (const msg_const_type * const_blk, uint32 version, void* parameter,
                        uint32 xx_arg1, uint32 xx_arg2)
{
  msg_ext_store_type *msg = NULL;
  msg_multi_client_store_type *msg_ext =NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */
  uint32 id = 0;
  uint32 *id_ptr = NULL;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
        byte stream_id = 0;

        switch(version)
        {
            case DIAGBUF_VERSION_0:
            {
                id=0;
                break;
            }
            case DIAGBUF_VERSION_1:
            {
                if (parameter == NULL)
                {
                    return;
                }
                id_ptr = (uint32 *)parameter;
                id = *id_ptr;
                break;
            }
        }

    stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_msg_prep(const_blk, num_args, &pkt_length, version, id);

            if (stm_msg)
            {

                if(version == DIAGBUF_VERSION_1)
                {
                    args = (uint32*) ((byte*)stm_msg + FPOS(msg_multi_client_store_type, msg)+ sizeof(msg_hdr_type) +sizeof(msg_desc_type));
                }
                else
                {
                    args = (uint32*) ((byte*)stm_msg + sizeof(msg_hdr_type) +sizeof(msg_desc_type));
                }

                args[0] = xx_arg1;
                args[1] = xx_arg2;

                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

                /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
                stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif
        if MSG_DIAGBUF(stream_id,stm_enabled)
        {
            void* msg_ptr;
            msg_ptr = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, version, id);

            if (msg_ptr)
            {

                if(version == DIAGBUF_VERSION_1)
                {
                    msg_ext= (msg_multi_client_store_type*) msg_ptr;

                    args = msg_ext->msg.args;
                }
                else
                {
                    msg= (msg_ext_store_type*)msg_ptr;

                    args = msg->args;
                }

                /*-----------------------------------------------------------------------
                    Store the arguments in the buffer.
                -----------------------------------------------------------------------*/

                args[0] = xx_arg1;
                args[1] = xx_arg2;

                diagbuf_commit (msg_ptr);
            }
        }
    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;

    if(version==DIAGBUF_VERSION_1)
    {
        msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg_ext, DIAG_F3_TRACE_STANDARD,(uint8)version,parameter,0);
    }
    else
    {
        msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_STANDARD,0,NULL,0);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send_2() */

/*===========================================================================

FUNCTION MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_3 (const msg_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
    msg_send_3_ext(const_blk, 0, NULL, xx_arg1, xx_arg2, xx_arg3 );
}
/*===========================================================================
DESCRIPTION
  This function is similar to MSG_SEND_3 function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).
===========================================================================*/

void
msg_send_3_ext (const msg_const_type * const_blk, uint32 version, void* parameter,
                uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3)
{
  msg_ext_store_type *msg = NULL;
  msg_multi_client_store_type *msg_ext =NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */
  uint32 id = 0;
  uint32 *id_ptr = NULL;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */
  #if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[3];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
    byte stream_id = 0;

    switch(version)
    {
        case DIAGBUF_VERSION_0:
        {
            id=0;
            break;
        }
        case DIAGBUF_VERSION_1:
        {
            if (parameter == NULL)
            {
                return;
            }
            id_ptr = (uint32 *)parameter;
            id = *id_ptr;
            break;
        }
    }

    stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_msg_prep(const_blk, num_args, &pkt_length, version, id);

            if (stm_msg)
            {

                if(version == DIAGBUF_VERSION_1)
                {
                    args = (uint32*) ((byte*)stm_msg + FPOS(msg_multi_client_store_type, msg)+ sizeof(msg_hdr_type) +sizeof(msg_desc_type));
                }
                else
                {
                    args = (uint32*) ((byte*)stm_msg + sizeof(msg_hdr_type) +sizeof(msg_desc_type));
                }

                args[0] = xx_arg1;
                args[1] = xx_arg2;
                args[2] = xx_arg3;

                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

                /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
                stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif
        if MSG_DIAGBUF(stream_id,stm_enabled)
        {
            void* msg_ptr;
            msg_ptr = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, version, id);

            if (msg_ptr)
            {
                if(version == DIAGBUF_VERSION_1)
                {
                    msg_ext= (msg_multi_client_store_type*) msg_ptr;

                    args = msg_ext->msg.args;
                }
                else
                {
                    msg= (msg_ext_store_type*)msg_ptr;

                    args = msg->args;
                }

                /*-----------------------------------------------------------------------
                    Store the arguments in the buffer.
                -----------------------------------------------------------------------*/

                args[0] = xx_arg1;
                args[1] = xx_arg2;
                args[2] = xx_arg3;

                diagbuf_commit (msg_ptr);
            }
        }
    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;

    if(version==DIAGBUF_VERSION_1)
    {
        msg_save_trace((const void *)const_blk, f3_args, 3, (void *)msg_ext, DIAG_F3_TRACE_STANDARD,(uint8)version,parameter,0);
    }
    else
    {
        msg_save_trace((const void *)const_blk, f3_args, 3, (void *)msg, DIAG_F3_TRACE_STANDARD,0,NULL,0);
    }
#endif /* FEATURE_SAVE_DEBUG_TRACE */
}    /* msg_send_3() */



/*===========================================================================

FUNCTION MSG_SEND_VAR

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with a variable number of
    arguments, specified by 'num_args'.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/

void
msg_send_var (const msg_const_type * const_blk, uint32 num_args, ...)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[9];  // Assuming at most 9 arguments
#endif /* FEATURE_SAVE_DEBUG_TRACE */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
        byte stream_id = 0;

        stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_msg_prep(const_blk, num_args, &pkt_length, 0, 0);

            if (stm_msg)
            {

                /*We always get only version 0 messages here*/
                args = (uint32*) ((byte*)stm_msg + sizeof(msg_hdr_type) +sizeof(msg_desc_type));

                /*lint -save -e{64,10,78,718,746,628,732,530} */
                    /* Initialize variable arguments */
                va_start (arg_list, num_args);
                /*lint -restore */
                /* Store arguments from variable list. */
                /*lint -save -e{64,10,78,718,746,628,732,530} */
                for (i = 0; i < num_args; i++)
                {
                    args[i] = va_arg (arg_list, uint32);
                }

                /*lint -restore */
                /* Reset variable arguments */
                va_end (arg_list);


                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

                /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
                stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif
        if MSG_DIAGBUF(stream_id,stm_enabled)
        {
            msg = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, 0, 0);

            if (msg)
            {
                /*-----------------------------------------------------------------------
                    Store the arguments in the buffer.
                -----------------------------------------------------------------------*/
                args = MSG_LARGE_ARGS(msg->args);

            /*lint -save -e{64,10,78,718,746,628,732,530} */
                /* Initialize variable arguments */
                va_start (arg_list, num_args);
            /*lint -restore */

            /* Store arguments from variable list. */
            /*lint -save -e{64,10,78,718,746,628,732,530} */
                for (i = 0; i < num_args; i++)
                {
                    args[i] = va_arg (arg_list, uint32);
                }               /* for */
            /*lint -restore */
            /* Reset variable arguments */
                va_end (arg_list);

                diagbuf_commit (msg);
            }
        }
    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

/*lint -save -e{64,10,718,746,628} */
    /* Initialize variable arguments */
    va_start (arg_list, num_args);

    /* Store arguments from variable list. */
    for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
    {
      f3_args[i] = va_arg (arg_list, uint32);
    }               /* for */

    /* Reset variable arguments */
    va_end (arg_list);
/*lint -restore */
    msg_save_trace((const void *)const_blk, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_STANDARD,0,NULL,0);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send_var() */

/*===========================================================================
DESCRIPTION
  This function is similar to MSG_SEND_VAR function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).
===========================================================================*/
void
msg_send_var_ext (const msg_const_type * const_blk, uint32 version, void* parameter, uint32 num_args, ...)
{
  uint32 *args = NULL;
  msg_ext_store_type *msg= NULL;
  msg_multi_client_store_type *msg_ext= NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  byte stream_id = 0;
  unsigned int i;
  uint32 id = 0;
  uint32 *id_ptr = NULL;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */
  uint8 args_count = MIN(num_args, MAX_VAR_ARGS);


#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[9];  // Assuming at most 9 arguments
#endif /* FEATURE_SAVE_DEBUG_TRACE */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
        switch(version)
        {
            case DIAGBUF_VERSION_0:
            {
                id=0;
                break;
            }
            case DIAGBUF_VERSION_1:
            {
                if (parameter == NULL)
                {
                    return;
                }
                id_ptr = (uint32 *)parameter;
                id = *id_ptr;
                break;
            }
        }
        stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_msg_prep(const_blk, num_args, &pkt_length, version, id);

            if (stm_msg)
            {

                if(version == DIAGBUF_VERSION_1)
                {
                    args = (uint32*) ( (byte*)stm_msg + FPOS(msg_multi_client_store_type, msg)+ sizeof(msg_hdr_type) +sizeof(msg_desc_type));
                }
                else
                {
                    args = (uint32*) ((byte*)stm_msg + sizeof(msg_hdr_type) +sizeof(msg_desc_type));
                }

                /*lint -save -e{64,10,78,718,746,628,732,530} */
                    /* Initialize variable arguments */
                va_start (arg_list, num_args);
                /*lint -restore */
                /* Store arguments from variable list. */
                /*lint -save -e{64,10,78,718,746,628,732,530} */
                for (i = 0; i < num_args; i++)
                {
                    args[i] = va_arg (arg_list, uint32);
                }

                /*lint -restore */
                /* Reset variable arguments */
                va_end (arg_list);


                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

                /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
                stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif
        if MSG_DIAGBUF(stream_id,stm_enabled)
        {
            void *msg_ptr;
            msg_ptr = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, version, id);

            if(msg_ptr)
            {
                if(version == DIAGBUF_VERSION_1)
                {
                    msg_ext= (msg_multi_client_store_type*) msg_ptr;

                    args = msg_ext->msg.args;
                }
                else
                {
                    msg= (msg_ext_store_type*)msg_ptr;

                    args = msg->args;
                }

                va_start(arg_list, num_args);
                /* Store arguments from variable list. */
                for (i = 0; i < args_count; i++)
                {
                    args[i] = va_arg(arg_list, uint32);
                }

                va_end(arg_list);

                diagbuf_commit (msg_ptr);
            }
        }
    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)
    /* Store arguments from variable list. */
    if(args)
    {
        for (i = 0; i < args_count; i++)
        {
            f3_args[i] = args[i];
        }
    }
    else
    {
        /* Initialize variable arguments */
        va_start(arg_list, num_args);
        /* Store arguments from variable list. */
        for (i = 0; i < args_count; i++)
        {
            f3_args[i] = va_arg(arg_list, uint32);
        }
        va_end(arg_list);
    }

    if(version==DIAGBUF_VERSION_1)
    {
        msg_save_trace((const void *)const_blk, f3_args, args_count, (void *)msg_ext, DIAG_F3_TRACE_STANDARD,(uint8)version,parameter,0);
    }
    else
    {
        msg_save_trace((const void *)const_blk, f3_args, args_count, (void *)msg, DIAG_F3_TRACE_STANDARD,0,NULL,0);
    }


#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send_var() */


#ifdef FEATURE_DIAG_STM
static uint32 stm_msg_drop_delta = 0;
static uint32 stm_msg_dropped = 0;

/*===========================================================================

FUNCTION DIAG_STM_MSG_HDR_PREP

DESCRIPTION
  This helper functions fills out commonly used fields in the msg header
  for a message packet being sent out over STM.

DEPENDENCIES

===========================================================================*/
static void diag_stm_msg_hdr_prep(msg_hdr_type *msg_hdr, unsigned int num_args)
{
  /* This is common to all F3s being sent over STM */
  msg_hdr->num_args = num_args;
  msg_get_time((qword*)&(msg_hdr->ts));
#ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
  msg_hdr->ts_type = MSG_TS_TYPE_GW;
#else
  msg_hdr->ts_type = MSG_TS_TYPE_CDMA_FULL;
#endif
  msg_hdr->drop_cnt = (uint8) ((stm_msg_drop_delta > 255) ? 255 : stm_msg_drop_delta);
  stm_msg_drop_delta = 0;   /* Reset delta drop count */
}

/*===========================================================================

FUNCTION DIAG_STM_MSG_PREP

DESCRIPTION
  This functions allocates a diag STM message packet and populates all packet
  fields, except for arguments. It is used for packets being sent over STM,
    which must be fully expanded in the context of the caller.

DEPENDENCIES

===========================================================================*/
void *diag_stm_msg_prep(const msg_const_type * const_blk, unsigned int num_args, uint32 *len, uint32 version, uint32 id)
{
  uint32 msg_size = 0;
  msg_multi_client_store_type *msg_ext = NULL;
  void * msg;
  char *fmt_ptr;
  int filename_len, str_len;

  fmt_ptr = (char *) const_blk->fmt;
  filename_len = MIN(MSG_MAX_STRLEN,(int)strlen(const_blk->fname));
  str_len= MIN(MSG_MAX_STRLEN, (int)strlen(const_blk->fmt));

  msg_size = STM_EXT_MSG_PKT_SIZE(str_len + filename_len + 2, num_args);


  if(version == DIAGBUF_VERSION_1)
  {
    /* Add additional space for multi-client header for packet if required */
    msg_size += FPOS(msg_multi_client_store_type, msg);
  }

  /* Allocate packet for STM */
  msg = (void *)diag_stm_msg_alloc(msg_size);

  /* Populate common fields in buffer */
  if (msg)
  {
    msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
    char *copy_ptr = NULL;
    uint32 copy_buf_size = msg_size;

    if(version == DIAGBUF_VERSION_1)
    {
      /* Fill out the header for multiclient msg pkts */
      msg_ext = (msg_multi_client_store_type*) msg;
      msg_ext->cmd_code = DIAG_CMD_EXT_F;
      msg_ext->version = DIAGBUF_VERSION_1;
      msg_ext->id = id;
      msg_ext->proc_id = DIAG_MY_PROC_ID;
      msg_ext->msg.hdr.cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;
      /* Update msg pointer to correct location */
      msg_hdr = &(msg_ext->msg.hdr);
    }
    else
    {
      /* No additional header required */
      msg_hdr = (msg_hdr_type *)msg;
    }

    /*  Set packet length to size allocated */
    *len = msg_size;

    /* Set msg_header fields */
    diag_stm_msg_hdr_prep(msg_hdr, num_args);

    /* Copy descriptor fields */
    copy_ptr = (char*)msg_hdr + sizeof(msg_hdr_type);
    copy_buf_size -= sizeof(msg_hdr_type);
    memscpy(copy_ptr, copy_buf_size,
            (void*)(&const_blk->desc), sizeof(msg_desc_type));
    copy_ptr += sizeof(msg_desc_type);
    copy_buf_size -= sizeof(msg_desc_type);

    msg_hdr->cmd_code = DIAG_EXT_MSG_F;

    /*  Reserve space for arguments */
    copy_ptr += (num_args*4);
    copy_buf_size -= (num_args*4);

    /* Copy format string */

    memscpy(copy_ptr,copy_buf_size,(void*)fmt_ptr, str_len);
    copy_ptr += str_len;

    /* NULL terminate format string */
    *copy_ptr++ = '\0';

    copy_buf_size -= (str_len + 1);

    /* Copy file name */
    memscpy(copy_ptr,copy_buf_size,(void*)(const_blk->fname),filename_len);
    copy_ptr += filename_len;

    /* NULL terminate filename */
    *copy_ptr++ = '\0';
  }
  else
  {
    /* Unable to allocate memory */

    /* Set packet length to zero */
    *len = 0;

    stm_msg_drop_delta++;
    if(stm_msg_dropped < MAX_VALUE_UINT32)
    {
      stm_msg_dropped++;
    }
  }
  return msg;
}
#endif /* FEATURE_DIAG_STM */

/*===========================================================================

FUNCTION MSG_SPRINTF

DESCRIPTION

  This will queue a debug message in Extended Message Format.  The message
  will be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This will build a message sprintf diagnostic Message with var #
  of parameters.

  Do not call directly; use macro MSG_SPRINTF_* defined in msg.h

  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_sprintf (const msg_const_type * const_blk, ...)
{
  /* Declared union for future use */
  typedef union
  {
    msg_ext_type ext;
  } msg_sprintf_desc_type;

  msg_sprintf_desc_type *msg = NULL;

  char *str = NULL;   /* Used to copy the file name and fmt string to the msg */
  va_list arg_list;   /* ptr to the variable argument list */

  unsigned int int_cnt = 0;           /* Calculate the # args, to allocate buffer */
  unsigned int fname_length = 0;      /* Stores the file name along with '\0'     */
  unsigned int fmt_length = 0;        /* Stores the fmt length,'\0' and arg size  */
  unsigned int total_allocated = 0;   /* Total buffer allocated                   */
  unsigned int fmt_len_available = 0; /* Remaining buffer for format string       */
  boolean diagbuf_flag = TRUE;        /* Flag indicating that buffer came from diagbuf (and not STM) */
  uint32 pkt_length = 0;        /* Final packet length */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  fname_length = strlen (const_blk->fname) + 1;

  /* Copy the length of the fmt string along with MSG_LENGTH_OF_ARGS */
  fmt_length = strlen (const_blk->fmt) + 1 + MSG_FMT_STR_ARG_SIZE;

  /* Calculate # of arguments to ensure enough space is allocated. */
  int_cnt = sizeof (msg_desc_type) - FSIZ (msg_ext_store_type, const_data_ptr) +
            fmt_length + fname_length;

  /* Calculates number of uint32 required    */
  int_cnt = (int_cnt + sizeof (uint32) - 1) / sizeof (uint32);

  if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
      byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);
        /*  Allocates the buffer required, fills in the header  */

      if MSG_DIAGBUF(stream_id, stm_enabled)
      {
          /* Need to allocate from diagbuf */
          msg = (msg_sprintf_desc_type *) msg_send_prep (const_blk,  int_cnt, 0, FALSE, stream_id, 0, 0);
      }
#ifdef FEATURE_DIAG_STM
      else if MSG_STM(stream_id, stm_enabled )
      {
          /* Allocate space for STM buffer */
          msg = (msg_sprintf_desc_type*)diag_stm_msg_alloc(STM_EXT_MSG_PKT_SIZE(fmt_length,int_cnt));

          if(msg)
          {
              msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
              diag_stm_msg_hdr_prep(msg_hdr, int_cnt);
          }
          diagbuf_flag = FALSE;
      }
#endif /* FEATURE_DIAG_STM */

        if (msg)
        {
            /* Queue a debug message in Extended Message Format. */
            msg->ext.hdr.cmd_code = DIAG_EXT_MSG_F;

            /* This function embedds the argument in the string itself.
            ** Hence the num_args is assigned 0 */
            msg->ext.hdr.num_args = 0;

            /* Copy 'desc'. */
            msg->ext.desc = const_blk->desc;

            /* Copy the format string where the argument list would start.
                 Since there are no arguments, the format string starts in the 'args'
                 field. */
            str = (char *) msg->ext.args;

            /* Calculate the buffer allocated */
            total_allocated = FPOS (msg_ext_store_type, args) + int_cnt *
                                                        FSIZ (msg_ext_store_type, args[0]);

            /* Calculate the buffer left to copy the format string */
            fmt_len_available = total_allocated - (FPOS (msg_ext_type, args) + fname_length);

            if( fmt_len_available < fmt_length)
            {
                 fmt_length = fmt_len_available;
            }

            /* Initialize variable argument list */
    /*lint -save -e{64,718,746,516} */
            va_start(arg_list, const_blk);
    /*lint -restore */

            /* Copy the format string with arguments */
    #if defined __GNUC__
            (void) vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
             str[fmt_length-1] = '\0';
    #else
            (void) _vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
            str[fmt_length-1] = '\0';
    #endif

        /* Reset variable arguments */
            va_end(arg_list);

            /* Move the str pass the format string, strlen excludes the terminal
            ** NULL hence 1 is added to include NULL. */
            str += strlen((const char *)str) + 1;

            /* Copy the filename and strlcpy adds the ending NULL terminal */
            /*Since the str dest size is computed before, there would not be any buffer overflow */
            (void)strlcpy(str, (const char *)const_blk->fname, fname_length );

            /* Move the str pass the filename, strlen excludes the terminal NULL
            ** hence 1 is added to include NULL. */
            str += strlen((const char *)str) + 1;

            /* str is now pointing to the byte after the last valid byte. str - msg
             gives the total length required. */
          pkt_length = (uint32)(str - (char *)msg);

#ifdef FEATURE_DIAG_STM
          if MSG_STM(stream_id, stm_enabled)
          {
              /* Send data to STM */
              diag_stm_write_pkt(msg,pkt_length);
          }

              if (!diagbuf_flag)
              {
                  /* Data not allocated from diagbuf, so free data */
                  diag_stm_msg_free(msg);

              /* clear stream_stm bit to avoid resending */
              stream_id &= ~DIAG_STREAM_STM;
          }
          else
          {
            diagbuf_write_stream_id (msg,stream_id);
          }
#endif /* FEATURE_DIAG_STM */

          if (diagbuf_flag)
          {
              /* Use diagbuf_shorten to release the buffer */
              diagbuf_shorten (msg, pkt_length);
            diagbuf_commit (msg);
        }
    }
    }

  return;
}  /* msg_sprintf */

/*===========================================================================

FUNCTION msg_sprintf_ext

DESCRIPTION
  This function is similar to msg_sprintf function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

PARAMETERS
  Version            Indicates the version of the packet.
  parameter          Based on the version value, parameter can be interpreted as follows-
                       Version      Pointer
                         0          NULL
                         1          uint32*
  num_args           Number of arguments being passed in
 *const_blk          Static constant constructed using the different
                     xx_msg_const macros

  Do not call directly; use macro MSG_SPRINTF_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_sprintf_ext (uint32 version, void * parameter, uint32 num_args,const msg_const_type * const_blk, ...)
{
  /* Declared union for future use */
  typedef union
  {
    msg_ext_type ext;
  } msg_sprintf_desc_type;

  msg_sprintf_desc_type *msg = NULL;
  msg_sprintf_ext_desc_type *msg_ext = NULL;
  void *msg_ptr = NULL; /* pointer to commit/send */

  char *str = NULL;   /* Used to copy the file name and fmt string to the msg */
  va_list arg_list;     /* ptr to the variable argument list */

  unsigned int int_cnt = 0;           /* Calculate the # args, to allocate buffer */
  unsigned int fname_length = 0;      /* Stores the file name along with '\0'     */
  unsigned int fmt_length = 0;        /* Stores the fmt length,'\0' and arg size  */
  unsigned int total_allocated = 0;   /* Total buffer allocated                   */
  unsigned int fmt_len_available = 0; /* Remaining buffer for format string       */
  uint32 *id = NULL;
  byte stream_id = 0;
  uint32 pkt_length = 0;
  boolean diagbuf_flag = TRUE;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  /* Exit immediately if no masks are enabled */
  if(!msg_enabled_mask)
    return;

  stream_id = msg_stream_check(&const_blk->desc);
    /*  Allocates the buffer required, fills in the header  */

  fname_length = strlen (const_blk->fname) + 1;

  /* Copy the length of the fmt string along with MSG_LENGTH_OF_ARGS */
  fmt_length = strlen (const_blk->fmt) + 1 + MSG_FMT_STR_ARG_SIZE;

        /* Initialize variable argument list */
  /*lint -save -e{64,718,746,516} */
      va_start(arg_list, const_blk);
  /*lint -restore */

  /* Calculate # of arguments to ensure enough space is allocated. */
    int_cnt = sizeof (msg_desc_type) - FSIZ (msg_ext_store_type, const_data_ptr) +
              fmt_length + fname_length;

  /* Calculates number of uint32 required    */
  int_cnt = (int_cnt + sizeof (uint32) - 1) / sizeof (uint32);

  if (version == DIAGBUF_VERSION_0)
  {
    if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      /*  Allocates the buffer required, fills in the header  */
      msg = (msg_sprintf_desc_type *) msg_send_prep (const_blk,  int_cnt, 0, FALSE, stream_id, 0, 0);
    }
#ifdef FEATURE_DIAG_STM
      else if MSG_STM(stream_id,stm_enabled)
      {
          /* Allocate space for STM buffer */
          msg = (msg_sprintf_desc_type*)diag_stm_msg_alloc(STM_EXT_MSG_PKT_SIZE(fmt_length,int_cnt));

      if (msg)
      {
        msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
        diag_stm_msg_hdr_prep(msg_hdr, int_cnt);
      }
          diagbuf_flag = FALSE;
      }
#endif /* FEATURE_DIAG_STM */
    /* Pointer will be used later to send/commit the message */
    msg_ptr = msg;

  }
  else if (version == DIAGBUF_VERSION_1)
  {
    if (parameter == NULL)
    {
      return;
    }
    id = (uint32 *) parameter;
    if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      msg_ext = (msg_sprintf_ext_desc_type *) msg_send_prep (const_blk,  int_cnt, 0, FALSE, stream_id, version, *id);
    }
#ifdef FEATURE_DIAG_STM
      else if MSG_STM(stream_id,stm_enabled)
      {
        uint32 pkt_size = STM_EXT_MSG_PKT_SIZE(fmt_length,int_cnt);
        pkt_size += FPOS(msg_multi_client_store_type, msg);

        /* Allocate space for STM buffer */
          msg_ext = (msg_sprintf_ext_desc_type*)diag_stm_msg_alloc(pkt_size);

        if (msg_ext)
        {
            msg_hdr_type *msg_hdr;

            /* Fill out the header for multiclient msg pkts */
            msg_ext->ext_hdr.cmd_code = DIAG_CMD_EXT_F;
            msg_ext->ext_hdr.version = DIAGBUF_VERSION_1;
            msg_ext->ext_hdr.id = *id;
            msg_ext->ext_hdr.proc_id = DIAG_MY_PROC_ID;

            /* Update msg pointer to correct location */
            msg_hdr = &(msg_ext->msg.hdr);

            diag_stm_msg_hdr_prep(msg_hdr, int_cnt);
        }

      diagbuf_flag = FALSE;
    }
#endif /* FEATURE_DIAG_STM */
    /* Pointer will be used later to send/commit the message */
    msg_ptr = msg_ext;

    if (msg_ext)
    {
      msg = (msg_sprintf_desc_type *) &(msg_ext->msg);
    }
  }

  if (msg)
  {
    /* Queue a debug message in Extended Message Format. */
    msg->ext.hdr.cmd_code = DIAG_EXT_MSG_F;

    /* This function embedds the argument in the string itself.
     ** Hence the num_args is assigned 0 */
    msg->ext.hdr.num_args = 0;

    /* Copy 'desc'. */
    msg->ext.desc = const_blk->desc;

    /* Copy the format string where the argument list would start.
       Since there are no arguments, the format string starts in the 'args'
       field. */
    str = (char *) msg->ext.args;

    /* Calculate the buffer allocated */
    total_allocated = FPOS (msg_ext_store_type, args) + int_cnt *
                            FSIZ (msg_ext_store_type, args[0]);

    /* Calculate the buffer left to copy the format string */
    fmt_len_available = total_allocated - (FPOS (msg_ext_type, args) + fname_length);

    if( fmt_len_available < fmt_length)
    {
      fmt_length = fmt_len_available;
    }

    /* Copy the format string with arguments */
  #if defined __GNUC__
    (void) vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
    str[fmt_length-1] = '\0';
  #else
    (void) _vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
    str[fmt_length-1] = '\0';
  #endif

    /* Reset variable arguments */
    va_end(arg_list);

    /* Move the str pass the format string, strlen excludes the terminal
    ** NULL hence 1 is added to include NULL. */
    str += strlen((const char *)str) + 1;

    /* Copy the filename and strlcpy adds the ending NULL terminal */
    /*Since the str dest size is computed before, there would not be any buffer overflow */
    (void)strlcpy(str, (const char *)const_blk->fname, fname_length );

    /* Move the str pass the filename, strlen excludes the terminal NULL
    ** hence 1 is added to include NULL. */
    str += strlen((const char *)str) + 1;

    /* str is now pointing to the byte after the last valid byte. str - msg
       gives the total length required. */
    pkt_length = (uint32)(str - (char*)msg_ptr);

#ifdef FEATURE_DIAG_STM
    if MSG_STM(stream_id,stm_enabled)
    {
        /* Send data to STM */
        diag_stm_write_pkt(msg_ptr,pkt_length);
    }

    if (!diagbuf_flag)
    {
        /* Data not allocated from diagbuf, so free data */
        diag_stm_msg_free(msg_ptr);

        /* clear stream_stm bit to avoid resending */
        stream_id &= ~DIAG_STREAM_STM;
    }
    else
    {
        diagbuf_write_stream_id (msg_ptr,stream_id);
    }
#endif /* FEATURE_DIAG_STM */
    if(diagbuf_flag)
    {
        diagbuf_shorten (msg_ptr, pkt_length);
        diagbuf_commit (msg_ptr);
    }
  }

  return;
}  /* msg_sprintf_ext */

/*===========================================================================

FUNCTION qsr_msg_send_prep

DESCRIPTION
  This message performs the common operations used by all messages.
  It allocates, fills in all data except arguments, and returns a pointer
  to the allocated message buffer.  It also handles message statisitics.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
static msg_qsr_store_type *
qsr_msg_send_prep (const msg_qsr_const_type * const_blk,unsigned int num_args, uint32 isHash, byte stream_id)
{

  msg_qsr_store_type *msg = NULL;
  unsigned int rsp_len = FPOS (msg_qsr_store_type, args) + num_args * FSIZ (msg_qsr_store_type,
                               args[0]);

  if (stream_id)
  {
    /*-----------------------------------------------------------------------
      Get a pointer to a buffer.  If it's a NULL pointer, there aren't any
      free buffers.  No effort is made to save higher priority messages in
      favor of lower priority messages, as message flooding hides enough
      details to make the usable margin on such a feature too small for
      implementation.
    -----------------------------------------------------------------------*/
    msg = (void *) diagbuf_alloc (DIAG_QSR_EXT_MSG_TERSE_F, rsp_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, 0);

    if (msg)
    {
        diagbuf_write_stream_id (msg,stream_id);
/*lint -save -e{545} Suspicious use of & */
      msg_get_time((qword*)&(msg->hdr.ts));
/*lint -restore */

#ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
      msg->hdr.ts_type = MSG_TS_TYPE_GW;
#else
      msg->hdr.ts_type = MSG_TS_TYPE_CDMA_FULL;
#endif

      msg->hdr.num_args = (uint8)num_args;

      msg->hdr.drop_cnt = (uint8) ((qsr_msg_drop_delta > 255) ? 255 : qsr_msg_drop_delta);

      qsr_msg_drop_delta = 0;   /* Reset delta drop count */

      /*-----------------------------------------------------------------------
        Set the pointer to the constant blk, to be expanded in DIAG context.
      -----------------------------------------------------------------------*/
      msg->qsr_const_data_ptr = const_blk;
      msg->qsr_flag = QSR_BITFLAG_IS_HASH & isHash;
    }
    else
    {
      qsr_msg_drop_delta++;
      qsr_msg_dropped++;        /* count up one more ignored message */
    }
  }

  return msg;
}               /* qsr_msg_send_prep() */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send (const msg_qsr_const_type * const_blk, uint32 isHash)
{
  msg_qsr_store_type *msg = NULL;
  const unsigned int num_args = 0;  /* # of message arguments */

  if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);

    /* Need to allocate from diagbuf and commit to buffer */
    msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

    if (msg)
    {
        diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

   if (isHash)
   {
    msg_save_trace((const void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_QSHRINK,0,NULL,0);
   }
   else
   {
     msg_save_trace((const void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_QSHRINK20,0,NULL,0);
   }

#endif /* FEATURE_SAVE_DEBUG_TRACE */

} /* qsr_int_msg_send */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send_1 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 1;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
        byte stream_id = 0;

        stream_id = msg_stream_check(&const_blk->desc);

        /* Need to allocate from diagbuf and commit to buffer */
        msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

        if (msg)
        {
            /*-----------------------------------------------------------------------
                Store the arguments in the buffer.
            -----------------------------------------------------------------------*/
            args = msg->args;

            args[0] = xx_arg1;

            diagbuf_commit (msg);
        }

    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    
    if (isHash)
    {
    msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_QSHRINK,0,NULL,0);
    }
    else
    {
      msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_QSHRINK20,0,NULL,0);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_int_msg_send_1 */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send_2 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1, uint32 xx_arg2)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
        byte stream_id = 0;

        stream_id = msg_stream_check(&const_blk->desc);

        /* Need to allocate from diagbuf and commit to buffer */
        msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

        if (msg)
        {
            /*-----------------------------------------------------------------------
                Store the arguments in the buffer.
            -----------------------------------------------------------------------*/
            args = MSG_LARGE_ARGS(msg->args);
            args[0] = xx_arg1;
            args[1] = xx_arg2;
            diagbuf_commit (msg);
        }
    }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    
    if (isHash)
    {
    msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_QSHRINK,0,NULL,0);
    }
    else
    {
      msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_QSHRINK20,0,NULL,0);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_int_msg_send_2 */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send_3 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[3];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
      byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);

      /* Need to allocate from diagbuf and commit to buffer */
      msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

      if (msg)
      {
        /*-----------------------------------------------------------------------
            Store the arguments in the buffer.
        -----------------------------------------------------------------------*/
        args = MSG_LARGE_ARGS(msg->args);
        args[0] = xx_arg1;
        args[1] = xx_arg2;
        args[2] = xx_arg3;
        diagbuf_commit (msg);
      }
   }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;
    
    if (isHash)
    {
    msg_save_trace((const void *)const_blk, f3_args, 3, (void *)msg, DIAG_F3_TRACE_QSHRINK,0,NULL,0);
    }
    else
    {
      msg_save_trace((const void *)const_blk, f3_args, 3, (void *)msg, DIAG_F3_TRACE_QSHRINK20,0,NULL,0);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_int_msg_send_3 */


/*===========================================================================

FUNCTION QSR_MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void qsr_msg_send ( const msg_qsr_const_type * const_blk)
{
  qsr_int_msg_send(const_blk, TRUE);

} /* qsr_msg_send */

/*===========================================================================

FUNCTION QSR_MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/

void qsr_msg_send_1 ( const msg_qsr_const_type * const_blk,uint32 xx_arg1)
{
  qsr_int_msg_send_1(const_blk, TRUE, xx_arg1);

} /* qsr_msg_send_1 */

/*===========================================================================

FUNCTION QSR_MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/

void qsr_msg_send_2 ( const msg_qsr_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  qsr_int_msg_send_2(const_blk, TRUE, xx_arg1, xx_arg2);

} /* qsr_msg_send_2 */

/*===========================================================================

FUNCTION QSR_MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void qsr_msg_send_3 ( const msg_qsr_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  qsr_int_msg_send_3(const_blk, TRUE, xx_arg1, xx_arg2, xx_arg3);

} /* qsr_msg_send_3 */


/*===========================================================================

FUNCTION QSR_MSG_SEND_VAR

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with a variable number of
    arguments, specified by 'num_args'.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_msg_send_var (const msg_qsr_const_type * const_blk,uint32 num_args, ...)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
#endif /* FEATURE_SAVE_DEBUG_TRACE */

    if((msg_enabled_mask != 0) && (msg_service_init == TRUE))
    {
        byte stream_id = 0;

        stream_id = msg_stream_check(&const_blk->desc);

        /* Need to allocate from diagbuf and commit to buffer */
        msg = qsr_msg_send_prep (const_blk, num_args, TRUE, stream_id);

        if (msg)
        {
            /*-----------------------------------------------------------------------
                Store the arguments in the buffer.
            -----------------------------------------------------------------------*/
            args = MSG_LARGE_ARGS(msg->args);

            /*lint -save -e{64,10,78,718,746,628,732,530} */
            /* Initialize variable arguments */
            va_start (arg_list, num_args);
            /*lint -restore */

            /* Store arguments from variable list. */
            /*lint -save -e{64,10,78,718,746,628,732,530} */
            for (i = 0; i < num_args; i++)
            {
                args[i] = va_arg (arg_list, uint32);
            }               /* for */
            /*lint -restore */
            /* Reset variable arguments */
            va_end (arg_list);

            diagbuf_commit (msg);
        }
   }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

/*lint -save -e{64,10,718,746,628} */
    /* Initialize variable arguments */
    va_start (arg_list, num_args);

    /* Store arguments from variable list. */
    for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
    {
      f3_args[i] = va_arg (arg_list, uint32);
    }               /* for */

    /* Reset variable arguments */
    va_end (arg_list);
/*lint -restore */
    msg_save_trace((const void *)const_blk, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_QSHRINK,0,NULL,0);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* qsr_msg_send_var() */


/*===========================================================================

FUNCTION MSG_V2_SEND

DESCRIPTION
  This function is similar to QSR_MSG_SEND function, except the msg value. The msg value in const_blk may be 
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h
  
DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send ( const msg_v2_const_type * const_blk)
{
  qsr_int_msg_send((msg_qsr_const_type *)const_blk, MSG_V2_CHECK_IS_HASH(const_blk->msg));
  
} /* msg_v2_send */

/*===========================================================================

FUNCTION MSG_V2_SEND_1

DESCRIPTION
  This function is similar to QSR_MSG_SEND_1 function, except the msg value. The msg value in const_blk may be 
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_1 ( const msg_v2_const_type * const_blk,uint32 xx_arg1)
{
  qsr_int_msg_send_1((msg_qsr_const_type *)const_blk, 
    MSG_V2_CHECK_IS_HASH(const_blk->msg),xx_arg1);

} /* msg_v2_send_1 */


/*===========================================================================

FUNCTION MSG_V2_SEND_2

DESCRIPTION
  This function is similar to QSR_MSG_SEND_2 function, except the msg value. The msg value in const_blk may be 
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_2 ( const msg_v2_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  qsr_int_msg_send_2((msg_qsr_const_type *)const_blk, 
    MSG_V2_CHECK_IS_HASH(const_blk->msg),xx_arg1, xx_arg2);

} /* msg_v2_send_2 */

/*===========================================================================

FUNCTION MSG_V2_SEND_3

DESCRIPTION
  This function is similar to QSR_MSG_SEND_3 function, except the msg value. The msg value in const_blk may be 
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_3 ( const msg_v2_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  qsr_int_msg_send_3((msg_qsr_const_type *)const_blk, 
    MSG_V2_CHECK_IS_HASH(const_blk->msg),xx_arg1,xx_arg2,xx_arg3);
    
} /* msg_v2_send_3 */


/*===========================================================================

FUNCTION MSG_V2_SEND_VAR

DESCRIPTION
  This function is similar to QSR_MSG_SEND_VAR function, except the msg value. The msg value in const_blk may be 
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_v2_send_var (const msg_v2_const_type * const_blk,uint32 num_args, ...)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
  msg_qsr_const_type *qsr_const_blk = (msg_qsr_const_type *)const_blk;

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
#endif

  if(msg_enabled_mask != 0)
  {
     byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = qsr_msg_send_prep (qsr_const_blk, num_args,MSG_V2_CHECK_IS_HASH(const_blk->msg),stream_id);
  
    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = MSG_LARGE_ARGS(msg->args);
  
  /*lint -save -e{64,10,78,718,746,628,732,530} */
      /* Initialize variable arguments */
      va_start (arg_list, num_args);
  /*lint -restore */
  
      /* Store arguments from variable list. */
  /*lint -save -e{64,10,78,718,746,628,732,530} */
      for (i = 0; i < num_args; i++)
      {
        args[i] = va_arg (arg_list, uint32);
      }               /* for */
  /*lint -restore */
      /* Reset variable arguments */
      va_end (arg_list);
  
      diagbuf_commit (msg);
    }
  }
  
#if defined (FEATURE_SAVE_DEBUG_TRACE)

/*lint -save -e{64,10,718,746,628} */
  /* Initialize variable arguments */
  va_start (arg_list, num_args);

  /* Store arguments from variable list. */
  for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
  {
    f3_args[i] = va_arg (arg_list, uint32);
  }               /* for */

  /* Reset variable arguments */
  va_end (arg_list);
/*lint -restore */
  msg_save_trace((const void *)const_blk, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_QSHRINK20,0,NULL,0);
#endif

} /* msg_send_var */

#ifdef FEATURE_DIAG_STM

/*===========================================================================

FUNCTION DIAG_STM_QSR_V4_MSG_PREP

DESCRIPTION
  This functions allocates a diag STM QSR4 message packet and populates all packet
  fields, except for arguments. It is used for packets being sent over STM,
  which must be fully expanded in the context of the caller.

PARAMETERS
 msg_v4_index - Index to the qshrink_4 table
 qsr_hash - qshrink4 hash value
 num_args - number of arguments
 stream_id - stream id of the msg
 arg_bytes - maximum number if bytes needed to store arguments in the msg
 len - length of the msg
 version - version number of the msg
 id - parameter for version 1 msg

===========================================================================*/
void *diag_stm_qsr_v4_msg_prep (uint32 qsr_hash,unsigned int num_args,
                                 byte stream_id, uint8 arg_bytes ,uint32 *len, uint32 version, uint32 id)
{
  msg_v4_store_type *msg = NULL;
  msg_v4_multi_client_store_type *msg_ext = NULL;
  void *msg_ptr = NULL;
  unsigned int rsp_len = FPOS (msg_v4_store_type, args) + num_args * arg_bytes;

   if ((qsr_hash >= (uint32)qsr_4_0_msg_aligned_start_address) &&
       (qsr_hash < (uint32)qsr_4_0_msg_unaligned_start_address))
  {
    /* Aligned Address Mapping */
    qsr_hash = (qsr_hash - (uint32)qsr_4_0_msg_aligned_start_address) >> 3;
  }
  else if (qsr_hash < (uint32)qsr_4_0_msg_end_address)
  {
    /* Unaligned address mapping */
    qsr_hash = (qsr_hash - (uint32)qsr_4_0_msg_unaligned_start_address) >> 3;
  }
  else
  {
    /* Message not part of QShrink  - Error to be investigated */
    return NULL;
  }

  if (version == DIAGBUF_VERSION_1)
  {
    rsp_len += FPOS(msg_v4_multi_client_store_type, msg);
  }

    msg_ptr = (void *) diag_stm_msg_alloc(rsp_len);

    if (msg_ptr)
    {

      msg = (msg_v4_store_type*) msg_ptr;
      if (version == DIAGBUF_VERSION_1)
      {
        /*Fill out the header for DSDA msg pkts*/
        msg_ext = (msg_v4_multi_client_store_type*) msg_ptr;
        msg_ext->cmd_code = DIAG_CMD_EXT_F;
        msg_ext->version = DIAGBUF_VERSION_1;
        msg_ext->id = id;
        msg_ext->proc_id = DIAG_MY_PROC_ID;
        msg_ext->msg.hdr.cmd_code = DIAG_QSR4_EXT_MSG_TERSE_F;
        msg = &(msg_ext->msg);
      }
      else
      {
        msg_ext = NULL;
      }
      *len = rsp_len;
      /* Set msg_header fields */
      diag_stm_msg_hdr_prep((msg_hdr_type *)msg, (arg_bytes<<4)|num_args);
      msg->qsr_hash = qsr_hash;
      msg->dbase_guid[0] = msg_v4_dbase_guid[15];
      msg->dbase_guid[1] = msg_v4_dbase_guid[14];

      /* QSR message DIAG_QSR_EXT_MSG_TERSE_F */
      msg->hdr.cmd_code = DIAG_QSR4_EXT_MSG_TERSE_F;

    }
    else
    {
      /* Unable to allocate memory */

      /* Set packet length to zero */
      *len = 0;

      stm_msg_drop_delta++;
      stm_msg_dropped++;
    }

  return msg_ptr;
}               /* qsr_v4_msg_send_prep() */
#endif /* FEATURE_DIAG_STM */


/*===========================================================================

FUNCTION MSG_V4_STREAM_CHECK

DESCRIPTION
  This helper function performs a check to see which streams a given QSR3 message
  is enabled on. In cases where STM is enabled, a LISTENER stream bit is also set
  if listeners are enabled.

PARAMETERS
  msg_v4_index -  Index to the qshrink_4 table
===========================================================================*/
static byte msg_v4_stream_check(uint16 msg_v4_index)
{
  byte stream_id = 0;

  /* Check the runtime mask */
  if (msg_v4_index < msg_v4_ss_num_table_entries)
  {
    if ((msg_v4_ss_table[msg_v4_index].rt_mask) &
            msg_stream_ssid_tbl_mask[DIAG_STREAM_1 - 1][diag_cur_preset_id - 1])
    {
      stream_id |= DIAG_STREAM_1;
    }
#if defined(DIAG_STREAM_2_MASK_CHECK)
    if ((msg_v4_ss_table[msg_v4_index].rt_mask) &
            msg_stream_ssid_tbl_mask[DIAG_STREAM_2 - 1][DIAG_DCI_MASK_1 - 1])
    {
      stream_id |= DIAG_STREAM_2;
    }
#endif
  }

  return stream_id;
}


/*===========================================================================

FUNCTION qsr_v4_msg_send_prep

DESCRIPTION
  This message performs the common operations used by all messages.
  It allocates, fills in all data except arguments, and returns a pointer
  to the allocated message buffer.  It also handles message statisitics.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
static void *
qsr_v4_msg_send_prep (uint32 qsr_hash,unsigned int num_args, byte stream_id, uint8 arg_bytes, uint32 version, uint32 id)
{
  msg_v4_store_type *msg = NULL;
  msg_v4_multi_client_store_type *msg_ext = NULL;
  void *msg_ptr = NULL;
  unsigned int rsp_len = FPOS (msg_v4_store_type, args) + num_args * arg_bytes;

  if(arg_bytes > 4)
    return NULL;

  if (version == DIAGBUF_VERSION_1)
  {
    rsp_len += FPOS(msg_v4_multi_client_store_type, msg);
  }

  msg_ptr = (void *) diagbuf_alloc (DIAG_QSR4_EXT_MSG_TERSE_F, rsp_len, 0, DIAGBUF_ALLOC_REQUEST_MSG,0,0);

  if (msg_ptr)
  {

    diagbuf_write_stream_id (msg_ptr,stream_id);

    msg = (msg_v4_store_type*) msg_ptr;
  }
  else
  {
    qsr_msg_drop_delta++;
    qsr_msg_dropped++;        /* count up one more ignored message */
    return NULL;
  }

  if (version == DIAGBUF_VERSION_1)
  {
    /*Fill out the header for DSDA msg pkts*/
    msg_ext = (msg_v4_multi_client_store_type*) msg_ptr;
    msg_ext->cmd_code = DIAG_CMD_EXT_F;
    msg_ext->version = DIAGBUF_VERSION_1;
    msg_ext->id = id;
    msg_ext->proc_id = DIAG_MY_PROC_ID;
    msg_ext->msg.hdr.cmd_code = DIAG_QSR4_EXT_MSG_TERSE_F;
    msg = &(msg_ext->msg);
  }
  else
  {
    msg_ext = NULL;
  }
  /*-----------------------------------------------------------------------
    Get a pointer to a buffer.  If it's a NULL pointer, there aren't any
    free buffers.  No effort is made to save higher priority messages in
    favor of lower priority messages, as message flooding hides enough
    details to make the usable margin on such a feature too small for
    implementation.
  -----------------------------------------------------------------------*/
  if(msg)
  {
/*lint -save -e{545} Suspicious use of & */
    diag_time_get((unsigned long *)msg->hdr.ts);
/*lint -restore */

  #ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
    msg->hdr.ts_type = MSG_TS_TYPE_GW;
  #else
    msg->hdr.ts_type = MSG_TS_TYPE_CDMA_FULL;
  #endif

    msg->hdr.num_args = (uint8)((arg_bytes<<4)|num_args);

    msg->hdr.drop_cnt = (unsigned char) ((qsr_msg_drop_delta > 255) ? 255 : qsr_msg_drop_delta);

    qsr_msg_drop_delta = 0;   /* Reset delta drop count */

    /*-----------------------------------------------------------------------
      Set the pointer to the constant blk, to be expanded in DIAG context.
    -----------------------------------------------------------------------*/

    msg->qsr_hash = qsr_hash;
    /* Send last 2 bytes of QSHrink Database Guid as part of hash  */
    msg->dbase_guid[0] = msg_v4_dbase_guid[15];
    msg->dbase_guid[1] = msg_v4_dbase_guid[14];

  }

  if (version==DIAGBUF_VERSION_1)
  {
    return (void *)msg_ext;
  }
  else
  {
    return (void *)msg;
  }

  }               /* qsr_v4_msg_send_prep() */


/*===========================================================================

FUNCTION MSG_V4_SEND

DESCRIPTION
  This function is similar to QSR_MSG_SEND function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v4_send (const msg_v4_const_type * xx_msg_const_ptr)
{
  uint32 qsr_hash_v4 = (uint32)xx_msg_const_ptr;
  msg_v4_store_type *msg = NULL;
  const unsigned int num_args = 0;  /* # of message arguments */

  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */


  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;


  if(msg_enabled_mask != 0)
  {
    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
       (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      /* In modem we right shift by 13 due to 8K alignment maintained
         by Qshrink4 hash. In threadX, this 8K alignment is not maintained
         and right shifting by 3 is done on the suggestion of QShrink team */
  
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3;
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      /* Unaligned address mapping */
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
       /* Message not part of QShrink  - Error to be investigated */
       return;
    }
    stream_id = msg_v4_stream_check(xx_msg_index);
#ifdef FEATURE_DIAG_STM
    if MSG_STM(stream_id,stm_enabled)
    {
      uint32 pkt_length;
      void *stm_msg;

      /* Allocate memory for msg and populate non argument fields */
      stm_msg = diag_stm_qsr_v4_msg_prep(qsr_hash_v4, num_args, stream_id, 0, &pkt_length, 0, 0);

      if (stm_msg)
      {
                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
       }
    }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      msg = qsr_v4_msg_send_prep (qsr_hash_v4, num_args, stream_id, 0, 0, 0);
    }
    if (msg)
    {
      diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)
    msg_save_trace((const void *)qsr_hash_v4, NULL, 0, (void *)msg, DIAG_F3_TRACE_QSHRINK40, 0, NULL, 0);
#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* msg_v4_send */

/*===========================================================================

FUNCTION MSG_V4_SEND_EXT

DESCRIPTION
  This function is similar to MSG_V4_SEND function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v4_send_ext (const msg_v4_const_type * xx_msg_const_ptr,uint32 version, void *parameter)
{
  uint32 qsr_hash_v4 = (uint32)xx_msg_const_ptr;
  void *msg = NULL;
  const unsigned int num_args = 0;  /* # of message arguments */
  uint32 id = 0;
	
    boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  #ifdef DIAG_STREAM_2_MASK_CHECK
  uint32 rt_mask_ctrl = 0;
  boolean valid_ssid_ctrl = FALSE;
  #endif

  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;

  if(version == DIAGBUF_VERSION_1)
  {
    if(parameter == NULL )
      return;
    else
      id = *((uint32 *) parameter);
  }


  if(msg_enabled_mask != 0)
  {
    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
       (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3;
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      /* Unaligned address mapping */
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
       /* Message not part of QShrink  - Error to be investigated */
       return;
    }
    stream_id = msg_v4_stream_check(xx_msg_index);

#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_qsr_v4_msg_prep(qsr_hash_v4, num_args, stream_id, 0, &pkt_length, version, id);

            if (stm_msg)
            {
                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

                /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
                stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
         msg = qsr_v4_msg_send_prep (qsr_hash_v4,num_args, stream_id, 0,version, id);
      }

      if (msg)
      {
        diagbuf_commit (msg);
      }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)
    msg_save_trace((const void *)qsr_hash_v4, NULL, 0, (void *)msg, DIAG_F3_TRACE_QSHRINK40,(uint8)version, parameter, 0);
#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* msg_v4_send_ext */

/*===========================================================================

FUNCTION MSG_V4_SEND_1

DESCRIPTION
  This function is similar to QSR_MSG_SEND_1 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v4_send_1 (const msg_v4_const_type * xx_msg_const_ptr,uint32 xx_arg1)
{
  uint32 qsr_hash_v4 = (uint32)xx_msg_const_ptr;
  msg_v4_store_type *msg = NULL;
  uint8 *args = NULL;
  const unsigned int num_args = 1;  /* # of message arguments */

  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */


#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;
  uint8 arg_bytes = 0;

  /*check the actual byte the xx_arg1 needed*/
  arg_bytes =  qsr_v4_check_bytes(arg_bytes,xx_arg1);

  if(msg_enabled_mask != 0)
  {
    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
       (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3; 
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      /* Unaligned address mapping */
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
       /* Message not part of QShrink  - Error to be investigated */
       return;
    }

    stream_id = msg_v4_stream_check(xx_msg_index);
#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;


            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_qsr_v4_msg_prep(qsr_hash_v4,num_args, stream_id, arg_bytes, &pkt_length, 0, 0);

            if (stm_msg)
            {
                /* set up args in struct */
              {
                    uint8* arg_ptr = (uint8 *)stm_msg;
                    arg_ptr += FPOS(msg_v4_store_type, args);

                qsr_v4_copy_args(arg_ptr,(uint8 *)&xx_arg1, arg_bytes);
              }

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);

              /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
              stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
        msg = qsr_v4_msg_send_prep (qsr_hash_v4,num_args, stream_id, arg_bytes, 0, 0);
      }
      if (msg)
      {
        /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
        args = (uint8*)msg->args;
        qsr_v4_copy_args(args,(uint8 *)&xx_arg1, arg_bytes);
        diagbuf_commit (msg);
      }
    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)
    f3_args[0] = xx_arg1;

    msg_save_trace((const void *)qsr_hash_v4, f3_args, 1, (void *)msg, DIAG_F3_TRACE_QSHRINK40, 0, NULL, arg_bytes);
#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* msg_v4_send_1 */


/*===========================================================================

FUNCTION MSG_V4_SEND_1_EXT

DESCRIPTION
  This function is similar to MSG_V4_SEND_1 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v4_send_1_ext(const msg_v4_const_type *xx_msg_const_ptr, uint32 version, void *parameter, uint32 xx_arg1)
{
  uint32 qsr_hash_v4 = (uint32)xx_msg_const_ptr;
  msg_v4_store_type *msg = NULL;
  const unsigned int num_args = 1;  /* # of message arguments */
  uint32 id = 0;
  
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */


#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */


  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;
  uint8 arg_bytes = 0;

  /*check the actual byte the xx_arg1 needed*/
  arg_bytes =  qsr_v4_check_bytes(arg_bytes, xx_arg1);

  if (version == DIAGBUF_VERSION_1)
  {
    if (parameter == NULL)
      return;
    else
      id = *((uint32 *)parameter);
  }

  if (msg_enabled_mask != 0)
  {

    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
         (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3;
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
      /* Message not part of QShrink  - Error to be investigated */
      return;
    }
    stream_id = msg_v4_stream_check(xx_msg_index);

#ifdef FEATURE_DIAG_STM
    if MSG_STM(stream_id,stm_enabled)
    {
      uint32 pkt_length;
      void *stm_msg;

      /* Allocate memory for msg and populate non argument fields */
      stm_msg = diag_stm_qsr_v4_msg_prep(qsr_hash_v4, num_args, stream_id, arg_bytes, &pkt_length, version, id);

      if (stm_msg)
      {
        /* set up args in struct */
        {
          uint8 *arg_ptr = (uint8 *)stm_msg;
          //msg_hdr_type *msg_hdr = (msg_hdr_type *)stm_msg;
          arg_ptr += FPOS(msg_v4_store_type, args);

          if (version == DIAGBUF_VERSION_1)
          {
            //msg_v4_multi_client_store_type *msg_ext = (msg_v4_multi_client_store_type *)stm_msg;
            /* Add padding for multclient message header */
            arg_ptr += FPOS(msg_v4_multi_client_store_type, msg);

            /* Update msg pointer to correct location */
            //msg_hdr = &(msg_ext->msg.hdr);
          }
          qsr_v4_copy_args(arg_ptr, (uint8 *)&xx_arg1, arg_bytes);
        }

        /* Send data to STM */
        diag_stm_write_pkt(stm_msg, pkt_length);

        /* Free data */
        diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
      }
    }
#endif /* FEATURE_DIAG_STM */


      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
        msg = qsr_v4_msg_send_prep(qsr_hash_v4, num_args, stream_id, arg_bytes, version, id);
      }
      if (msg)
      {
        /*-----------------------------------------------------------------------
          Store the arguments in the buffer.
          -----------------------------------------------------------------------*/
        uint8 *arg_ptr = (uint8 *)msg;
        arg_ptr += FPOS(msg_v4_store_type, args);

        if (version == DIAGBUF_VERSION_1)
        {
          /* Add padding for multclient message header */
          arg_ptr += FPOS(msg_v4_multi_client_store_type, msg);
        }

      qsr_v4_copy_args(arg_ptr, (uint8 *)&xx_arg1, arg_bytes);

      diagbuf_commit(msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

  f3_args[0] = xx_arg1;

  msg_save_trace((const void *)qsr_hash_v4, f3_args, 1, (void *)msg, DIAG_F3_TRACE_QSHRINK40,(uint8)version, parameter, arg_bytes);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* msg_v4_send_1_ext */
/*===========================================================================

FUNCTION MSG_V4_SEND_2

DESCRIPTION
  This function is similar to QSR_MSG_SEND_2 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v4_send_2 (const msg_v4_const_type * xx_msg_const_ptr, uint32 xx_arg1, uint32 xx_arg2)
{
  uint32 qsr_hash_v4 = (uint32)xx_msg_const_ptr;
  msg_v4_store_type *msg = NULL;
  uint8 *args = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */

  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */


  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;
  uint8 arg_bytes = 0;

  /*check the actual bytes needed for each argument*/
  arg_bytes =  qsr_v4_check_bytes(arg_bytes,xx_arg1);
  arg_bytes =  qsr_v4_check_bytes(arg_bytes,xx_arg2);


  if(msg_enabled_mask != 0)
  {
    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
       (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3;
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
       /* Message not part of QShrink  - Error to be investigated */
       return;
    }
    stream_id = msg_v4_stream_check(xx_msg_index);
#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id,stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_v4_msg_prep(qsr_hash_v4,num_args,stream_id, arg_bytes, &pkt_length, 0, 0);
          if (stm_msg)
          {
              /* set up args in struct */
            {
              uint8 * arg_ptr = (uint8 *)stm_msg;
              arg_ptr += FPOS(msg_v4_store_type, args);
              qsr_v4_copy_args(arg_ptr,(uint8 *)&xx_arg1,arg_bytes);
              arg_ptr += arg_bytes;
              qsr_v4_copy_args(arg_ptr,(uint8 *)&xx_arg2,arg_bytes);
            }

            /* Send data to STM */
            diag_stm_write_pkt(stm_msg,pkt_length);

            /* Free data */
            diag_stm_msg_free(stm_msg);

            /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
            stream_id &= ~DIAG_STREAM_STM;
          }
      }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      msg = qsr_v4_msg_send_prep (qsr_hash_v4,num_args, stream_id, arg_bytes, 0, 0);

    }

    if (msg)
    {
      /*-----------------------------------------------------------------------
            Store the arguments in the buffer.
           -----------------------------------------------------------------------*/
      args = (uint8 *)&(msg->args);
      qsr_v4_copy_args(args,(uint8 *)&xx_arg1, arg_bytes);
      args += arg_bytes;
      qsr_v4_copy_args(args,(uint8 *)&xx_arg2, arg_bytes);

      diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)


    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;

    msg_save_trace((const void *)qsr_hash_v4, f3_args, 2, (void *)msg, DIAG_F3_TRACE_QSHRINK40, 0, NULL, arg_bytes);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* msg_v4_send_2 */
/*===========================================================================

FUNCTION MSG_V4_SEND_2_EXT

DESCRIPTION
  This function is similar to QSR_MSG_SEND_2 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v4_send_2_ext (const msg_v4_const_type * xx_msg_const_ptr, uint32 version, void *parameter, uint32 xx_arg1, uint32 xx_arg2)
{
  uint32 qsr_hash_v4 = (uint32)xx_msg_const_ptr;
  msg_v4_store_type *msg = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */
  uint32 id = 0;

    boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */


  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;
  uint8 arg_bytes = 0;

  /*check the actual bytes needed for each argument*/
  arg_bytes =  qsr_v4_check_bytes(arg_bytes, xx_arg1);
  arg_bytes =  qsr_v4_check_bytes(arg_bytes, xx_arg2);

  if(version == DIAGBUF_VERSION_1)
  {
    if(parameter == NULL )
      return;
    else
      id = *((uint32 *)parameter);
  }


  if(msg_enabled_mask != 0)
  {
    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
       (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3;
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
       /* Message not part of QShrink  - Error to be investigated */
       return;
    }
    stream_id = msg_v4_stream_check(xx_msg_index);
#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id,stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_v4_msg_prep(qsr_hash_v4, num_args,stream_id, arg_bytes, &pkt_length, version, id);
          if (stm_msg)
          {
            /* set up args in struct */
            {
                  uint8 * arg_ptr = stm_msg;
                  //msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                  arg_ptr += FPOS(msg_v4_store_type, args);

              if(version == DIAGBUF_VERSION_1)
              {
                //msg_v4_multi_client_store_type *msg_ext = (msg_v4_multi_client_store_type*) stm_msg;
                /* Add padding for multclient message header */
                arg_ptr += FPOS(msg_v4_multi_client_store_type, msg);

                /* Update msg pointer to correct location */
                //msg_hdr = &(msg_ext->msg.hdr);
              }

          qsr_v4_copy_args(arg_ptr, (uint8 * ) & xx_arg1, arg_bytes);
          arg_ptr += arg_bytes;
          qsr_v4_copy_args(arg_ptr, (uint8 * ) & xx_arg2, arg_bytes);
        }

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);

          /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
          stream_id &= ~DIAG_STREAM_STM;
        }
      }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      msg = qsr_v4_msg_send_prep (qsr_hash_v4,num_args,stream_id, arg_bytes, version, id);
    }

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
          -----------------------------------------------------------------------*/
      uint8* arg_ptr = (uint8 *)msg;
      //msg_hdr_type *msg_hdr = (msg_hdr_type*)msg;
      arg_ptr += FPOS(msg_v4_store_type, args);

      if(version == DIAGBUF_VERSION_1)
      {
        //msg_v4_multi_client_store_type *msg_ext = (msg_v4_multi_client_store_type*) msg;
        /* Add padding for multclient message header */
        arg_ptr += FPOS(msg_v4_multi_client_store_type, msg);

        /* Update msg pointer to correct location */
        //msg_hdr = &(msg_ext->msg.hdr);
      }

        qsr_v4_copy_args(arg_ptr,(uint8 *)&xx_arg1, arg_bytes);
        arg_ptr += arg_bytes;
        qsr_v4_copy_args(arg_ptr,(uint8 *)&xx_arg2, arg_bytes);
        diagbuf_commit (msg);
    }

  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)


    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;

    msg_save_trace((const void *)qsr_hash_v4, f3_args, 2, (void *)msg, DIAG_F3_TRACE_QSHRINK40,(uint8)version, parameter, arg_bytes);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* msg_v4_send_2_ext */
/*===========================================================================

FUNCTION MSG_V4_SEND_3

DESCRIPTION
  This function is similar to QSR_MSG_SEND_3 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v4_send_3 (const msg_v4_const_type * xx_msg_const_ptr, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  uint32 qsr_hash_v4 =(uint32)xx_msg_const_ptr;
  msg_v4_store_type *msg = NULL;
  uint8 *args = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */

  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */


#if defined (FEATURE_SAVE_DEBUG_TRACE)
    msg_arg_type    f3_args[3];
#endif /* FEATURE_SAVE_DEBUG_TRACE */


  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;
  uint8 arg_bytes = 0;

  /*check the actual bytes needed for each argument*/
  arg_bytes =  qsr_v4_check_bytes(arg_bytes,xx_arg1);
  arg_bytes =  qsr_v4_check_bytes(arg_bytes,xx_arg2);
  arg_bytes =  qsr_v4_check_bytes(arg_bytes,xx_arg3);

  if(msg_enabled_mask != 0)
  {
    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
       (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3;
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
       /* Message not part of QShrink  - Error to be investigated */
      return;
    }
    stream_id = msg_v4_stream_check(xx_msg_index);
#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id,stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_v4_msg_prep(qsr_hash_v4, num_args, stream_id, arg_bytes, &pkt_length, 0, 0);

          if (stm_msg)
          {
              /* set up args in struct */
              {
                  uint8 * arg_ptr = (uint8 *)stm_msg;
                  arg_ptr += FPOS(msg_v4_store_type, args);
                  qsr_v4_copy_args(arg_ptr,(uint8 *)&xx_arg1, arg_bytes);
                  arg_ptr += arg_bytes;
                  qsr_v4_copy_args(arg_ptr,(uint8 *)&xx_arg2, arg_bytes);
                  arg_ptr += arg_bytes;
                  qsr_v4_copy_args(arg_ptr,(uint8 *)&xx_arg3, arg_bytes);
              }

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
          }
      }
#endif /* FEATURE_DIAG_STM */


      if MSG_DIAGBUF(stream_id,stm_enabled)
  {
    msg = qsr_v4_msg_send_prep (qsr_hash_v4, num_args, stream_id, arg_bytes, 0, 0);
  }
    if (msg)
    {

      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
        -----------------------------------------------------------------------*/
          args = (uint8 *)&msg->args;
      qsr_v4_copy_args(args,(uint8 *)&xx_arg1,arg_bytes);
      args += arg_bytes;
      qsr_v4_copy_args(args,(uint8 *)&xx_arg2,arg_bytes);
      args += arg_bytes;
      qsr_v4_copy_args(args,(uint8 *)&xx_arg3,arg_bytes);

      diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)


    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;

    msg_save_trace((const void *)qsr_hash_v4, f3_args, 3, (void *)msg, DIAG_F3_TRACE_QSHRINK40, 0, NULL, arg_bytes);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* msg_v4_send_3 */
/*===========================================================================

FUNCTION MSG_V4_SEND_3_EXT

DESCRIPTION
  This function is similar to QSR_MSG_SEND_3 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v4_send_3_ext(const msg_v4_const_type *xx_msg_const_ptr, uint32 version, void *parameter, uint32 xx_arg1,
     uint32 xx_arg2, uint32 xx_arg3)
{
  uint32 qsr_hash_v4 = (uint32)xx_msg_const_ptr;
  msg_v4_store_type *msg = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */
  uint32 id = 0;

  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */


#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[3];
#endif /* FEATURE_SAVE_DEBUG_TRACE */


  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;
  uint8 arg_bytes = 0;

  /*check the actual bytes needed for each argument*/
  arg_bytes =  qsr_v4_check_bytes(arg_bytes, xx_arg1);
  arg_bytes =  qsr_v4_check_bytes(arg_bytes, xx_arg2);
  arg_bytes =  qsr_v4_check_bytes(arg_bytes, xx_arg3);

  if (version == DIAGBUF_VERSION_1)
  {
    if (parameter == NULL)
      return;
    else
      id = *((uint32 *)parameter);
  }

  if (msg_enabled_mask != 0)
  {
    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
         (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3;
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
      /* Message not part of QShrink  - Error to be investigated */
      return;
    }
    stream_id = msg_v4_stream_check(xx_msg_index);
#ifdef FEATURE_DIAG_STM
    if MSG_STM(stream_id,stm_enabled)
    {
      uint32 pkt_length;
      void *stm_msg;

      /* Allocate memory for msg and populate non argument fields */
      stm_msg = diag_stm_qsr_v4_msg_prep(qsr_hash_v4, num_args, stream_id, arg_bytes, &pkt_length, version, id);

      if (stm_msg)
      {
        /* set up args in struct */
        {
          uint8 *arg_ptr = (uint8 *)stm_msg;
          //msg_hdr_type *msg_hdr = (msg_hdr_type *)stm_msg;
          arg_ptr += FPOS(msg_v4_store_type, args);

          if (version == DIAGBUF_VERSION_1)
          {
            //msg_v4_multi_client_store_type *msg_ext = (msg_v4_multi_client_store_type *)stm_msg;
            /* Add padding for multclient message header */
            arg_ptr += FPOS(msg_v3_multi_client_store_type, msg);

            /* Update msg pointer to correct location */
            //msg_hdr = &(msg_ext->msg.hdr);
          }

          qsr_v4_copy_args(arg_ptr, (uint8 *)&xx_arg1, arg_bytes);
          arg_ptr += arg_bytes;
          qsr_v4_copy_args(arg_ptr, (uint8 *)&xx_arg2, arg_bytes);
          arg_ptr += arg_bytes;
          qsr_v4_copy_args(arg_ptr, (uint8 *)&xx_arg3, arg_bytes);
        }

        /* Send data to STM */
        diag_stm_write_pkt(stm_msg, pkt_length);

        /* Free data */
        diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
      }
    }
#endif /* FEATURE_DIAG_STM */

    /* Allocate memory for msg and populate non argument fields */
      if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      msg = qsr_v4_msg_send_prep(qsr_hash_v4, num_args, stream_id, arg_bytes, version, id);
    }
    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
          -----------------------------------------------------------------------*/
      uint8 *arg_ptr = (uint8 *)msg;
      //msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
      arg_ptr += FPOS(msg_v4_store_type, args);

      if (version == DIAGBUF_VERSION_1)
      {
        //msg_v4_multi_client_store_type *msg_ext = (msg_v4_multi_client_store_type *)msg;
        /* Add padding for multclient message header */
        arg_ptr += FPOS(msg_v4_multi_client_store_type, msg);

        /* Update msg pointer to correct location */
        //msg_hdr = &(msg_ext->msg.hdr);
      }

      qsr_v4_copy_args(arg_ptr, (uint8 *)&xx_arg1, arg_bytes);
      arg_ptr += arg_bytes;
      qsr_v4_copy_args(arg_ptr, (uint8 *)&xx_arg2, arg_bytes);
      arg_ptr += arg_bytes;
      qsr_v4_copy_args(arg_ptr, (uint8 *)&xx_arg3, arg_bytes);

      diagbuf_commit(msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)


  f3_args[0] = xx_arg1;
  f3_args[1] = xx_arg2;
  f3_args[2] = xx_arg3;

  msg_save_trace((const void *)qsr_hash_v4, f3_args, 3, (void *)msg, DIAG_F3_TRACE_QSHRINK40, (uint8)version, parameter, arg_bytes);


#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* msg_v4_send_3 */
/*===========================================================================

FUNCTION MSG_V4_SEND_VAR

DESCRIPTION
  This function is similar to QSR_MSG_SEND_VAR function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_v4_send_var (const msg_v4_const_type * xx_msg_const_ptr, uint32 num_args, ...)
{
  uint32 qsr_hash_v4 = (uint32)xx_msg_const_ptr;
  msg_v4_store_type *msg = NULL;
  uint8 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;

  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */


#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
#endif


  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;
  uint8 arg_bytes = 0;
  uint32 arg[MAX_ARGS_NUM];

  /*lint -save -e{64,10,78,718,746,628,732,530} */
    /* Initialize variable arguments */
  va_start (arg_list, num_args);
  /*lint -restore */

  /* Store arguments from variable list. */
  /*lint -save -e{64,10,78,718,746,628,732,530} */
  for (i = 0; i < num_args; i++)
  {
    arg[i] = va_arg (arg_list, uint32);
    arg_bytes =  qsr_v4_check_bytes(arg_bytes,arg[i]);
  } /* for */
  /*lint -restore */

  /* Reset variable arguments */
  va_end (arg_list);

  if(msg_enabled_mask != 0)
  {
    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
       (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3;
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
       /* Message not part of QShrink  - Error to be investigated */
      return;
    }
    stream_id = msg_v4_stream_check(xx_msg_index);
		

      if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      msg = qsr_v4_msg_send_prep (qsr_hash_v4,num_args, stream_id, arg_bytes, 0, 0);
    }
    if(msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = (uint8 *)&msg->args;
      for (i = 0; i < num_args; i++)
      {
        qsr_v4_copy_args(args,(uint8 *)&arg[i],arg_bytes);
          args += arg_bytes;
      }

      diagbuf_commit (msg);
    }
  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)


  /*lint -save -e{64,10,718,746,628} */
    /* Initialize variable arguments */
    va_start (arg_list, num_args);

    /* Store arguments from variable list. */
    for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
    {
      f3_args[i] = va_arg (arg_list, uint32);
    }               /* for */

    /* Reset variable arguments */
    va_end (arg_list);
/*lint -restore */
    //XXX TODO: don't cast like this --rh
    msg_save_trace((const void *)qsr_hash_v4, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_QSHRINK40, 0, NULL, arg_bytes);

#endif

} /* msg_send_var */
/*===========================================================================

FUNCTION MSG_V4_SEND_VAR_EXT

DESCRIPTION
  This function is similar to QSR_MSG_SEND_VAR function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_v4_send_var_ext(const msg_v4_const_type *xx_msg_const_ptr, uint32 version, void *parameter, uint32 num_args, ...)
{
  uint32 qsr_hash_v4 = (uint32)xx_msg_const_ptr;
  msg_v4_store_type *msg = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
  uint32 id = 0;

  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */


#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
#endif


  uint16 xx_msg_index = 0;
  uint16 msg_index;
  byte stream_id = 0;
  uint8 arg_bytes = 0;
  uint32 arg[MAX_ARGS_NUM];

  if (version == DIAGBUF_VERSION_1)
  {
    if (parameter == NULL)
      return;
    else
      id = *((uint32 *)parameter);
  }

  /*lint -save -e{64,10,78,718,746,628,732,530} */
  /* Initialize variable arguments */
  va_start(arg_list, num_args);
  /*lint -restore */

  /* Store arguments from variable list. */
  /*lint -save -e{64,10,78,718,746,628,732,530} */
  for (i = 0; i < num_args; i++)
  {
    arg[i] = va_arg(arg_list, uint32);
    arg_bytes =  qsr_v4_check_bytes(arg_bytes, arg[i]);
  } /* for */
  /*lint -restore */

  /* Reset variable arguments */
  va_end(arg_list);


  if (msg_enabled_mask != 0)
  {
    if ((qsr_hash_v4 >= (uint32)qsr_4_0_msg_aligned_start_address) &&
         (qsr_hash_v4 < (uint32)qsr_4_0_msg_unaligned_start_address))
    {
      /* Aligned Address Mapping */
      msg_index = (~((uint32)qsr_4_0_msg_aligned_start_address) & qsr_hash_v4) >> 3;
      if (msg_index < msg_v4_ss_num_aligned_entries)
      {
        xx_msg_index = (uint16)msg_v4_ss_aam_table[msg_index].index_to_v4_ssid_table;
      }
    }
    else if (qsr_hash_v4 < (uint32)qsr_4_0_msg_end_address)
    {
      xx_msg_index = qsr_v4_get_address_table_index(qsr_hash_v4);
    }
    else
    {
      /* Message not part of QShrink  - Error to be investigated */
      return;
    }
    stream_id = msg_v4_stream_check(xx_msg_index);
#ifdef FEATURE_DIAG_STM
    if (version == DIAGBUF_VERSION_1)
    {
      if MSG_STM(stream_id,stm_enabled)
      {
        uint32 pkt_length;
        void *stm_msg;

        /* Allocate memory for msg and populate non argument fields */
        stm_msg = diag_stm_qsr_v4_msg_prep(qsr_hash_v4, num_args, stream_id, arg_bytes, &pkt_length, version, id);

        if (stm_msg)
        {
          /* set up args in struct */
          uint8 *arg_ptr = (uint8 *)stm_msg + FPOS(msg_v4_multi_client_store_type, msg);
          arg_ptr += FPOS(msg_v4_store_type, args);


          for (i = 0; i < MIN(num_args, MAX_VAR_ARGS); i++)
          {
            qsr_v4_copy_args(arg_ptr, (uint8 *)&arg[i], arg_bytes);
            arg_ptr += arg_bytes;
          }


          /* Send data to STM */
          diag_stm_write_pkt(stm_msg, pkt_length);

          /* Free data */
          diag_stm_msg_free(stm_msg);

          /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
          stream_id &= ~DIAG_STREAM_STM;
        }
      }
    }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      msg = qsr_v4_msg_send_prep(qsr_hash_v4, num_args, stream_id, arg_bytes, version, id);
    }
    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      uint8 *arg_ptr = (uint8 *)msg;
      //msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
      arg_ptr += FPOS(msg_v4_store_type, args);
      if (version == DIAGBUF_VERSION_1)
      {
        //msg_v4_multi_client_store_type *msg_ext = (msg_v4_multi_client_store_type *)msg;
        /* Add padding for multclient message header */
        arg_ptr += FPOS(msg_v4_multi_client_store_type, msg);

        /* Update msg pointer to correct location */
        //msg_hdr = &(msg_ext->msg.hdr);
      }

      for (i = 0; i < num_args; i++)
      {
        qsr_v4_copy_args(arg_ptr, (uint8 *)&arg[i], arg_bytes);
        arg_ptr += arg_bytes;
      }
      diagbuf_commit(msg);
    }

  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  /*lint -save -e{64,10,718,746,628} */
  /* Initialize variable arguments */
  va_start(arg_list, num_args);

  /* Store arguments from variable list. */
  for (i = 0; i < MIN(num_args, MAX_VAR_ARGS); i++)
  {
    f3_args[i] = va_arg(arg_list, uint32);
  }               /* for */

  /* Reset variable arguments */
  va_end(arg_list);
/*lint -restore */
  //XXX TODO: don't cast like this --rh
  msg_save_trace((const void *)qsr_hash_v4, f3_args, MIN(num_args, MAX_VAR_ARGS),(void *)msg, DIAG_F3_TRACE_QSHRINK40, (uint8)version, parameter, arg_bytes);

#endif

} /* msg_send_var */

/*===========================================================================

FUNCTION qsr_v4_check_bytes

DESCRIPTION
  This function is used to calculate the bytes needed to store the argument.

PARAMETERS
  arg_bytes - Number of bytes needed to store the current largest argument
  arg -  The argument that need to be checked how many bytes should use

RETURN
  uint8 - Number of bytes needed to store the current largest argument

===========================================================================*/
static uint8
qsr_v4_check_bytes(uint8 arg_bytes, uint32 arg)
{
  int byte_sized = 0;

  if (arg > MAX_24BIT_VALUE)
  {
    byte_sized = 4;
  }
  else if ((arg > MAX_16BIT_VALUE))
  {
    byte_sized = 3;
  }
 else if ((arg > MAX_8BIT_VALUE))
  {
    byte_sized = 2;
  }
   else
  {
    byte_sized = 1;
  }
/*compare the current largest bytes needed and the bytes needed by this arg, return the larger one,
  which means the return largest bytes needed for current arguments in the msg including this arg*/
  return (MAX(arg_bytes,byte_sized));
}

/*===========================================================================

FUNCTION qsr_v4_copy_args

DESCRIPTION
  Function used to copy message arguments for Qshrink4 F3s.

DEPENDENCIES
  None

PARAMETERS
  dst - destination pointer
  src - source pointer
  length - number of bytes to copy

===========================================================================*/
static inline void qsr_v4_copy_args(uint8 dst[], uint8 src[], uint8 length)
{
   uint8 i = 0;
   if(length > 4)
   return;

  for (i = 0; i < length; i++)
  {
    dst[i] = src[i];
  }
return;
}

/*===========================================================================

FUNCTION qsr_v4_get_address_table_index

DESCRIPTION
  Binary search on address table to get the index pointed to by the function
  
DEPENDENCIES
  The table is expected to have limited entries which are unmapped in QShrink 4.0
  message sections.
 
PARAMETERS 
  search_address -  The address that need to be searched from the qshrink 4 table 
===========================================================================*/
uint16
qsr_v4_get_address_table_index (const uint32 search_address)
{

  int16 mid_index;
  int16 start_index = 0;  // starting at 1, since 0 is for legacy
  int16 end_index = msg_v4_ss_num_unaligned_entries - 1;
  uint16 table_index = 0;

  if (search_address > end_index ) 
  {
    return table_index;
  }
  /* Binary search in msg_v4_ss_uam_table for search_address */

  /* the binary search modified to work with structures holding the start and end ssid values */
  while( start_index <= end_index )
  {
    mid_index = ( start_index + end_index ) >> 1;

    if( msg_v4_ss_uam_table[mid_index].start_address > search_address )
    {
      end_index = mid_index - 1;
    }
    else if(search_address >= msg_v4_ss_uam_table[mid_index].start_address)
    {
      if (mid_index == msg_v4_ss_num_unaligned_entries-1)
      {
        table_index =  msg_v4_ss_uam_table[mid_index].index_to_v4_ssid_table;
        break;
      }
      else if (search_address < msg_v4_ss_uam_table[mid_index+1].start_address)
      {
        table_index =  msg_v4_ss_uam_table[mid_index].index_to_v4_ssid_table;
        break;
      }
      else
      {
        start_index = mid_index + 1;
      }
    }
  }

  /* adjust the table index before returning */
  return table_index;
 
}



/*===========================================================================

FUNCTION       msg_v4_errlog_

DESCRIPTION    These functions send the desired f3 message in addition to
        invoking err_put_log. This reduces it one function call for top
        level macro.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void msg_v4_errlog_3 ( const msg_v4_const_type * const_blk, uint32 code1, uint32 code2, uint32 code3,
    const char * file,
uint32 line)
{
 msg_v4_send_3 (const_blk, (uint32)code1, (uint32)code2, (uint32)code3);
 err_put_log (line, file);
}

void msg_v4_errlog_2 (const msg_v4_const_type * const_blk, uint32 code1, uint32 code2,
    const char * file,
    uint32 line)
{
 msg_v4_send_2 (const_blk, (uint32)code1, (uint32)code2);
  err_put_log (line, file);
}

void msg_v4_errlog_1 ( const msg_v4_const_type * const_blk, uint32 code1,
    const char * file,
    uint32 line)
{
 msg_v4_send_1 (const_blk, (uint32)code1);
 err_put_log (line, file);

}

void msg_v4_errlog_0 (const msg_v4_const_type * const_blk,
    const char * file,
    uint32 line)
{
 msg_v4_send (const_blk);
  err_put_log (line, file);
}

void msg_v4_errlog_3_ext ( const msg_v4_const_type * const_blk, uint32 version, void* parameter, uint32 code1, uint32 code2, uint32 code3,
    const char * file,
uint32 line)
{
 msg_v4_send_3_ext (const_blk, version, parameter, (uint32)code1, (uint32)code2, (uint32)code3);
 err_put_log (line, file);
}

void msg_v4_errlog_2_ext (const msg_v4_const_type * const_blk, uint32 version, void* parameter, uint32 code1, uint32 code2,
    const char * file,
    uint32 line)
{
 msg_v4_send_2_ext (const_blk, version, parameter, (uint32)code1, (uint32)code2);
 err_put_log (line, file);
}

void msg_v4_errlog_1_ext ( const msg_v4_const_type * const_blk, uint32 version, void* parameter, uint32 code1,
    const char * file,
    uint32 line)
{
 msg_v4_send_1_ext (const_blk, version, parameter, (uint32)code1);
 err_put_log (line, file);

}

void msg_v4_errlog_0_ext (const msg_v4_const_type * const_blk, uint32 version, void* parameter,
    const char * file,
    uint32 line)
{
 msg_v4_send_ext (const_blk, version, parameter);
 err_put_log (line, file);
}



