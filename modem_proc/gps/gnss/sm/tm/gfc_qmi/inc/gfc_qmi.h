/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GFC_QMI Interface to PDAPI Header file.

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

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/gfc_qmi/inc/gfc_qmi.h#2 $
$DateTime: 2020/12/21 22:57:28 $

=============================================================================

EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
11/15/11   ssu     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#ifndef GFC_QMI_H
#define GFC_QMI_H

#include "pdapi_lite.h"
#include "sm_api.h"

/*
******************************************************************************
* tm_gfc_qmi_prtl_init
*
* Function description:
*
*   This function initialises the GFC_QMI module . This sets up the internal
*   states of the GFC_QMI and registers with GM core.
*
* Parameters: None.
*   
* Return value: None.
*
******************************************************************************
*/
void tm_gfc_qmi_prtl_init(void);

/*
******************************************************************************
* tm_gfc_qmi_inject_ext_pos
*
* Function description:
*
*   This function handles Ext position injection
*
* Parameters: .
*   sm_FixInfoStructType* : Pointer to Fix Rpt
* Return value: None.
*
******************************************************************************
*/
void tm_gfc_qmi_inject_ext_pos(sm_FixInfoStructType* p_fix_rpt);


/*
******************************************************************************
* tm_gfc_qmi_prtl_deinit
*
* Function description:
*
*   This function de-initialises the GFC_QMI module . This is used to free up the 
*   allocated memory and clean up the states
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/

void tm_gfc_qmi_prtl_deinit(void);

/*===========================================================================
FUNCTION gfc_qmi_internal_handle_gtp_1_0_fix

DESCRIPTION
This function handles GTP 1.0 fix from TM CORE

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE

===========================================================================*/
void gfc_qmi_internal_handle_gtp_1_0_fix(const sm_FixInfoStructType *p_gnss_fix_rpt);

/*===========================================================================
FUNCTION gfc_qmi_internal_handle_gtp_2_0_fix

DESCRIPTION
This function handles GTP 2.0 fix from TM CORE

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE

===========================================================================*/
void gfc_qmi_internal_handle_gtp_2_0_fix(const sm_FixInfoStructType *p_gnss_fix_rpt);


#endif /* GFC_QMI_H */
