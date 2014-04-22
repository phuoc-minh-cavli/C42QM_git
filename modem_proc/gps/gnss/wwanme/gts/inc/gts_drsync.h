#ifndef GTS_DRSYNC_H
#define GTS_DRSYNC_H
/*===========================================================================

  GTS Dead Reckoning Sync Header File

DESCRIPTION

  This file contains the interfaces into the GTS Dead Reckoning Sync module.
  DRSync API only supports one client at a time, since multiple clients 
  does not make sense for this feature. 

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2023 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: 

when         who     what, where, why
----------   ---     ------------------------------------------------------
12/03/09     dmc     Initial release
==========================================================================*/

/*--------------------------------------------------------------------------
 * Include Files
 * -----------------------------------------------------------------------*/
#include "gps_common.h"
#include "gnss_common.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/

/* Max delay in waiting for GPS engine to be enabled when registering PPS HW for 
 * external clients */
#define GTS_PPS_CLIENT_REGISTER_DELAY_TIMER_MS  1
#define GTS_PPS_CLIENT_REGISTER_DELAY_MAX_WAIT 30

/*--------------------------------------------------------------------------
 * Type Declarations
 * -----------------------------------------------------------------------*/
/* Enum types for configuring GPS GPIO pin on SDX55 */
typedef enum
{
  GTS_GPIO_DISABLE = 0,
  GTS_GPIO_PPS_IN,
  GTS_GPIO_EXT_BLANKING,
  GTS_GPIO_DR_SYNC,
  GTS_GPIO_TST_DEBUG_OUT
} gts_GpioSelOptionType;

typedef enum
{
  GTS_DRSYNC_IDLE = 0,
  GTS_DRSYNC_RUNNING
} gts_DRSyncStateType;

/* This structure contains all the data and state information that the
 * DR Sync keeps track of. */
typedef struct
{
  /* Chip Family Info */
  DalChipInfoFamilyType e_ChipInfoFamily; 
  DalPlatformInfoPlatformType e_PlatformInfo;
  U32 q_DalGpioConfig;

  /* Handle for TLMM GPIO handler. Once attached, handler would
   stay the same until reboot */
  DalDeviceHandle *p_hTlmm;

  /* Handle to TLMM GPIO interrupt object. Once attached, handler would stay
     the same until reboot */
  DalDeviceHandle *p_HGpioInt;

  /* Handle to get chip info */
  DalDeviceHandle *p_ChipInfo;

  gts_PulseClientType e_ClientInUse;

  /* Type of DRSync Request */

  /*DR Sync Feature on PoR*/  
  boolean b_FeatureEn;           

  /* DR Sync module enabled/disabled */
  boolean b_Enabled;                

  /* Hardware initialization needed */
  boolean b_HWInitNeeded;           

  /* DR Sync state */
  gts_DRSyncStateType e_State;       

  /* DR Sync config */
  gnss_DRSyncConfigType z_Config;  

  /* GTS Time info structure */
  gts_TimeType z_GtsTime;           

  U32 q_Count;

  /* Trigger Frequency */
  U8 u_TrigFreq;

  /* DPO Status */
  U8 b_DpoStatus;
  
  /* USTMR Counts used for Assert and Deassert */
  U64 t_ATUstmr;
  U64 t_DATUstmr;
  U64 t_ProgUstmr;  

  boolean u_PulseSet;
  
  /* Hardware initialization needed */
  boolean b_IntRegistered;

  /* PPS and ISR Count */
  U32 q_PpsCount; 
  U32 q_ISRCount; 

  /* GPIO number */
  U32 q_GpioNum;
  U32 q_GpioOption;

  /* PPS processing counter */ 
  U32 q_PpsProcCtr;

  /* PPS AssertMs */
  DBL d_AssertTimeMsec;

  /* Qtime at which ISR is triggered */
  gts_QtimeType z_IsrQtime;

  /* non-deferrable Timer to program assert and de-assert value. Once expire write HIGH or LOW to PPS GPIO based on timer ID */
  os_TimerMsgType *gts_ProgAssertTimer;
  os_TimerMsgType *gts_ProgDeAssertTimer;
} gts_DRSyncParamType;

/*--------------------------------------------------------------------------
 * Variable Prototypes
 *-----------------------------------------------------------------------*/

