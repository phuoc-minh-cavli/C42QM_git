/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             GPS Common Module

GENERAL DESCRIPTION
  This module contains the functions for Generic Data Transport (GDT).
  GDT is used as a bulk data transfer method between different modules in GNSS
  subsystem in Modem Processor (MP) to Application Processor (AP).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2014-2015 Qualcomm Atheros, Inc. All Rights Reserved. 
  Qualcomm Atheros Confidential and Proprietary.

Copyright (c) 2015 - 2020 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/gdt/2_0/src/gnss_gdt.c#1 $ 
  $DateTime: 2020/07/21 21:23:44 $ 
  $Author: pwbldsvc $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/25/14    ah     Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "gnss_gdt.h"
#include "gnss_gdt_protected.h"
#include "tle_api.h"
#include "tm_api.h"
#include "aries_os_api.h"
#include "gps_fs_api.h"
#include "event.h"
#include "event_defs.h"
#include "aries_os_api.h"
#ifndef FEATURE_QMI_PDS_EXT
#include "loc_gdt.h"
#endif /* ! FEATURE_QMI_PDS_EXT */

#ifndef FEATURE_CGPS_PLAYBACK
#include <stdio.h>
#include "stringl.h"
#include "err.h"
#include "msg.h"
#endif

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/


/*=============================================================================

       Macros

=============================================================================*/



/*=============================================================================

       Typedefs

=============================================================================*/

typedef struct 
{
  boolean (*StartCbApi)( gdt_StartArgumentType z_StartParam );
  boolean (*OpenCbApi)( gdt_OpenArgumentType z_OpenParam );
  boolean (*SendAckCbApi)( gdt_SendAckArgumentType z_SendAckParam );
  boolean (*CloseCbApi)( gdt_CloseArgumentType z_CloseParam );
  boolean (*EndCbApi)( gdt_EndArgumentType z_EndParam );
  boolean (*ReceiveReadyCbApi)( gdt_ReceiveReadyArgumentType z_RcvRdyParam );
  boolean (*ReceiveOpenCbApi)( gdt_OpenArgumentType z_RcvOpenParam );
  boolean (*ReceiveDataCbApi)( gdt_ReceiveDataArgumentType z_RcvDataParam );
  boolean (*ReceiveCloseCbApi)( gdt_CloseArgumentType z_RcvCloseParam );
  boolean (*ReceiveEndCbApi)( gdt_ReceiveEndArgumentType z_RcvEndParam );
} gdt_CallBackFnType;

typedef struct 
{
  e_GDTServiceIDType e_Service;
  char               FilePath[C_GNSS_GDT_FILEPATH_LEN]; 
  e_GDTServiceType   e_ServiceType;
  gdt_CallBackFnType z_CbInfo;
} gdt_SubscriberType;

typedef struct
{
  uint32          q_SessId;
  boolean         b_SessIdValid;
  gdt_MemHandle   p_MemHandle;
  uint32          q_BufSize;
  boolean         b_LockStatus;
  int             l_FileDescriptor;
  boolean         b_FileDescriptorValid;
} gdt_BufTableEntry;


const gdt_SubscriberType gdt_Users[] = 
{ { e_GDT_SERVICE_GTP_WWAN, "/hlos/gdt/MyTdpFile.txt",
    e_GDT_SERVICE_TYPE_SENDER,
    #ifndef FEATURE_CGPS_XTRA_T
    { NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL
    }
    #else
    { tle_ProcSendBeginResponse,
      tle_ProcSendOpenResponse,
      tle_RcvdSendAck,
      tle_ProcSendCloseResponse,
      tle_ProcSendEndResponse,
      NULL, NULL, NULL, NULL, NULL
    }
    #endif /* ! FEATURE_CGPS_XTRA_T */
  },
  { e_GDT_SERVICE_GTP_WWAN_DL, "/hlos/gdt/RcvdTdpFile.txt",
    e_GDT_SERVICE_TYPE_RECEIVER,
#ifndef FEATURE_CGPS_XTRA_T
    { NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL
    }
#else
    { NULL, NULL, NULL, NULL, NULL,
      tle_ProcReceiveReadyResponse,
      tle_ProcReceiveOpenResponse,
      tle_ProcReceiveDataResponse,
      tle_ProcReceiveCloseResponse,
      tle_ProcReceiveEndResponse
    }
#endif /* ! FEATURE_CGPS_XTRA_T */
  },
  { e_GDT_SERVICE_GTP_CSM_UL, "/hlos/gdt/CsmFile.txt",
    e_GDT_SERVICE_TYPE_SENDER,
#if 1
    { NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL
    }
#else
    { tm_fusion_csm_proc_send_begin_response,
      tm_fusion_csm_proc_send_open_response,
      tm_fusion_csm_rcvd_send_ack,
      tm_fusion_csm_proc_send_close_response,
      tm_fusion_csm_proc_send_end_response,
      NULL, NULL, NULL, NULL, NULL
    }
#endif 
  }
};



/*==============================================================================

       Variables

==============================================================================*/

/* Tracks GDT session data for each GDT service */
static gdt_BufTableEntry gdt_BufTable[e_GDT_SERVICE_LAST + 1];

/* Tracks current GDT file offset as applicable 
     NOTE: This should only be modified in context of FS task */
static uint32 gdt_ReadCurrOffset[e_GDT_SERVICE_LAST + 1];

/* GDT session ID counter */
static uint32 gdt_SessCount;

/* Track if GDT has been initialized */
static boolean b_gdtInited = FALSE;



/*=============================================================================

       Functions

=============================================================================*/


/*===========================================================================
FUNCTION tle_GetEndianArchitecture

DESCRIPTION
  This function returns the endianness of the modem processor architecture

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  C_GDT_BIG_ENDIAN   : If MP has a big endian architecture
  C_GDT_LITTLE_ENDIAN: If MP has a little endian architecture

SIDE EFFECTS
  None.

===========================================================================*/
uint8 gdt_GetEndianArchitecture( void )
{
  /* determine at runtime endian-ness of OS */
  uint16 data_uint16 = 1;
  uint8  data_uint8[2];

  memset(data_uint8, 0x00, sizeof(data_uint8));
  memscpy( (void*)&data_uint8[0], sizeof(data_uint8), (void*)&data_uint16, sizeof(data_uint16) );

  if ( data_uint8[0] == 0 )
  {
    return C_GDT_BIG_ENDIAN;
  }

  return C_GDT_LITTLE_ENDIAN;
}


/* NOTE: These static helper functions below will need to be enhanced with mutex protection if an extension to send memory
    buffers between tasks is needed */

/*===========================================================================
FUNCTION gdt_Init

DESCRIPTION
  This function initializes any global data structures used only once
 
PARAMETERS 
 
DEPENDENCIES

RETURN VALUE 
  TRUE: SUCCESS
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static void gdt_Init()
{
  int i;
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_Init()",0,0,0);

  for (i = 0; i <= e_GDT_SERVICE_LAST; ++i)
  {
    gdt_BufTable[i].q_SessId = 0;
    gdt_BufTable[i].b_SessIdValid = 0;
    gdt_BufTable[i].p_MemHandle = NULL;
    gdt_BufTable[i].b_LockStatus = FALSE;
    gdt_BufTable[i].l_FileDescriptor = 0;
    gdt_BufTable[i].b_FileDescriptorValid = FALSE;
    gdt_ReadCurrOffset[i] = 0;
  }

  b_gdtInited = TRUE;
  gdt_SessCount = 1;
 
  return;
}

/*===========================================================================
FUNCTION gdt_Deinit

DESCRIPTION
  This function de-initializes GDT and frees up all registered buffers

  WARNING - This function should *NOT* be used on target because it can be very
            dangerous if the buffer is freed incorrectly.
 
PARAMETERS 
 
DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS

===========================================================================*/
void gdt_Deinit()
{
#ifdef FEATURE_CGPS_PLAYBACK
  int i;

  for (i = 0; i <= e_GDT_SERVICE_LAST; ++i)
  {
    if (NULL != gdt_BufTable[i].p_MemHandle)
    {
      os_MemFree((void **)&gdt_BufTable[i].p_MemHandle);
    }
  }

  b_gdtInited = FALSE;
#endif
}

/*===========================================================================
FUNCTION gdt_RegisterBuf

DESCRIPTION
  This function registers the allocated buffer in the buffer table to track the lock status of the buffer
 
PARAMETERS 
  cpz_Session - Points to the session information
  (input)

  cp_ByteBuffer - Points to the buffer for the given session
  (input)

  q_BufSize - Size of buffer being registered
  (input)
 
DEPENDENCIES

RETURN VALUE 
  TRUE: SUCCESS
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static boolean gdt_RegisterBuf(const gdt_SessionType * const cpz_Session, const gdt_MemHandle cp_ByteBuffer,
  uint32 q_BufSize)
{
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_RegisterBuf()",0,0,0);
    
  if ( ( NULL == cpz_Session ) || 
       ( NULL == cp_ByteBuffer ) || 
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) )
  {
    return FALSE;
  }

  gdt_BufTable[cpz_Session->e_ServiceId].p_MemHandle = cp_ByteBuffer;
  gdt_BufTable[cpz_Session->e_ServiceId].q_BufSize = q_BufSize;
  gdt_BufTable[cpz_Session->e_ServiceId].b_LockStatus = FALSE;
  MSG_1(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Buffer registered with size (%u)",q_BufSize);
  MSG_1(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Buffer registered = 0x%x",gdt_BufTable[cpz_Session->e_ServiceId].p_MemHandle);
  
  return TRUE;
}


/*===========================================================================
FUNCTION gdt_UnregisterBuf

DESCRIPTION
  This function unregisters the allocated buffer from the buffer table
 
PARAMETERS 
  cpz_Session - Points to the session information
  (input)

  cp_ByteBuffer - Points to the buffer for the given session if available, otherwise NULL
  (input)
 
DEPENDENCIES

RETURN VALUE 
  TRUE: SUCCESS
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static boolean gdt_UnregisterBuf(const gdt_SessionType * const cpz_Session, const gdt_MemHandle p_ByteBuffer)
{
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_UnregisterBuf()",0,0,0);
  
  if ( ( NULL == cpz_Session ) ||
       ( NULL == p_ByteBuffer ) ||
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) )
  {
    return FALSE;
  }

  if (p_ByteBuffer != gdt_BufTable[cpz_Session->e_ServiceId].p_MemHandle ||
       gdt_BufTable[cpz_Session->e_ServiceId].b_LockStatus)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Provided buffer does not match handle or buffer still locked.",0,0,0);
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"p_ByteBuffer = 0x%x, gdt_BufTable.p_MemHandle = 0x%x, lock = %u",
          p_ByteBuffer, gdt_BufTable[cpz_Session->e_ServiceId].p_MemHandle,
          gdt_BufTable[cpz_Session->e_ServiceId].b_LockStatus);
    return FALSE;
  }

  gdt_BufTable[cpz_Session->e_ServiceId].p_MemHandle = NULL;
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Buffer unregistered",0,0,0);
  
  return TRUE;
}


/*===========================================================================
FUNCTION gdt_GetBuf

DESCRIPTION
  This function finds the corresponding buffer for the given session information
 
PARAMETERS 
  cpz_Session - Points to the session information
  (input)

  p_ByteBuffer - Points to the buffer for the given session if available, otherwise NULL
  (output)

  pq_BufSize - Returns the registered buffer size if supplied pointer is not NULL
  (output)
 
DEPENDENCIES

RETURN VALUE 
  TRUE: SUCCESS
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static boolean gdt_GetBuf(const gdt_SessionType * const cpz_Session, gdt_MemHandle * p_ByteBuffer,
  uint32 * pq_BufSize)
{
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_GetBuf()",0,0,0);
  
  if ( ( NULL == cpz_Session ) ||
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) ||
       ( NULL == p_ByteBuffer ) )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return FALSE;
  }

  *p_ByteBuffer = gdt_BufTable[cpz_Session->e_ServiceId].p_MemHandle;
  
  if(NULL != pq_BufSize)
  {
    *pq_BufSize = gdt_BufTable[cpz_Session->e_ServiceId].q_BufSize;
  }
  
  return TRUE;
}

/*===========================================================================
FUNCTION gdt_GetLockStatus

DESCRIPTION
  This accesssor function gets the lock status on the specified buffer
 
PARAMETERS 
  pz_Session - Points to the session information
  (input)

  pb_LockStatus -Points to the current lock status of the buffer under the supplied service
 
DEPENDENCIES

RETURN VALUE 
  TRUE: SUCCESS
  FALSE: Otherwise
 
SIDE EFFECTS

===========================================================================*/
static boolean gdt_GetLockStatus(const gdt_SessionType * const cpz_Session, boolean * pb_LockStatus)
{
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_GetLockStatus()",0,0,0);
  
  if ( ( NULL == cpz_Session ) ||
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) ||
       ( NULL == pb_LockStatus ) )
  {
    return FALSE;
  }
  
  *pb_LockStatus = gdt_BufTable[cpz_Session->e_ServiceId].b_LockStatus;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Current lock status: %d",*pb_LockStatus,0,0);
  
  return TRUE;
}

