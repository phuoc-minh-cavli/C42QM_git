/*! \file geran_nv.c 
 
  @brief This module contains functionality related to GERAN NV items
    Create /nv/item_files/conf/geran.conf in EFS.
    The geran.conf file lists the EFS-NV items owned by GERAN.
    This file is used by tools such as QXDM to access the items, 
    and determines which items are included in QCN back-ups.
 
                Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/gcommon/static/src/geran_static_nv.c#3 $ */
/* $DateTime: 2020/11/25 06:33:21 $$Author: pwbldsvc $ */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   -------- ---------------------------------------------
23/11/20   sal      CR2821780: Added efs file path for NV74417.
04/10/18   kh		Geran compiler errors new features
11/09/15   dmb      Updates for EFS usage per subscription
dd/mm/yy   who      Sample text for edit history

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#include "mcfg_fs.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#include "geran_dual_sim.h"
#include "rr_nv_g.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
#include "geran_nv.h"
#include "string.h"
#include <stringl.h>
#include "fs_public.h"
#include "msg.h"
#include "gprs_mem.h"
#include "geran_multi_sim.h"
#include "mmgsdilib.h"
#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
#include "sys_cnst.h"
#endif /* FEATURE_GERAN_R13_TEST_FRAMEWORK */
#include "geran_nv.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  const char* str_ptr;
  uint8 str_len;
} item_info_t;


/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/
geran_nv_data_t geran_nv_data;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

const char* const geran_efs_nv_items[] =
{
 "/nv/item_files/gsm/gl1/search_all_w_cell",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_type_2_en",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_trafi_mdm_threshold",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_idle_mdm_threshold",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_trafi_sens",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_idle_sens",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_qsc_large_delta",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_qsc_small_delta",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_qsc_hysteriesis_time",
  "/nv/item_files/gsm/gl1/antenna_switch_tx_div_min_delta_btw_antenna",
  "/nv/item_files/gsm/gl1/antenna_switching_tx_diversity_threshold",
  "/nv/item_files/gsm/gl1/antenna_switching_tx_diversity_enable",
  "/nv/item_files/gsm/gl1/gsm_rx_diversity",
  "/nv/item_files/gsm/gl1/gsm_commercial_recovery_restart",
  "/nv/item_files/gsm/l1/l1_debug",
  "/nv/item_files/gsm/l1/l1_q6_clock",
  "/nv/item_files/modem/geran/gfw_debug",
  "/nv/item_files/modem/geran/vamos_support",
  "/nv/item_files/modem/geran/gfw_diag_group1",
  "/nv/item_files/modem/geran/gfw_diag_group2",
  "/nv/item_files/modem/geran/gfw_diag_group3",
  "/nv/item_files/modem/geran/gfw_diag_group4",
  "/nv/item_files/modem/geran/grr/g2l_blind_redir_control",
  "/nv/item_files/modem/geran/g2x_ta_support",
  "/nv/item_files/modem/geran/gfw_aeq_phase4_control",
#ifdef FEATURE_FSK_SUPPORT
  "/nv/item_files/modem/geran/fsk_feature_control",
#endif
  "/nv/item_files/gsm/gl1/lif_feature_control"
};


#ifdef FEATURE_MODEM_CONFIG_REFRESH
static geran_nv_refresh_data_t geran_nv_refresh_data;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*----------------------------------------------------------------------------
 * Static Function Declarations
 *--------------------------------------------------------------------------*/

#ifdef FEATURE_MODEM_CONFIG_REFRESH
static void geran_nv_refresh_register_cb(void);
static void geran_nv_refresh_init(void);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*---------------------------------------------------------------------------
 * Function Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Creates /nv/item_files/conf/geran.conf
 */
