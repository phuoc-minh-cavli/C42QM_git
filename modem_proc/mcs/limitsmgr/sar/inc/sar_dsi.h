#ifndef __SAR_DSI_H__
#define __SAR_DSI_H__

/*!
  @file
  	lmtsmgr_sar_dsi.h

  @brief
	Definitions of functions which allow SAR DSI to interface with QMI.
*/

/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/
/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcs.mpss/5.1/limitsmgr/sar/inc/sar_dsi.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/14   rj      Added support for GPIO polarity and BootUp BackOff
04/05/13   rj      GPIO Based SAR Control added
03/24/13   sk     support for diag command to set SAR DSI
08/14/12   ag     Initial implementation

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/


#include <qmi_csi.h>
#include "comdef.h"
#include "lmtsmgr_i.h"

#include "lmtsmgr_diag.h"

/* DAL APIs */
#include "DDITlmm.h"
#include "DalDevice.h"
#include "DALDeviceId.h"


/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/

#define IS_SAR_DSI_GPIO_VALUE_VALID(gpio_value) ((gpio_value == DAL_GPIO_LOW_VALUE) || \
                                                     (gpio_value == DAL_GPIO_HIGH_VALUE))

#define SWITCH_SAR_DSI_GPIO_VALUE(gpio_value) ((gpio_value == DAL_GPIO_LOW_VALUE) ? DAL_GPIO_HIGH_VALUE : DAL_GPIO_LOW_VALUE)

#define IS_SAR_DSI_VALUE_VALID(dsi) (((!sar_dsi.sar_gpio_dsi.gpio_efs.is_gpio_dsi_active)&& \
                                       ((dsi) <RF_SAR_STATE_MAX)) || \
                                     ((sar_dsi.sar_gpio_dsi.gpio_efs.is_gpio_dsi_active)&& \
                                      ((dsi) <RF_SAR_STATE_8)))

/*=============================================================================

                             TYPEDEFS

=============================================================================*/



/*=============================================================================

                      	FUNCTION PROTOTYPES

=============================================================================*/

/*=============================================================================

  FUNCTION:  sar_dsi_service_init

=============================================================================*/
/*!
    @brief
    	Initializes SAR DSI layer's functionalities
 
    @return
    	None
*/
/*===========================================================================*/

void sar_dsi_service_init (void);

/*=============================================================================

  FUNCTION:  sar_dsi_service_deinit

=============================================================================*/
/*!
    @brief
    	Cleans up/tears down the underlying transport layer functionalities
	connected to SAR

    @return
    	None
*/
/*===========================================================================*/

void sar_dsi_service_deinit (void);

/*=============================================================================

  FUNCTION:  sar_dsi_process_req

=============================================================================*/
/*!
    @brief
    	Callback function called by QCSI infrastructure when a REQ message to SAR 
			is received
 
    @note
    	QCSI infrastructure decodes the data before forwarding it to this layer
 
    @return
   	 qmi_csi_cb_error
*/
/*===========================================================================*/

qmi_csi_cb_error sar_dsi_process_req ( 
	void *, qmi_req_handle, unsigned int, void *, unsigned int, void *);

/*=============================================================================

  FUNCTION:  sar_get_current_dsi

=============================================================================*/
/*!
    @brief
    	Returns the current DSI
 
    @return
    	None
*/
/*===========================================================================*/
cxm_sar_dsi_type sar_get_current_dsi ( void );

/*=============================================================================

  FUNCTION:  sar_handle_set_dsi_ind

=============================================================================*/
/*!
    @brief
    Sets the current dsi value
 
    @return
    None
*/
/*===========================================================================*/
void sar_handle_set_dsi_ind
(
  void *sar_rcvd_msg 
);

/*=============================================================================

  FUNCTION:  sar_dsi_service_deinit

=============================================================================*/
/*!
    @brief
        De-Initializes SAR DSI layer's functionalities
 
    @return
        None
*/
/*===========================================================================*/
void sar_dsi_service_deinit 
(
  void
);

#endif /* __SAR_DSI_H__ */