/*===========================================================================
FUNCTION gdt_SetLockStatus

DESCRIPTION
  Changes the status of the write lock on the specified buffer
 
PARAMETERS 
 
  pz_Session - Points to the session information
  (input)
 
DEPENDENCIES

RETURN VALUE 
  TRUE: SUCCESS
  FALSE: Otherwise
 
SIDE EFFECTS

===========================================================================*/
static boolean gdt_SetLockStatus(const gdt_SessionType * const cpz_Session, boolean b_LockStatus)
{
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_SetLockStatus() - lock status = %u",b_LockStatus,0,0);
  
  if ( ( NULL == cpz_Session ) ||
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) )
  {
    return FALSE;
  }

  gdt_BufTable[cpz_Session->e_ServiceId].b_LockStatus = b_LockStatus;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Lock status for service (%d) changed to: %d",
    cpz_Session->e_ServiceId,gdt_BufTable[cpz_Session->e_ServiceId].b_LockStatus,0);
  
  return TRUE;
}


/*===========================================================================
FUNCTION gdt_GetFileDescriptorAndValidity

DESCRIPTION
  This function sets the corresponding file descriptor for the given session information
 
PARAMETERS 
  cpz_Session - Points to the session information
  (input)

  pl_FileDescriptor - File descriptor to update in global buffer table
  (output)

  pb_FileDescriptorValid - Validity of the file descriptor
 
DEPENDENCIES

RETURN VALUE 
  TRUE: SUCCESS
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static boolean gdt_GetFileDescriptorAndValidity(const gdt_SessionType * const cpz_Session, int * pl_FileDescriptor,
  boolean * pb_FileDescriptorValid)
{
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_GetFileDescriptorAndValidity()",0,0,0);
  
  if ( ( NULL == cpz_Session ) ||
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) ||
       ( NULL == pl_FileDescriptor ) ||
       ( NULL == pb_FileDescriptorValid ) )
  {
    return FALSE;
  }

  *pl_FileDescriptor = gdt_BufTable[cpz_Session->e_ServiceId].l_FileDescriptor;
  *pb_FileDescriptorValid = gdt_BufTable[cpz_Session->e_ServiceId].b_FileDescriptorValid;
  
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: File descriptor for service (%d) retrieved: %d, validity (%d)",
    cpz_Session->e_ServiceId,
    gdt_BufTable[cpz_Session->e_ServiceId].l_FileDescriptor,
    gdt_BufTable[cpz_Session->e_ServiceId].b_FileDescriptorValid);
  
  return TRUE;
}


/*===========================================================================
FUNCTION gdt_SetFileDescriptorAndValidity

DESCRIPTION
  This function sets the corresponding file descriptor for the given session information
 
PARAMETERS 
  cpz_Session - Points to the session information
  (input)

  l_FileDescriptor - File descriptor to update in global buffer table
  (input)

  b_FileDescriptorValid - Validity of file descriptor
 
DEPENDENCIES

RETURN VALUE 
  TRUE: SUCCESS
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static boolean gdt_SetFileDescriptorAndValidity(const gdt_SessionType * const cpz_Session,
  int l_FileDescriptor, boolean b_FileDescriptorValid)
{
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_SetFileDescriptorAndValidity()",0,0,0);
  
  if ( ( NULL == cpz_Session ) ||
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) )
  {
    return FALSE;
  }

  gdt_BufTable[cpz_Session->e_ServiceId].l_FileDescriptor = l_FileDescriptor;
  gdt_BufTable[cpz_Session->e_ServiceId].b_FileDescriptorValid = b_FileDescriptorValid;
  
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: File descriptor for service (%d) changed to: %d, validity (%d)",
    cpz_Session->e_ServiceId,
    gdt_BufTable[cpz_Session->e_ServiceId].l_FileDescriptor,
    gdt_BufTable[cpz_Session->e_ServiceId].b_FileDescriptorValid);
  
  return TRUE;
}


/*===========================================================================
FUNCTION gdt_GenerateNextSessionId

DESCRIPTION
  This function generates next session ID when a valid new session is requested
 
PARAMETERS 
 
DEPENDENCIES

RETURN VALUE 
  TRUE: Session ID matches valid session ID in table
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static uint32 gdt_GenerateNextSessionId()
{
  return gdt_SessCount++;
}


/*===========================================================================
FUNCTION gdt_CheckSessionId

DESCRIPTION
  This function checks the session ID for the given session information
 
PARAMETERS 
  cpz_Session - Points to the session information
  (input)
 
DEPENDENCIES

RETURN VALUE 
  TRUE: Session ID matches valid session ID in table
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static boolean gdt_CheckSessionId(const gdt_SessionType * const cpz_Session)
{
  if ( ( NULL == cpz_Session ) ||
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) )
  {
    return FALSE;
  }

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_CheckSessionId() - session (%d) vs table (%d)",
    cpz_Session->q_SessId, gdt_BufTable[cpz_Session->e_ServiceId].q_SessId,0);

  if (gdt_BufTable[cpz_Session->e_ServiceId].b_SessIdValid &&
       gdt_BufTable[cpz_Session->e_ServiceId].q_SessId == cpz_Session->q_SessId)
  {
    return TRUE;  
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION gdt_GetSessionValidity

DESCRIPTION
  This function gets the session validity for the given session information
 
PARAMETERS 
  cpz_Session - Points to the session information
  (input)
 
DEPENDENCIES

RETURN VALUE 
  TRUE: Session valid
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static boolean gdt_GetSessionValidity(const gdt_SessionType * const cpz_Session)
{
  if ( ( NULL == cpz_Session ) ||
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) )
  {
    return FALSE;
  }

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_GetSessionValidity()",0,0,0);

  if (gdt_BufTable[cpz_Session->e_ServiceId].q_SessId != cpz_Session->q_SessId)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Mismatching session ID, Existing sessID:%lu, injected sessId:%lu", 
            gdt_BufTable[cpz_Session->e_ServiceId].q_SessId, 
            cpz_Session->q_SessId, 0);

    return FALSE;
  }
#ifndef FEATURE_CGPS_PLAYBACK
  if (gdt_BufTable[cpz_Session->e_ServiceId].b_SessIdValid)
  {
    return TRUE;  
  }
#endif
  return FALSE;
}



/*===========================================================================
FUNCTION gdt_SetSessionIdAndValidity

DESCRIPTION
  This function sets the given session ID for the given session information
 
PARAMETERS 
  cpz_Session - Points to the session information
  (input)

  b_Valid - Session ID validity to update in global buffer table
  (intput)
 
DEPENDENCIES

RETURN VALUE 
  TRUE: SUCCESS
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
static boolean gdt_SetSessionIdAndValidity(const gdt_SessionType * const cpz_Session, boolean b_Valid)
{
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_SetSessionIdAndValidity() Valid:%d",
          b_Valid,0,0);
  
  if ( ( NULL == cpz_Session ) ||
       ( cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ) ||
       ( cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ) )
  {
    return FALSE;
  }

  gdt_BufTable[cpz_Session->e_ServiceId].b_SessIdValid = b_Valid;
  gdt_BufTable[cpz_Session->e_ServiceId].q_SessId = cpz_Session->q_SessId;
  
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Session ID (%d) for service (%d) is now %d (valid=1)",
    gdt_BufTable[cpz_Session->e_ServiceId].q_SessId,
    cpz_Session->e_ServiceId, b_Valid);
  
  return TRUE;
}


/*===========================================================================

FUNCTION
  gdt_SendDataFS

DESCRIPTION
  This function sends a command to the GPS FS task to send data to AP.
  The current implementation transfers the send data in a blob to the FS task via IPC.
 
PARAMETERS 
   pz_SendBlob - Blob consisting of buffer and length to be sent

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
static void gdt_SendDataFS( const gdt_SendBlobType * const pz_SendBlob)
{
  os_IpcMsgType * p_Msg;

  if (NULL == pz_SendBlob)
  {
    return;
  }

  p_Msg = (os_IpcMsgType *) os_IpcCreate(sizeof(*pz_SendBlob), 
                                         IPC_ALLOCATION_DYNAMIC,
                                         (uint32) THREAD_ID_GPS_FS);

  if(NULL != p_Msg)
  {
    p_Msg->q_MsgId = (uint32) C_GPS_FS_MSG_GDT_EXTENDED_SEND;
    p_Msg->q_DestThreadId = (uint32) THREAD_ID_GPS_FS;
    p_Msg->q_Size = sizeof(*pz_SendBlob);

    memscpy( (gdt_SendBlobType *)p_Msg->p_Data, sizeof(*pz_SendBlob), pz_SendBlob, sizeof(*pz_SendBlob) );

    if(!os_IpcSend(p_Msg, (uint32) THREAD_ID_GPS_FS))
    {
      (void)os_IpcDelete(p_Msg);
    }
  }
}


/*===========================================================================

FUNCTION
  gdt_ReceiveDataFS

DESCRIPTION
  This function sends a command to the GPS FS task to receive data from AP.
  The current implementation transfers the blob buffer to contain the read data
  to the FS task via IPC.
 
PARAMETERS 
   pz_ReceiveBlob - Blob consisting of buffer, format and length to be sent

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
static void gdt_ReceiveDataFS( const gdt_ReceiveBlobType * const pz_ReceiveBlob)
{
  os_IpcMsgType * p_Msg;

  if (NULL == pz_ReceiveBlob)
  {
    return;
  }

  p_Msg = (os_IpcMsgType *) os_IpcCreate(sizeof(*pz_ReceiveBlob), 
                                         IPC_ALLOCATION_DYNAMIC,
                                         (uint32) THREAD_ID_GPS_FS);

  if(NULL != p_Msg)
  {
    p_Msg->q_MsgId = (uint32) C_GPS_FS_MSG_GDT_EXTENDED_RECEIVE;
    p_Msg->q_DestThreadId = (uint32) THREAD_ID_GPS_FS;
    p_Msg->q_Size = sizeof(*pz_ReceiveBlob);

    memscpy( (gdt_ReceiveBlobType *)p_Msg->p_Data, sizeof(*pz_ReceiveBlob), pz_ReceiveBlob, sizeof(*pz_ReceiveBlob) );

    if(!os_IpcSend(p_Msg, (uint32) THREAD_ID_GPS_FS))
    {
      (void)os_IpcDelete(p_Msg);
    }
  }
}



/*===========================================================================
FUNCTION gdt_SendBegin

DESCRIPTION
  This function starts a data transposrt session between a module in MP and
  the corresponding module in AP.
 
PARAMETERS 
 
  pz_Session - Points to the session information
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the request, and sent the access request
                 to AP to get the permission
 
  e_GDT_FAIL: Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendBegin(gdt_SessionType * const pz_Session)
{
  uint32 q_Length = 0;
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_SendBegin()",0,0,0);
  
  if(NULL == pz_Session || pz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ||
    pz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
    gdt_Users[pz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_SENDER) 
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }

  q_Length = ((uint32) strlen(gdt_Users[pz_Session->e_ServiceId].FilePath)) + 1;
  if (!b_gdtInited)
  {
    gdt_Init();
  }

  if (gdt_GetSessionValidity(pz_Session))
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Cannot call SendBegin while another session is ongoing",0,0,0);
    return e_GDT_FAIL;
  }

  /* Update session id */
  pz_Session->q_SessId = gdt_GenerateNextSessionId();
  if (!gdt_SetSessionIdAndValidity(pz_Session, TRUE))
  {
    return e_GDT_FAIL;
  }

  gdt_ReportEventSessionStartReq(pz_Session);

  /* Send Begin message to AP */ 
  #ifdef FEATURE_QMI_PDS_EXT
  #error code not present
