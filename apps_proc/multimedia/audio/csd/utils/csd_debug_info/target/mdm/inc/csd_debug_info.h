#ifndef _CSD_INIT_INFO_H_
#define _CSD_INIT_INFO_H_
/*! \file csd_init_info.h
    \brief CSD module info debug definition

    Copyright (C) 2014 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/utils/csd_debug_info/target/mdm/inc/csd_debug_info.h#2 $
$DateTime: 2019/01/03 07:23:13 $
$Author: pwbldsvc $
$Change: 17964421 $
$Revision: #2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/14   rp      Added support for multiple CSD init and Deinit logging
12/06/12   rp      Updated for 9x25
04/05/12   rp      intial version
============================================================================*/
#include "mmdefs.h"
#include "time_svc.h"

typedef enum csd_debug_msg_type
{ 
  CSD_MSG_TYPE_CLIENT_TO_CSD = 1,
  CSD_MSG_TYPE_CSD_TO_CLIENT,
  CSD_MSG_TYPE_CSD_INIT,
}csd_debug_msg_type_e;


#define CSD_INIT_SUB_MOD_START_BIT  1
#define CSD_INIT_MSG_MAX_SIZE  6

#define CSD_DEBUG_BUF_SIZE 1280  /* 3840 words, ie 15KBytes memory */

/*Define the extern csd debug init buffer*/
extern uint16_t  csd_debug_buf_pos;
extern uint32_t  csd_debug_buf[CSD_DEBUG_BUF_SIZE];
extern time_type csd_debug_ts;

typedef enum csd_init_senquence_stages
{
  CSD_APR_INIT_STAGE          = 0,    /* APR  */
  CSD_OS_DEP_INIT_STAGE       = 1,    /* csd_os_dep_dal_init  */
  CSD_PMEM_INIT_STAGE         = 2,    /* csd_pmem_init  */
  CSD_HANLE_INIT_STAGE        = 3,    /* csd_handle_init */
  CSD_RM_INIT_STAGE           = 4,    /* csd_rm_init  */
  CSD_ATL_INIT_STAGE          = 5,    /* csd_atl_init */
  CSD_ACDB_INIT_STAGE         = 6,    /* csd_acdb_init */
  CSD_DEV_INIT_STAGE          = 7,    /* csd_dev_init */
  CSD_DEV_DEBUG_INIT_STAGE    = 8,    /* csd_dev_debug_init */
  CSD_SSR_INIT_STAGE          = 9,    /* csd_ssr_init */
  CSD_AS_INIT_STAGE           = 10,   /* csd_as_init */
  CSD_AS_DEBUG_INIT_STAGE     = 11,   /* csd_as_debug_init  */
  CSD_AC_INIT_STAGE           = 12,   /* csd_ac_init */
  CSD_AC_DEBUG_INIT_STAGE     = 13,   /* csd_ac_debug_init */
  CSD_VOC_COMM_INIT_STAGE     = 14,   /* csd_voc_common_init */
  CSD_VS_INIT_STAGE           = 15,   /* csd_vs_init */
  CSD_VC_INIT_STAGE           = 16,   /* csd_vc_init */
  CSD_VM_INIT_STAGE           = 17,   /* csd_vm_init */
  CSD_VOICE_DEBUG_INIT_STAGE  = 18,   /* csd_voice_debug_init */
  CSD_ALM_INIT_STAGE          = 19,   /* csd_alm_init */
  CSD_OCMEM_INIT_STAGE        = 20,   /* csd_ocmem_init */
  CSD_VSM_INIT_STAGE          = 21,   /* csd_vsm_init */
  CSD_DEBUG_INIT_STAGE_MAX    = 22    /* MAX_FOR_ALL */
}csd_init_senquence_stages_e;

typedef enum csd_init_state
{ 
  CSD_DEBUG_INIT_STATE = 1,
  CSD_DEBUG_DEINIT_STATE
}csd_init_states_e;

typedef enum csd_start_type
{
  CSD_START = 0xF0,
  CSD_END
}csd_start_type_e;

typedef enum csd_init_status
{
  CSD_STATE_SUCCESS =1,
  CSD_STATE_FAIL
}csd_init_status_e;


