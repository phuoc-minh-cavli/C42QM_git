/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 C A L L   M A N A G E R   D I A G N O S T I C S   P A C K E T   M O D U L E

GENERAL DESCRIPTION
  This module contains function to support diagnostic packet processing.

  These functions support
  1. Call origination (CDMA subsystem).
  2. Call end (CDMA subsystem).
  3. Change of operating mode.


EXTERNALIZED FUNCTIONS

  cmdiag_init
    Registers the callback functions for call origination, call end and
    change of operating mode with the diagnostic monitor.



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmdiag_init() must be called to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1991 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           Edit History

$Header: //commercial/MPSS.CT.1.0/Main/modem_proc/stubs/src/cmdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "cm.h"
#include "sys.h"
#include "assert.h"
#include "msg.h"

//static boolean cmdiag_inited = FALSE;

/*==========================================================================

PACKET   DIAG_CONTROL_F

PURPOSE  Request sent from the DM to the DMSS to change the operating mode

============================================================================*/
typedef struct
{
  uint8 xx_cmd_code;
  uint16 mode;            /* Mode to change to */
}
cmdiag_mode_control_req_type;

typedef cmdiag_mode_control_req_type cmdiag_mode_control_rsp_type;

/* Mode change commands */
typedef enum
{
  CMDIAG_MODE_OFFLINE_A_F = 0,  /* Go to offline analog */
  CMDIAG_MODE_OFFLINE_D_F,      /* Go to offline digital */
  CMDIAG_MODE_RESET_F,          /* Reset. Only exit from offline */
  CMDIAG_MODE_FTM_F,            /* FTM mode - if supported */
  CMDIAG_MODE_ONLINE_F,         /* Online mode - if supported */
  CMDIAG_MODE_LPM_F,            /* LPM mode - if supported */
  CMDIAG_MODE_POWER_OFF_F,        /* Power off mode */
  CMDIAG_MODE_MAX_F             /* Last (and invalid) mode enum value */
}
cmdiag_mode_enum_type;

#define CM_STATE_INFO                       0

/* Mode change request                        */
#define DIAG_CONTROL_F    41 

/*---------------------------------------------------------------------------
    CM STATE INFO -- SUBCOMMAND 0
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_STATE_INFO)
 /* No additional fields */
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_STATE_INFO)
  int  call_state;                 /* ENUM for EVENT_ID 416 */
  int  oprt_mode;                  /* ENUM for EVENT_ID 417 */
  int  system_mode;                /* ENUM for EVENT_ID 418 */
  int  mode_pref;                  /* ENUM for EVENT_ID 520 */
  int  band_pref;                  /* ENUM for EVENT_ID 521 */
  int  roam_pref;                  /* ENUM for EVENT_ID 522 */
  int  srv_domain_pref;            /* ENUM for EVENT_ID 523 */
  int  acq_order_pref;             /* ENUM for EVENT_ID 524 */
  int  hybr_pref;                  /* ENUM for EVENT_ID 525 */
  int  network_sel_mode_pref;      /* ENUM for EVENT_ID 526 */
  int  srv_status;
DIAGPKT_RSP_END

static sys_oprt_mode_e_type oprt_mode = SYS_OPRT_MODE_NONE;

