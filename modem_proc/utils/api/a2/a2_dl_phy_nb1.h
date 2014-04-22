#ifndef LTE_A2_DL_PHY_NB1_H
#define LTE_A2_DL_PHY_NB1_H

/*!
  @file a2_dl_phy_nb1.h

  @brief
   The interface to the downlink phy portion of the a2 driver for LTE.

  The A2 driver will call the callback registered in
  a2_dl_phy_register_tb_cb() whenever data is recieved.  This callback
  can be used to notify the MAC task that data is available.  The available
  data can be fetched using a2_dl_phy_get_data()

  @ingroup a2_ext_interface
*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/utils.mpss/2.3.4/api/a2/a2_dl_phy_nb1.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/16   vt     CR1103363 Initial Checkin


===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <dsm_pool.h>


/*==============================================================================

  FUNCTION:  a2_dl_phy_get_data_nb1

==============================================================================*/
/*!
  @brief
  Used to get already received buffered downlink data sitting in DDR.

  This function will pack the already recieved data into a DSM item.

  The expected caller of this funciton is the MAC DL task.

  @return
    The DSM item having the data copied from DDR
*/
/*============================================================================*/

dsm_item_type* a2_dl_phy_get_data_nb1
(
  uint32 * trblk_addr,
  uint32   trblk_len_in_bytes
);




#endif /* LTE_A2_DL_PHY_NB1_H */