/* ***********************************************************************************
  CSD INIT message consists of 6 words.
  Csd init message fields are as below.

  <------------------------------------32 Bits ---------------------------------------->
  ---------------------------------------------------------------------------------------
  | MESSAGE TYPE (CSD_MSG_TYPE_CLIENT_TO_CSD)  |               MESSAGE SIZE                 |
  ---------------------------------------------------------------------------------------
  |                          CSD STATE(INIT/DEINIT)                                     |
  ---------------------------------------------------------------------------------------
  |                          TIME STAMP [0]                                             |
  ---------------------------------------------------------------------------------------
  |                          TIME STAMP [1]                                             |
  ---------------------------------------------------------------------------------------
  |CSD STAGE (0xFF)|	CSD_SUB_STAGE_1(0xFF)|CSD_SUB_STAGE_2(0xFF)| CSD_SUB_STAGE_3(0xFF)|
  ---------------------------------------------------------------------------------------
  |                          Return value  (RC)                                         |
  ---------------------------------------------------------------------------------------

   CSD STAGE  represented by csd_init_senquence_stages_e values. it followed by the
   CSD sub stages.
   Return value (RC) represent the oepration status, in case failure, it hold the error
   code value.
 ************************************************************************************** */
#define CSD_DEBUG_INIT_LOG(STATE, RC, INIT_STAGE, SUB_STAGE_1, SUB_STAGE_2, SUB_STAGE_3)\
{\
  if( CSD_DEBUG_BUF_SIZE > CSD_INIT_MSG_MAX_SIZE ) \
  {\
    time_get_ms( csd_debug_ts );\
    if( csd_debug_buf_pos >(CSD_DEBUG_BUF_SIZE-CSD_INIT_MSG_MAX_SIZE) )\
    {\
      csd_debug_buf_pos = 0;\
    }\
    csd_debug_buf[csd_debug_buf_pos++] = ( (CSD_MSG_TYPE_CSD_INIT << 16)\
                                                   |(CSD_INIT_MSG_MAX_SIZE ));\
    csd_debug_buf[csd_debug_buf_pos++] = STATE;\
    csd_debug_buf[csd_debug_buf_pos++] = csd_debug_ts[0];\
    csd_debug_buf[csd_debug_buf_pos++] = csd_debug_ts[1];\
    csd_debug_buf[csd_debug_buf_pos++] = ( ((INIT_STAGE & 0xFF) << 24)\
                                                   |((SUB_STAGE_1 & 0xFF) << 16)\
                                                   |((SUB_STAGE_2 & 0xFF) << 8)\
                                                   |( SUB_STAGE_3 & 0xFF));\
    csd_debug_buf[csd_debug_buf_pos++] = RC;\
  }\
}\



#ifdef MDM_QMI_CSD_DEBUG
#define CSD_CLIENT_CONNECT_REQ         0x100
#define CSD_CLIENT_DISCONNECT_REQ      0x101
#define MAX_ROWS_PER_REQ_MSG            7
#define MAX_ROWS_PER_RESP_MSG           5
#define MSG_ID_OFFSET               16

/* ***********************************************************************************
  CSD QMI command request message consists of max 7 words.
  CSD QMI command request message fields are as below.

  <------------------------------------32 Bits ---------------------------------------->
  ---------------------------------------------------------------------------------------
  | MESSAGE TYPE (CSD_MSG_TYPE_CLIENT_TO_CSD) |               MESSAGE SIZE              |
  ---------------------------------------------------------------------------------------
  |                             MSG ID                                                  |
  ---------------------------------------------------------------------------------------
  |                          TIME STAMP [0]                                             |
  ---------------------------------------------------------------------------------------
  |                          TIME STAMP [1]                                             |
  ---------------------------------------------------------------------------------------
  |                          Pay Load 1                                                 |
  ---------------------------------------------------------------------------------------
  |                          Pay Load 2                                                 |
  ---------------------------------------------------------------------------------------
  |                          Pay Load 3                                                 |
  ---------------------------------------------------------------------------------------
  Handle info, CSD device ids, set devices ids are stored in the payload 
 ************************************************************************************** */

