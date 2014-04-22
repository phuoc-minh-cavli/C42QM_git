#ifndef _DS_QMI_NAS_MMGSDI_H_
#define _DS_QMI_NAS_MMGSDI_H_

/*===========================================================================

                         DS_QMI_NAS_MMGSDI.H

DESCRIPTION

 The Qualcomm Network Access Services MMGSDI Interface header file.

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qmi_nas_mmgsdi.h#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/03/10    hs     Initial version
===========================================================================*/
 
/*===========================================================================
  FUNCTION QMI_NAS_MMGSDI_EVT_CB()

  DESCRIPTION
    Callback function called by mmgsdi to report events

  PARAMETERS
    event  : Info about the event generated

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_mmgsdi_evt_cb( const mmgsdi_event_data_type *p_evt );

#endif // !_DS_QMI_NAS_MMGSDI_H_