/* Local data structure to store DRSync various info and state. Used by GTS module */
extern gts_DRSyncParamType gts_DRSyncParams;

/*--------------------------------------------------------------------------
 * Function Declarations
 * -----------------------------------------------------------------------*/

/* -------------------------------------------------------------------------
 * gts_DRSyncInit
 *
 * Description: 
 *   This function initializes some local variables and reads configuration
 *  paramaters from NV.  NV must be initialized before this function can be 
 *  called.
 *
 * Parameters:
 *   None
 *
 * Return value: 
 *   None
 * -----------------------------------------------------------------------*/
void gts_DRSyncInit();

/* -------------------------------------------------------------------------
 * gts_DRSyncConfigSet
 *
 * Description: 
 *   This function sets the Dead Reckoning Sync configuration parameters.
 *   DRSync API only supports one client at a time, since multiple clients 
 *   does not make sense for this feature. DRSync API is only exposed to 
 *   the MC module, clients access via IPC.
 *
 * Parameters: 
 *   p_Config - Pointer to config structure
 *
 * Return value: None
 * -----------------------------------------------------------------------*/
void gts_DRSyncConfigSet( const gnss_DRSyncConfigType *p_Config );

/* -------------------------------------------------------------------------
 * gts_DRSyncConfigGet
 *
 * Description: 
 *   This function gets the Dead Reckoning Sync configuration parameters.
 *   DRSync API only supports one client at a time, since multiple clients 
 *   does not make sense for this feature. DRSync API is only exposed to 
 *   the MC module, clients access via IPC. The response is sent via IPC 
 *   to SM.
 *
 * Parameters: 
 *   p_Config - Pointer to config structure
 *
 * Return value: None
 * -----------------------------------------------------------------------*/
void gts_DRSyncConfigGet(void);

/* -------------------------------------------------------------------------
 * gts_DRSyncArm
 *
 * Description: 
 *   This function arms the DRSync module such that a pulse will be generated.
 *   DRSync API only supports one client at a time, since multiple clients 
 *   does not make sense for this feature. DRSync API is only exposed to 
 *   the MC module, clients access via IPC.
 *
 * Parameters: None
 *
 * Return value: None
 * -----------------------------------------------------------------------*/
void gts_DRSyncArm(void);

/* -------------------------------------------------------------------------
 * gts_DRSyncActive
 *
 * Description: 
 *   This function checks to see if the DR Sync module is active or not.
 *
 * Parameters: None
 *
 * Return value: 
 *   TRUE or FALSE
 * -----------------------------------------------------------------------*/
boolean gts_DRSyncActive(void);

/* -------------------------------------------------------------------------
 * gts_DRSyncProc
 *
 * Description: 
 *   This function is called from the 1 second MC tick processing loop and 
 *  will set up the next DR sync pulse as needed.
 *
 * Parameters: 
 *   p_Clock - Pointer to clock structure
 *
 * Return value: None
 * -----------------------------------------------------------------------*/
void gts_DRSyncProc(void);

/*
 ******************************************************************************
 * Function:
 *   gts_ProcDRSyncAssertTimerExpiry
 *
 * Description:
 *   This function processes program assert timer expiry
 *
 * Parameters:
 *   None
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/
void gts_ProcDRSyncAssertTimerExpiry ( void );

/*
 ******************************************************************************
 * Function:
 *   gts_ProcDRSyncDeAssertTimerExpiry
 *
 * Description:
 *   This function processes program de-assert timer expiry
 *
 * Parameters:
 *   None
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/
void gts_ProcDRSyncDeAssertTimerExpiry ( void );

/*
 ******************************************************************************
 * Function:
 *
 *   gts_ProcDiagPpsPolarityCmd
 *
 * Description:
 *
 *  Processes DIAG cmd to switch pulse polarity. Default: Pulse HIGH
 *
 * Parameters:
 *
 *  u_Param - 0 - Pulse HIGH, 1 - Pulse LOW
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_ProcDiagPpsPolarityCmd( const uint8 u_Param );

/*
 ******************************************************************************
 * Function:
 *
 *  gts_ResetGpioHwConfig
 *
 * Description:
 *
 *  Processes cmd to reset GPIO HW config
 *
 * Parameters:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_ResetGpioHwConfig( void );


#endif /* GTS_DRSYNC_H */