#define QMI_CSD_REQ_SET_BUFFER( msg_id, payload )                                                            \
{                                                                                                            \
  if( CSD_DEBUG_BUF_SIZE > CSD_INIT_MSG_MAX_SIZE )                                                           \
  {                                                                                                          \
    time_get_ms(csd_debug_ts );                                                                              \
    if( csd_debug_buf_pos > ( CSD_DEBUG_BUF_SIZE - MAX_ROWS_PER_REQ_MSG ) )                                  \
      csd_debug_buf_pos = 0;                                                                                 \
    csd_debug_buf[csd_debug_buf_pos++] = (CSD_MSG_TYPE_CLIENT_TO_CSD << MSG_ID_OFFSET);                      \
    csd_debug_buf[csd_debug_buf_pos++] = msg_id;                                                             \
    csd_debug_buf[csd_debug_buf_pos++] = csd_debug_ts[0];                                                    \
    csd_debug_buf[csd_debug_buf_pos++] = csd_debug_ts[1];                                                    \
    if( payload != NULL )                                                                                    \
    {                                                                                                        \
      if( ( msg_id == QMI_CSD_OPEN_PASSIVE_CONTROL_VOICE_STREAM_REQ_V01 ) ||                                 \
        ( msg_id == QMI_CSD_OPEN_FULL_CONTROL_VOICE_STREAM_REQ_V01 ) ||                                      \
        ( msg_id == QMI_CSD_OPEN_VOICE_CONTEXT_REQ_V01 ) ||                                                  \
        ( msg_id == QMI_CSD_OPEN_VOICE_MANAGER_REQ_V01 ) )                                                   \
      {                                                                                                      \
        if( 0 == memcmp( "default modem voice2", payload, 20 ) )                                             \
            csd_debug_buf[csd_debug_buf_pos++] = 3;                                                          \
        else if( 0 == memcmp( "default modem voice", payload, 19 ) )                                         \
            csd_debug_buf[csd_debug_buf_pos++] = 1;                                                          \
        else if ( 0 == memcmp( "default volte voice", payload, 19 ) )                                        \
            csd_debug_buf[csd_debug_buf_pos++] = 2;                                                          \
        else                                                                                                 \
        {                                                                                                    \
          csd_memscpy( &csd_debug_buf[csd_debug_buf_pos++],                                                  \
                                       sizeof(csd_debug_buf[0]),                                             \
                                             payload, 4 );                                                   \
        }                                                                                                    \
        csd_debug_buf[csd_debug_buf_pos-5] |= 5;                                                             \
      }                                                                                                      \
      else if( ( msg_id == QMI_CSD_INIT_REQ_V01 ) ||                                                         \
             ( msg_id == QMI_CSD_DEINIT_REQ_V01 ) ||                                                         \
             ( msg_id == QMI_CSD_OPEN_DEVICE_CONTROL_REQ_V01 ) ||                                            \
             ( msg_id == QMI_CSD_QUERY_DRIVER_VERSION_REQ_V01 )  ||                                          \
             ( msg_id == CSD_CLIENT_CONNECT_REQ )  ||                                                        \
             ( msg_id == CSD_CLIENT_DISCONNECT_REQ ) )                                                       \
      {                                                                                                      \
       csd_debug_buf[csd_debug_buf_pos-4] |= 4;                                                              \
      }                                                                                                      \
      else                                                                                                   \
      {                                                                                                      \
        csd_memscpy( &csd_debug_buf[csd_debug_buf_pos++],                                                    \
                    sizeof(csd_debug_buf[0]),                                                                \
                   payload, 4 );                                                                             \
        if( msg_id == QMI_CSD_IOCTL_DEV_CMD_ENABLE_REQ_V01 )                                                 \
        {                                                                                                    \
          csd_memscpy( &csd_debug_buf[csd_debug_buf_pos++],                                                  \
                     sizeof(csd_debug_buf[0]),                                                               \
                     (void*)(((uint32_t*)payload)+2), 4 );                                                   \
          csd_memscpy( &csd_debug_buf[csd_debug_buf_pos++],                                                  \
                     sizeof(csd_debug_buf[0]),                                                               \
                     (void*)(((uint32_t*)payload)+5), 4 );                                                   \
        }                                                                                                    \
        else if( msg_id == QMI_CSD_IOCTL_DEV_CMD_DISABLE_REQ_V01 )                                           \
        {                                                                                                    \
          csd_memscpy( &csd_debug_buf[csd_debug_buf_pos++],                                                  \
                     sizeof(csd_debug_buf[0]),                                                               \
                     (void*)(((uint32_t*)payload)+2), 4 );                                                   \
          csd_memscpy( &csd_debug_buf[csd_debug_buf_pos++],                                                  \
                     sizeof(csd_debug_buf[0]),                                                               \
                     (void*)(((uint32_t*)payload)+3), 4 );                                                   \
        }                                                                                                    \
        else                                                                                                 \
        {                                                                                                    \
          csd_memscpy( &csd_debug_buf[csd_debug_buf_pos++],                                                  \
                       8,(void*)(((uint32_t*)payload)+2), 8 );                                               \
          csd_debug_buf_pos++;                                                                               \
        }                                                                                                    \
        csd_debug_buf[csd_debug_buf_pos-7] |= 7;                                                             \
      }                                                                                                      \
    }                                                                                                        \
    else                                                                                                     \
    {                                                                                                        \
      csd_debug_buf[csd_debug_buf_pos-4] |= 4;                                                               \
    }                                                                                                        \
  }                                                                                                          \
}                                                                                                            \
/* Use this macro for all the response get back to the client*/
/* ***********************************************************************************
  CSD QMI command response message consists of max 5 words.
  CSD QMI command response message fields are as below.

  <------------------------------------32 Bits ---------------------------------------->
  ---------------------------------------------------------------------------------------
  | MESSAGE TYPE (CSD_MSG_TYPE_CSD_TO_CLIENT) |               MESSAGE SIZE              |
  ---------------------------------------------------------------------------------------
  |                             MSG ID                                                  |
  ---------------------------------------------------------------------------------------
  |                          TIME STAMP [0]                                             |
  ---------------------------------------------------------------------------------------
  |                          TIME STAMP [1]                                             |
  ---------------------------------------------------------------------------------------
  |                               RC                                                    |
  ---------------------------------------------------------------------------------------
  Response message status would be stored in the RC value
 ************************************************************************************** */
