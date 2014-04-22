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
/* $Header: //components/rel/geran.mpss/5.2.0/gcommon/src/geran_nv.c#4 $ */
/* $DateTime: 2019/06/20 01:08:29 $$Author: pwbldsvc $ */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   -------- ---------------------------------------------
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
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/


#define G2W_DSDS_TA_SUPPORT  0x01
#define G2W_TSTS_TA_SUPPORT  0x02
#define G2T_DSDS_TA_SUPPORT  0x04
#define G2T_TSTS_TA_SUPPORT  0x08
#define G2C_DSDS_TA_SUPPORT  0x10
#define G2C_TSTS_TA_SUPPORT  0x20
#define G2L_DSDS_TA_SUPPORT  0x40
#define G2L_TSTS_TA_SUPPORT  0x80

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Static Function Declarations
 *--------------------------------------------------------------------------*/

#ifdef FEATURE_MODEM_CONFIG_REFRESH
static gas_id_t geran_nv_refresh_map_sub_index_to_gas_id(mcfg_refresh_index_type sub_index);
static mcfg_refresh_index_type geran_nv_refresh_map_gas_id_to_sub_index(void);
static boolean geran_nv_refresh_validate_and_store_cb_params(mcfg_refresh_info_s_type *p_info);

#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*---------------------------------------------------------------------------
 * Function Definitions
 *--------------------------------------------------------------------------*/


/*!
 * \brief Wrapper API for mcfg_fs_read for reading EFS Items for primary subscription
 * - consistent with legacy efs_get API usage 
 * 
 * \return length of data read if successful else -1
 */
int geran_efs_read_primary(const char *path, void *data, uint32 length)
{
  mcfg_fs_status_e_type efs_status;
  
  efs_status = mcfg_fs_read(path,
                            data,
                            length,
                            MCFG_FS_TYPE_EFS,
                            MCFG_FS_SUBID_0); /* primary subscription */

  if (MCFG_FS_STATUS_OK == efs_status)
  {
    return (int)length;
  }
  else
  {
    MSG_GERAN_MED_1("EFS READ ERROR: status: %d ", (int)efs_status);
    return(-1);
  }
} /* geran_efs_read_primary */

/*!
 * \brief Wrapper API for mcfg_fs_read for reading EFS Items per subscription
 * 
 * \return length of data read if successful else -1
 */
int geran_efs_read_per_sub(const char *path, void *data, uint32 length)
{
  mcfg_fs_status_e_type efs_status;
  
  efs_status = mcfg_fs_read(path,
                            data,
                            length,
                            MCFG_FS_TYPE_EFS,
                            MCFG_FS_SUBID_0);

  if (MCFG_FS_STATUS_OK == efs_status)
  {
    return (int)length;
  }
  else
  {
    MSG_GERAN_MED_1("EFS READ ERROR: status: %d ", (int)efs_status);
    return(-1);
  }
}

/*!
 * \brief Wrapper API for mcfg_fs_write for writing EFS Items per subscription
 * 
 * \return length of data wrote if successful else -1
 */
int geran_efs_write_per_sub(const char *path, void *data, uint32 length)
{
  mcfg_fs_status_e_type efs_status;
  
  efs_status = mcfg_fs_write(path,
                            data,
                            length,
                            MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR | MCFG_FS_O_TRUNC | MCFG_FS_O_RDWR,
                            MCFG_FS_ALLPERMS,
                            MCFG_FS_TYPE_EFS,
                            MCFG_FS_SUBID_0);

  if (MCFG_FS_STATUS_OK == efs_status)
  {
    return (int)length;
  }
  else
  {
    MSG_GERAN_MED_1("EFS WRITE ERROR: status: %d ", (int)efs_status);
    return(-1);
  }
}

/*!
 * \brief Wrapper API for mcfg_fs_stat for getting the EFS item stats per subscription
 * 
 * \return boolean - TRUE if successful, FALSE otherwise
 */
boolean geran_efs_stat_per_sub(const char *path, struct fs_stat *data)
{
  mcfg_fs_status_e_type efs_status;
  
  efs_status = mcfg_fs_stat(path,
                            data,
                            MCFG_FS_TYPE_EFS,
                            MCFG_FS_SUBID_0);

  if (MCFG_FS_STATUS_OK == efs_status)
  {
    return TRUE;
  }
  else
  {
    MSG_GERAN_MED_1("EFS STAT ERROR: status: %d ", (int)efs_status);
    return FALSE;
  }
}