#else
  return loc_gdt_SendBeginRequest(pz_Session->e_ServiceId, pz_Session->q_SessId, 
                                  q_Length, gdt_Users[pz_Session->e_ServiceId].FilePath);
  #endif /* FEATURE_QMI_PDS_EXT */
}

/*===========================================================================
FUNCTION gdt_SendData

DESCRIPTION
  This function receives a chunk of data that needs to be transported to AP.
  Different chuncks of data will be accumulated and will be sent to AP at once,
  when the sender calls gdp_SessionEnd().
 
PARAMETERS 
 
  cpz_GdtIface - Points to the GDT interface
  (input)

DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has accepted the data to be uploaded.
 
   Note: This does not confirm that data is transferred.
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendData(const gdt_InterfaceType *cpz_GdtIface)
{
  boolean b_LockStatus = FALSE;
  boolean b_FileDescriptorValid = FALSE;
  int l_FileDescriptor = 0;
  gdt_MemHandle p_TempHandle = NULL;
  uint32 q_BufSize = 0;
  gdt_SendBlobType z_SendBlob;
  const gdt_SessionType *cpz_Session; 
  uint32 q_DataSize;
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_SendData()",0,0,0);
  if (NULL == cpz_GdtIface)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }
    
  cpz_Session = &cpz_GdtIface->z_SessInfo; 
  q_DataSize = cpz_GdtIface->q_MsgLength;

  if (NULL == cpz_Session || NULL == cpz_GdtIface->z_GdtBuffer || q_DataSize > C_GNSS_GDT_MAX_BUF_SIZE ||
      cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID || cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
      gdt_Users[cpz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_SENDER)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }

  if(!gdt_CheckSessionId(cpz_Session) ||
    !gdt_GetFileDescriptorAndValidity(cpz_Session, &l_FileDescriptor, &b_FileDescriptorValid) ||
    !gdt_GetBuf(cpz_Session, &p_TempHandle, &q_BufSize))
  {
    return e_GDT_FAIL;
  }

  if(!b_FileDescriptorValid)
  {
    return e_GDT_FAIL;
  }

  if(p_TempHandle != cpz_GdtIface->z_GdtBuffer)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"p_TempHandle = 0x%x , cpz_GdtIface->z_GdtBuffer = 0x%x",
          p_TempHandle,cpz_GdtIface->z_GdtBuffer,0);
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Buffer mismatch. Incorrect buffer supplied.",0,0,0);
    return e_GDT_FAIL;
  }

  if (q_DataSize > q_BufSize)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Data size is greater than allocated buffer.",0,0,0);
    return e_GDT_FAIL;
  }

#ifdef  FEATURE_CGPS_PLAYBACK
  gdt_SetLockStatus(cpz_Session, FALSE);
#endif

  /* Check lock status */
  if (!gdt_GetLockStatus(cpz_Session, &b_LockStatus))
  {
    return e_GDT_FAIL;
  }

  if (b_LockStatus)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Send buffer already locked. Cannot send until buffer unlocked",0,0,0);
    return e_GDT_FAIL;
  }

  /* Update lock status in buffer table */
  if (!gdt_SetLockStatus(cpz_Session, TRUE))
  {
    return e_GDT_FAIL;
  }

  gdt_ReportEventSendReq(cpz_Session, q_DataSize);

  /* Call FS task */
  z_SendBlob.z_SessionInfo = *cpz_Session;
  z_SendBlob.q_DataSize = q_DataSize;
  z_SendBlob.p_ByteBuffer = cpz_GdtIface->z_GdtBuffer;
  gdt_SendDataFS(&z_SendBlob);
  
  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_ProtectedExtendedSend

DESCRIPTION
  This function receives a chunk of data that needs to be transported to AP.
  Different chunks of data will be accumulated and will be sent to AP at once,
  when the sender calls gdp_SendEnd().  This function is expected to only be
  called in the context of FS task and NOT by service users of GDT.  GDT
  services should call gdt_SendData which will result in this function being
  called.
 
PARAMETERS 
 
  pz_SendBlob - Blob containing buffer, length and session info for sending
  (input)

DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has accepted the data to be uploaded.
 
   Note: This does not confirm that data is transferred.
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ProtectedExtendedSend(const gdt_SendBlobType * const pz_SendBlob)
{
  gdt_SendAckArgumentType z_AckArg;
  gdt_SendAckResponseType z_AckResp;
  boolean b_FileDescriptorValidity = FALSE;
  int l_FileDescriptor = 0;
  int result = 0;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_ProtectedExtendedSend()",0,0,0);

  if (NULL == pz_SendBlob || NULL == pz_SendBlob->p_ByteBuffer || 
    pz_SendBlob->z_SessionInfo.e_ServiceId <= e_GDT_SERVICE_INVALID ||
    pz_SendBlob->z_SessionInfo.e_ServiceId > e_GDT_SERVICE_LAST)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }

  if(!gdt_CheckSessionId(&pz_SendBlob->z_SessionInfo) ||
     !gdt_GetFileDescriptorAndValidity(&pz_SendBlob->z_SessionInfo, &l_FileDescriptor, &b_FileDescriptorValidity))
  {
    return e_GDT_FAIL;
  }

  if(!b_FileDescriptorValidity)
  {
    return e_GDT_FAIL;
  }

#ifndef FEATURE_CGPS_PLAYBACK
  result = rfs_write (l_FileDescriptor,
                      (void *)pz_SendBlob->p_ByteBuffer,
                      pz_SendBlob->q_DataSize);
#endif

  if (!gdt_SetLockStatus(&pz_SendBlob->z_SessionInfo, FALSE))
  {
    return e_GDT_FAIL;
  }

  z_AckResp.z_SessionInfo = pz_SendBlob->z_SessionInfo;

  if (result < 0 || result != pz_SendBlob->q_DataSize)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs write with result = %d",result,0,0);
    z_AckResp.e_EndStatus = e_GDT_ACK_FAILED;
    z_AckArg.pz_AckResponse = &z_AckResp;
    gdt_RcvdSendAckResponse(z_AckArg);
    return e_GDT_FAIL;
  }

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"rfs write successful",0,0,0);
  z_AckResp.e_EndStatus = e_GDT_ACK_SUCCESS;
  z_AckArg.pz_AckResponse = &z_AckResp;
  gdt_RcvdSendAckResponse(z_AckArg);

  return e_GDT_SUCCESS;
}



/*===========================================================================
FUNCTION gdt_SendEnd

DESCRIPTION
  This function finalizes the data transmission session between MP and AP.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  e_Status - The final status of this session
  (input)

  b_ForceCleanup - If TRUE, force cleanup of remaining session information
  (input)


RETURN VALUE 
  e_GDT_SUCCESS: GDT has received the final upload session status from AP 
 
  e_GDT_FAIL  - There is an internal error in GDT
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendEnd(const gdt_SessionType * const cpz_Session, 
                                 e_GDTResponseType e_Status, boolean b_ForceCleanup)
{
#ifndef FEATURE_CGPS_PLAYBACK
  int i;
#endif
  int l_fd = 0;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_SendEnd()",0,0,0);
  
  if (NULL == cpz_Session || cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ||
      cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
      gdt_Users[cpz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_SENDER)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }

  if(!gdt_CheckSessionId(cpz_Session))
  {
    return e_GDT_FAIL;
  }
  
  if(b_ForceCleanup)
  {
    if (gdt_GetSessionValidity(cpz_Session))
    {
      /* Update lock status in buffer table */
      if (!gdt_SetLockStatus(cpz_Session, FALSE))
      {
        return e_GDT_FAIL;
      }

      /* Update session ID validity*/
      if (!gdt_SetSessionIdAndValidity(cpz_Session, FALSE))
      {
        return e_GDT_FAIL;
      }
    }
  }

  gdt_ReportEventSessionEndReq(cpz_Session, (uint16)e_Status);

  /* Notify AP of Send End */
  #ifdef FEATURE_QMI_PDS_EXT
  #error code not present
#else
  return loc_gdt_SendEndRequest(cpz_Session->e_ServiceId, cpz_Session->q_SessId, 
                                e_Status);
  #endif /* FEATURE_QMI_PDS_EXT */
}