#define QMI_CSD_RESP_SET_BUFFER( msg_id, rc )                                                                         \
{                                                                                                                     \
  if( CSD_DEBUG_BUF_SIZE > MAX_ROWS_PER_RESP_MSG )                                                                    \
  {                                                                                                                   \
    time_get_ms( csd_debug_ts );                                                                                      \
    if( csd_debug_buf_pos > ( CSD_DEBUG_BUF_SIZE - MAX_ROWS_PER_RESP_MSG ) )                                          \
      csd_debug_buf_pos = 0;                                                                                          \
    csd_debug_buf[csd_debug_buf_pos++] = ((CSD_MSG_TYPE_CSD_TO_CLIENT << MSG_ID_OFFSET) | (MAX_ROWS_PER_RESP_MSG));   \
    csd_debug_buf[csd_debug_buf_pos++] = msg_id ;                                                                     \
    csd_debug_buf[csd_debug_buf_pos++] = csd_debug_ts[0];                                                             \
    csd_debug_buf[csd_debug_buf_pos++] = csd_debug_ts[1];                                                             \
    csd_debug_buf[csd_debug_buf_pos++] = rc;                                                                          \
    if( csd_debug_buf_pos == CSD_DEBUG_BUF_SIZE )                                                                     \
      csd_debug_buf_pos = 0;                                                                                          \
  }                                                                                                                   \
}                                                                                                                     \

#else
#define QMI_CSD_REQ_SET_BUFFER( msg_id, payload )
#define QMI_CSD_RESP_SET_BUFFER( msg_id, rc )

#endif

typedef enum
{
  CSD_INIT_INFO_ACDB_DAL_MALLOC = CSD_INIT_SUB_MOD_START_BIT,  /* 1  */
  CSD_INIT_INFO_ACDB_CSD_MEM_MALLOC,                           /* 2  */
  CSD_INIT_INFO_ACDB_QUERY_ACDB_FILE,                          /* 3  */
  CSD_INIT_INFO_ACDB_IOCTL_INITIALIZE,                         /* 4  */ 
  CSD_INIT_INFO_ACDB_ACPH,                                     /* 5  */
  CSD_INIT_INFO_ACDB_ACPH_RTC_DSP,                             /* 6  */
  CSD_INIT_INFO_ACDB_ACPH_RTC_ADIE,                            /* 7 */
  CSD_INIT_INFO_ACDB_ACPH_RTC_FTS,                             /* 8 */
  CSD_INIT_INFO_ACDB_ACPH_RTC_MCS,                             /* 9 */
  CSD_INIT_INFO_ACDB_RESET,                                    /* 10 */
}csd_init_info_acdb;

