#ifndef DS_MUX_DEFS_H
#define DS_MUX_DEFS_H
/*===========================================================================

                        D S _  M U X _ D E F S . H

DESCRIPTION

  DS MUX  defination header file.
  All definitions and structures needed by DS_MUX task modules.
  

Copyright (c) 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/dev/data.mpss/3.4/adubey.data.mpss.3.4.ds_mux_changes/interface/tasks/inc/ds_mux_defs.h#1 
  $ $DateTime: 2019/11/26 00:18:17 $ $Author: pwbldsvc $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef FEATURE_DSM_WM_CB
#define FEATURE_DSM_WM_CB
#endif
#include "dsm.h"
#include "queue.h"
#include "task.h"
#ifndef MOB_TX
#include "rdevmap.h"
#else
#include "rdevmap_mob.h"
#endif

#include "ds_mux_task.h"
#include "ds_mux_platform.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "qurt_mutex.h"



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/** SUCCESS */
#define    TRUE                                       1
/** FAILURE */
#define    FALSE                                      0
/** Max size in bytes in Information field as per 3GPP 27.010 */
#define DS_MUX_CMUX_MAX_FRAME_N1                      32768
/** Max number of acknowledgement timer  as per 3GPP 27.010*/
#define DS_MUX_CMUX_MAX_TX_T1                         255
/** maximum number of re-transmissions  as per 3GPP 27.010*/
#define DS_MUX_CMUX_MAX_N2                            100

/** Max number of response timer  as per 3GPP 27.010*/
#define DS_MUX_CMUX_MAX_TX_T2                         255

/** Max number of wake up response timer   as per 3GPP 27.010*/
#define DS_MUX_CMUX_MAX_TX_T3                         255

/** Default size in bytes in Information field per 3GPP 27.010, basic mode */
#define DS_MUX_CMUX_DEFAULT_FRAME_N1                  31
/** Default size in bytes in Information field per 3GPP 27.010, advanced mode */
#define DS_MUX_CMUX_ADVANCED_MODE_DEFAULT_FRAME_N1    63
/** Max Number of re-tranmission as per 3GPP 27.010 */
#define DS_MUX_CMUX_DEFAULT_MAX_TX_N2                 3
/** Default value of acknowledgement timer in ms as per 3GPP 27.010 */
#define DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1              10
/** Default value of DLCI0 response timer in ms as per 3GPP 27.010 */
#define DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2            30
/** Default value of response timer as per 3GPP 27.010 */
#define DS_MUX_CMUX_DEFAULT_TIMER_T3                  10
/*Default priority*/
#define DS_MUX_DEFAULT_DLCI_PRIORITY                  63

#define DS_MUX_MAX_LOGICAL_CONNECTION                 0x3

#define DS_MUX_MAX_DLCI_ID                            DS_MUX_MAX_LOGICAL_CONNECTION

#define DS_MUX_DLCI_ID_0                              0x0

#define DS_MUX_DLCI_ID_1                              0x1

#define DS_MUX_DLCI_ID_2                              0x2

#define DS_MUX_DLCI_ID_3                              0x3

#define DS_MUX_MSG_RESP                               0x0

#define DS_MUX_MSG_CMD                                0x1


#define DS_MUX_DLCI_MSG_TYPE_EA_MASK                  0x01

#define DS_MUX_DLCI_MSG_TYPE_CR_MASK                  0x02

#define DS_MUX_DLCI_MSC_FC_MASK                       0x02
#define DS_MUX_DLCI_MSC_RTC_MASK                      0x04
#define DS_MUX_DLCI_MSC_IC_MASK                       0x40
#define DS_MUX_DLCI_MSC_DV_MASK                       0x80

#define DS_MUX_DLCI_MSC_VAL_LEN                       0x02

/* from the mux standard 27.010 section 5.4.6.3
 * EA is true always these values are message type and ea bit set to 1
 */
#define   DS_MUX_DLCI_FRAME_VAL_PN                   0x81
#define   DS_MUX_DLCI_FRAME_VAL_PSC                  0x41
#define   DS_MUX_DLCI_FRAME_VAL_CLD                  0xC1
#define   DS_MUX_DLCI_FRAME_VAL_TEST                 0x21
#define   DS_MUX_DLCI_FRAME_VAL_FCON                 0xA1
#define   DS_MUX_DLCI_FRAME_VAL_FCOFF                0x61
#define   DS_MUX_DLCI_FRAME_VAL_MSC                  0xE1
#define   DS_MUX_DLCI_FRAME_VAL_NSC                  0x11
#define   DS_MUX_DLCI_FRAME_VAL_RPN                  0x91
#define   DS_MUX_DLCI_FRAME_VAL_RLS                  0x51
#define   DS_MUX_DLCI_FRAME_VAL_SNC                  0xD1

