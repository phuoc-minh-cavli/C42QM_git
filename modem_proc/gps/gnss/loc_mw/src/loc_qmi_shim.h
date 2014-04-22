#ifndef LOC_QMI_SHIM_H
#define LOC_QMI_SHIM_H
/*============================================================================
  @file loc_qmi_shim.h

  @brief
    This module glues loc-api to the QCSI framework.  It translates the 
    data-structurs to/fro between loc-api and QMI-ULP. 
    
    
  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  opyright (c) 2015 - 2017 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc

============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_qmi_shim.h#1 $ 
  $DateTime: 2020/07/21 21:23:44 $ 
  $Author: pwbldsvc $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
03/31/2017  yh   LocMW clean up round 2
2015-06-11  sjk  LB 2.0 Integration 
2011-03-08  spn  Initial revision.


=============================================================================*/

/*****************************************************************************
 * Include Files
 * *************************************************************************/

#include "customer.h"
#include "comdef.h"

#include "loc_api_2.h"
#include "loc_api_internal.h"
#include "loc_batching.h"
#include "loc_client.h"
#include "loc_utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Preprocessor Definitions and Constants
 * *************************************************************************/


/*****************************************************************************
 * Type Declarations
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
 @brief 
  Structure with information that the client needs to provide in case an 
  indication is to be generated for a client of QMI-LOC service to receive

@warning
  The memory for p_Msg needs to be allocated by the caller. The caller will 
  need to free the memory. 
*//* ------------------------------------------------------------------------*/
typedef locIndInfoStructT locQmiShimIndInfoStructT;

/*****************************************************************************
 * Function Declarations and Documentation
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to initialize the glue layer, so it can attach itself
  with QCSI Framework and initialize the QMI-Loc Service.

@retval  TRUE  - Successful initialization of the module
@retval  FALSE - Module initialization was not successful
*//* ------------------------------------------------------------------------*/
boolean locQmiShimInit
( 
  void 
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to reset and disconnect the shim layer from the QCSI
  framework and de-register the QMI-Loc Service. After this function call, 
  the loc-middleware task will need to call locQmiShimInit() to be able to 
  communicate over the QMI-framework. 
*//* ------------------------------------------------------------------------*/
void locQmiShimCleanup 
( 
  void 
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is called by the Service owner indicating that there is a 
  QMI-Event that is queued up in QCSI that needs to be handled.
*//* ------------------------------------------------------------------------*/
void locQmiShimHandleEvent 
( 
  void 
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called by the module that intends to send out an 
  indication to the client of the QMI-LOC, Service. 

@return TRUE    The indication was successfully sent over QCSI
@return FALSE   The indication was not successfully sent over QCSI

@warning
  This function assumes that it is called in the context of the service 
  providers task. ( Loc-MW task )
*//* ------------------------------------------------------------------------*/
boolean locQmiShimSendInd
(
  loc_client_handle_type                  z_LocClientHandle,
  const locQmiShimIndInfoStructT         *pz_IndInfo
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called by the module that intends to broadcast an 
  indication to multiple clients of the QMI-LOC, Service. 

@return TRUE    The indication was successfully sent over QCSI
@return FALSE   The indication was not successfully sent over QCSI

@warning
  This function assumes that it is called in the context of the service 
  providers task. ( Loc-MW task )
*//* ------------------------------------------------------------------------*/
boolean locQmiShimBroadcastInd
(
   uint64                           t_EventMask,
   const locQmiShimIndInfoStructT*  pz_IndInfo
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is called by loc_initialize_modules to read the NV items. 
*/
/* ------------------------------------------------------------------------*/
void locQmiShimNvInit(void );

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC Indication for sub-components' commands

@param[in]   l_ClientHandle        Loc MW client handle 
@param[in]   q_QmiIndMsgId         QMI_LOC Msg ID
@param[in]   p_QmiIndMsg           QMI_LOC IND Msg
@param[in]   e_QmiIndMsgStatus     QMI_LOC status

@retval    TRUE           success
@retval    FALSE          failure
---------------------------------------------------------------------------*/
boolean locQmi_ProcessInd
(
   loc_client_handle_type       l_ClientHandle,
   uint32                       q_QmiIndMsgId,
   void*                        p_QmiIndMsg,
   qmiLocStatusEnumT_v02        e_QmiIndMsgStatus
);

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC Indication for PA GET commands

@param[in]   l_ClientHandle       Loc MW client handle 
@param[in]   e_CmdType            PDSM command type
@param[in]   p_CmdInfo            Loc_PA data pointer
@param[in]   e_IndStatus          Indication status
                                   
@retval    TRUE           success
@retval    FALSE          failure
---------------------------------------------------------------------------*/
boolean locQmi_ProcessPaGetInd
(
   loc_client_handle_type l_ClientHandle,
   pdsm_lite_pa_e_type         e_ParamType,
   pdsm_lite_pa_info_s_type*   p_PdsmPaInfo,
   qmiLocStatusEnumT_v02  e_IndStatus
);

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC Indication for PA SET commands

@param[in]   l_ClientHandle       Loc MW client handle 
@param[in]   e_CmdType            PDSM command type
@param[in]   p_CmdInfo            Loc_PA data pointer
@param[in]   e_IndStatus          Indication status
                                   
@retval    TRUE           success
@retval    FALSE          failure
---------------------------------------------------------------------------*/
boolean locQmi_ProcessPaSetInd
(
  loc_client_handle_type l_ClientHandle,
  pdsm_lite_pa_e_type         e_ParamType,
  pdsm_lite_pa_info_s_type*   p_PdsmPaInfo,
  qmiLocStatusEnumT_v02  e_IndStatus
);

/*---------------------------------------------------------------------------
@brief
  Function to process cmd request coming from the location middleware task

@param[in] pz_Cmd : LocMW command data
---------------------------------------------------------------------------*/
void locQmi_ProcessCmdRequest 
(
   
);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef LOC_QMI_SHIM_H */