/*! \brief  pmem sub modules initialization  */
typedef enum 
{
  CSD_INIT_INFO_PMEM_LIST = CSD_INIT_SUB_MOD_START_BIT,      /*  1 */
  CSD_INIT_INFO_PMEM_ALLOC                                   /*  2 */
}csd_init_info_pmem;

/*! \brief  list sub modules initialization  */
typedef enum 
{
  CSD_INIT_INFO_HANDLE_MALLOC = CSD_INIT_SUB_MOD_START_BIT,  /*  1 */
  CSD_INIT_INFO_HANDLE_INIT,                                 /*  2 */
}csd_init_info_handle;

/*! \brief  atl sub modules initialization  */
typedef enum 
{
  CSD_INIT_ATL_MALLOC = CSD_INIT_SUB_MOD_START_BIT,         /*  1 */
  CSD_INIT_ATL_APR_SERV_INIT,                               /*  2 */
  CSD_INIT_ATL_RESOURCE_CREATE,                             /*  3 */
  CSD_INIT_ATL_TRANS_INIT                                   /*  4 */
}csd_init_info_atl;

/*! \brief  dev sub modules initialization  */
typedef enum 
{
  CSD_INIT_DEV_HANDLE_MALLOC = CSD_INIT_SUB_MOD_START_BIT,  /*  1 */
  CSD_INIT_DEV_RESOURCE_CREATE,                             /*  2 */
  CSD_INIT_DEV_MANAGER_INIT,                                /*  3 */
  CSD_INIT_DEV_TARGET_INIT,                                 /*  4 */
  CSD_INIT_DEV_FB_SPKR_PROT_INIT                            /*  5 */
}csd_init_info_dev;

/*! \brief  dev sub modules initialization  */
typedef enum
{
  CSD_INIT_DEV_TARGET_OEM_INIT  = CSD_INIT_SUB_MOD_START_BIT,   /*  1 */
  CSD_INIT_DEV_TARGET_TLMM_INIT,                                /*  2 */
  CSD_INIT_DEV_TARGET_CLK_INIT,                                 /*  3 */
  CSD_INIT_DEV_TARGET_CODEC_INIT,                               /*  4 */
  CSD_INIT_DEV_TARGET_AFE_INIT,                                 /*  5 */
  CSD_INIT_DEV_TARGET_MBHC_INIT,                                /*  6 */
}csd_init_info_dev_target;

/*! \brief  dev target tlmm sub modules initialization  */
typedef enum
{
  CSD_INIT_DEV_TARGET_TLMM_HANDLE = CSD_INIT_SUB_MOD_START_BIT,  /*  1 */
  CSD_INIT_DEV_TARGET_TLMM_DEV_ATTACH,                           /*  2 */
  CSD_INIT_DEV_TARGET_TLMM_DEV_OPEN,                             /*  3 */
  CSD_INIT_DEV_TARGET_TLMM_SYNC_CREATE,                          /*  4 */
  CSD_INIT_DEV_TARGET_TLMM_CODEC_PRESENT,                        /*  5 */
  CSD_INIT_DEV_TARGET_TLMM_GET_PLATFORM,                         /*  6 */
  CSD_INIT_DEV_TARGET_TLMM_GET_CHIP_INFO,                        /*  7 */
  CSD_INIT_DEV_TARGET_TLMM_ENABLE_RESET_GPIO,                    /*  8 */
  CSD_INIT_DEV_TARGET_TLMM_DISABLE_RESET_GPIO,                   /*  9 */
  CSD_INIT_DEV_TARGET_TLMM_CODEC_INIT,                           /*  10 */
  CSD_INIT_DEV_TARGET_TLMM_USECASE_CLEANUP,                      /*  11 */
}csd_init_info_dev_tlmm_target;