/*!
 * \brief Reads general GERAN EFS-NV items
 */
void geran_read_efs_nv_items(void)
{
#ifdef FEATURE_DUAL_SIM
  uint8 default_rf_chain;
  uint8 g2x_ta_support;

  /* -1 indicates file not present */
  geran_nv_data.default_rf_chain = -1;

  /* If no NV item file is present, G2W/G2T/G21x TA is enabled for DSDS */
  geran_nv_data.g2x_ta_support = (G2W_DSDS_TA_SUPPORT | G2T_DSDS_TA_SUPPORT | G2C_DSDS_TA_SUPPORT);

  if (geran_efs_read_primary(GERAN_EFS_DEF_RF_CHAIN,
                             &default_rf_chain,
                             sizeof(default_rf_chain)) > 0)
  {
    if ((default_rf_chain == 0) || (default_rf_chain == 1))
    {
      MSG_GERAN_HIGH_1("default_rf_chain=%d", default_rf_chain);
      geran_nv_data.default_rf_chain = default_rf_chain;
    }
  }

  if (geran_efs_read_primary(GERAN_EFS_G2X_TA_SUPPORT,
                             &g2x_ta_support,
                             sizeof(g2x_ta_support)) > 0)
  {
    MSG_GERAN_HIGH_1("G2X tuneaway support 0x%02X", g2x_ta_support);
    geran_nv_data.g2x_ta_support = g2x_ta_support;
  }
#endif /* FEATURE_DUAL_SIM */

  geran_nv_data.valid = TRUE;

  return;
}

#ifdef FEATURE_DUAL_SIM
/*!
 * \brief Returns the value of EFS-NV item default_rf_chain.
 * 
 * \return int - {0,1} or -1 if not defined
 */
int geran_nv_get_default_rf_chain(void)
{
  if (geran_nv_data.valid)
  {
    return geran_nv_data.default_rf_chain;
  }

  return -1;
}
#endif /* FEATURE_DUAL_SIM */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
* \brief Returns the GPRS->WCDMA tuneaway support based on the values of EFS-NV items.
 * 
 * \return int - {GERAN_TA_SUPPORTED, GERAN_TA_NOT_SUPPORTED, GERAN_NV_MEM_NOT_VALID} 
 */
int geran_nv_g2w_ta_supported(void)
{
  sys_modem_dual_standby_pref_e_type mode_pref = geran_get_sys_multi_sim_pref();

  if (geran_nv_data.valid)
  {
    if (SYS_MODEM_DS_PREF_DUAL_STANDBY == mode_pref)
    {
      return((geran_nv_data.g2x_ta_support & G2W_DSDS_TA_SUPPORT)?GERAN_TA_SUPPORTED:GERAN_TA_NOT_SUPPORTED);
    }
    else if (SYS_MODEM_DS_PREF_TRIPLE_STANDBY == mode_pref)
    {
      return((geran_nv_data.g2x_ta_support & G2W_TSTS_TA_SUPPORT)?GERAN_TA_SUPPORTED:GERAN_TA_NOT_SUPPORTED);
    }
    else
    {
      return(GERAN_TA_NOT_SUPPORTED);
    }
  }

  return(GERAN_NV_MEM_NOT_VALID);
}
/*!
 * \brief Returns the GPRS->TDSCDMA tuneaway support based on the values of EFS-NV items.
 * 
 * \return int - {GERAN_TA_SUPPORTED, GERAN_TA_NOT_SUPPORTED, GERAN_NV_MEM_NOT_VALID} 
 */
int geran_nv_g2t_ta_supported(void)
{
  sys_modem_dual_standby_pref_e_type mode_pref = geran_get_sys_multi_sim_pref();

  if (geran_nv_data.valid)
  {
    if (SYS_MODEM_DS_PREF_DUAL_STANDBY == mode_pref)
    {
      return((geran_nv_data.g2x_ta_support & G2T_DSDS_TA_SUPPORT)?GERAN_TA_SUPPORTED:GERAN_TA_NOT_SUPPORTED);
    }
    else if (SYS_MODEM_DS_PREF_TRIPLE_STANDBY == mode_pref)
    {
      return((geran_nv_data.g2x_ta_support & G2T_TSTS_TA_SUPPORT)?GERAN_TA_SUPPORTED:GERAN_TA_NOT_SUPPORTED);
    }
    else
    {
      return(GERAN_TA_NOT_SUPPORTED);
    }
  }

  return(GERAN_NV_MEM_NOT_VALID);
}