/* Frame Start/Stop Flag Code
 */
#define DS_MUX_UTIL_START_STOP_FLAG_CODE    0xF9u

/* Mask with location of EA bit 
 */
#define DS_MUX_UTIL_OCTET_EA_MASK 0x01u

/* Mask with location of C/R bit in Addr Field
 */
#define DS_MUX_UTIL_ADDR_OCTET_CR_MASK  0x02u


/* Mask with location of P/F bit in Ctrl Field
 */
#define DS_MUX_UTIL_CTRL_OCTET_PF_MASK  0x10u


/* SABM Control field Code without PF, i.e. P/F bit set to zero(0)
   Note: this is done, so later it can be masked with
   DS_MUX_UTIL_CTRL_OCTET_PF_MASK, to get the full Ctrl Octet
 */
#define DS_MUX_UTIL_CTRL_OCTET_SABM_CODE_WO_PF  0x2Fu

/* UA Control field Code without PF, i.e. P/F bit set to zero(0)
   Note: this is done, so later it can be masked with
   DS_MUX_UTIL_CTRL_OCTET_PF_MASK, to get the full Ctrl Octet
 */
#define DS_MUX_UTIL_CTRL_OCTET_UA_CODE_WO_PF  0x63u

/* DM Control field Code without PF, i.e. P/F bit set to zero(0)
   Note: this is done, so later it can be masked with
   DS_MUX_UTIL_CTRL_OCTET_PF_MASK, to get the full Ctrl Octet
 */
#define DS_MUX_UTIL_CTRL_OCTET_DM_CODE_WO_PF  0x0Fu

/* DISC Control field Code without PF, i.e. P/F bit set to zero(0)
   Note: this is done, so later it can be masked with
   DS_MUX_UTIL_CTRL_OCTET_PF_MASK, to get the full Ctrl Octet
 */
#define DS_MUX_UTIL_CTRL_OCTET_DISC_CODE_WO_PF  0x43u

/* UIH Control field Code without PF, i.e. P/F bit set to zero(0)
   Note: this is done, so later it can be masked with
   DS_MUX_UTIL_CTRL_OCTET_PF_MASK, to get the full Ctrl Octet
 */
#define DS_MUX_UTIL_CTRL_OCTET_UIH_CODE_WO_PF  0xEFu

/* UI Control field Code without PF, i.e. P/F bit set to zero(0)
   Note: this is done, so later it can be masked with
   DS_MUX_UTIL_CTRL_OCTET_PF_MASK, to get the full Ctrl Octet
 */
#define DS_MUX_UTIL_CTRL_OCTET_UI_CODE_WO_PF  0x03u

/* max num bytes of info contained in a basic frame */
#define DS_MUX_UTIL_BASIC_FRAME_MAX_INFO_LEN  32767 /* 2^15 - 1*/


#define  CL_FOUR_CTL_HDR_MIDDLE_FRAME         0x01u /* 1000 0000 flipped 0000 0001 */
#define  CL_FOUR_CTL_HDR_LAST_FRAME           0x81u /* 1000 0001 flipped 1000 0001 */
#define  CL_FOUR_CTL_HDR_FIRST_FRAME          0x41u /* 1000 0010 flipped 0100 0001 */
#define  CL_FOUR_CTL_HDR_ENTIRE_FRAME         0xc1u /* 1000 0011 flipped 1100 0001 */


#define  IS_DLCI_INVALID( dlci_id)  ( dlci_id > DS_MUX_MAX_DLCI_ID )

#define  DS_MUX_PN_MSG_LEN                    8


#define  DS_MUX_SET_ERROR_AND_BREAK(result)     \
{                                               \
  result = DS_MUX_FAILURE;                      \
  break;                                        \
}


//extern rex_tcb_type    * rex_ds_mux_tcb;

//#define DS_MUX_SIG_ARR_LEN REX_SIGS_ARRAY_LEN
  
#define DS_MUX_SET_SIGNAL( sig ) \
  qurt_signal_set(&qti_bridge_qurt.qti_bridge_signal, (1<< sig))

#define DS_MUX_CLR_SIGNAL( sig ) \
  qurt_signal_clear(&qti_bridge_qurt.qti_bridge_signal, (1<< sig))