/*! \brief  dev target codec sub modules initialization  */
typedef enum
{
  CSD_INIT_DEV_TARGET_CODEC_OVERRIDE = CSD_INIT_SUB_MOD_START_BIT,  /*  1 */
  CSD_INIT_DEV_TARGET_CODEC_PRESENT,                                /*  2 */
  CSD_INIT_DEV_TARGET_CODEC_PMIC,                                   /*  3 */
  CSD_INIT_DEV_TARGET_CODEC_WCD_INIT,                               /*  4 */
  CSD_INIT_DEV_TARGET_CODEC_RESET_CODEC,                            /*  5 */
}csd_init_info_dev_codec_target;



/*! \brief  dev debug sub modules initialization  */
typedef enum 
{
  CSD_INIT_DEV_DEBUG_MALLOC = CSD_INIT_SUB_MOD_START_BIT,    /* 1 */
}csd_init_info_dev_debug;


/*! \brief  as sub modules initialization  */
typedef enum 
{
  CSD_INIT_AS_SESSION_LIST = CSD_INIT_SUB_MOD_START_BIT,      /*  1 */
  CSD_INIT_AS_STREAM_LIST,                                    /*  2 */
  CSD_INIT_AS_STREAM_USED_INIT,                               /*  3 */
  CSD_INIT_AS_STREAM_ADD,                                     /*  4 */
  CSD_INIT_AS_SESSION_ADD,                                    /*  5 */
  CSD_INIT_AS_PP_INIT,                                        /*  6 */
  CSD_INIT_AS_PMEM_INIT,                                      /*  7 */
  CSD_INIT_AS_ACDB_INIT                                       /*  8 */
}csd_init_info_as;

/*! \brief  as debug sub modules initialization  */
typedef enum 
{
   CSD_INIT_AS_DEBUG_MALLOC = CSD_INIT_SUB_MOD_START_BIT,      /* 1 */
}csd_init_info_as_debug;



/*! \brief  voc common sub modules initialization  */
typedef enum 
{
  CSD_INIT_VOC_COMM_CTL_EVT_CREATE = CSD_INIT_SUB_MOD_START_BIT, /* 1 */
  CSD_INIT_VOC_COMM_THREAD_CREATE,                               /* 2 */
  CSD_INIT_VOC_COMM_CTL_EVT_WAIT,                                /* 3 */
  CSD_INIT_VOC_COMM_LOCK_CREATE,                                 /* 4 */
  CSD_INIT_VOC_COMM_FREE_SESS_INIT,                              /* 5 */
  CSD_INIT_VOC_COMM_ACTIVE_SESS_INIT,                            /* 6 */
  CSD_INIT_VOC_COMM_DEINIT_EVT_CREATE,                           /* 7 */
  CSD_INIT_VOC_COMM_DEINIT_EVT_DESTROY                           /* 8 */
}csd_init_info_voc_common;


/*! \brief  vs sub modules initialization  */
typedef enum 
{
  CSD_INIT_VS_SESSION_GET = CSD_INIT_SUB_MOD_START_BIT,       /* 1 */
  CSD_INIT_VS_DNS_CVS_LOOK_UP,                                /* 2 */
  CSD_INIT_VS_DNS_MVM_LOOK_UP,                                /* 3 */
  CSD_INIT_VS_APR_REGISTER,                                   /* 4 */
  CSD_INIT_VS_APR_LOCK_CREATE,                                /* 5 */
  CSD_INIT_VS_FREE_CMD_LIST_INIT,                             /* 6 */
  CSD_INIT_VS_ACTIVE_CMD_LIST_INIT,                           /* 7 */
  CSD_INIT_VS_DISPATCH_CMD_LIST_INIT,                         /* 8 */
  CSD_INIT_VS_FREE_CMD_LIST_ADD,                              /* 9 */
  CSD_INIT_VS_FREE_SESS_LIST_INIT,                            /* 10 */
  CSD_INIT_VS_ACTIVE_SESS_LIST_INIT,                          /* 11 */
  CSD_INIT_VS_FREE_SESS_LIST_ADD,                             /* 12 */
  CSD_INIT_VS_OBJECT_MGR_INIT,                                /* 13 */
}csd_init_info_vs;


/*! \brief  ac sub modules initialization  */
typedef enum 
{
  CSD_INIT_AC_RESOURSE_CREATE = CSD_INIT_SUB_MOD_START_BIT,   /* 1  */
  CSD_INIT_AC_INIT_AC_OBJ,                                    /* 2 */
  CSD_INIT_AC_INIT_AS_OBJ                                     /* 3 */
}csd_init_info_ac;