/*===========================================================================
FUNCTION gdt_SendOpen

DESCRIPTION
  This function initializes data transmission session from a client to GDT.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly initialized to start data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendOpen(const gdt_SessionType * const cpz_Session)
{
  int l_fd = 0;
  boolean b_FileDescriptorValid = FALSE;
  gdt_OpenResponseType z_Response;
  gdt_OpenArgumentType z_Arg;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_SendOpen()",0,0,0);

  if(NULL == cpz_Session || cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ||
     cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
     gdt_Users[cpz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_SENDER) 
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }
  
  if( NULL == gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.OpenCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - Open",0,0,0);
    return e_GDT_FAIL;
  }

  z_Response.z_SessionInfo = *cpz_Session;
  z_Response.e_OpenStatus = e_GDT_FAIL;
  z_Arg.pz_OpenResponse = &z_Response;

#ifdef FEATURE_CGPS_PLAYBACK
  if(cpz_Session->e_ServiceId <= e_GDT_SERVICE_LAST)
  {
    gdt_SetSessionIdAndValidity(cpz_Session, TRUE);
  }
#endif

  if(!gdt_CheckSessionId(cpz_Session) ||
     !gdt_GetFileDescriptorAndValidity(cpz_Session, &l_fd, &b_FileDescriptorValid))
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.OpenCbApi(z_Arg);
    return e_GDT_FAIL;
  }

  if(b_FileDescriptorValid)
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.OpenCbApi(z_Arg);
    return e_GDT_FAIL;
  }

#ifndef FEATURE_CGPS_PLAYBACK
  l_fd = rfs_open (gdt_Users[cpz_Session->e_ServiceId].FilePath,
                    RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_CREAT | RFS_O_APPEND, 0);
  
  if (l_fd < 0)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs open with fd = %d", l_fd,0,0);
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.OpenCbApi(z_Arg);
    return e_GDT_FAIL;
  }
#endif

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: file descriptor = %d",l_fd,0,0);
  
  /* Save file descriptor */
  if (!gdt_SetFileDescriptorAndValidity(cpz_Session, l_fd, TRUE))
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.OpenCbApi(z_Arg);
    return e_GDT_FAIL;
  }

  gdt_ReportEventSessionOpenReq(cpz_Session);
  
  z_Arg.pz_OpenResponse->e_OpenStatus = e_GDT_SUCCESS;
  gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.OpenCbApi(z_Arg);

  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_SendClose

DESCRIPTION
  This function finalizes data transmission session from a client to GDT.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly finalized data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendClose(const gdt_SessionType * const cpz_Session)
{
  int l_FileDescriptor;
  gdt_CloseResponseType z_Response;
  gdt_CloseArgumentType z_Arg;
  boolean b_FileDescriptorValid;

#ifndef FEATURE_CGPS_PLAYBACK
  int result = 0;
#endif

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_SendClose()",0,0,0);
  
  if(NULL == cpz_Session || cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ||
     cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
     gdt_Users[cpz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_SENDER)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }
  
  if( NULL == gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.CloseCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - Close",0,0,0);
    return e_GDT_FAIL;
  }

  z_Response.z_SessionInfo = *cpz_Session;
  z_Response.e_CloseStatus = e_GDT_FAIL;
  z_Arg.pz_CloseResponse = &z_Response;

  if(!gdt_CheckSessionId(cpz_Session) ||
    !gdt_GetFileDescriptorAndValidity(cpz_Session, &l_FileDescriptor, &b_FileDescriptorValid))
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.CloseCbApi(z_Arg);
    return e_GDT_FAIL;
  }

  if(!b_FileDescriptorValid)
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.CloseCbApi(z_Arg);
    return e_GDT_FAIL;
  }

#ifndef FEATURE_CGPS_PLAYBACK
  result = rfs_close (l_FileDescriptor);
  if (result != 0)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs_close with result = %d", result,0,0);
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.CloseCbApi(z_Arg);
    return e_GDT_FAIL;
  }
#endif

  if (!gdt_SetFileDescriptorAndValidity(cpz_Session, 0, FALSE))
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.CloseCbApi(z_Arg);
    return e_GDT_FAIL;
  }

  gdt_ReportEventSessionCloseReq(cpz_Session);
  
  z_Arg.pz_CloseResponse->e_CloseStatus = e_GDT_SUCCESS;
  gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.CloseCbApi(z_Arg);
  
  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_ReceiveBegin

DESCRIPTION
  This function initializes data transmission session from a client to GDT.  The MP client calls this
  to initiate the receiving of data upon receiving notification that the data is available.

DEPENDENCIES 
 
PARAMETERS 
  pz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly initialized to start data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveBegin(gdt_SessionType * const pz_Session)
{
  int l_fd = 0;
  int result = 0;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_ReceiveBegin()",0,0,0);
  
  if(NULL == pz_Session || pz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ||
    pz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
    gdt_Users[pz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_RECEIVER) 
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }
  
  if (!b_gdtInited)
  {
    gdt_Init();
  }

  if (gdt_GetSessionValidity(pz_Session))
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Cannot call ReceiveBegin while another session is ongoing",0,0,0);
    return e_GDT_FAIL;
  }

  /* Update session id */
  pz_Session->q_SessId = gdt_GenerateNextSessionId();
  if (!gdt_SetSessionIdAndValidity(pz_Session, TRUE))
  {
    return e_GDT_FAIL;
  }

  if(pz_Session->e_ServiceId == e_GDT_SERVICE_GTP_WWAN_DL)
  {
    strlcpy(pz_Session->c_DataPath, gdt_Users[pz_Session->e_ServiceId].FilePath,
            sizeof(pz_Session->c_DataPath));
  }

#ifndef FEATURE_CGPS_PLAYBACK
  l_fd = rfs_open (gdt_Users[pz_Session->e_ServiceId].FilePath, RFS_O_RDWR | RFS_O_CREAT | RFS_O_TRUNC, 0);
  
  if (l_fd < 0)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs open with fd = %d", l_fd,0,0);
    return e_GDT_FAIL;
  }

  result = rfs_close(l_fd);
  if (0 != result)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs_close with result = %d", result,0,0);
    return e_GDT_FAIL;
  }
#endif

  gdt_ReportEventSessionStartReq(pz_Session);

  return e_GDT_SUCCESS;
}

/*===========================================================================
FUNCTION gdt_ReceiveOpen

DESCRIPTION
  This function initializes data transmission session from a client to GDT.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly initialized to start data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveOpen(const gdt_SessionType * const cpz_Session)
{
  int l_fd = 0;
  boolean b_FileDescriptorValid = FALSE;
  gdt_OpenResponseType z_Response;
  gdt_OpenArgumentType z_Arg;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_ReceiveOpen()",0,0,0);

  if(NULL == cpz_Session || cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ||
    cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
    gdt_Users[cpz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_RECEIVER) 
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }
  
  if( NULL == gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveOpenCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - ReceiveOpen",0,0,0);
    return e_GDT_FAIL;
  }

  z_Response.z_SessionInfo = *cpz_Session;
  z_Response.e_OpenStatus = e_GDT_FAIL;
  z_Arg.pz_OpenResponse = &z_Response;

#ifdef FEATURE_CGPS_PLAYBACK
  if(cpz_Session->e_ServiceId <= e_GDT_SERVICE_LAST)
  {
    gdt_SetSessionIdAndValidity(cpz_Session, TRUE);
  }
#endif

  if(!gdt_CheckSessionId(cpz_Session) ||
    !gdt_GetFileDescriptorAndValidity(cpz_Session, &l_fd, &b_FileDescriptorValid))
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveOpenCbApi(z_Arg);
    return e_GDT_FAIL;
  }

  if(b_FileDescriptorValid)
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveOpenCbApi(z_Arg);
    return e_GDT_FAIL;
  }

#ifndef FEATURE_CGPS_PLAYBACK
  l_fd = rfs_open (gdt_Users[cpz_Session->e_ServiceId].FilePath, RFS_O_RDONLY, 0);
  
  if (l_fd < 0)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs open with fd = %d", l_fd,0,0);
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveOpenCbApi(z_Arg);
    return e_GDT_FAIL;
  }
#endif

  gdt_ReadCurrOffset[cpz_Session->e_ServiceId] = 0;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: file descriptor = %d",l_fd,0,0);

  /* Save file descriptor */
  if (!gdt_SetFileDescriptorAndValidity(cpz_Session, l_fd, TRUE))
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveOpenCbApi(z_Arg);
    return e_GDT_FAIL;
  }

  gdt_ReportEventSessionOpenReq(cpz_Session);
  
  z_Arg.pz_OpenResponse->e_OpenStatus = e_GDT_SUCCESS;
  gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveOpenCbApi(z_Arg);

  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_ReceiveData

DESCRIPTION
  This function receives a chunk of data that needs to be supplied to MP.
  The data will be provided in the format the client prefers it (eg. TLV).
 
PARAMETERS 
 
  cpz_GdtIface - Points to the GDT interface
  (input)

  e_Format - Expected GDT file parsing format
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly initialized to start data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveData(const gdt_InterfaceType *cpz_GdtIface, e_GDTFormatType e_Format)
{
  gdt_MemHandle p_TempHandle;
  gdt_ReceiveBlobType z_ReceiveBlob;
  const gdt_SessionType *cpz_Session; 
  uint32 q_RcvBufSize;
  uint32 q_BufSize;
  boolean b_LockStatus = FALSE;
  boolean b_FileDescriptorValid = FALSE;
  int l_FileDescriptor = 0;
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_ReceiveData()",0,0,0);
  
  if (NULL == cpz_GdtIface)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }
    
  cpz_Session = &cpz_GdtIface->z_SessInfo; 
  q_RcvBufSize = cpz_GdtIface->q_GdtBufferSize;

  if (NULL == cpz_Session || NULL == cpz_GdtIface->z_GdtBuffer || q_RcvBufSize > C_GNSS_GDT_MAX_BUF_SIZE ||
      cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID || cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
      gdt_Users[cpz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_RECEIVER)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }
  
  if(!gdt_CheckSessionId(cpz_Session) ||
    !gdt_GetFileDescriptorAndValidity(cpz_Session, &l_FileDescriptor, &b_FileDescriptorValid) ||
    !gdt_GetBuf(cpz_Session, &p_TempHandle, &q_BufSize))
  {
    return e_GDT_FAIL;
  }

  if(!b_FileDescriptorValid)
  {
    return e_GDT_FAIL;
  }

  if(p_TempHandle != cpz_GdtIface->z_GdtBuffer)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Buffer mismatch. Incorrect buffer supplied.",0,0,0);
    return e_GDT_FAIL;
  }

  if (q_RcvBufSize != q_BufSize)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Receive buffer size is not the same as what was allocated.",0,0,0);
    return e_GDT_FAIL;
  }

#ifdef  FEATURE_CGPS_PLAYBACK
  gdt_SetLockStatus(cpz_Session, FALSE);