/*!
 * \brief Returns the GPRS->CDMA tuneaway support based on the values of EFS-NV items.
 * 
 * \return int - {GERAN_TA_SUPPORTED, GERAN_TA_NOT_SUPPORTED, GERAN_NV_MEM_NOT_VALID} 
 */
int geran_nv_g21x_ta_supported(void)
{
  sys_modem_dual_standby_pref_e_type mode_pref = geran_get_sys_multi_sim_pref();

  if (geran_nv_data.valid)
  {
    if (SYS_MODEM_DS_PREF_DUAL_STANDBY == mode_pref)
    {
      return((geran_nv_data.g2x_ta_support & G2C_DSDS_TA_SUPPORT)?GERAN_TA_SUPPORTED:GERAN_TA_NOT_SUPPORTED);
    }
    else if (SYS_MODEM_DS_PREF_TRIPLE_STANDBY == mode_pref)
    {
      return((geran_nv_data.g2x_ta_support & G2C_TSTS_TA_SUPPORT)?GERAN_TA_SUPPORTED:GERAN_TA_NOT_SUPPORTED);
    }
    else
    {
      return(GERAN_TA_NOT_SUPPORTED);
    }
  }

  return(GERAN_NV_MEM_NOT_VALID);
}

/*!
 * \brief Returns the GPRS->LTE tuneaway support based on the values of EFS-NV items.
 * 
 * \return int - {GERAN_TA_SUPPORTED, GERAN_TA_NOT_SUPPORTED, GERAN_NV_MEM_NOT_VALID} 
 */
