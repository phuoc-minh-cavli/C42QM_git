#ifndef CUSTDIAG_H
#define CUSTDIAG_H
/*===========================================================================

DESCRIPTION
  Configuration for DIAG.

  Copyright (c) 2002-2012  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tn/4.0/cust/custdiag.h#1 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/02/12   is      Support handling reset and powerdown CM commands on Master Diag
11/04/11   is      Support for buffered threshold mode
09/20/11   hm      Enable DLOAD command only on Sparrow 
08/28/11   hm      Enabled NV, ESN and ignoring reset on modem
07/21/11   hm      Uncommented GSM & WCDMA log codes
05/10/11   hm      Featurised DIAG_CM_AVAIL to be defined only on modem
04/20/11   hm      Define DIAG_FEATURE_EFS2
04/08/11   is      Enable Central Routing for modem in MP mode
03/25/11   is      Fix compilation errors for modem-only configuration
03/15/11   is      Sync with MDM9600 feature flags
03/04/11   is      Support for RIVA and enable central routing
03/01/11   is      Use C/C++ strings for 8960 modem
02/22/11   is      Feature flag cleanup and support for Diag over SMD-Lite
01/20/11   is      Resolve compilation error on bring-up Q6 modem
10/20/10   is      Updated for 8960 Apps only bringup
05/19/10   sg      Added ONDEVICE_DIAG_MOUNT,ONDEVICE_DIAG_DIR
05/10/10   is      Added DIAG_CM_AVAIL
02/26/10   sg      Added DIAG_TX_SLEEP_THRESHOLD_DEFAULT
                   DIAG_TX_SLEEP_TIME_DEFAULT
03/12/09   vg      Added FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
02/24/06   hn      Removed old FEATURE_DIAG_PACKET_COUPLING
04/22/05   adm     Added FEATURE_SMD also for diagcomm_fwd compilation check
11/22/04    cr     Only compile diagcomm_fwd for multi-processor builds.
11/11/02   cah     Added GSM features.
05/01/02   jct     Created
===========================================================================*/

/*---------------------------------------------------------------------------
                             Diagnostics
---------------------------------------------------------------------------*/

/* ------ *.builds file features ------
   #define FEATURE_DIAG_MP_MASTER_APPS - Diag MP (Master on Apps) configuration [DEFAULT]
   #define FEATURE_DIAG_APPS_ONLY - Diag Apps-only configuration
   #define FEATURE_DIAG_MODEM_ONLY - Diag Modem-only configuration
*/

#ifdef FEATURE_EFS2
  #define DIAG_FEATURE_EFS2
#endif

#if defined(IMAGE_APPS_PROC)
  #define DIAG_IMAGE_APPS_PROC
  #define DIAG_FRAMING
  #define DIAG_SIO_SUPPORT          /* Supports SIO/SMD */
  #define DIAG_TOOLSDIAG_INIT       /* Initialize toolsdiag table */
  #define DIAG_TOOLSDIAG_DLOAD      /* DLoad moved to Sparrow */
  #define DIAG_BUFFERING_SUPPORT    /* Support for buffering modes */
  #define DIAG_C_STRING             /* Use C/C++ strlcpy() API */
  #define DIAG_DYNAMIC_SLEEP        /* Dynamic sleep is supported */
  #define DIAG_HANDLE_CM_RESET      /* Handle modem reset command on Apps */
  #define DIAG_HANDLE_CM_POWER_OFF  /* Handle power off command on Apps */
  
  #ifdef FEATURE_DIAG_APPS_ONLY
    #define DIAG_RSP_SEND           /* Specifies this proc will send the common table rsp */
    #define DIAG_APPS_ONLY_BRINGUP  /* New feature to identify APPS ONLY diag, as opposed to say Diag on single-proc targets */
    #define DIAG_TOOLSDIAG_ESN
  #else
    #define DIAG_MP_MASTER          /* Defines this as the Master Diag in MP configuration */
    #define DIAG_REQ_FWD            /* Forward requests to slave procs */
    #define DIAG_SMDL_SUPPORT         /* Supports SMDL */
    #define DIAG_SMD_SUPPORT        /* SMD needed for MP mode only */
    #define DIAG_MP                 /* Multi-processor Diag */
    #define DIAG_CENTRAL_ROUTING    /* Enables central routing */
    #define DIAG_TOOLSDIAG_DLOAD_ONLY /* Enables only the DLOAD command on Sparrow */
  #endif
#endif /* IMAGE_APPS_PROC */