#endif

  /* Check lock status */
  if (!gdt_GetLockStatus(cpz_Session, &b_LockStatus))
  {
    return e_GDT_FAIL;
  }

  if (b_LockStatus)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Rcv buffer already locked. ",0,0,0);
    return e_GDT_FAIL;
  }

  /* Update lock status in buffer table */
  if (!gdt_SetLockStatus(cpz_Session, TRUE))
  {
    return e_GDT_FAIL;
  }

  //gdt_ReportEventReceiveReq(cpz_Session, q_DataSize); // TODO, new event

  /* Call FS task */
  z_ReceiveBlob.z_SessionInfo = *cpz_Session;
  z_ReceiveBlob.q_BufSize = q_RcvBufSize;
  z_ReceiveBlob.p_ByteBuffer = cpz_GdtIface->z_GdtBuffer;
  z_ReceiveBlob.e_Format = e_Format;
  gdt_ReceiveDataFS(&z_ReceiveBlob);
  
  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_ProtectedExtendedReceive

DESCRIPTION
  This function receives a chunk of data that needs to be provided to client.
  Data will be read per instructions by GDT client.  This function is expected 
  to only be called in the context of FS task and NOT by service users of GDT.
  GDT services should call gdt_ReceiveData which will result in this function being
  called.
 
PARAMETERS 
 
  pz_ReceiveBlob - Blob containing buffer, length and session info for receiving
  (input)

DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has accepted the data to be uploaded.
 
   Note: This does not confirm that data is transferred.
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ProtectedExtendedReceive(const gdt_ReceiveBlobType * const pz_ReceiveBlob)
{
  gdt_ReceiveDataArgumentType z_ReceiveArg  = {0};
  gdt_ReceiveDataResponseType z_ReceiveResp = {0};
  boolean b_FileDescriptorValidity = FALSE;
  int l_FileDescriptor = 0;
  int i = 0;
  int result = -1;
  boolean b_ServiceFound = FALSE;
  uint32 l_vLength = 0;
  uint32 l_tag = 0;
  uint32 l_tlvLength = 0;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_ProtectedExtendedReceive()",0,0,0);

  if (NULL == pz_ReceiveBlob || NULL == pz_ReceiveBlob->p_ByteBuffer || 
    pz_ReceiveBlob->z_SessionInfo.e_ServiceId <= e_GDT_SERVICE_INVALID ||
    pz_ReceiveBlob->z_SessionInfo.e_ServiceId > e_GDT_SERVICE_LAST ||
    gdt_Users[pz_ReceiveBlob->z_SessionInfo.e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_RECEIVER)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }

  if(!gdt_CheckSessionId(&pz_ReceiveBlob->z_SessionInfo) ||
     !gdt_GetFileDescriptorAndValidity(&pz_ReceiveBlob->z_SessionInfo,
     &l_FileDescriptor, &b_FileDescriptorValidity))
  {
    return e_GDT_FAIL;
  }

  if(!b_FileDescriptorValidity)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid file descriptor",0,0,0);
    return e_GDT_FAIL;
  }

#ifndef FEATURE_CGPS_PLAYBACK
  switch(pz_ReceiveBlob->e_Format)
  {
    case e_GDT_FORMAT_TLV:
      {
        /* Set position indicator to last read location to get tag of current TLV */
        result = rfs_seek (l_FileDescriptor,
                            gdt_ReadCurrOffset[pz_ReceiveBlob->z_SessionInfo.e_ServiceId],
                            RFS_SEEK_SET);

        if (result < 0)
        {
          MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs seek with result = %d",result,0,0);
          return e_GDT_FAIL;
        }
                
        result = rfs_read (l_FileDescriptor,
                            &l_tag,
                            C_GNSS_GDT_SIZE_OF_TAG);

        if (result < 0)
        {
          MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs read with result = %d",result,0,0);
          return e_GDT_FAIL;
        }

        /* Set position indicator to last read location to get length of current TLV */
        result = rfs_seek (l_FileDescriptor,
                            gdt_ReadCurrOffset[pz_ReceiveBlob->z_SessionInfo.e_ServiceId] + C_GNSS_GDT_SIZE_OF_TAG,
                            RFS_SEEK_SET);

        if (result < 0)
        {
          MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs seek with result = %d",result,0,0);
          return e_GDT_FAIL;
        }
                
        result = rfs_read (l_FileDescriptor,
                            &l_vLength,
                            C_GNSS_GDT_SIZE_OF_LENGTH);

        if (result < 0)
        {
          MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs read with result = %d",result,0,0);
          return e_GDT_FAIL;
        }

        /* GDT Communication between AP-Mp uses big endian (network byte order) */
        l_tag = gdt_bswap_32(l_tag);

        /* Check bit 1 of byte 1 (ie. bit 25) of tag for endianness per SDS */
        if ((uint8)((l_tag >> 25) & 1) != gdt_GetEndianArchitecture())
        {
          /* MP expects the data to be big endian */
          l_vLength = gdt_bswap_32(l_vLength);
        }

        MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Current TLV has value length:%d", l_vLength, 0,0);

        /* Now that we know the value is of length l_length, we can reset position indicator to 
              start of entire TLV and copy the entire TLV to the supplied buffer */
        result = rfs_seek (l_FileDescriptor,
                            gdt_ReadCurrOffset[pz_ReceiveBlob->z_SessionInfo.e_ServiceId],
                            RFS_SEEK_SET);

        if (result < 0)
        {
          MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs seek with result = %d",result,0,0);
          return e_GDT_FAIL;
        }
        
        l_tlvLength = l_vLength + C_GNSS_GDT_SIZE_OF_TAG + C_GNSS_GDT_SIZE_OF_LENGTH;
        if (l_tlvLength > pz_ReceiveBlob->q_BufSize)
        {
          MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: TLV length (%u) is longer than length of buffer (%u)",
            l_tlvLength, pz_ReceiveBlob->q_BufSize,0);
          return e_GDT_FAIL;
        }
        
        result = rfs_read (l_FileDescriptor,
                            (void *)pz_ReceiveBlob->p_ByteBuffer,
                            l_tlvLength);  
      }
      break;
    default:
      MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Unsupported file format",0,0,0);
      return e_GDT_FAIL;
  }
#endif

  if (!gdt_SetLockStatus(&pz_ReceiveBlob->z_SessionInfo, FALSE))
  {
    return e_GDT_FAIL;
  }

  z_ReceiveResp.e_ReceiveDataStatus = e_GDT_FAIL;
  z_ReceiveResp.z_ReadBlob.z_SessionInfo = pz_ReceiveBlob->z_SessionInfo;

  for (i = 0; i <= e_GDT_SERVICE_LAST; ++i)
  {
    if (gdt_Users[i].e_Service == pz_ReceiveBlob->z_SessionInfo.e_ServiceId)
    {
      b_ServiceFound = TRUE;
      break;
    }
  }

  if( NULL == gdt_Users[i].z_CbInfo.ReceiveDataCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - ReceiveData",0,0,0);
    return e_GDT_FAIL;
  }
  
  if (result < 0 || result != l_tlvLength)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs read with result = %d",result,0,0);
    z_ReceiveArg.pz_ReceiveDataResponse = &z_ReceiveResp;
    if (b_ServiceFound)
    {
      /* ToDo: gdt_LogDataBlob */
      gdt_Users[i].z_CbInfo.ReceiveDataCbApi(z_ReceiveArg);
    }
    else
    {
      return e_GDT_FAIL;
    }
    return e_GDT_FAIL;
  }

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"rfs read successful",0,0,0);
  /* Update offset for next read */
  gdt_ReadCurrOffset[pz_ReceiveBlob->z_SessionInfo.e_ServiceId] += l_tlvLength;
    
  z_ReceiveResp.e_ReceiveDataStatus = e_GDT_SUCCESS;
  z_ReceiveResp.z_ReadBlob.p_ByteBuffer = pz_ReceiveBlob->p_ByteBuffer;
  z_ReceiveResp.z_ReadBlob.q_BufSize = pz_ReceiveBlob->q_BufSize;
  z_ReceiveResp.z_ReadBlob.q_DataSize = l_tlvLength;
  z_ReceiveResp.z_ReadBlob.e_Format = pz_ReceiveBlob->e_Format;
  z_ReceiveArg.pz_ReceiveDataResponse = &z_ReceiveResp;
  if (b_ServiceFound)
  {
    /* ToDo: gdt_LogDataBlob */
    gdt_Users[i].z_CbInfo.ReceiveDataCbApi(z_ReceiveArg);
  }
  else
  {
    return e_GDT_FAIL;
  }

  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_ReceiveClose

DESCRIPTION
  This function closes the current data transmission session from a client to GDT.  The MP client calls this to
  terminate interactions with the downloaded file.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  e_Status - Status of receive session
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly finalized current data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveClose(const gdt_SessionType * const cpz_Session,
  e_GDTTransferStatusType e_Status)
{
  int l_FileDescriptor;
  gdt_CloseResponseType z_Response;
  gdt_CloseArgumentType z_Arg;
  boolean b_FileDescriptorValid;

#ifndef FEATURE_CGPS_PLAYBACK
  int result = 0;
#endif

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_ReceiveClose(), status:%d",e_Status,0,0);
  
  if(NULL == cpz_Session || cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ||
     cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
     gdt_Users[cpz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_RECEIVER)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }
  
  if( NULL == gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveCloseCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - ReceiveClose",0,0,0);
    return e_GDT_FAIL;
  }

  z_Response.z_SessionInfo = *cpz_Session;
  z_Response.e_CloseStatus = e_GDT_FAIL;
  z_Arg.pz_CloseResponse = &z_Response;

  if(!gdt_CheckSessionId(cpz_Session) ||
    !gdt_GetFileDescriptorAndValidity(cpz_Session, &l_FileDescriptor, &b_FileDescriptorValid))
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"File descriptor invalid", 0,0,0);
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveCloseCbApi(z_Arg);
    return e_GDT_FAIL;
  }

  if(!b_FileDescriptorValid)
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveCloseCbApi(z_Arg);
    return e_GDT_FAIL;
  }

#ifndef FEATURE_CGPS_PLAYBACK
  result = rfs_close (l_FileDescriptor);
  if (result != 0)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"Failed rfs_close with result = %d", result,0,0);
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveCloseCbApi(z_Arg);
    return e_GDT_FAIL;
  }
#endif
  gdt_ReadCurrOffset[cpz_Session->e_ServiceId] = 0;

  if (!gdt_SetFileDescriptorAndValidity(cpz_Session, 0, FALSE))
  {
    gdt_Users[cpz_Session->e_ServiceId].z_CbInfo.ReceiveCloseCbApi(z_Arg);
    return e_GDT_FAIL;
  }

  gdt_ReportEventSessionCloseReq(cpz_Session);

  /* Notify AP of Receive Close */
  #ifdef FEATURE_QMI_PDS_EXT
  #error code not present
#else
  /* Inform AP that GDT is done with the file */
  return loc_gdt_ReceiveDoneReqInd(cpz_Session->e_ServiceId, cpz_Session->q_SessId,
                                   e_Status);
  #endif /* FEATURE_QMI_PDS_EXT */

  return e_GDT_FAIL;
}


