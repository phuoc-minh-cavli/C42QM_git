#ifndef RFLM_CCS_RESOURCES_H
#define RFLM_CCS_RESOURCES_H


/*
   @file
   rflm_resources.h

   @brief
   RFLM CCS Driver's interface for resources request from other clients.

   @details

*/

/*===========================================================================
Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfmodem_jolokia.mpss/1.10/api/ccs/rflm_ccs_resources.h#1 $
$DateTime: 2020/04/16 12:27:55 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
25/05/18   mns     MMU Changes
10/01/13   ra      Added support for CCS memory collapse 

==========================================================================*/


typedef struct
{
  uint32 mem_slp_cntl;    /*!< Retention fields in MSS_OFFLINE_MEM_SLP_CNTL */
  uint32 mem_slp_cntl2;  /*!< Retention fields in MSS_OFFLINE_MEM_SLP_CNTL2 */
  uint32 spare[2];              /*!< Future use */
} rfm_sleep_cfg_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
   rfm_ccs_wakeup. This function is called by MCPM_FW_Assert_Wakeup(mcpm_tech_type eTech)
   For initializing q6 bitmap of MMU.
  @param
   None

  @return
   void
*/
void rfm_ccs_wakeup(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
   rfm_ccs_wakeup. This function is called by MCPM_FW_Assert_Sleep(mcpm_tech_type eTech)
 
  @param
   None

  @return
   void
*/
void rfm_ccs_sleep(void);


void rfm_get_mcpm_rfi_sleep_control(rfm_sleep_cfg_t *);
#endif  /* RFLM_CCS_RESOURCES.H */