#if defined(IMAGE_LPASS_PROC)
  #define DIAG_IMAGE_QDSP6_PROC     /* TODO: This feature should be changed to DIAG_IMAGE_LPASS_PROC */
  #define DIAG_SIO_SUPPORT          /* Supports SIO/SMD */
  #define DIAG_SMD_SUPPORT          /* SMD needed for MP mode only */
  #define DIAG_MP                   /* Multi-processor Diag */
  #define DIAG_MP_SLAVE_LAST
#endif /* IMAGE_LPASS_PROC */

#if defined(IMAGE_MODEM_PROC) && !defined(WCN_IMAGE)
  #define DIAG_IMAGE_MODEM_PROC
  #define DIAG_TOOLSDIAG_INIT
  #define DIAG_TOOLSDIAG_ESN
  #define DIAG_RSP_SEND             /* Specifies this proc will send the common table rsp */
  #define DIAG_SIO_SUPPORT          /* Supports SIO/SMD */
  #define FEATURE_DIAG_NV           /* Enable diagnv; Only modem 8960 has NV right now */
  #define DIAG_CHECK_SECURITY       /* Defines missing chk_security_code_ptr[] */
  #define DIAG_C_STRING             /* Use C/C++ strlcpy() API */
  #define DIAG_DIAGTBL_INIT         /* Registers a set of Diag commands on this proc */
  
  #ifdef FEATURE_DIAG_MODEM_ONLY
    #define DIAG_FRAMING
  #else
    #define DIAG_MP_SLAVE_LAST
    #define DIAG_SMD_SUPPORT        /* SMD needed for MP mode only */
    #define DIAG_MP                 /* Multi-processor Diag */
    #define DIAG_CENTRAL_ROUTING    /* Enables central routing */
    #define DIAG_IGNORE_RESET	
  #endif
#endif /* IMAGE_MODEM_PROC */

#if defined(WCN_IMAGE)              /* RIVA */
  #define DIAG_IMAGE_RIVA_PROC
  #define DIAG_SMDL_SUPPORT         /* Supports SMDL */
  #define DIAG_SMD_SUPPORT          /* SMD needed for MP mode only */
  #define DIAG_MP                   /* Multi-processor Diag */
  #define DIAG_MP_SLAVE_LAST
  #define DIAG_CENTRAL_ROUTING      /* Enables central routing */
#endif /* WCN_IMAGE */

#define FEATURE_DIAG_TASK           /* Defines the Diag task */
#define DIAG_DOG_SUPPORT            /* Build supports Dog */

#if defined(DIAG_SIO_SUPPORT) && defined(FEATURE_DIAG_DEFAULT_BITRATE_NV)
  #define DIAG_DEFAULT_BITRATE_NV   /* Enables DIAG baud to be stored and retrieved from NV */
#endif

/* Diag should check validity of address when doing memory ops. */
#define FEATURE_DIAG_HW_ADDR_CHECK

/* Manually register dispatch tables. */
#define FEATURE_DIAG_MANUAL_DISPATCH_TABLE

/* Diag config values */
#define DIAG_TX_SLEEP_THRESHOLD_DEFAULT (1024*16)
#define DIAG_TX_SLEEP_TIME_DEFAULT 10

/* On Device feature */
//#define FEATURE_ONDEVICE_DIAG
#define ONDEVICE_DIAG_MOUNT   "/hdev/sdc4h0p1"
#define ONDEVICE_DIAG_DIR     "/hdev/sdc4h0p1/diag"
#define ONDEVICE_DIAG_CFG     "/hdev/sdc4h0p1/config/Diag.cfg"  

  /* ---------------------------------------------------------------------
   *                    Core Image Feature Flags
   * --------------------------------------------------------------------- */
#ifdef FEATURE_COREIMG

  /* COREIMG Feature flags can be added here */

#else /* #ifdef FEATURE_COREIMG */

  /* ---------------------------------------------------------------------
   *                 Non-Core Image Feature Flags
   * --------------------------------------------------------------------- */

#ifdef FEATURE_IS2000
  #define DIAG_IS2000
#endif /* FEATURE_IS2000 */

#ifdef FEATURE_ADSP_DIAG
  #define FEATURE_DIAG_ADSP_CALLBACK
#endif /* FEATURE_ADSP_DIAG */
   
#if !defined(FEATURE_GSM_PLT) && !defined(FEATURE_HWTC) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_LTE_PLT) && defined (IMAGE_MODEM_PROC)
  #define DIAG_CM_AVAIL
#endif 

#if defined(IMAGE_MODEM_PROC) && defined(FEATURE_WCDMA)
  #define FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
#endif


#if defined(FEATURE_WCDMA)
  #define DIAG_WCDMA
#endif

#if defined(FEATURE_GSM)
  #define DIAG_GSM
#endif


#endif /* #ifndef FEATURE_COREIMG */
#endif /* CUSTDIAG_H */