/*===========================================================================
FUNCTION gdt_ReceiveEnd

DESCRIPTION
  This function finalizes the data transmission session between MP and AP.  This is the final message sent by
  the MP client in the callflow to notify AP the data transfer session is finished.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  e_Status - The final status of this session
  (input)

  b_ForceCleanup - If TRUE, force cleanup of remaining session information
  (input)


RETURN VALUE 
  e_GDT_SUCCESS: GDT has received the final download session status from MP 
 
  e_GDT_FAIL  - There is an internal error in GDT
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveEnd(const gdt_SessionType * const cpz_Session, 
                                   e_GDTResponseType e_Status, boolean b_ForceCleanup)
{
#ifndef FEATURE_CGPS_PLAYBACK
  int i;
#endif
  int l_fd = 0;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_ReceiveEnd()",0,0,0);
  
  if (NULL == cpz_Session || cpz_Session->e_ServiceId <= e_GDT_SERVICE_INVALID ||
      cpz_Session->e_ServiceId > e_GDT_SERVICE_LAST ||
      gdt_Users[cpz_Session->e_ServiceId].e_ServiceType != e_GDT_SERVICE_TYPE_RECEIVER)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }

  if(!gdt_CheckSessionId(cpz_Session))
  {
    return e_GDT_FAIL;
  }
  
#ifndef FEATURE_CGPS_PLAYBACK
  i = rfs_unlink (gdt_Users[cpz_Session->e_ServiceId].FilePath);
  if (i < 0)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: rfs_unlink failed.  Error code: %d",i,0,0);
  }
#endif

  if(b_ForceCleanup)
  {
    if (gdt_GetSessionValidity(cpz_Session))
    {
      /* Update lock status in buffer table */
      if (!gdt_SetLockStatus(cpz_Session, FALSE))
      {
        return e_GDT_FAIL;
      }

      /* Update session ID validity*/
      if (!gdt_SetSessionIdAndValidity(cpz_Session, FALSE))
      {
        return e_GDT_FAIL;
      }
    }
  }

  gdt_ReportEventSessionEndReq(cpz_Session, (uint16)e_Status);

  /* Notify AP of Send End */
#ifdef FEATURE_QMI_PDS_EXT
  #error code not present
#else

  /* Inform AP of the receive end */
  return loc_gdt_DownloadEndReqInd(cpz_Session->e_ServiceId, cpz_Session->q_SessId, 
                                   e_Status);
#endif /* FEATURE_QMI_PDS_EXT */

  return e_GDT_FAIL;
}


/*===========================================================================
FUNCTION gdt_BufAlloc

DESCRIPTION
  This function allocates a requested buffer from the requesting user service.  Buffer will be capped at a set
  maximum (currently 500 Kb).  At this time, each service is only allowed to hold a single buffer.  Due to this
  limitation, any subsequent calls to gdt_BufAlloc will return with an error if the service already holds a
  buffer.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)
  
  q_ReqBufferSize - The requested buffer size in bytes
  (input)
  
  cp_ByteBuffer - Local handle to the allocated buffer
  (output)
  
  q_BufferSize - Size of the allocated buffer
  (output)


RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly allocated a buffer
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_BufAlloc(const gdt_SessionType * const cpz_Session, 
                                  uint32 q_ReqBufferSize,
                                  gdt_MemHandle * const p_ByteBuffer,
                                  uint32 * const pq_BufferSize)
{
  gdt_MemHandle p_TempHandle = NULL;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_BufAlloc(), requesting buffer size = %d",q_ReqBufferSize,0,0);
  
  if(NULL == cpz_Session || NULL == p_ByteBuffer || NULL == pq_BufferSize)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }

  /* Check if buffer already allocated */
  if (!gdt_GetBuf(cpz_Session, &p_TempHandle, NULL))
  {
    return e_GDT_FAIL;
  }
  
  if(NULL != p_TempHandle)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Buffer for this service has already been allocated.",0,0,0);
    return e_GDT_FAIL;
  }

  /* Check for buffer size */
  *pq_BufferSize = q_ReqBufferSize > C_GNSS_GDT_MAX_BUF_SIZE ? C_GNSS_GDT_MAX_BUF_SIZE : q_ReqBufferSize;

  /* Allocate byte buffer */
  *p_ByteBuffer = (gdt_MemHandle)os_MemAlloc(*pq_BufferSize, OS_MEM_SCOPE_TASK);

  if(NULL == *p_ByteBuffer)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Buffer size (%d) not allocated.",*pq_BufferSize,0,0);
    return e_GDT_FAIL;
  }

  /* Add buffer to list */
  if (!gdt_RegisterBuf(cpz_Session, *p_ByteBuffer, *pq_BufferSize))
  {
    return e_GDT_FAIL;
  }
  
  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_BufFree

DESCRIPTION
  This function frees the buffer associated with the service

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  cp_ByteBuffer - Local handle to the allocated buffer
  (input)
  

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly freed the buffer
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_BufFree(const gdt_SessionType * const cpz_Session,
                                   gdt_MemHandle p_ByteBuffer)
{
  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_BufFree()",0,0,0);

  if(NULL == cpz_Session || NULL == p_ByteBuffer)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid params",0,0,0);
    return e_GDT_FAIL;
  }

  if (!gdt_UnregisterBuf(cpz_Session, p_ByteBuffer))
  {
    return e_GDT_FAIL;
  }
  
  os_MemFree((void **)&p_ByteBuffer);
    
  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_RcvdSendBeginResponse

DESCRIPTION
  This function is the callback for the upload begin request. GDT needs
  to forward the response to the corresponding module API
 
PARAMETERS 
 
  z_Response - The recieved response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to the user
                 API
 
  e_GDT_FAIL:   Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdSendBeginResponse(gdt_StartArgumentType z_Response)
{
  int i;
  boolean b_ServiceFound = FALSE;

  if(NULL == z_Response.pz_StartResponse ||
    z_Response.pz_StartResponse->z_SessionInfo.e_ServiceId <= e_GDT_SERVICE_INVALID ||
    z_Response.pz_StartResponse->z_SessionInfo.e_ServiceId > e_GDT_SERVICE_LAST)
  {
    return e_GDT_FAIL;
  }

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_RcvdSendBeginResponse(), serviceId:%d, sessId:%lu, status:%d", 
          (int32) z_Response.pz_StartResponse->z_SessionInfo.e_ServiceId, 
          z_Response.pz_StartResponse->z_SessionInfo.q_SessId, 
          (int32) z_Response.pz_StartResponse->e_StartStatus);

  gdt_ReportEventSessionStartResp(z_Response);

  for (i = 0; i <= e_GDT_SERVICE_LAST; ++i)
  {
    if (gdt_Users[i].e_Service == z_Response.pz_StartResponse->z_SessionInfo.e_ServiceId)
    {
      b_ServiceFound = TRUE;
      break;
    }
  }

  if( NULL == gdt_Users[i].z_CbInfo.StartCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - Start",0,0,0);
    return e_GDT_FAIL;
  }
  
  if (b_ServiceFound)
  {
    gdt_Users[i].z_CbInfo.StartCbApi(z_Response);
  }
  else
  {
    return e_GDT_FAIL;
  }
  
  return e_GDT_SUCCESS;
}

/*===========================================================================
FUNCTION gdt_RcvdSendEndResponse

DESCRIPTION
  This function is the callback for the upload end message. GDT needs
  to forward the response to the corresponding module API
 
PARAMETERS 
 
  z_Response - The recieved response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to the 
                 user API
 
  e_GDT_FAIL:   Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdSendEndResponse(gdt_EndArgumentType z_Response)
{
  int i;
  boolean b_ServiceFound = FALSE;

  if(NULL == z_Response.pz_EndResponse ||
    z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId <= e_GDT_SERVICE_INVALID ||
    z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId > e_GDT_SERVICE_LAST)
  {
    return e_GDT_FAIL;
  }

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_RcvdSendEndResponse(), serviceId:%d, sessId:%lu, status:%d",
          (int32) z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId, 
          z_Response.pz_EndResponse->z_SessionInfo.q_SessId,
          (int32) z_Response.pz_EndResponse->e_EndStatus);

#ifndef FEATURE_CGPS_PLAYBACK
  i = rfs_unlink (gdt_Users[z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId].FilePath);
  if (i < 0)
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: rfs_unlink failed.  Error code: %d",i,0,0);
  }
#endif

  for (i = 0; i <= e_GDT_SERVICE_LAST; ++i)
  {
    if (gdt_Users[i].e_Service == z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId)
    {
      b_ServiceFound = TRUE;
      break;
    }
  }

  if( NULL == gdt_Users[i].z_CbInfo.EndCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - End",0,0,0);
    return e_GDT_FAIL;
  }

  if (b_ServiceFound)
  {
    gdt_Users[i].z_CbInfo.EndCbApi(z_Response);
  }
  else
  {
    return e_GDT_FAIL;
  }

  if(gdt_GetSessionValidity(&z_Response.pz_EndResponse->z_SessionInfo))
  {
    /* Update lock status in buffer table */
    if (!gdt_SetLockStatus(&z_Response.pz_EndResponse->z_SessionInfo, FALSE))
    {
      return e_GDT_FAIL;
    }
    /* Update session ID validity*/
    if (!gdt_SetSessionIdAndValidity(&z_Response.pz_EndResponse->z_SessionInfo, FALSE))
    {
      return e_GDT_FAIL;
    }
  }

  gdt_ReportEventSessionEndResp(z_Response);
  
  return e_GDT_SUCCESS;
}