static void create_efs_nv_conf_file(void)
{
  int fd;     // File Descriptor

  // Create geran.conf if it doesn't exist
  fd = efs_open("/nv/item_files/conf/geran.conf",
                O_WRONLY | O_CREAT | O_EXCL,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  // A return of 0 or greater means success
  if (fd >= 0)
  {
    // Limit string lengths to 127
    #define MAX_STR_LEN 127

    // Limit number of items to 255
    #define MAX_NUM_OF_ITEMS 255

    // Number of items defined in geran_efs_nv_items.h
    #define NUM_OF_ITEMS (sizeof(geran_efs_nv_items) / sizeof(const char*))

    item_info_t *items_ptr;
    uint8 num_of_items;

    // Cap the number of items at MAX_NUM_OF_ITEMS
    num_of_items = MIN(NUM_OF_ITEMS, MAX_NUM_OF_ITEMS);

    // Allocate memory to store the information about the items
    items_ptr = GPRS_MEM_MALLOC(sizeof(item_info_t) * num_of_items);

    if (items_ptr != NULL)
    {
      int i;
      item_info_t *item_ptr;
      int result; // result of file operation
      uint16 file_len = 0;
      char *file_buffer_ptr;

      // Start at the beginning of the list of items
      item_ptr = items_ptr;

      for (i = 0; i < num_of_items; i++)
      {
        const char* str_ptr = geran_efs_nv_items[i]; // see geran_efs_nv_items.h
        uint8 str_len = strlen(str_ptr);  // find the length of the string

        // Cap the string length at MAX_STR_LEN
        str_len = MIN(str_len, MAX_STR_LEN);

        // Store a pointer to the text string and its length
        item_ptr->str_ptr = str_ptr;
        item_ptr->str_len = str_len;

        // Increase the total file length count by the size of the string
        file_len += str_len;

        // Add 2 because \r\n will be added to each string
        file_len += 2;

        // Point to next item
        ++item_ptr;
      }

      // Allocate a buffer to store the file contents
      file_buffer_ptr = (char*)GPRS_MEM_MALLOC(file_len);

      if (file_buffer_ptr != NULL)
      {
        char* file_buffer_write_ptr = file_buffer_ptr;

        // Start at the beginning of the list of items
        item_ptr = items_ptr;

        for (i = 0; i < num_of_items; i++)
        {
          // Copy the string in to the file contents buffer
          // Buffer overflow cannot occur as we have worked out above how much we're going
          // to write but use memscpy anyway so all use of memcpy is expunged
          memscpy(file_buffer_write_ptr, file_len, item_ptr->str_ptr, item_ptr->str_len);

          // Increase the file buffer write pointer by the length of the string just copied
          file_buffer_write_ptr += item_ptr->str_len;

          // Add \r\n and increase the file buffer write pointer by 2
          memscpy(file_buffer_write_ptr, 2, "\r\n", 2);
          file_buffer_write_ptr += 2;

          // Point to next item
          ++item_ptr;
        }

        // Write to the file
        result = efs_write(fd, (const void*)file_buffer_ptr, file_len);

        if (result <= 0)
        {
          MSG_GERAN_ERROR_1("Error (%d) writing geran.conf",result);
        }

        // Free allocated memory
        GPRS_MEM_FREE(file_buffer_ptr);
      }

      // Free allocated memory
      GPRS_MEM_FREE(items_ptr);
    }

    // Close the file
    (void) efs_close(fd);
  }

  return;
}


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*!
 * \brief return pointer to geran_nv_refresh_data.
 */
geran_nv_refresh_data_t *geran_nv_refresh_get_data_ptr(void)
{
  return &geran_nv_refresh_data;
}

/*!
 * \brief this function registers call back with MCFG for NV refresh.
 */
static void geran_nv_refresh_register_cb(void)
{
  mcfg_refresh_registration_s_type p_reg;
  geran_nv_refresh_data_t         *geran_nv_refresh_data_ptr = geran_nv_refresh_get_data_ptr();

  p_reg.order = geran_nv_refresh_data_ptr->reg_order;
  p_reg.slot_mask = geran_nv_refresh_data_ptr->reg_slot_mask;
  p_reg.sub_mask = geran_nv_refresh_data_ptr->reg_sub_mask;
  p_reg.group = geran_nv_refresh_data_ptr->reg_group;
  p_reg.cb = geran_nv_refresh_cb;

  MSG_GERAN_HIGH_4("NV Refresh Register cb:: order=%d, slot=%d, sub=%d, group=%d", 
    p_reg.order, 
    p_reg.slot_mask, 
    p_reg.sub_mask, 
    p_reg.group);  
  
  /* register call back with MCFG for NV refresh */
  mcfg_refresh_register(&p_reg);
}

/*!
 * \brief Initialisation function for NV refresh.
 */
static void geran_nv_refresh_init(void)
{
  MSG_GERAN_HIGH_0("NV Refresh Init");
  memset(&geran_nv_refresh_data, 0, sizeof(geran_nv_refresh_data));
  
  /* order: Default order (i.e. 50) */
  geran_nv_refresh_data.reg_order = MCFG_REFRESH_ORDER_50;

  /* Our refresh will be based on SUB and will not be based on SLOT.  */
  geran_nv_refresh_data.reg_type = MCFG_REFRESH_TYPE_SUBS;
  geran_nv_refresh_data.reg_slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  geran_nv_refresh_data.reg_sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;

  /* group: reserved*/
  geran_nv_refresh_data.reg_group = 0;
  
  /* register call back with MCFG for NV refresh */
  geran_nv_refresh_register_cb();
}
#endif

/*!
 * \brief Module initialisation function. Called during RR task initialisation.
 */
void geran_nv_init(void)
{
  // Zero the NV data structure
  memset(&geran_nv_data, 0, sizeof(geran_nv_data));

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  geran_nv_refresh_init();
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  // Create /nv/item_files/conf/geran.conf if it doesn't exist
  create_efs_nv_conf_file();
#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
  geran_nv_data.si9_content_type  = 0xFF;
  geran_nv_data.si21_content_type = 0xFF;
#endif /* FEATURE_GERAN_R13_TEST_FRAMEWORK */
}