/*===========================================================================
Function: cmdiag_cm_state_info
Description:

       This function processes the umts specific call manager state
       information request packet from QXDM and sends the response back.

============================================================================*/
PACKED void *cmdiag_cm_state_info
(
  PACKED void* req_pkt,

  uint16       pkt_len
)
{
  DIAG_SUBSYS_CM_CM_STATE_INFO_rsp_type *rsp_ptr ;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_STATE_INFO_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_STATE_INFO_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_STATE_INFO,
                                 rsp_len );

  if (rsp_ptr != NULL)
  {
    /* Fill the response packet */
    rsp_ptr->call_state            = 0;
    rsp_ptr->oprt_mode             = oprt_mode; /* for now, this is the one we want to change and notify TM */
    rsp_ptr->system_mode           = SYS_SYS_MODE_NO_SRV;
    rsp_ptr->mode_pref             = CM_MODE_PREF_GPS_ONLY;
    rsp_ptr->acq_order_pref        = CM_GW_ACQ_ORDER_PREF_AUTOMATIC;
    rsp_ptr->srv_domain_pref       = CM_SRV_DOMAIN_PREF_ANY;
    rsp_ptr->band_pref             = (int)CM_BAND_PREF_NO_CHANGE;
    rsp_ptr->roam_pref             = CM_ROAM_PREF_ANY;
    rsp_ptr->hybr_pref             = CM_HYBR_PREF_OFF;
    rsp_ptr->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
    rsp_ptr->srv_status            = SYS_SRV_STATUS_NO_SRV;
  }

  return rsp_ptr ;
}

/*===========================================================================

FUNCTION CMDIAG_CONTROL

DESCRIPTION
  This procedure processes a request to change modes of operation in the
  DMSS.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void *cmdiag_control (PACKED void *req_pkt, uint16 pkt_len)
{

  cmdiag_mode_control_req_type *req = NULL;
  cmdiag_mode_control_rsp_type *rsp;
  const int rsp_len = sizeof (cmdiag_mode_control_rsp_type);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(req_pkt != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req = (cmdiag_mode_control_req_type *) req_pkt;

  MSG_MED ("Received mode change to %d", req->mode, 0, 0);

  switch (req->mode)
  {
    case CMDIAG_MODE_OFFLINE_A_F:
    case CMDIAG_MODE_OFFLINE_D_F:

      oprt_mode = SYS_OPRT_MODE_OFFLINE_CDMA;

#if 0 /* need to handle FTM mode */
      if (ftm_mode == FTM_MODE)
      {
        /* Instruct the task service to set the system to offline state. */
        tmc_task_offline ();
      }
#endif
      break;

    case CMDIAG_MODE_FTM_F:
      oprt_mode = SYS_OPRT_MODE_FTM;
      break;

    case CMDIAG_MODE_ONLINE_F:
      oprt_mode = SYS_OPRT_MODE_ONLINE;
      break;

    case CMDIAG_MODE_LPM_F:
      oprt_mode = SYS_OPRT_MODE_LPM;
      break;

    case CMDIAG_MODE_RESET_F:
      oprt_mode = SYS_OPRT_MODE_RESET;
      break;

    case CMDIAG_MODE_POWER_OFF_F:
      oprt_mode = SYS_OPRT_MODE_PWROFF;
      break;

    default:
      MSG_HIGH ("Received invalid mode control %d", req->mode, 0, 0);
  }

  if (oprt_mode != SYS_OPRT_MODE_NONE)
  {
    /* Notify TM here */

    rsp = (cmdiag_mode_control_rsp_type *)
      diagpkt_alloc (DIAG_CONTROL_F, rsp_len);

    if(rsp==NULL)
    {
        MSG_HIGH( "rsp_NULL",0,0,0);
        return NULL;
    }
    rsp->mode = req->mode;
  }
  else
  {
    rsp = (cmdiag_mode_control_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len);
  }

  return (rsp);
}

/* Dispatch table to register with the diagnostics packet service. */
static const diagpkt_user_table_entry_type cmdiag_tbl[] = {
  {DIAG_CONTROL_F,    DIAG_CONTROL_F,     cmdiag_control},
};

static const diagpkt_user_table_entry_type cm_subsys_diag_tbl[] =
{
  {CM_STATE_INFO,                       CM_STATE_INFO,
                                        cmdiag_cm_state_info},  
};

//DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, cmdiag_tbl);
//DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_CM, cm_subsys_diag_tbl);

/* Not sure if we need this function */
void cmdiag_init (void)
{
  //cmdiag_inited = TRUE;

  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, cmdiag_tbl);
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_CM, cm_subsys_diag_tbl);
}