/*===========================================================================
FUNCTION gdt_RcvdSendAckResponse

DESCRIPTION
  This function is the callback for the send ACK message. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 
 
  cpz_Session - Points to the session information from MP to AP
  (input)

  z_Response - The recieved response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdSendAckResponse(gdt_SendAckArgumentType z_Response)
{
  int i;
  boolean b_ServiceFound = FALSE;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_RcvdSendAckResponse()",0,0,0);

  if(NULL == z_Response.pz_AckResponse ||
    z_Response.pz_AckResponse->z_SessionInfo.e_ServiceId <= e_GDT_SERVICE_INVALID ||
    z_Response.pz_AckResponse->z_SessionInfo.e_ServiceId > e_GDT_SERVICE_LAST)
  {
    return e_GDT_FAIL;
  }

  gdt_ReportEventSendResp(z_Response);

  for (i = 0; i <= e_GDT_SERVICE_LAST; ++i)
  {
    if (gdt_Users[i].e_Service == z_Response.pz_AckResponse->z_SessionInfo.e_ServiceId)
    {
      b_ServiceFound = TRUE;
      break;
    }
  }

  if( NULL == gdt_Users[i].z_CbInfo.SendAckCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - SendAck",0,0,0);
    return e_GDT_FAIL;
  }

  if (b_ServiceFound)
  {
    gdt_Users[i].z_CbInfo.SendAckCbApi(z_Response);
  }
  else
  {
    return e_GDT_FAIL;
  }
  
  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_ReceiveReadyResponse

DESCRIPTION
  This function is the callback for getting receive ready from AP. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 
  
  z_Response - The received response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdReceiveReadyResponse(gdt_ReceiveReadyArgumentType z_Response)
{
  int i;
  boolean b_ServiceFound = FALSE;

  if(NULL == z_Response.pz_ReadyResponse ||
    z_Response.pz_ReadyResponse->z_SessionInfo.e_ServiceId <= e_GDT_SERVICE_INVALID ||
    z_Response.pz_ReadyResponse->z_SessionInfo.e_ServiceId > e_GDT_SERVICE_LAST)
  {
    return e_GDT_FAIL;
  }

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_RcvdReceiveReadyResponse(), serviceId:%d, sessId:%lu, status:%d", 
          (int32) z_Response.pz_ReadyResponse->z_SessionInfo.e_ServiceId, 
          z_Response.pz_ReadyResponse->z_SessionInfo.q_SessId, 
          (int32) z_Response.pz_ReadyResponse->e_StartStatus);

  for (i = 0; i <= e_GDT_SERVICE_LAST; ++i)
  {
    if (gdt_Users[i].e_Service == z_Response.pz_ReadyResponse->z_SessionInfo.e_ServiceId)
    {
      b_ServiceFound = TRUE;
      break;
    }
  }

  if( NULL == gdt_Users[i].z_CbInfo.ReceiveReadyCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - ReceiveReady",0,0,0);
    return e_GDT_FAIL;
  }

  if (b_ServiceFound)
  {
    gdt_Users[i].z_CbInfo.ReceiveReadyCbApi(z_Response);
  }
  else
  {
    return e_GDT_FAIL;
  }
  
  return e_GDT_SUCCESS;
}

/*===========================================================================
FUNCTION gdt_RcvdReceiveCloseResponse

DESCRIPTION
  This function is the callback for getting the ack for an MP receive close from AP. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 

  z_Response - The received response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdReceiveCloseResponse(gdt_CloseArgumentType z_Response)
{
  int i;
  boolean b_ServiceFound = FALSE;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_RcvdReceiveCloseResponse()",0,0,0);

  if(NULL == z_Response.pz_CloseResponse ||
     z_Response.pz_CloseResponse->z_SessionInfo.e_ServiceId <= e_GDT_SERVICE_INVALID ||
     z_Response.pz_CloseResponse->z_SessionInfo.e_ServiceId > e_GDT_SERVICE_LAST)
  {
    return e_GDT_FAIL;
  }

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_RcvdReceiveCloseResponse(), serviceId:%d, sessId:%lu, status:%d", 
          (int32) z_Response.pz_CloseResponse->z_SessionInfo.e_ServiceId, 
          z_Response.pz_CloseResponse->z_SessionInfo.q_SessId, 
          (int32) z_Response.pz_CloseResponse->e_CloseStatus);

  for (i = 0; i <= e_GDT_SERVICE_LAST; ++i)
  {
    if (gdt_Users[i].e_Service == z_Response.pz_CloseResponse->z_SessionInfo.e_ServiceId)
    {
      b_ServiceFound = TRUE;
      break;
    }
  }

  if( NULL == gdt_Users[i].z_CbInfo.ReceiveCloseCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - ReceiveClose",0,0,0);
    return e_GDT_FAIL;
  }

  if (b_ServiceFound)
  {
    gdt_Users[i].z_CbInfo.ReceiveCloseCbApi(z_Response);
  }
  else
  {
    return e_GDT_FAIL;
  }
  
  return e_GDT_SUCCESS;
}


/*===========================================================================
FUNCTION gdt_RcvdReceiveEndResponse

DESCRIPTION
  This function is the callback for getting the ack for an MP receive end from AP. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 

  z_Response - The received response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdReceiveEndResponse(gdt_ReceiveEndArgumentType z_Response)
{
  int i;
  boolean b_ServiceFound = FALSE;
  gdt_EndArgumentType z_Arg;

  if(NULL == z_Response.pz_EndResponse ||
    z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId <= e_GDT_SERVICE_INVALID ||
    z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId > e_GDT_SERVICE_LAST)
  {
    return e_GDT_FAIL;
  }

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: gdt_RcvdReceiveEndResponse(), serviceId:%d, sessId:%lu, status:%d",
          (int32) z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId, 
          z_Response.pz_EndResponse->z_SessionInfo.q_SessId,
          (int32) z_Response.pz_EndResponse->e_EndStatus);

  for (i = 0; i <= e_GDT_SERVICE_LAST; ++i)
  {
    if (gdt_Users[i].e_Service == z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId)
    {
      b_ServiceFound = TRUE;
      break;
    }
  }

  if( NULL == gdt_Users[i].z_CbInfo.ReceiveEndCbApi )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED,"GDT: Invalid Function Callback - ReceiveEnd",0,0,0);
    return e_GDT_FAIL;
  }

  if (b_ServiceFound)
  {
    gdt_Users[i].z_CbInfo.ReceiveEndCbApi(z_Response);
  }
  else
  {
    return e_GDT_FAIL;
  }

  if(gdt_GetSessionValidity(&z_Response.pz_EndResponse->z_SessionInfo))
  {
    /* Update lock status in buffer table */
    if (!gdt_SetLockStatus(&z_Response.pz_EndResponse->z_SessionInfo, FALSE))
    {
      return e_GDT_FAIL;
    }
    /* Update session ID validity*/
    if (!gdt_SetSessionIdAndValidity(&z_Response.pz_EndResponse->z_SessionInfo, FALSE))
    {
      return e_GDT_FAIL;
    }
  }

  z_Arg.pz_EndResponse = z_Response.pz_EndResponse;
  gdt_ReportEventSessionEndResp(z_Arg);
  
  return e_GDT_SUCCESS;

}