/*--------------------------------------------------------------------------
  DS_MUX Task Signal Definitions.  The value of existing signals
  should not be changed while adding or deleting a signal.  Some signals
  (marked Reserved) have fixed value that should stay as it is.
--------------------------------------------------------------------------*/
typedef enum
{
  DS_MUX_CMD_Q_SIGNAL                =  2,  /* DS_MUX command Q signal          */
  DS_MUX_DOG_HB_REPORT_SIGNAL        =  3,  /* Dog heart beat report signal       */
  DS_MUX_SIG_MASK_CHANGE_SIGNAL      =  4,  /* Indicates sig mask has changed*/

  DS_MUX_RX_DATA_SIGNAL              =  5,  /* Logical Client(via SIO) --> DS_MUX data sig  */
  DS_MUX_TX_DLCI1_DATA_SIGNAL        =  6,  /* Logical Client(via SIO) --> DS_MUX data sig  */
  DS_MUX_TX_DLCI2_DATA_SIGNAL        =  7,  /* Logical Client(via SIO) --> DS_MUX data sig  */
  DS_MUX_TX_DLCI3_DATA_SIGNAL        =  8,  /* Logical Client(via SIO) --> DS_MUX data sig  */
  DS_MUX_TX_DLCI4_DATA_SIGNAL        =  9,  /* Logical Client(via SIO) --> DS_MUX data sig  */
  DS_MUX_TX_DLCI5_DATA_SIGNAL        =  10,  /* Logical Client(via SIO) --> DS_MUX data sig  */
  DS_MUX_TX_DLCI6_DATA_SIGNAL        =  11,  /* Logical Client(via SIO) --> DS_MUX data sig  */
  DS_MUX_TX_DLCI7_DATA_SIGNAL        =  12, /* Logical Client(via SIO) --> DS_MUX data sig  */
        
  /*-------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
  -------------------------------------------------------------------------*/
  DS_MUX_TASK_OFFLINE_SIGNAL         = 13,   /* Reserved TASK_OFFLINE_SIG     */
  DS_MUX_TASK_STOP_SIGNAL            = 14,   /* Reserved TASK_STOP_SIG        */
  DS_MUX_TASK_START_SIGNAL           = 15,   /* Reserved TASK_START_SIG       */

  DS_MUX_MAX_SIGNALS                         /* use next value for enum       */
} ds_mux_sig_enum_type;


/*---------------------------------------------------------------------------
  Data Task Command Definitions. These are the commands that can be sent to
  one.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_MUX_MIN_CMD                   = -1,
  DS_MUX_CMD                       =  0,
  /* Always must be last */
  DS_MUX_MAX_DEFINED_CMD_TYPES
} ds_mux_cmd_enum_type;

typedef enum
{
   DS_MUX_STATE_CLOSED              =  1,
   DS_MUX_STATE_INITIALIZING        =  2,
   DS_MUX_STATE_PHY_PORT_CONFIG     =  3,
   DS_MUX_STATE_OPENED              =  4,
   DS_MUX_STATE_PSC_RCVD            =  5,
   DS_MUX_STATE_IN_POWER_SAVE       =  6,
   DS_MUX_STATE_WAKEUP_FLAG_SENT    =  8,
   DS_MUX_STATE_WAKEUP_RCVD         =  9,
   DS_MUX_STATE_CLD_RCVD            =  10,
   DS_MUX_STATE_MAX                 = 0xFF
}ds_mux_state_enum_type;

/*---------------------------------------------------------------------------
  External client notify events.
---------------------------------------------------------------------------*/
typedef enum
{
   DS_MUX_EVENT_MIN               =  0,
   DS_MUX_EVENT_INIT_START        =  1,
   DS_MUX_EVENT_INIT_COMPLETE     =  2,
   DS_MUX_EVENT_INIT_FAILED       =  3,
   DS_MUX_EVENT_CLOSE_START       =  4,
   DS_MUX_EVENT_CLOSE_COMPLETE    =  5,
   DS_MUX_EVENT_CLOSE_FAILED      =  6,
   DS_MUX_EVENT_MAX               = 0xFF
}ds_mux_ext_client_event_enum_type;


