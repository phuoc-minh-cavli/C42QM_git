/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GFC_QMI module which is the interface between QMI and GM 

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/gfc_qmi/src/gfc_qmi_internal.h#1 $
$DateTime: 2020/07/21 21:23:44 $

=============================================================================

EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/29/12   ssu     Adding geofence purge request support.
11/15/11   ssu     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef GFC_QMI_INTERNAL_H
#define GFC_QMI_INTERNAL_H


#define NON_QMI_TRXN_ID 0xABABABAB

/* The data type is used to synchronously return the status of the Geofencing operation from GFC_QMI . */
typedef enum
{
  GFC_QMI_OP_TYPE_ADD_GF,
  GFC_QMI_OP_TYPE_DELETE_GF,
  GFC_QMI_OP_TYPE_PURGE_GF,
  GFC_QMI_OP_TYPE_EDIT_GF,
  GFC_QMI_OP_TYPE_QUERY_GF,
  GFC_QMI_OP_TYPE_QUERY_GF_IDS,
  GFC_QMI_OP_TYPE_SET_GF_ENGINE_CONFIG,
  GFC_QMI_OP_TYPE_SET_GF_CLIENT_CONFIG,  
  GFC_QMI_OP_TYPE_MAX = 0x1FFFFFFF
} gfc_qmi_op_type;


/*===========================================================================
gfc_qmi_internal_register_cbs

Description:
   This function is used by GFC_QMI to register the function callbacks with the QMI

Parameters:   
  p_geofence_func_cbs: The function callbacks from the QMI .

Return value: 
  FALSE: If the registration succeeds.
  TRUE: Otherwise. 
=============================================================================*/
boolean gfc_qmi_internal_register_cbs(
  const gfc_qmi_cb_rsp_s_type* p_geofence_func_cbs
);


/*
******************************************************************************
* gfc_qmi_internal_set_engine_config_at_bootup
*
* Function description:
*
*   This function is used to set the geofence engine configuration
*   at boot up time , by reading the required configuration items from NV.
*
* Parameters: None.
*   
* Return value: None.
*
******************************************************************************
*/
void gfc_qmi_internal_set_geofence_engine_config_at_bootup(void);

#endif // GFC_QMI_INTERNAL_H