/*===========================================================================

FUNCTION gdt_ReportEventSessionStartReq

DESCRIPTION
  Report event: GDT Session initiated

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionStartReq (const gdt_SessionType * const cpz_Session)
{
  uint16 w_ServiceId;
  gdt_SessStartReqEventType z_SessStartReqEvent;

  if(NULL == cpz_Session)
  {
    return;
  }

  memset(&z_SessStartReqEvent, 0x00, sizeof(z_SessStartReqEvent));
  w_ServiceId = (uint16) cpz_Session->e_ServiceId;
  
  (void) memsmove((void *)&(z_SessStartReqEvent.w_ServiceId),
    sizeof(z_SessStartReqEvent.w_ServiceId),
    &w_ServiceId, sizeof(w_ServiceId));
  (void) memsmove((void *)&(z_SessStartReqEvent.q_SessionId),
    sizeof(z_SessStartReqEvent.q_SessionId),
    &cpz_Session->q_SessId, sizeof(cpz_Session->q_SessId));

  event_report_payload(EVENT_GNSS_GDT_SESS_START_REQ, sizeof(z_SessStartReqEvent), &z_SessStartReqEvent);
}


/*===========================================================================

FUNCTION gdt_ReportEventSessionStartResp

DESCRIPTION
  Report event: GDT Session initiated response

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionStartResp (gdt_StartArgumentType z_Response)
{
  uint16 w_ServiceId, w_GdtResp;
  gdt_SessStartRespEventType z_SessStartRespEvent;

  if(NULL == z_Response.pz_StartResponse)
  {
    return;
  }

  memset(&z_SessStartRespEvent, 0x00, sizeof(z_SessStartRespEvent));
  w_ServiceId = (uint16) z_Response.pz_StartResponse->z_SessionInfo.e_ServiceId;
  w_GdtResp = (uint16) z_Response.pz_StartResponse->e_StartStatus;
  
  (void) memsmove((void *)&(z_SessStartRespEvent.w_ServiceId),
    sizeof(z_SessStartRespEvent.w_ServiceId),
    &w_ServiceId, sizeof(w_ServiceId));
  (void) memsmove((void *)&(z_SessStartRespEvent.q_SessionId),
    sizeof(z_SessStartRespEvent.q_SessionId),
    &z_Response.pz_StartResponse->z_SessionInfo.q_SessId,
    sizeof(z_Response.pz_StartResponse->z_SessionInfo.q_SessId));
  (void) memsmove((void *)&(z_SessStartRespEvent.w_GdtResp),
    sizeof(z_SessStartRespEvent.w_GdtResp),
    &w_GdtResp, sizeof(w_GdtResp));
  (void) memsmove((void *)&(z_SessStartRespEvent.l_ClientHandle),
    sizeof(z_SessStartRespEvent.l_ClientHandle),
    &z_Response.pz_StartResponse->l_Client_handle,
    sizeof(z_Response.pz_StartResponse->l_Client_handle));
  (void) memsmove((void *)&(z_SessStartRespEvent.q_TransactionId),
    sizeof(z_SessStartRespEvent.q_TransactionId),
    &z_Response.pz_StartResponse->q_TransactionId,
    sizeof(z_Response.pz_StartResponse->q_TransactionId));

  event_report_payload(EVENT_GNSS_GDT_SESS_START_RESP, sizeof(z_SessStartRespEvent), &z_SessStartRespEvent);
}


/*===========================================================================

FUNCTION gdt_ReportEventSendReq

DESCRIPTION
  Report event: GDT Session send data request

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSendReq (const gdt_SessionType * const cpz_Session, uint32 q_DataSize)
{
  uint16 w_ServiceId;
  gdt_SendReqEventType z_SendReqEvent;

  if(NULL == cpz_Session)
  {
    return;
  }

  memset(&z_SendReqEvent, 0x00, sizeof(z_SendReqEvent));
  w_ServiceId = (uint16) cpz_Session->e_ServiceId;
  
  (void) memsmove((void *)&(z_SendReqEvent.w_ServiceId),
    sizeof(z_SendReqEvent.w_ServiceId),
    &w_ServiceId, sizeof(w_ServiceId));
  (void) memsmove((void *)&(z_SendReqEvent.q_SessionId),
    sizeof(z_SendReqEvent.q_SessionId),
    &cpz_Session->q_SessId, sizeof(cpz_Session->q_SessId));
  (void) memsmove((void *)&(z_SendReqEvent.q_DataSize),
    sizeof(z_SendReqEvent.q_DataSize),
    &q_DataSize, sizeof(q_DataSize));

  event_report_payload(EVENT_GNSS_GDT_SEND_REQ, sizeof(z_SendReqEvent), &z_SendReqEvent);
}


/*===========================================================================

FUNCTION gdt_ReportEventSendResp

DESCRIPTION
  Report event: GDT Session send data response

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSendResp (gdt_SendAckArgumentType z_Response)
{
  uint16 w_ServiceId, w_EndStatus;
  gdt_SendRespEventType z_SendRespEvent;

  if(NULL == z_Response.pz_AckResponse)
  {
    return;
  }

  memset(&z_SendRespEvent, 0x00, sizeof(z_SendRespEvent));
  w_ServiceId = (uint16) z_Response.pz_AckResponse->z_SessionInfo.e_ServiceId;
  w_EndStatus = (uint16) z_Response.pz_AckResponse->e_EndStatus;

  (void) memsmove((void *)&(z_SendRespEvent.w_ServiceId),
    sizeof(z_SendRespEvent.w_ServiceId),
    &w_ServiceId, sizeof(w_ServiceId));
  (void) memsmove((void *)&(z_SendRespEvent.q_SessionId),
    sizeof(z_SendRespEvent.q_SessionId),
    &z_Response.pz_AckResponse->z_SessionInfo.q_SessId,
    sizeof(z_Response.pz_AckResponse->z_SessionInfo.q_SessId));
  (void) memsmove((void *)&(z_SendRespEvent.w_GdtResp),
    sizeof(z_SendRespEvent.w_GdtResp),
    &w_EndStatus, sizeof(w_EndStatus));

  event_report_payload(EVENT_GNSS_GDT_SEND_RESP, sizeof(z_SendRespEvent), &z_SendRespEvent);
}


/*===========================================================================

FUNCTION gdt_ReportEventSessionEndReq

DESCRIPTION
  Report event: GDT Session send end req

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionEndReq (const gdt_SessionType * const cpz_Session, uint16 w_Status)
{
  uint16 w_ServiceId;
  gdt_SessEndReqEventType z_SessEndReqEvent;

  if(NULL == cpz_Session)
  {
    return;
  }

  memset(&z_SessEndReqEvent, 0x00, sizeof(z_SessEndReqEvent));
  w_ServiceId = (uint16) cpz_Session->e_ServiceId;

  (void) memsmove((void *)&(z_SessEndReqEvent.w_ServiceId),
    sizeof(z_SessEndReqEvent.w_ServiceId),
    &w_ServiceId, sizeof(w_ServiceId));
  (void) memsmove((void *)&(z_SessEndReqEvent.q_SessionId),
    sizeof(z_SessEndReqEvent.q_SessionId),
    &cpz_Session->q_SessId,sizeof(cpz_Session->q_SessId));
  (void) memsmove((void *)&(z_SessEndReqEvent.w_GdtResp),
    sizeof(z_SessEndReqEvent.w_GdtResp),
    &w_Status, sizeof(w_Status));

  event_report_payload(EVENT_GNSS_GDT_SESS_END_REQ, sizeof(z_SessEndReqEvent), &z_SessEndReqEvent);
}


/*===========================================================================

FUNCTION gdt_ReportEventSessionEndResp

DESCRIPTION
  Report event: GDT Session end response

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionEndResp (gdt_EndArgumentType z_Response)
{
  uint16 w_ServiceId, w_GdtResp;
  gdt_SessEndRespEventType z_SessEndRespEvent;

  if(NULL == z_Response.pz_EndResponse)
  {
    return;
  }

  memset(&z_SessEndRespEvent, 0x00, sizeof(z_SessEndRespEvent));
  w_ServiceId = (uint16) z_Response.pz_EndResponse->z_SessionInfo.e_ServiceId;
  w_GdtResp = (uint16) z_Response.pz_EndResponse->e_EndStatus;
  
  (void) memsmove((void *)&(z_SessEndRespEvent.w_ServiceId),
    sizeof(z_SessEndRespEvent.w_ServiceId),
    &w_ServiceId, sizeof(w_ServiceId));
  (void) memsmove((void *)&(z_SessEndRespEvent.q_SessionId),
    sizeof(z_SessEndRespEvent.q_SessionId),
    &z_Response.pz_EndResponse->z_SessionInfo.q_SessId,
    sizeof(z_Response.pz_EndResponse->z_SessionInfo.q_SessId));
  (void) memsmove((void *)&(z_SessEndRespEvent.w_GdtAckResp),
    sizeof(z_SessEndRespEvent.w_GdtAckResp),
    &w_GdtResp, sizeof(w_GdtResp));
  (void) memsmove((void *)&(z_SessEndRespEvent.l_ClientHandle),
    sizeof(z_SessEndRespEvent.l_ClientHandle),
    &z_Response.pz_EndResponse->l_Client_handle,
    sizeof(z_Response.pz_EndResponse->l_Client_handle));
  (void) memsmove((void *)&(z_SessEndRespEvent.q_TransactionId),
    sizeof(z_SessEndRespEvent.q_TransactionId),
    &z_Response.pz_EndResponse->q_TransactionId,
    sizeof(z_Response.pz_EndResponse->q_TransactionId));

  event_report_payload(EVENT_GNSS_GDT_SESS_END_RESP, sizeof(z_SessEndRespEvent), &z_SessEndRespEvent);
}


/*===========================================================================

FUNCTION gdt_ReportEventSessionOpenReq

DESCRIPTION
  Report event: GDT Session open

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionOpenReq (const gdt_SessionType * const cpz_Session)
{
  uint16 w_ServiceId;
  gdt_SessOpenReqEventType z_SessOpenReqEvent;

  if(NULL == cpz_Session)
  {
    return;
  }

  memset(&z_SessOpenReqEvent, 0x00, sizeof(z_SessOpenReqEvent));
  w_ServiceId = (uint16) cpz_Session->e_ServiceId;
  
  (void) memsmove((void *)&(z_SessOpenReqEvent.w_ServiceId),
    sizeof(z_SessOpenReqEvent.w_ServiceId),
    &w_ServiceId, sizeof(w_ServiceId));
  (void) memsmove((void *)&(z_SessOpenReqEvent.q_SessionId),
    sizeof(z_SessOpenReqEvent.q_SessionId),
    &cpz_Session->q_SessId, sizeof(cpz_Session->q_SessId));

  event_report_payload(EVENT_GNSS_GDT_SESS_OPEN_REQ, sizeof(z_SessOpenReqEvent), &z_SessOpenReqEvent);
}


/*===========================================================================

FUNCTION gdt_ReportEventSessionCloseReq

DESCRIPTION
  Report event: GDT Session close

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionCloseReq (const gdt_SessionType * const cpz_Session)
{
  uint16 w_ServiceId;
  gdt_SessCloseReqEventType z_SessCloseReqEvent;

  if(NULL == cpz_Session)
  {
    return;
  }

  memset(&z_SessCloseReqEvent, 0x00, sizeof(z_SessCloseReqEvent));
  w_ServiceId = (uint16) cpz_Session->e_ServiceId;
  
  (void) memsmove((void *)&(z_SessCloseReqEvent.w_ServiceId),
    sizeof(z_SessCloseReqEvent.w_ServiceId),
    &w_ServiceId, sizeof(w_ServiceId));
  (void) memsmove((void *)&(z_SessCloseReqEvent.q_SessionId),
    sizeof(z_SessCloseReqEvent.q_SessionId),
    &cpz_Session->q_SessId, sizeof(cpz_Session->q_SessId));

  event_report_payload(EVENT_GNSS_GDT_SESS_CLOSE_REQ, sizeof(z_SessCloseReqEvent), &z_SessCloseReqEvent);
}

/*===========================================================================

FUNCTION
  gdt_LogDataBlob

DESCRIPTION
  Logs a GDT BLOB of data

PARAMETERS
  cpz_Iface - Points to the GDT interface containing the raw data to be logged
  (input)
 
  u_Format  - The format of data that needs to be used by the log parser
  (input)     C_GDT_BLOB_FORMAT_BINARY or C_GDT_BLOB_FORMAT_BINARY
 
 w_ContentType - The content type of the raw data used by parser
 (input)

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If the log is generated successfuly
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean gdt_LogDataBlob( const gdt_InterfaceType * cpz_Iface,
                         uint8 u_Format, uint16 w_ContentType)
{
  gdt_DataBlobDmLogType  *pz_LogPtr = NULL; 
  gdt_PackedDataBlobType *pz_PackedBlob = NULL;
  const unsigned char *cpz_Data = NULL; 
  uint32 q_Length = 0; 
  uint16 w_PartNo;
  uint16 w_TotalParts;
  uint16 w_StartIndex = 0;
  uint32 q_SrcTask;
  uint16  w_ServiceId;
  uint32 q_BytesTotal = 0;


  if(NULL == cpz_Iface) 
  {
    return FALSE;
  }

  if( (u_Format != C_GDT_BLOB_FORMAT_BINARY) &&
      (u_Format != C_GDT_BLOB_FORMAT_ASCII) )
  {
    /* Invalid Log format */
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Logging of format %u is not supported", 
             u_Format,0,0);
    return FALSE;
  }

  if(!log_status(LOG_GNSS_DATA_BLOB_C))
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Logging is not enabled", 
             0,0,0);
    return FALSE;
  }

  cpz_Data  = cpz_Iface->z_GdtBuffer;
  q_Length = cpz_Iface->q_MsgLength;

  if ( (NULL == cpz_Data) ||
       (q_Length == 0) )
  {
    MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Logging Failed Data:%p, len:%lu ", 
             cpz_Data, q_Length, 0);
    return FALSE;
  }


  q_SrcTask = os_CurrentThreadIdGet();
  if (cpz_Iface->z_SessInfo.e_ServiceId < 0)
  {
    w_ServiceId = 0;
  }
  else
  {
    w_ServiceId = (uint16) (cpz_Iface->z_SessInfo.e_ServiceId + 1);
  }

  w_TotalParts = (uint16) (q_Length / C_GDT_BLOB_MAX_DATA_SIZE + 1);
  w_PartNo = 1;

  MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Logging of format totalParts:%u, service:%d, len:%lu ", 
           w_TotalParts, w_ServiceId, q_Length);
  
  for (w_PartNo = 1; w_PartNo <= w_TotalParts; ++w_PartNo) 
  {
    q_BytesTotal += C_GDT_BLOB_MAX_DATA_SIZE;

    pz_LogPtr = (gdt_DataBlobDmLogType *) log_alloc(LOG_GNSS_DATA_BLOB_C, 
                                                    sizeof(gdt_DataBlobDmLogType));
    if(NULL == pz_LogPtr)
    {
      MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH,"GDT: Log allocation failed ", 0, 0, 0);
      return FALSE;
    }

    pz_PackedBlob = &pz_LogPtr->z_Blob;
    pz_LogPtr->u_Version = 1;
    pz_PackedBlob->q_SrcTask = q_SrcTask;
    pz_PackedBlob->w_ServiceId = w_ServiceId;
    pz_PackedBlob->q_SessionId = cpz_Iface->z_SessInfo.q_SessId;
    pz_PackedBlob->u_Format  = u_Format;
    pz_PackedBlob->w_ContentType = w_ContentType;
    pz_PackedBlob->w_TotalParts = w_TotalParts;
    if(q_Length >= w_StartIndex)
    {
      memscpy(&pz_PackedBlob->sz_Data[0], sizeof(pz_PackedBlob->sz_Data),
              &cpz_Data[w_StartIndex], (q_Length - w_StartIndex));
    }

    if (q_BytesTotal < q_Length)
    {
      pz_PackedBlob->q_Length = C_GDT_BLOB_MAX_DATA_SIZE;
      w_StartIndex += C_GDT_BLOB_MAX_DATA_SIZE;
    }
    else
    {
      pz_PackedBlob->q_Length = q_Length - (q_BytesTotal - C_GDT_BLOB_MAX_DATA_SIZE);
    }
    pz_PackedBlob->w_Part  = w_PartNo;

    /* send the log */
    log_commit(pz_LogPtr);  
  }

  return TRUE;
}

