#ifndef TLE_IFACE_H
#define TLE_IFACE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TLE Interface Header
GENERAL DESCRIPTION
   This file contains constants, structures and functions prototypes
   related to TLE Interface.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
==============================================================================*/



/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/tle/2_0/inc/tle_iface.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
5/01/2015  ahmadh Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "tle_api.h"
#include "aries_os_api.h"

#ifdef __cplusplus
extern "C" {
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

/*==============================================================================

                                  VARIABLES

==============================================================================*/


/*=============================================================================

       Structures and enumerated types

=============================================================================*/

typedef enum
{
  C_TDP_SESSION_SRC_INVALID,
  C_TDP_UPLD_SRC_MIN, 
  C_TDP_UPLD_SRC_PERIODIC_TIMER = C_TDP_UPLD_SRC_MIN,
  C_TDP_UPLD_SRC_PERIODIC_FORCED,
  C_TDP_UPLD_SRC_MEMORY_FULL,
  C_TDP_UPLD_SRC_END = C_TDP_UPLD_SRC_MEMORY_FULL,

  C_TDP_DL_SRC_MIN,
  C_TDP_DL_SRC_SERV_CELL_NOT_FOUND = C_TDP_DL_SRC_MIN,
  C_TDP_DL_SRC_NEIGHBOR_PARTITION_NOT_FOUND,
  C_TDP_DL_SRC_PARTITION_REFRESH,
  C_TDP_DL_SRC_DIAG,
  C_TDP_DL_SRC_END = C_TDP_DL_SRC_DIAG,
  C_TDP_SESSION_SRC_MAX
} e_TleSessionSrcType ;

/*=============================================================================

       Functions

=============================================================================*/

#ifdef FEATURE_CGPS_PLAYBACK
/*===========================================================================

  FUNCTION      tle_PbInjectPartition

  DESCRIPTION
    This function is for PE Playback to inject TDP server response message to
    TLE.

  PARAMETERS
    cp_buf - Pointer to the server response message
    (Input)

    cq_BufLen - Message length
    (Input)

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void tle_PbInjectPartition(const char *cp_buf, const uint32 cq_BufLen);

/*===========================================================================

  FUNCTION      tle_PbProcessGdtBlobData

  DESCRIPTION
    This function is for PE Playback to inject a GDT blob parsed
    from 0x18AF (LOG_GNSS_DATA_BLOB_C) log packet

  PARAMETERS
    p_Data - void * to packed log data payload. This is the actual payload
    (input)  data of the log (log_hdr_type->p_Data)

    q_Size - Message length
    (Input)

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void tle_PbProcessGdtBlobData(const void *p_Data, uint32 q_Size);

/*===========================================================================

  FUNCTION      tle_PbProcessGdtDownloadRcvReady

  DESCRIPTION
    This function is used as a call back API for PE playback to send a 
    download receive ready message.

  PARAMETERS
    cpz_RcvReadyMsg - Points to the download receive ready 
    (input)  

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void tle_PbProcessGdtDownloadRcvReady(const gdt_ReceiveReadyResponseType *cpz_RcvReadyMsg);

/*===========================================================================

  FUNCTION      tle_PbProcessGdtDownldRcvEndResponse

  DESCRIPTION
    This function is used as a call back API for PE playback to send a 
    GDT end response message.

  PARAMETERS
    cpz_GdtEndMsg - Points to the GDT end response message 
    (input)  

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void tle_PbProcessGdtDownldRcvEndResponse(const gdt_EndResponseType *cpz_GdtEndMsg);

/*===========================================================================

  FUNCTION      tle_PbInitiateTdpDownload

  DESCRIPTION
    This function is used as a call back API for PE playback to initiate a GTP 
    download session.

  PARAMETERS
    e_Src - The source for session 
    (input)  

    t_CurrentTimeMs - Session request time
    (input)

  DEPENDENCIES
    None

  RETURN VALUE
    true  - session started successfuly
    false - otherwise

  SIDE EFFECTS
    None

===========================================================================*/
bool tle_PbInitiateTdpDownload(e_TleSessionSrcType e_Src, uint64 t_CurrentTimeMs);

/*===========================================================================

FUNCTION  tle_PbProcessClientDownldDataFromQmi

DESCRIPTION
  This function is used as a call back for PE playback to processes the injected 
  client downloaded data by AP through QMI message 
 
 PARAMETERS
   cpz_Data - WWAN client downloaded data received from AP
   (input)       

RETURN VALUE

DEPENDENCIES
 
===========================================================================*/
void tle_PbProcessClientDownldDataFromQmi(const tle_GtpClientDownloadedReqType * cpz_Data);

/*=============================================================================

  FUNCTION      tle_PbProcUpldEndResp

  DESCRIPTION
    This function is used as a call back for PE playback to processes the 
    reponse from a GDT upload end message 
 
  PARAMETERS
    cpz_EndResponse - Points to the status of the end upload session
    (input)
                
  DEPENDENCIES  None

  PARAMETERS
     

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_PbProcUpldEndResp(const gdt_EndResponseType *cpz_EndResponse);

/*=============================================================================

  FUNCTION    tle_PbProcUploadBeginResp

  DESCRIPTION
    This function is used as a call back for PE playback to processes the 
    response for a GDT upload begin 
 
  PARAMETERS
    cpz_BeginResp - Points to the response received from GDT
    (input)
                
  DEPENDENCIES  None

  PARAMETERS
     

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_PbProcUploadBeginResp(const gdt_StartResponseType * cpz_BeginResp);

/*=============================================================================

  FUNCTION      tle_PbProcUpldOpenResponse

  DESCRIPTION
    This function is used as a call back for PE playback to processes the 
    response from a GDT open message. 
 
  PARAMETERS
    cpz_OpenResponse - Points to the status of the GDT open request
    (input)
                
  DEPENDENCIES  None

  PARAMETERS
     

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_PbProcUpldOpenResponse(const gdt_OpenResponseType *cpz_OpenResponse);

/*=============================================================================

  FUNCTION      tle_PbProcUpldCloseResponse

  DESCRIPTION
    This function is used as a call back for PE playback to processes the 
    response from a GDT close message 
 
  PARAMETERS
    cpz_CloseResponse - Points to the status of the GDT close request
    (input)
                
  DEPENDENCIES  None

  PARAMETERS
     

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_PbProcUpldCloseResponse(const gdt_CloseResponseType *cpz_CloseResponse);

/*=============================================================================

  FUNCTION      tle_PbProcUpldSendAck

  DESCRIPTION
    This function is used as a call back for PE playback to processes an ack 
    received from a GDT upload send message 
 
  PARAMETERS
    cpz_AckMsg - Points to the ACK received from GDT
    (input)
                
  DEPENDENCIES  None

  PARAMETERS
     

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_PbProcUpldSendAck(const gdt_SendAckResponseType *cpz_AckMsg);
#endif /* FEATURE_CGPS_PLAYBACK */

/*==============================================================================
  FUNCTION      tle_task

  DESCRIPTION 	
    The entry function into the TDP task.

  DEPENDENCIES
    None

  PARAMETERS
    dummy       - An unused variable required by Rex

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tle_task(dword dummy); 

void tle_IfaceInit(void);

/*=============================================================================

  FUNCTION      tdp_InjectTdpMeasBlk
  
  DESCRIPTION
    This API is used by WWAN ME to inject TDP measurement block to TDP
  
  DEPENDENCIES  None
  
  PARAMETERS
    cpz_TdpMeasBlk - Points to the measuremnet block to be injected
    (input)
  
  
  RETURN VALUE
    TRUE  - If the IPC is sent to TDP
    FALSE - Otherwise 
  
  SIDE EFFECTS  

=============================================================================*/
boolean tdp_InjectTdpMeasBlk(const tdp_MeasAndCellDataBlkStructType * cpz_TdpMeasBlk);

/*==============================================================================
  FUNCTION      tdp_GsmCellChange

  DESCRIPTION 	
    Alert tle of a change in serving cell to a new GSM cell.

  DEPENDENCIES
    None

  PARAMETERS
    b_SubscrID - The subscription of the cell change.
 
    cpz_GsmCell - The cell info of the new cell.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tdp_GsmCellChange(uint8 b_SubscrID,
                       const cgps_GsmCellInfoType * const cpz_GsmCell);

/*==============================================================================
  FUNCTION      tdp_WcdmaCellChange

  DESCRIPTION 	
    Alert tle of a change in serving cell to a new WCDMA cell.

  DEPENDENCIES
    None

  PARAMETERS
    b_SubscrID - The subscription of the cell change.
 
    cpz_WcdmaCell - The cell info of the new cell.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tdp_WcdmaCellChange(uint8 b_SubscrID,
                         const cgps_WcdmaCellInfoType * const cpz_WcdmaCell);

/*==============================================================================
  FUNCTION      tdp_SSChange

  DESCRIPTION 	
    Alert tle of a change in serving cell type or in 1x cell information (SID,
    NID, bsid, lat, lon, search window size).
 
  DEPENDENCIES
    None

  PARAMETERS
    p_SSInfo - The type must be:
               CGPS_SSINFO_SRV_SYS - Contains the serving system type.
               CGPS_SSINFO_CELLINFO - Contains the complete 1x serving cell
                info.  If position is not valid, the lat and lon shall be set
                to 0.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tdp_SSChange(const cgps_SSInfoType * const p_SSInfo);

#ifdef FEATURE_CGPS_PLAYBACK

/*===========================================================================
FUNCTION
    tle_PbSetGenBsaPath

DESCRIPTION
    This function is for PE Playback to set up a path in which generated BSA files will
    be stored

PARAMETERS
 FileFolder - the path to store the files
 (input)

 FileBaseName - The base for output file name
 (input)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
    None

===========================================================================*/
void tle_PbSetGenBsaPath(const char *FileFolder,
                         const char *FileBaseName);

/*==============================================================================
  FUNCTION      tle_PbProcIpcMsg()
  
  DESCRIPTION 	
    For PE Playback to process IPC messages.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    p_Msg       - Pointer to the IPC message to process.
  
  RETURN VALUE  
    None
  
  SIDE EFFECTS  
    None

==============================================================================*/
void tle_PbProcIpcMsg(const os_IpcMsgType * p_Msg);

/*===========================================================================
FUNCTION
    tle_PbSetupApClient

DESCRIPTION
    This function is for PE Playback to set up real time connetion to an
    emulated application processor.

PARAMETERS
    w_Port - Poort number for the AP emulator
    (Input)

    pc_Server - AP emulator IP address 
    (Input)
 
    q_NameLen - The length of the address
    (Input)
 
    pc_GdtRootStr - The root folder for GDT files
    (input)
 
    q_ServiceMask - Which services will be enabled in AP client
    (input)

DEPENDENCIES
    This emulator must be started before the call to this function.

RETURN VALUE
    TRUE  - PE playback is connected to the AP emulator
    FALSE - Otherwise

SIDE EFFECTS
    None

===========================================================================*/
boolean tle_PbSetupApClient(uint32 q_Port, const char *pc_Server, 
                            uint32 q_NameLen, const char *pc_GdtRootStr,
                            uint32 q_ServiceMask);
/*===========================================================================
FUNCTION
    tle_PbSetupServingCell

DESCRIPTION
    This function is for PE Playback to set up a serving cell to be used for
    GTP download.

PARAMETERS
    pc_CellStr - serving cell in a comma delimited string
    (Input)      (rat, L1, L2, L3, L4)

    q_Len - The length of the string
    (Input)
 
DEPENDENCIES
    This emulator must be started before the call to this function.

RETURN VALUE
    TRUE  - serving cell value is stored 
    FALSE - Otherwise

SIDE EFFECTS
    None

===========================================================================*/
boolean tle_PbSetupServingCell(const char *pc_CellStr, uint32 q_Len);


/*===========================================================================
FUNCTION
    tle_PbSetupServingCellFile

DESCRIPTION
    This function is for PE Playback to send a .txt file which contains a 
    group of cells which are going to be used as serving cells for
    GTP download.

PARAMETERS
    pc_CellFileStr - The file which contains data in comma delimited format
    (Input)      (rat, L1, L2, L3, L4)

    q_Len - The length of file name
    (Input)
 
DEPENDENCIES
    This emulator must be started before the call to this function.

RETURN VALUE
    TRUE  - The file content is read 
    FALSE - Otherwise

SIDE EFFECTS
    None

===========================================================================*/
boolean tle_PbSetupServingCellFile(const char *pc_CellFileStr, uint32 q_Len);

/*===========================================================================
FUNCTION tle_PbProcessGdtDownloadOpenResponse

DESCRIPTION
  This function is for PE playback to processes GDT response to a Open request 
  message. 
   
PARAMETERS 
  cpz_OpenResp - Points to the GDT open response
  (input)
   
DEPENDENCIES

RETURN VALUE 
  
SIDE EFFECTS

===========================================================================*/
void  tle_PbProcessGdtDownloadOpenResponse(const gdt_OpenResponseType * cpz_OpenResp);

/*===========================================================================
FUNCTION tle_PbProcessGdtDownloadRcvDataAck

DESCRIPTION
  This function processes a raw downloaded GDT BLOB received in PE playback. 
   
PARAMETERS 
  cpz_RcvRawData - Points to the GDT received data as raw data. 
  (input)
   
DEPENDENCIES

RETURN VALUE 
  
SIDE EFFECTS

===========================================================================*/
void  tle_PbProcessGdtDownloadRcvDataAck(const gdt_ReceiveDataResponseType * cpz_RcvRawData);

/*===========================================================================
FUNCTION tle_PbProcReceiveCloseResponse

DESCRIPTION
  This function is used as a call back to processes GDT response to a prior close
  request received from AP client in PE playback.
   
PARAMETERS 
  cpz_CloseResp - points to the GDT close response
  (input)
   
DEPENDENCIES

RETURN VALUE 
  

SIDE EFFECTS

===========================================================================*/
void  tle_PbProcReceiveCloseResponse(const gdt_CloseResponseType *cpz_CloseResp);

#endif /* FEATURE_CGPS_PLAYBACK */

/*===========================================================================

FUNCTION tle_IfaceReadPcid

DESCRIPTION
  This function reads the PCID from TLE module. 
 
PARAMETERS 
  t_Pcid - PCID
  (Output)
 
DEPENDENCIES

RETURN VALUE 
  TRUE  - If the PCID is available to be read.
  FALSE - Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tle_IfaceReadPcid(uint64 * pt_Pcid);

/*==============================================================================
  FUNCTION      tle_TaskStop

  DESCRIPTION 	
    Performs the steps necessary to stop the tle task.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tle_TaskStop(void);

#ifdef __cplusplus
}
#endif

#endif /* TLE_IFACE_H */