int geran_nv_g2l_ta_supported(void)
{
  sys_modem_dual_standby_pref_e_type mode_pref = geran_get_sys_multi_sim_pref();

  if (geran_nv_data.valid)
  {
    if (SYS_MODEM_DS_PREF_DUAL_STANDBY == mode_pref)
    {
      return((geran_nv_data.g2x_ta_support & G2L_DSDS_TA_SUPPORT)?GERAN_TA_SUPPORTED:GERAN_TA_NOT_SUPPORTED);
    }
    else if (SYS_MODEM_DS_PREF_TRIPLE_STANDBY == mode_pref)
    {
      return((geran_nv_data.g2x_ta_support & G2L_TSTS_TA_SUPPORT)?GERAN_TA_SUPPORTED:GERAN_TA_NOT_SUPPORTED);
    }
    else
    {
      return(GERAN_TA_NOT_SUPPORTED);
    }
  }

  return(GERAN_NV_MEM_NOT_VALID);
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
#define SI9_CONTENT_TYPE_MAX 3

const byte si9_data[SI9_CONTENT_TYPE_MAX][N201_MAX] = {
  /* SI9 does not contain SI21 schedule info */
  {0x49, 0x06, 0x04, 0xd9, 0x00, 0x00, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b},
  /* SI9 contains SI21 schedule info */
  {0x49, 0x06, 0x04, 0xd9, 0x00, 0x00, 0xd0, 0xae, 0x1e, 0xb7, 0xfb, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b},
  /* SI9 contains multiple SI21 schedule info */
  {0x49, 0x06, 0x04, 0xd9, 0x00, 0x00, 0xd0, 0xae, 0x1e, 0xb7, 0xfe, 0x81, 0x55, 0x22, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b},
};

void geran_nv_get_SI9_content(uint8* l3_msg_ptr)
{
  if (l3_msg_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("Null l3_msg_ptr");
    return;
  }

  MSG_GERAN_HIGH_1_G("Get SI9 Content: type=%d", geran_nv_data.si9_content_type);
  if (geran_nv_data.si9_content_type < SI9_CONTENT_TYPE_MAX)
  {
    memscpy(l3_msg_ptr, N201_MAX, si9_data[geran_nv_data.si9_content_type], N201_MAX);
    return;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Content not available");
    return;
  }
}

void geran_nv_update_SI9_content_type(uint8 content_type)
{
  geran_nv_data.si9_content_type = content_type;
  MSG_GERAN_HIGH_1_G("SI9 Content Type updated (%d)", content_type);
}

#define SI21_CONTENT_TYPE_MAX 6

const byte si21_data[SI21_CONTENT_TYPE_MAX][N201_MAX] = {
  /* SI21 does not contain EAB info */
  {0x49, 0x06, 0x46, 0x00, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b},
  /* SI21 contains EAB info, no access class is barred */
  {0x49, 0x06, 0x46, 0x00, 0x80, 0x03, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b},
  /* SI21 contains EAB info, all access class are barred, subcategory 0 */
  {0x49, 0x06, 0x46, 0x00, 0xff, 0xe3, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b},
  /* SI21 contains EAB info, all access class are barred, subcategory 1 */
  {0x49, 0x06, 0x46, 0x00, 0xff, 0xeb, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b},
  /* SI21 contains EAB info, all access class are barred, subcategory 2 */
  {0x49, 0x06, 0x46, 0x00, 0xff, 0xf3, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b},
  /* SI21 contains EAB info, all access class are barred, subcategory 3 (invalid value) */
  {0x49, 0x06, 0x46, 0x00, 0xff, 0xfb, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b}
};

void geran_nv_get_SI21_content(uint8* l3_msg_ptr)
{
  if (l3_msg_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("Null l3_msg_ptr");
    return;
  }

  MSG_GERAN_HIGH_1_G("Get SI21 Content: type=%d", geran_nv_data.si21_content_type);
  if (geran_nv_data.si21_content_type < SI21_CONTENT_TYPE_MAX)
  {
    memscpy(l3_msg_ptr, N201_MAX, si21_data[geran_nv_data.si21_content_type], N201_MAX);
    return;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Content not available");
    return;
  }
}

void geran_nv_update_SI21_content_type(uint8 content_type)
{
  geran_nv_data.si21_content_type = content_type;
  MSG_GERAN_HIGH_1_G("SI21 Content Type updated (%d)", content_type);
}
#endif /* FEATURE_GERAN_R13_TEST_FRAMEWORK */

#ifdef FEATURE_MODEM_CONFIG_REFRESH


/*!
 * \brief maps SUB index to GAS-ID.
 */
static gas_id_t geran_nv_refresh_map_sub_index_to_gas_id(mcfg_refresh_index_type sub_index)
{
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;

  switch (sub_index)
  {
    case MCFG_REFRESH_INDEX_0:
      as_id = SYS_MODEM_AS_ID_1;
      break;
      
    case MCFG_REFRESH_INDEX_1:
      as_id = SYS_MODEM_AS_ID_2;
      break;
      
    case MCFG_REFRESH_INDEX_2:
      as_id = SYS_MODEM_AS_ID_3;
      break;
      
    default:
      MSG_GERAN_ERROR_0("Invalid SUB Index received");
      break;
  }

   MSG_GERAN_HIGH_1("Mapping:: sub_index=%d ==> gas_id=1", sub_index);

  return GERAN_ACCESS_STRATUM_ID_1;
}


/*!
 * \brief maps GAS-ID to SUB index.
 */
static mcfg_refresh_index_type geran_nv_refresh_map_gas_id_to_sub_index(void )
{
  sys_modem_as_id_e_type  as_id;
  mcfg_refresh_index_type sub_index = MCFG_REFRESH_INDEX_INVALID;

  as_id = geran_map_gas_id_to_nas_id();

  switch (as_id)
  {
    case SYS_MODEM_AS_ID_1:
      sub_index = MCFG_REFRESH_INDEX_0;
      break;
      
    case SYS_MODEM_AS_ID_2:
      sub_index = MCFG_REFRESH_INDEX_1;
      break;
      
    case SYS_MODEM_AS_ID_3:
      sub_index = MCFG_REFRESH_INDEX_2;
      break;
      
    default:
      MSG_GERAN_ERROR_0("Invalid GAS ID received");
      break;
  }
  
  MSG_GERAN_HIGH_1("Mapping:: gas_id=1 ==> sub_index=%d", sub_index);

  return sub_index;
}


/*!
 * \brief validates params received from MCFG in call back function.
 * It store the params if they are valid.
 *
 *\param p_info(in) -contains sub to be refreshed and reference id
 *
 *\return boolean - TRUE: Params are valid and stored
 *                          FALSE: Invalid params received
 */
static boolean geran_nv_refresh_validate_and_store_cb_params(mcfg_refresh_info_s_type *p_info)
{
  geran_nv_refresh_data_t *geran_nv_refresh_data_ptr = geran_nv_refresh_get_data_ptr();
  boolean                  param_valid = TRUE;
  
  MSG_GERAN_HIGH_4("Rcvd params:: type=%d, slot=%d, sub=%d, ref_id=%d", 
    p_info->type, 
    p_info->slot_index, 
    p_info->sub_index, 
    p_info->reference_id);
  
  if ((p_info->type & geran_nv_refresh_data_ptr->reg_type) != 0)
  {
    switch (p_info->type)
    {
      case MCFG_REFRESH_TYPE_SUBS:
      {
        if ((p_info->sub_index <= MCFG_REFRESH_INDEX_MAX) &&
            (((1 << p_info->sub_index) & geran_nv_refresh_data_ptr->reg_sub_mask) != 0))
        {
          MSG_GERAN_HIGH_2("Storing params:: ref_id=%d for sub_index=%d", 
            p_info->reference_id, 
            p_info->sub_index);  
          geran_nv_refresh_data_ptr->ref_ids[p_info->sub_index] = p_info->reference_id;
        }
        else
        {
          param_valid = FALSE;
        }
        
        break;
      }

      /* Refresh based on sub_id is only supported in GERAN */
      default:
        param_valid = FALSE;       
        break;
    }
  }
  else
  {
    param_valid = FALSE;
  }

  return param_valid;
}


/*!
 * \brief Call back function for handling NV refresh.
 *
 *\param p_info(in) -contains sub to be refreshed and reference id
 *
 *\return boolean - TRUE: if refresh can be done
 *                          FALSE: error in received parameters
 */
 boolean geran_nv_refresh_cb(mcfg_refresh_info_s_type *p_info)
{

  if (p_info == NULL)
  {
    MSG_GERAN_ERROR_0("p_info is NULL in call back");
    return FALSE;
  }
  
  MSG_GERAN_HIGH_1("NV Refresh Call Back:: sub_index=%d", p_info->sub_index);
  
  /* Validate and store received parameters */
  if (!geran_nv_refresh_validate_and_store_cb_params(p_info))
  {
    MSG_GERAN_ERROR_0("Invalid params received in call back");
    return FALSE;
  }

  /* Post a message to GRR to refresh NV items */
   rr_nv_send_nv_refresh_ind();

  return TRUE;
}

#endif /* FEATURE_MODEM_CONFIG_REFRESH */


/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*!
 * \brief function for informing MCFG about refresh done. Called from RR task.
 */
void geran_nv_refresh_done(const boolean done)
{
  geran_nv_refresh_data_t *geran_nv_refresh_data_ptr = geran_nv_refresh_get_data_ptr();
  mcfg_refresh_index_type  sub_index = geran_nv_refresh_map_gas_id_to_sub_index();

  if (geran_nv_refresh_data_ptr == NULL)
  {
    ERR_GERAN_FATAL_0_G("geran_nv_refresh_data_ptr == NULL");
  }

  if (sub_index <= MCFG_REFRESH_INDEX_MAX)
  {
    uint32 reference_id = geran_nv_refresh_data_ptr->ref_ids[sub_index];
    
    MSG_GERAN_HIGH_3_G("NV Refresh Done:: sub_index=%d, ref_id=%d, done=%d", sub_index, reference_id, done);

    /* NV refresh is not done if SUB is ACTIVE */
    if (done)
    {
      mcfg_refresh_done_w_status(reference_id, MCFG_REFRESH_SUCCESS);
    }
    else
    {
      mcfg_refresh_done_w_status(reference_id, MCFG_REFRESH_STATUS_SUB_IS_ACTIVE);
    }
  }
}
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*!
 * \Function to check Ciphering logging enabled or not. 
 * 
 * \return boolean - By default TRUE if Test sim is configured, FALSE otherwise.
 */
boolean geran_ciphering_logging_enabled(void)
{
  sys_modem_as_id_e_type  as_id = SYS_MODEM_AS_ID_NONE;

  as_id = geran_map_gas_id_to_nas_id();

  /* Call UIM API "mmgsdi_is_test_mode_logging_enabled_for_subscription" is used to get whether security key logging is enabled. 
   * If enabled, logging of keys is allowed in the caller.
   * If disabled, If a test card is in use,it will always return TRUE.If it is a commercial card,
   * it will take into account the value in the protected NV item (/nv/item_files/modem/uim/mmgsdi/test_mode_logging_enabled".
   * Based on above efs value, it act accordingly. 
   */
  if (TRUE == mmgsdi_is_test_mode_logging_enabled_for_subscription(as_id))
  {
    return TRUE;
  }

  MSG_GERAN_HIGH_0("Ciphering related logging is disabled at UIM Level.");
  return FALSE;
}

/* EOF */