/*! \brief  ac debug sub modules initialization  */
typedef enum 
{
  CSD_INIT_AC_DEBUG_MALLOC = CSD_INIT_SUB_MOD_START_BIT       /* 1 */
}csd_init_info_ac_debug;


/*! \brief  vc sub modules initialization  */
typedef enum 
{
  CSD_INIT_VC_SESSION_GET = CSD_INIT_SUB_MOD_START_BIT,       /* 1 */
  CSD_INIT_VC_DNS_CVP_LOOK_UP,                                /* 2 */
  CSD_INIT_VC_DNS_MVM_LOOK_UP,                                /* 3 */
  CSD_INIT_VC_APR_REGISTER,                                   /* 4 */
  CSD_INIT_VC_APR_LOCK_CREATE,                                /* 5 */
  CSD_INIT_VC_FREE_CMD_LIST_INIT,                             /* 6 */
  CSD_INIT_VC_ACTIVE_CMD_LIST_INIT,                           /* 7 */
  CSD_INIT_VC_DISPATCH_CMD_LIST_INIT,                         /* 8 */
  CSD_INIT_VC_FREE_CMD_LIST_ADD,                              /* 9 */
  CSD_INIT_VC_FREE_SESS_LIST_INIT,                            /* 10 */
  CSD_INIT_VC_ACTIVE_SESS_LIST_INIT,                          /* 11 */
  CSD_INIT_VC_FREE_SESS_LIST_ADD,                             /* 12 */
  CSD_INIT_VC_OBJECT_MGR_INIT                                 /* 13 */
}csd_init_info_vc;


/*! \brief  vm sub modules initialization  */
typedef enum 
{
  CSD_INIT_VM_SESSION_GET = CSD_INIT_SUB_MOD_START_BIT,       /* 1 */
  CSD_INIT_VM_DNS_CVS_LOOK_UP,                                /* 2 */
  CSD_INIT_VM_DNS_CVP_LOOK_UP,                                /* 3 */
  CSD_INIT_VM_DNS_MVM_LOOK_UP,                                /* 4 */
  CSD_INIT_VM_APR_REGISTER,                                   /* 5 */
  CSD_INIT_VM_APR_LOCK_CREATE,                                /* 6 */
  CSD_INIT_VM_FREE_CMD_LIST_INIT,                             /* 7 */
  CSD_INIT_VM_ACTIVE_CMD_LIST_INIT,                           /* 8 */
  CSD_INIT_VM_DISPATCH_CMD_LIST_INIT,                         /* 9 */
  CSD_INIT_VM_FREE_CMD_LIST_ADD,                              /* 10 */
  CSD_INIT_VM_FREE_SESS_LIST_INIT,                            /* 11 */
  CSD_INIT_VM_ACTIVE_SESS_LIST_INIT,                          /* 12 */
  CSD_INIT_VM_FREE_SESS_LIST_ADD,                             /* 13 */
  CSD_INIT_VM_OBJECT_MGR_INIT                                 /* 14 */
}csd_init_info_vm;

/*! \brief  Audio License Manager sub modules initialization  */
typedef enum 
{
  CSD_INIT_ALM_MALLOC = CSD_INIT_SUB_MOD_START_BIT,    /* 1 */
  CSD_INIT_ALM_RESOURCE_CREATE,                        /* 2 */
  CSD_INIT_ALM_APR_DNS_LOOK_UP,                        /* 3 */
  CSD_INIT_ALM_APR_REGISTER,                           /* 4 */
  CSD_INIT_ALM_APR_EVT_CREATE                          /* 5 */
}csd_init_info_alm;

/*! \brief  Voice Service Manager initialization  */
typedef enum 
{
  CSD_INIT_VSM_MALLOC = CSD_INIT_SUB_MOD_START_BIT,    /* 1 */
  CSD_INIT_VSM_FREE_SESSION_LIST_INIT,                 /* 2 */
  CSD_INIT_VSM_USED_SESSION_LIST_INIT,                 /* 3 */
  CSD_INIT_VSM_ADD_SESSION_LIST,                       /* 4 */
  CSD_INIT_VSM_APR_LOCK_CREATE                         /* 5 */
}csd_init_info_vsm;


#endif /* _CSD_INIT_INFO_H_ */


