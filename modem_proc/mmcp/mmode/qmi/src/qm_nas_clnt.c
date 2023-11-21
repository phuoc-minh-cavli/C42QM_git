/*===========================================================================

                        QM_NAS_CLNT.C 

DESCRIPTION

  QMI_MMODE source file for nas client information module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:
  

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/13    sm     Initial version
===========================================================================*/

#include <math.h>
#include "qmi_mmode_svc.h"
#include "qm_nas_clnt.h"
#include "qm_util.h"
#include "qm_si.h"
#include "msg.h"
#include "qmi_nas_utils.h"

#ifndef FEATURE_MMODE_QMI_DISABLE_IMS_EXT
#define NASI_CMD_VAL_SET_E911_STATE                   0x007A
#define NASI_CMD_VAL_E911_STATE_READY_IND             0x007B
#endif

//===========================================================================
// Function declarations
//===========================================================================
static boolean is_cmd_buf_p_valid( qmi_mmode_qcsi_transaction_type *cmd_buf_p ); 

/*=========================================================================== 
  FUNCTION IS_CMD_BUF_P_VALID() 
 
  DESCRIPTION 
    Validates cmd_buf_p pointer 
 
  PARAMETERS 
    cmd_buf_p: pointer to command buffer for client 
 
  RETURN VALUE 
    TRUE if valid, FALSE otherwise 
 
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
static boolean is_cmd_buf_p_valid( qmi_mmode_qcsi_transaction_type *cmd_buf_p ) 
{ 
  return qmi_nas_validate_cmd_buf_p( cmd_buf_p, &qmi_nas_state );
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_RESET_INFO()

  DESCRIPTION
    This function clears out client specific information for the specified clid

  PARAMETERS
    clid: client to reset information for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_reset_info ( qmi_nasi_client_state_type *cl_sp )
{

  if ( cl_sp != NULL )
  {
    QM_MSG_HIGH_1("Clearing client config information for clnt %p", cl_sp->clnt);
    memset( &(cl_sp->clnt_config_sig_info), 0, sizeof( qm_nas_clnt_config_s_type ) );
  }
  else
  {
    QM_MSG_ERROR("Memory was not reset successfully to clnt_config_sig_info as it was NULL");
  }
}

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================
  FUNCTION QM_NAS_CLNT_PROCESS_CONFIG_SIG_INFO()

  DESCRIPTION
    This function sets the config_sig_info information for the specific client

  PARAMETERS
    clid: client to store config_sig_info information for
    config: information to store for client 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_process_config_sig_info( qmi_nasi_client_state_type *cl_sp, nas_config_sig_info_req_msg_v01 *config )
{
  qm_nas_clnt_config_s_type *clnt_cl_sp;
  uint32 i = 0;

  QM_MSG_HIGH_1("Storing config sig info for clnt %p", cl_sp->clnt);

  if ( config && cl_sp )
  {
    clnt_cl_sp = &cl_sp->clnt_config_sig_info;
    
    clnt_cl_sp->use_config_sig2 = FALSE;
    memset( clnt_cl_sp->config_sig2, 0, sizeof(struct config_sig2_s)*QM_SIG_TYPE_MAX*QM_RAT_MAX );

    if ( config->rssi_threshold_list_valid )
    {
      memset( clnt_cl_sp->config_sig[QM_SIG_TYPE_RSSI].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt_cl_sp->config_sig[QM_SIG_TYPE_RSSI].threshold_list_len = config->rssi_threshold_list_len;
      for ( i = 0; i < config->rssi_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig[QM_SIG_TYPE_RSSI].threshold_list[i] = (float)config->rssi_threshold_list[i];
      }
    }

    if ( config->ecio_threshold_list_valid )
    {
      memset( clnt_cl_sp->config_sig[QM_SIG_TYPE_ECIO].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt_cl_sp->config_sig[QM_SIG_TYPE_ECIO].threshold_list_len = config->ecio_threshold_list_len;
      for ( i = 0; i < config->ecio_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig[QM_SIG_TYPE_ECIO].threshold_list[i] = (float)(config->ecio_threshold_list[i]*-0.5);
      }
    }

    #if defined(FEATURE_HDR)
    if ( config->hdr_sinr_threshold_list_valid )
    {
      memset( clnt_cl_sp->config_sig[QM_SIG_TYPE_SINR].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt_cl_sp->config_sig[QM_SIG_TYPE_SINR].threshold_list_len = config->hdr_sinr_threshold_list_len;
      for ( i = 0; i < config->hdr_sinr_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig[QM_SIG_TYPE_SINR].threshold_list[i] = (float)config->hdr_sinr_threshold_list[i];
      }
    }
    #endif

    if ( config->lte_snr_threshold_list_valid )
    {
      memset( clnt_cl_sp->config_sig[QM_SIG_TYPE_SNR].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt_cl_sp->config_sig[QM_SIG_TYPE_SNR].threshold_list_len = config->lte_snr_threshold_list_len;
      for ( i = 0; i < config->lte_snr_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig[QM_SIG_TYPE_SNR].threshold_list[i] = (float)(config->lte_snr_threshold_list[i]/10.0);
      }
    }

    if ( config->io_threshold_list_valid )
    {
      memset( clnt_cl_sp->config_sig[QM_SIG_TYPE_IO].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt_cl_sp->config_sig[QM_SIG_TYPE_IO].threshold_list_len = config->io_threshold_list_len;
      for ( i = 0; i < config->io_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig[QM_SIG_TYPE_IO].threshold_list[i] = (float)config->io_threshold_list[i];
      }
    }

    if ( config->lte_rsrq_threshold_list_valid )
    {
      memset( clnt_cl_sp->config_sig[QM_SIG_TYPE_RSRQ].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt_cl_sp->config_sig[QM_SIG_TYPE_RSRQ].threshold_list_len = config->lte_rsrq_threshold_list_len;
      for ( i = 0; i < config->lte_rsrq_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig[QM_SIG_TYPE_RSRQ].threshold_list[i] = (float)config->lte_rsrq_threshold_list[i];
      }
    }

    if ( config->lte_rsrp_threshold_list_valid )
    {
      memset( clnt_cl_sp->config_sig[QM_SIG_TYPE_RSRP].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt_cl_sp->config_sig[QM_SIG_TYPE_RSRP].threshold_list_len = config->lte_rsrp_threshold_list_len;
      for ( i = 0; i < config->lte_rsrp_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig[QM_SIG_TYPE_RSRP].threshold_list[i] = (float)config->lte_rsrp_threshold_list[i];
      }
    }

    #ifdef FEATURE_WCDMA
    if ( config->rscp_threshold_list_valid )
    {
      memset( clnt_cl_sp->config_sig[QM_SIG_TYPE_RSCP].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt_cl_sp->config_sig[QM_SIG_TYPE_RSCP].threshold_list_len = config->rscp_threshold_list_len;
      for ( i = 0; i < config->rscp_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig[QM_SIG_TYPE_RSCP].threshold_list[i] = (float)config->rscp_threshold_list[i];
      }
    }
    #endif

    #if defined(FEATURE_TDSCDMA)
    if ( config->tds_sinr_threshold_list_valid )
    {
      memset( clnt_cl_sp->tds_sinr_config.threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt_cl_sp->tds_sinr_config.threshold_list_len = config->tds_sinr_threshold_list_len;
      for ( i = 0; i < config->tds_sinr_threshold_list_len; i++ )
      {
        clnt_cl_sp->tds_sinr_config.threshold_list[i] = (float)config->tds_sinr_threshold_list[i];
      }
    }
    #endif
  }
}
#endif
/*===========================================================================
  FUNCTION QM_NAS_CLNT_PROCESS_CONFIG_SIG_INFO2()

  DESCRIPTION
    This function sets the config_sig_info information for the specific client

  PARAMETERS
    clid: client to store config_sig_info information for
    config: information to store for client 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_process_config_sig_info2( qmi_nasi_client_state_type *cl_sp, nas_config_sig_info2_req_msg_v01 *config )
{
  qm_nas_clnt_config_s_type *clnt_cl_sp;
  uint32 i = 0;


  if ( config != NULL && cl_sp != NULL )
  {
    QM_MSG_HIGH_1("Storing config sig info 2 for clnt %p", cl_sp->clnt);
    clnt_cl_sp = &cl_sp->clnt_config_sig_info;
    
    clnt_cl_sp->use_config_sig2 = TRUE;
    #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
    memset( clnt_cl_sp->config_sig, 0, sizeof(struct config_sig_s)*QM_SIG_TYPE_MAX );
    #endif
    #if defined(FEATURE_TDSCDMA)
    memset( &clnt_cl_sp->tds_sinr_config, 0, sizeof(struct config_sig_s) );
    #endif

    // CDMA RSSI
    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
    if ( config->cdma_rssi_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list_len = config->cdma_rssi_threshold_list_len;
      for ( i = 0; i < config->cdma_rssi_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list[i] = (float)config->cdma_rssi_threshold_list[i]/10;
      }
    }

    if ( config->cdma_rssi_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].delta = (float)config->cdma_rssi_delta/10;
    }

    // CDMA ECIO
    if ( config->cdma_ecio_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list_len = config->cdma_ecio_threshold_list_len;
      for ( i = 0; i < config->cdma_ecio_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list[i] = (float)config->cdma_ecio_threshold_list[i]/10;
      }
    }

    if ( config->cdma_ecio_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].delta = (float)config->cdma_ecio_delta/10;
    }
    #endif

    #if defined(FEATURE_HDR)
    // HDR RSSI
    if ( config->hdr_rssi_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list,0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list_len = config->hdr_rssi_threshold_list_len;
      for ( i = 0; i < config->hdr_rssi_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list[i] = (float)config->hdr_rssi_threshold_list[i]/10;
      }
    }

    if ( config->hdr_rssi_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].delta = (float)config->hdr_rssi_delta/10;
    }

    // HDR ECIO
    if ( config->hdr_ecio_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list_len = config->hdr_ecio_threshold_list_len;
      for ( i = 0; i < config->hdr_ecio_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list[i] = (float)config->hdr_ecio_threshold_list[i]/10;
      }
    }

    if ( config->hdr_ecio_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].delta = (float)config->hdr_ecio_delta/10;
    }

    // HDR SINR
    if ( config->hdr_sinr_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list_len = config->hdr_sinr_threshold_list_len;
      for ( i = 0; i < config->hdr_sinr_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list[i] = (float)config->hdr_sinr_threshold_list[i];
      }
    }

    if ( config->hdr_sinr_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].delta = (float)config->hdr_sinr_delta;
    }

    // HDR IO
    if ( config->hdr_io_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list_len = config->hdr_io_threshold_list_len;
      for ( i = 0; i < config->hdr_io_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list[i] = (float)config->hdr_io_threshold_list[i]/10;
      }
    }

    if ( config->hdr_io_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].delta = (float)config->hdr_io_delta/10;
    }
    #endif
    #if defined(FEATURE_GSM) 
    // GSM RSSI
    if ( config->gsm_rssi_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list,0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list_len = config->gsm_rssi_threshold_list_len;
      for ( i = 0; i < config->gsm_rssi_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list[i] = (float)config->gsm_rssi_threshold_list[i]/10;
      }
    }

    if ( config->gsm_rssi_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].delta = (float)config->gsm_rssi_delta/10;
    }
    #endif

    #if defined(FEATURE_WCDMA)
    // WCDMA RSSI
    if ( config->wcdma_rssi_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list,0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list_len = config->wcdma_rssi_threshold_list_len;
      for ( i = 0; i < config->wcdma_rssi_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list[i] = (float)config->wcdma_rssi_threshold_list[i]/10;
      }
    }

    if ( config->wcdma_rssi_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].delta = (float)config->wcdma_rssi_delta/10;
    }

    // WCDMA ECIO
    if ( config->wcdma_ecio_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list_len = config->wcdma_ecio_threshold_list_len;
      for ( i = 0; i < config->wcdma_ecio_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list[i] = (float)config->wcdma_ecio_threshold_list[i]/10;
      }
    }

    if ( config->wcdma_ecio_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].delta = (float)config->wcdma_ecio_delta/10;
    }
    #endif

    #if defined(FEATURE_LTE)
    // LTE RSSI
    if ( config->lte_rssi_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list,0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list_len = config->lte_rssi_threshold_list_len;
      for ( i = 0; i < config->lte_rssi_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list[i] = (float)config->lte_rssi_threshold_list[i]/10;
      }
    }

    if ( config->lte_rssi_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].delta = (float)config->lte_rssi_delta/10;
    }

    // LTE SNR
    if ( config->lte_snr_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list_len = config->lte_snr_threshold_list_len;
      for ( i = 0; i < config->lte_snr_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list[i] = (float)config->lte_snr_threshold_list[i]/10;
      }
    }

    if ( config->lte_snr_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].delta = (float)config->lte_snr_delta/10;
    }

    // LTE RSRQ
    if ( config->lte_rsrq_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list_len = config->lte_rsrq_threshold_list_len;
      for ( i = 0; i < config->lte_rsrq_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list[i] = (float)config->lte_rsrq_threshold_list[i]/10;
      }
    }

    if ( config->lte_rsrq_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].delta = (float)config->lte_rsrq_delta/10;
    }

    // LTE RSRP
    if ( config->lte_rsrp_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list_len = config->lte_rsrp_threshold_list_len;
      for ( i = 0; i < config->lte_rsrp_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list[i] = (float)config->lte_rsrp_threshold_list[i]/10;
      }
    }

    if ( config->lte_rsrp_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].delta = (float)config->lte_rsrp_delta/10;
    }
    #endif

    #if defined(FEATURE_TDSCDMA)
    // TDS RSCP
    if ( config->tdscdma_rscp_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].delta = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list_len = config->tdscdma_rscp_threshold_list_len;
      for ( i = 0; i < config->tdscdma_rscp_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list[i] = (float)config->tdscdma_rscp_threshold_list[i]/10;
      }
    }

    if ( config->tdscdma_rscp_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list_len = 0;

      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].delta = (float)config->tdscdma_rscp_delta/10;
    }

    // TDS RSSI
    if ( config->tds_rssi_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].delta = 0;
  
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list_len = config->tds_rssi_threshold_list_len;
      for ( i = 0; i < config->tds_rssi_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list[i] = (float)config->tds_rssi_threshold_list[i];
      }
    }

    if ( config->tdscdma_rssi_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list_len = 0;
  
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].delta = (float)config->tdscdma_rssi_delta;
    }

    // TDS ECIO
    if ( config->tds_ecio_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].delta = 0;
  
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list_len = config->tds_ecio_threshold_list_len;
      for ( i = 0; i < config->tds_ecio_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list[i] = (float)config->tds_ecio_threshold_list[i];
      }
    }

    if ( config->tdscdma_ecio_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list_len = 0;
  
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].delta = (float)config->tdscdma_ecio_delta;
    }

    // TDS SINR
    if ( config->tds_sinr_threshold_list_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].delta = 0;
  
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list_len = config->tds_sinr_threshold_list_len;
      for ( i = 0; i < config->tds_sinr_threshold_list_len; i++ )
      {
        clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list[i] = (float)config->tds_sinr_threshold_list[i];
      }
    }

    if ( config->tdscdma_sinr_delta_valid )
    {
      memset(clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list_len = 0;
  
      clnt_cl_sp->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].delta = (float)config->tdscdma_sinr_delta;
    }
    #endif
  }
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_GET_BINDING()

  DESCRIPTION
    This function retrieves the subscription for the given client.

  PARAMETERS
    cl_sp: client to query bound subscription for.

  RETURN VALUE
    enum qm_subs_e: bound subscription for a client, QM_SUBS_MIN if error.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qm_subs_e qm_nas_clnt_get_binding( qmi_nasi_client_state_type *cl_sp)
{
  enum qm_subs_e subs_id = QM_SUBS_MIN;

  if ( cl_sp != NULL )
  {
    subs_id = qm_util_map_nas_subs_to_qm_subs(cl_sp->report_status.bound_subs);
  }

  return subs_id;
}

/*===========================================================================
  FUNCTION CROSSED_THRESHOLD_OR_DELTA()

  DESCRIPTION
    Static function to check if rssi threshold has been crossed.

  PARAMETERS
    clid: client ID to check
    rat: RAT to check thresholds for
    new_value: the new value
    old_value: the previous value

  RETURN VALUE
    boolean: whether new value crossed a threshold from old value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean crossed_threshold_or_delta( qmi_nasi_client_state_type* cl_sp, enum qm_rat_e rat, enum qm_sig_type_e type, 
                                           float new_value, float old_value )
{
  uint32 i;
  boolean ret_val = FALSE;
  qm_nas_clnt_config_s_type *clnt_cl_sp;

  if ( cl_sp )
  {
    clnt_cl_sp = &cl_sp->clnt_config_sig_info;

    #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
    if( !clnt_cl_sp->use_config_sig2 )
    {
      struct config_sig_s config_info;

      #if defined(FEATURE_TDSCDMA)
      if ( rat == QM_RAT_TDSCDMA && type == QM_SIG_TYPE_SINR )
      {
        config_info = clnt_cl_sp->tds_sinr_config;
      }
      else
      #endif
      {
        config_info = clnt_cl_sp->config_sig[type];
      }

      // compare thresholds only if the list exists
      if( config_info.threshold_list_len > 0 &&
          config_info.threshold_list_len <= MAX_NUM_SIG_INFO_THRESHOLDS)
      {
        for(i=0; i<config_info.threshold_list_len; i++)
        {
          // If new_value < threshold <= old_value
          if( ( new_value < config_info.threshold_list[i] ) &&
              ( config_info.threshold_list[i] <= old_value ) )
          {
            ret_val = TRUE;
            break;
          }

          // If old_value < threshold <= new_value
          if( (old_value < config_info.threshold_list[i] ) &&
              (config_info.threshold_list[i]  <= new_value ) )
          {
            ret_val = TRUE;
            break;
          }
        }
      }
    }
    else
    #endif
    if( clnt_cl_sp->use_config_sig2 )
    {
      if ( rat > QM_RAT_MIN && rat < QM_RAT_MAX )
      {
      struct config_sig2_s config2_info = clnt_cl_sp->config_sig2[type][rat];
      if( config2_info.threshold_list_len > 0 &&
          config2_info.threshold_list_len <= MAX_NUM_SIG_INFO2_THRESHOLDS)
      {
        for(i=0; i<config2_info.threshold_list_len; i++)
        {
          // If new_value < threshold <= old_value
          if( ( new_value < (config2_info.threshold_list[i]) ) &&
              ( (config2_info.threshold_list[i]) <= old_value ) )
          {
            ret_val = TRUE;
            break;
          }

          // If old_value < threshold <= new_value
          if( (old_value < (config2_info.threshold_list[i]) ) &&
              ((config2_info.threshold_list[i])  <= new_value ) )
          {
            ret_val = TRUE;
            break;
          }
        }
      }
      else if( config2_info.delta > 0 )
      {
        float diff;
        diff = new_value - old_value;

        diff = (diff < 0) ? (-1*diff) : diff;

        ret_val = ( diff >= config2_info.delta );
      }
    }

    }
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_CROSSED_THRESHOLD()

  DESCRIPTION
    This function determines if provided new value has crossed threshold or delta
    for specified client.

  PARAMETERS
    clid: client ID to check
    subscription: subscription this request is for
    rat: RAT to check thresholds for
    type: the measurement type to check threshold for
    new_value: the new value
    old_value: the previous value

  RETURN VALUE
    boolean: whether new value crossed a threshold from old value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_nas_clnt_crossed_threshold( qmi_nasi_client_state_type* cl_sp, enum qm_subs_e subscription,
                                       enum qm_rat_e rat, enum qm_sig_type_e type,
                                       float new_value, float old_value )
{
  boolean ret_val = FALSE;

  if ( cl_sp )
  {

    if( qm_util_map_nas_subs_to_qm_subs(cl_sp->report_status.bound_subs) == subscription &&
        cl_sp->report_status.report_sig_info )
    {
      if ( qm_si_is_rat_active( subscription, rat ) )
      {
        switch ( type )
        {
          case QM_SIG_TYPE_RSSI:
            if ( rat > QM_RAT_MIN && rat < QM_RAT_MAX )
            {
              ret_val = crossed_threshold_or_delta( cl_sp, rat, type, new_value, old_value );
            }
            break;

          #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) || defined(FEATURE_HDR) || defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)
          case QM_SIG_TYPE_ECIO:
            if ( rat == QM_RAT_1X || rat == QM_RAT_HDR || rat == QM_RAT_UMTS || rat == QM_RAT_TDSCDMA )
            {
              ret_val = crossed_threshold_or_delta( cl_sp, rat, type, new_value, old_value );
            }
            break;
          #endif
          #if defined(FEATURE_HDR) || defined(FEATURE_TDSCDMA)
          case QM_SIG_TYPE_SINR:
            if ( rat == QM_RAT_HDR || rat == QM_RAT_TDSCDMA )
            {
              ret_val = crossed_threshold_or_delta( cl_sp, rat, type, new_value, old_value );
            }
            break;
          #endif
          #if defined(FEATURE_HDR)
          case QM_SIG_TYPE_IO:
            if ( rat == QM_RAT_HDR )
            {
              ret_val = crossed_threshold_or_delta( cl_sp, rat, type, new_value, old_value );
            }
            break;
          #endif

          #if defined(FEATURE_LTE)
          case QM_SIG_TYPE_RSRQ:
          case QM_SIG_TYPE_RSRP:
          case QM_SIG_TYPE_SNR:
            if ( rat == QM_RAT_LTE )
            {
              ret_val = crossed_threshold_or_delta( cl_sp, rat, type, new_value, old_value );
            }
            break;
          #endif
          #if defined(FEATURE_TDSCDMA)
          case QM_SIG_TYPE_RSCP:
            if ( rat == QM_RAT_TDSCDMA )
            {
              ret_val = crossed_threshold_or_delta( cl_sp, rat, type, new_value, old_value );
            }
            break;
          #endif
          default:
            QM_MSG_HIGH_1("Unsupported type %d", type);
            break;
        }
      }
    }
  }

  return ret_val;
}

#ifndef FEATURE_MMODE_QMI_DISABLE_IMS_EXT
/*===========================================================================
  FUNCTION QM_NAS_CLNT_SEND_IND_ALL()

  DESCRIPTION
    This function calls the QMI Framework API to send out the NAS service
    indication to all client that are registered for the IND.

  PARAMETERS
    clid: client ID to send ind to
    cmd: Which indication is being sent
    ind: pointer to dsm item to send

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_send_ind_all( uint16 cmd, void *cstruct, enum qm_subs_e subs )
{
  qmi_nasi_client_state_type *      cl_sp = NULL;
  dsm_item_type                    *ind = NULL;

  cl_sp = (qmi_nasi_client_state_type *)q_check(&(qmi_nas_state.client_q));
  while ( cl_sp != NULL )
  {
    if ( cl_sp->report_status.bound_subs == (enum qmi_nas_subs_e)subs ) // TODO: move bound_subs into qm_nas_clnt
    {
      switch ( cmd )
      {
        case NASI_CMD_VAL_E911_STATE_READY_IND:
          if ( cl_sp->report_status.report_e911_state_ready )
          {
            ind = NULL;
            // no encoding procedure needed as payload is empty
            qmi_nasi_send_ind( cl_sp, NASI_CMD_VAL_E911_STATE_READY_IND, ind );
          }
          break;
  
        default:
          QM_MSG_ERROR_1("Not handled cmd %d to qm_nas_clnt_send_ind_all", cmd);
          break;
      }
    }

    cl_sp = q_next(&(qmi_nas_state.client_q), &(cl_sp->link));
  }
}
#endif