/*---------------------------------------------------------------------------
  Result code.
---------------------------------------------------------------------------*/
typedef enum
{
   DS_MUX_SUCCESS                 =  0,
   DS_MUX_FAILURE                 =  1,
   DS_MUX_NULL_PARAM              =  2,
   DS_MUX_INVALID_PARAM           =  3,
   DS_MUX_INVALID_MUX_STATE       =  4,
   DS_MUX_FLOW_DISABLED           =  5,
   DS_MUX_CLD_PROCESSED           =  6,
   DS_MUX_MAX                     = 0xFF
}ds_mux_result_enum_type;

typedef enum
{
  DS_MUX_DLCI_MIN               = 0,
  DS_MUX_DLCI_INITIALIZED       = DS_MUX_DLCI_MIN,
  DS_MUX_DLCI_ALLOCATING        = 1,
  DS_MUX_DLCI_CONNECTED         = 2,
  DS_MUX_DLCI_DISCONNECTING     = 3,
  DS_MUX_DLCI_DISCONNECTED      = 4,
}ds_mux_dlci_state_enum_type;


typedef enum
{
  DS_MUX_SET_NONE            = 0x0,
  DS_MUX_SET_MODE            = 0x1,
  DS_MUX_SET_SUBSET          = 0x2,
  DS_MUX_SET_PORT_SPEED      = 0x4,
  DS_MUX_SET_FRAME_SIZE_N1   = 0x8,
  DS_MUX_SET_ACK_TIMER_T1    = 0x10,
  DS_MUX_SET_RE_TRIES_N2     = 0x20,
  DS_MUX_SET_RESP_TIMER_T2   = 0x40,
  DS_MUX_SET_WAKEUP_TIMER_T3 = 0x80,
  DS_MUX_SET_WINDOW_SIZE_K   = 0x100,
  DS_MUX_SET_ALL_CMUX_FIELDS = 0x100
  
}ds_mux_cmux_mask_enum_type;


typedef enum
{
  DS_MUX_MODE_MIN = 0,
  DS_MUX_MODE_BASIC = DS_MUX_MODE_MIN,
  DS_MUX_MODE_ADVANCED = 1,
}ds_mux_mode_enum_type;


typedef enum
{
  DS_MUX_SUBSET_MIN = 0,
  DS_MUX_SUBSET_UIH = DS_MUX_SUBSET_MIN,
  DS_MUX_SUBSET_UI  = 1,
  DS_MUX_SUBSET_I   = 2,
}ds_mux_subset_enum_type;

typedef enum
{
  DS_MUX_PHY_PORT_SPEED_INVALID = 0,
  DS_MUX_PHY_PORT_SPEED_1 = 1,  /* 9,600 bit/s */
  DS_MUX_PHY_PORT_SPEED_2 = 2,  /* 19,200 bit/s */
  DS_MUX_PHY_PORT_SPEED_3 = 3,  /* 38,400 bit/s */
  DS_MUX_PHY_PORT_SPEED_4 = 4,  /* 57,600 bit/s */
  DS_MUX_PHY_PORT_SPEED_5 = 5,  /* 115,200 bit/s */
  DS_MUX_PHY_PORT_SPEED_6 = 6,  /* 230,400 bit/s */
}ds_mux_port_speed_enum_type;

typedef enum
{
  DS_MUX_WINDOW_SIZE_INVALID = 0,
  DS_MUX_WINDOW_SIZE_1 = 1,
  DS_MUX_WINDOW_SIZE_2 = 2,
  DS_MUX_WINDOW_SIZE_3 = 3,
  DS_MUX_WINDOW_SIZE_4 = 4,
  DS_MUX_WINDOW_SIZE_5 = 5,
  DS_MUX_WINDOW_SIZE_6 = 6,
  DS_MUX_WINDOW_SIZE_7 = 7,
}ds_mux_window_size_enum_type;

typedef enum
{
  DS_MUX_CL_TYPE_INVALID=0, /* This enumerated value is for internal use */
  DS_MUX_CL_TYPE_1,
  DS_MUX_CL_TYPE_2,
  DS_MUX_CL_TYPE_3,
  DS_MUX_CL_TYPE_4,
  DS_MUX_CL_TYPE_MAX
}ds_mux_cl_type_enum_type;

/**
 *  Enum to identify the Frame Type
 **/
typedef enum
{
  DS_MUX_FRAME_TYPE_INVALID_FRAME,
  DS_MUX_FRAME_TYPE_UTIL_SABM,
  DS_MUX_FRAME_TYPE_UTIL_UA,
  DS_MUX_FRAME_TYPE_UTIL_DM,
  DS_MUX_FRAME_TYPE_UTIL_DISC,
  DS_MUX_FRAME_TYPE_UTIL_UIH,
  DS_MUX_FRAME_TYPE_UTIL_UI,
  DS_MUX_FRAME_TYPE_MAX
} ds_mux_frame_type_enum_type;

/* enum for the control channel message types*/
typedef enum
{
  DS_MUX_MSG_TYPE_VAL_INVALID = 0,
  DS_MUX_MSG_TYPE_VAL_PN      = 1,
  DS_MUX_MSG_TYPE_VAL_PSC     = 2,
  DS_MUX_MSG_TYPE_VAL_CLD     = 3, 
  DS_MUX_MSG_TYPE_VAL_TEST    = 4,
  DS_MUX_MSG_TYPE_VAL_FCON    = 5,
  DS_MUX_MSG_TYPE_VAL_FCOFF   = 6,
  DS_MUX_MSG_TYPE_VAL_MSC     = 7,
  DS_MUX_MSG_TYPE_VAL_NSC     = 8,
  DS_MUX_MSG_TYPE_VAL_RPN     = 9,/* not supported*/
  DS_MUX_MSG_TYPE_VAL_RLS     = 10,/* not supported*/
  DS_MUX_MSG_TYPE_VAL_SNC     = 11, /* not supported*/

}ds_mux_msg_type_enum_type;

typedef enum
{
  DS_MUX_FLOW_DISABLE_MIN    = 0x0,
  DS_MUX_FLOW_DISABLE_FCON   = 0x1,
  DS_MUX_FLOW_DISABLE_MSC    = 0x2,
  DS_MUX_FLOW_DISABLE_PSC    = 0x4,
  DS_MUX_FLOW_DISABLE_RX_WMK = 0x8,
  DS_MUX_FLOW_DISABLE_TX_WMK = 0x10,
  DS_MUX_FLOW_DISABLE_LPM    = 0x11,
  DS_MUX_FLOW_DISABLE_PHY_DTR_LOW   = 0x12

}ds_mux_flow_ctl_mask_enum_type;

/*--------------------------------------------------------------------------
  Type definition for the signal handler.
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
---------------------------------------------------------------------------*/

typedef boolean (*ds_mux_sig_handler_type)
(
  ds_mux_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                         /* Parameter carrying user data */
);

/*--------------------------------------------------------------------------
  Type definition for the command handler
---------------------------------------------------------------------------*/

typedef void (*ds_mux_cmd_handler_type)
(
  ds_mux_cmd_enum_type cmd,        /* Actual command to be processed         */
  void *user_data_ptr              /* Command specific user parameters       */
);

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/

typedef struct
{
  ds_mux_sig_handler_type sig_handler;
  void *user_data_ptr;
} ds_mux_sig_handler_info_type;

/*--------------------------------------------------------------------------
  DS_MUX Task Command queues and Command Data Buffers
---------------------------------------------------------------------------*/

typedef struct     
{
  q_link_type       link;		     /* For linking onto cmd queue             */
  ds_mux_cmd_enum_type cmd;         /* Actual command to be processed         */
  void              *user_data_ptr; /* Command specific user parameters    */
} ds_mux_cmd_type;

/*--------------------------------------------------------------------------
  +CMUX AT commands parameter
---------------------------------------------------------------------------*/
typedef struct 
{

  uint16   mask;
  /**<  represent fields        */
  ds_mux_mode_enum_type   operating_mode;               
  /**< Mode 0 - basic, 1- advanced.        */
  ds_mux_subset_enum_type  subset;                         
  /**< 0-UIH, 1-UI, 2-I frames.            */
  ds_mux_port_speed_enum_type  port_speed;                    
  /**< port speed valid values: 1-6.       */
  uint16  frame_size_N1;            
  /**< Max frame size (1-32768).           */
  uint16  response_timer_T1;            
  /**<  Time UE waits for an acknowledgement before
   resorting to other action (e.g. transmitting a frame)
   default 100ms min value 10 ms.
 */
  uint8   re_transmissions_N2;      
  /**< Max re-tries N2 (0-255).            */
   
  uint16  response_timer_T2;            
  /**< Time mux control channel waits before 
 re-transmitting a command default 300ms and
 min value 20 ms.
 */
  uint32  wake_up_timer_T3;            
  /**<  Time UE waits for response for a 
 power wake-up command default 10ms and min
 value 1 ms.
 */
  uint8   window_size_k;                
  /**< Window size default 2, range (1-7). */

} dlci_cmux_param_type;



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DS_MUX_DEFS_H */
