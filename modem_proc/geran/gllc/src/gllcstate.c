/*****************************************************************************
***
*** TITLE  gllcstate.c
***
***  GPRS LLC Layer State Management Functions
***
***
*** DESCRIPTION
***
***  The GPRS LLC State Manager is responsible for the state transition
***  control of the LLC module, for the LLME (Logical Link Management Entity)
***  and a number of LLEs (Logical Link Entities).
***
***  The state processing is scheduled by the occurance of LLC sub-events as
***  determined by the GPRS LLC Event Manager (gllc_hdlr.c). These events
***  may relate to the arrival of a message, the expiry of a timer, etc. The
***  Event Manager performs most of the preprocessing which is necessary to
***  present the event in a unified format for state transition processing.
***
***  All State + Event combinations are given representation to ensure a
***  predictable outcome. Many combinations however, result in some default
***  action, which may be in many cases to ignore the event. However, this
***  principle is key to attaining a robust architecture.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  gllc_llme_reset()
***  gllc_lle_reset()
***
***  gllc_llme_process_assign_req_mes()
***  gllc_llme_process_trigger_req_mes()
***  gllc_llme_process_suspend_req_mes()
***  gllc_llme_process_resume_req_mes()
***  gllc_llme_process_strt_rdy_tmr_req_mes()
***  gllc_llme_process_stop_rdy_tmr_req_mes()
***  gllc_llme_process_test_mode_req_mes()
***  gllc_llme_process_grr_status_ind_mes()
***
***  gllc_lle_process_est_req_mes()
***  gllc_lle_process_est_res_mes()
***  gllc_lle_process_rel_req_mes()
***  gllc_lle_process_act_ind_mes()
***  gllc_lle_process_deact_ind_mes()
***  gllc_lle_process_xid_req_mes()
***  gllc_lle_process_xid_res_mes()
***  gllc_lle_process_inact_sapi_res_mes()
***  gllc_lle_process_data_req_mes()
***  gllc_lle_process_unitdata_req_mes()
***
***  gllc_lle_process_i_s_frame()
***  gllc_lle_process_s_frame()
***  gllc_lle_process_ui_frame()
***  gllc_lle_process_u_frame()
***  gllc_lle_process_rej_frame()
***
***  gllc_lle_process_timer_t200_expiry()
***  gllc_lle_process_timer_t201_expiry()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
***
*** Copyright (c) 2001-2017 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/gllc/src/gllcstate.c#7 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

#include "comdef.h"
#include "geran_tasks.h"
#include "gprs_mem.h"
#include "gprs_pdu.h"
#include "gprsmod.h"
#include "gllc.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gllci.h"
#include "gllclog.h"
#include "gllsap.h"
#include "gmmllcif.h"
#include "mmtask.h"
#include "mmtask_v.h"
#include "geran_msgs.h"
#include "rex.h"
#include "stdlib.h"
#include "string.h"
#include "task.h"
#include "com_iei.h"
#include "gsndcp_v_g.h"
#include "secapi.h"
#include "geran_nv.h"
#include "gllc_static_main.h"

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
#include "rr_public_store.h"
#endif

#include "ds3gpp_api.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

#define FEATURE_DSM_WM_CB

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

/* Up link state process support function prototypes. */

LOCAL void gllc_lle_flush_retrx_buf( gllc_ll_sapi_t lle );
LOCAL void gllc_lle_flush_reseq_buf( gllc_ll_sapi_t lle );

LOCAL void gllc_lle_enter_state_abm( gllc_ll_sapi_t lle );
LOCAL void gllc_lle_enter_state_adm( gllc_ll_sapi_t lle );

LOCAL void gllc_llme_xid_reset_procedures( uint32 rxd_tlli );

LOCAL void gllc_lle_N_R_acknowledgement_procedures
(
  gllc_ll_sapi_t lle,
  uint16         recv_seq_num_N_R
);

LOCAL boolean gllc_lle_ACK_acknowledgement_procedures( gllc_ll_sapi_t lle );

LOCAL boolean gllc_lle_SACK_acknowledgement_procedures
(
  gllc_ll_sapi_t lle,
  uint8          *sack_bitmap_R,
  uint8          sack_bitmap_len_K
);

LOCAL void gllc_lle_peer_rcvr_busy_procedures( gllc_ll_sapi_t lle );

LOCAL void gllc_lle_peer_rcvr_busy_clearance_procedures
(
  gllc_ll_sapi_t lle
);

LOCAL void gllc_lle_initiate_reestablishment
(
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_req
);

LOCAL void gllc_lle_process_u_format_cr_xid_cmd
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         rxd_tlli
);

LOCAL void gllc_lle_process_u_format_cr_xid_res
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         rxd_tlli
);

LOCAL void gllc_lle_sabm_contention_procedures
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         rxd_tlli
);

LOCAL void gllc_lle_process_u_frame_in_unassigned
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
);

LOCAL void gllc_lle_process_u_frame_in_adm
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
);

LOCAL void gllc_lle_process_u_frame_in_loc_est
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
);

LOCAL void gllc_lle_process_u_frame_in_abm
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
);

LOCAL void gllc_lle_process_u_frame_in_loc_rel
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
);


/* Down link state process support function prototypes. */

LOCAL void gllc_lle_update_sack_bitmap_R( gllc_ll_sapi_t lle );

LOCAL void gllc_lle_resequencing_procedures
(
  gllc_ll_sapi_t    lle,
  gllc_i_s_frame_t  *i_s_frame,
  uint16            l3_pdu_length,
  uint32            rxd_tlli
);

LOCAL boolean gllc_lle_rcvd_empty_gmm_info_msg
(
  gllc_ll_sapi_t      lle,
  gllc_ui_frame_t     *ui_frame,
  uint16              l3_pdu_length
);

LOCAL boolean gllc_lle_ui_frame_is_duplicated
(
  gllc_ll_sapi_t      lle,
  gllc_ui_frame_t     *ui_frame
);

LOCAL void gllc_lle_update_unacknowledged_recv_state
(
  gllc_ll_sapi_t      lle,
  gllc_ui_frame_t     *ui_frame
);



/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/* ===========================================================================
   Logical Link Management Entity (LLME) state information and operating
   parameters.
   ======================================================================== */

gllc_llme_t gllc_llme;


/* ===========================================================================
   Logical Link Entities (LLEs) state information and operating parameters.
   ======================================================================== */

gllc_lle_t gllc_lle[ GLLC_LL_NUM_SAPIS ];


/* ---------------------------------------------------------------------------
   GMM Ready Timer restart timeout and current state.
   ------------------------------------------------------------------------ */

boolean gllc_gmm_ready_timer_active = FALSE;
uint32  gllc_gmm_ready_time         = 0;

/* ---------------------------------------------------------------------------
   Database to store PSM info. This database is written to the EFS during
   PSM entry and populated from EFS during task start.
   ------------------------------------------------------------------------ */
gllc_psm_info_t gllc_psm_info;

/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/

/* Utility macro to xlate cipher key Kc from uint8[8] to uint32[2] format. */

#define GLLC_LLME_XLATE_KC( in8_p, out32_p )                                 \
        {                                                                    \
          (out32_p)[0] = ( ((in8_p)[7] <<  0) | ((in8_p)[6] <<  8) |         \
                           ((in8_p)[5] << 16) | ((in8_p)[4] << 24)   );      \
                                                                             \
          (out32_p)[1] = ( ((in8_p)[3] <<  0) | ((in8_p)[2] <<  8) |         \
                           ((in8_p)[1] << 16) | ((in8_p)[0] << 24)   );      \
        }

/* Utility macro to xlate cipher key Kc128 from uint8[8] to uint32[4] format. */

#define GLLC_LLME_XLATE_KC128( in8_p, out32_p )                              \
        {                                                                    \
          (out32_p)[0] = ( ((in8_p)[15] <<  0) | ((in8_p)[14] <<  8) |       \
                           ((in8_p)[13] << 16) | ((in8_p)[12] << 24)   );    \
                                                                             \
          (out32_p)[1] = ( ((in8_p)[11] <<  0) | ((in8_p)[10] <<  8) |       \
                            ((in8_p)[9] << 16) |  ((in8_p)[8] << 24)   );    \
                                                                             \
          (out32_p)[2] = ( ((in8_p)[7] <<  0) | ((in8_p)[6] <<  8) |         \
                           ((in8_p)[5] << 16) | ((in8_p)[4] << 24)   );      \
                                                                             \
          (out32_p)[3] = ( ((in8_p)[3] <<  0) | ((in8_p)[2] <<  8) |         \
                           ((in8_p)[1] << 16) | ((in8_p)[0] << 24)   );      \
        }


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_DUMP_PSM_INFO()
===
===  DESCRIPTION
===
===   Prints PSM info.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
LOCAL void gllc_dump_psm_info(void)
{
  uint8 sapi;

  MSG_GERAN_LOW_2_G("PSM dump LLME:: llc_version=%d, IOV_UI=%d", 
    gllc_psm_info.llme_info.llc_version, gllc_psm_info.llme_info.input_offset_IOV_UI);

  for (sapi = GLLC_LL_SAPI_1_GMM; sapi <= GLLC_LL_LAST_SAPI; sapi++)
  {
    MSG_GERAN_LOW_3_G("PSM dump LL %d:: l2_info_valid=%d, l3_xid_len=%d", 
      gllc_sapi_addr_lookup[sapi], gllc_psm_info.lle_info[sapi].l2_info_valid, gllc_psm_info.lle_info[sapi].l3_xid_len);

    if (gllc_psm_info.lle_info[sapi].l2_info_valid)
    {
      MSG_GERAN_LOW_5_G("PSM dump LL  :: xid_pending=0x%X, IOV_I=%d, T200=%d, N200=%d, N201_U=%d",
        gllc_psm_info.lle_info[sapi].l2_info.xid_pending,
        gllc_psm_info.lle_info[sapi].l2_info.input_offset_IOV_I,
        gllc_psm_info.lle_info[sapi].l2_info.retrx_interval_T200,
        gllc_psm_info.lle_info[sapi].l2_info.max_retrx_cnt_N200,
        gllc_psm_info.lle_info[sapi].l2_info.max_info_octets_N201_U);
      MSG_GERAN_LOW_5_G("PSM dump LL  :: N201_I=%d, kD=%d, kU=%d, mD=%d, mU=%d",
        gllc_psm_info.lle_info[sapi].l2_info.max_info_octets_N201_I,
        gllc_psm_info.lle_info[sapi].l2_info.max_reseq_buf_frames_kD,
        gllc_psm_info.lle_info[sapi].l2_info.max_retrx_buf_frames_kU,
        gllc_psm_info.lle_info[sapi].l2_info.max_reseq_buf_octets_mD,
        gllc_psm_info.lle_info[sapi].l2_info.max_retrx_buf_octets_mU);
      MSG_GERAN_LOW_4_G("PSM dump LL  :: unack_encipher_oc=%d, ack_encipher_oc=%d, unack_decipher_oc=%d, ack_decipher_oc=%d",
        gllc_psm_info.lle_info[sapi].l2_info.unack_encipher_oc,
        gllc_psm_info.lle_info[sapi].l2_info.ack_encipher_oc,
        gllc_psm_info.lle_info[sapi].l2_info.unack_decipher_oc,
        gllc_psm_info.lle_info[sapi].l2_info.ack_decipher_oc);
    }
  }
}

/*===========================================================================
===
===  FUNCTION      GLLC_WRITE_PSM_INFO()
===
===  DESCRIPTION
===
===   Writes PSM info to the EFS file.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   TRUE - if EFS write is successful, FALSE - otherwise.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL boolean gllc_write_psm_info(void)
{
  int32   fs_handle;
  void   *data_ptr;
  uint32  data_len;
  uint8   sapi;

  MSG_GERAN_HIGH_0_G("Writing PSM info to EFS");
  gllc_dump_psm_info();

  /* Open EFS for write */
  fs_handle = mcfg_fopen(GERAN_EFS_LLC_PSM_INFO,
                         MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY,
                         MCFG_FS_ALLPERMS,
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type) SYS_MODEM_AS_ID_1);

  if (fs_handle == MCFG_FS_ERROR)
  {
    MSG_GERAN_ERROR_2_G("PSM Write:: EFS open failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /* Write LLME info */
  data_len = sizeof(gllc_llme_psm_info_t);
  data_ptr = &gllc_psm_info.llme_info;
  if (mcfg_fwrite(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
  {
    MSG_GERAN_ERROR_2_G("PSM Write:: EFS write failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /* Write LLE info */
  for (sapi = GLLC_LL_SAPI_1_GMM; sapi <= GLLC_LL_LAST_SAPI; sapi++)
  {
    data_len = sizeof(boolean);
    data_ptr = &gllc_psm_info.lle_info[sapi].l2_info_valid;
    if (mcfg_fwrite(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
    {
      MSG_GERAN_ERROR_2_G("PSM Write:: EFS write failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }

    if (gllc_psm_info.lle_info[sapi].l2_info_valid)
    {
      data_len = sizeof(gllc_lle_l2_info_t);
      data_ptr = &gllc_psm_info.lle_info[sapi].l2_info;
      if (mcfg_fwrite(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
      {
        MSG_GERAN_ERROR_2_G("PSM Write:: EFS write failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        return FALSE;
      }
    }

    data_len = sizeof(uint8);
    data_ptr = &gllc_psm_info.lle_info[sapi].l3_xid_len;
    if (mcfg_fwrite(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
    {
      MSG_GERAN_ERROR_2_G("PSM Write:: EFS write failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }

    if (gllc_psm_info.lle_info[sapi].l3_xid_len)
    {
      data_len = gllc_psm_info.lle_info[sapi].l3_xid_len;
      data_ptr = gllc_psm_info.lle_info[sapi].l3_xid_info;
      if (mcfg_fwrite(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
      {
        MSG_GERAN_ERROR_2_G("PSM Write:: EFS write failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        return FALSE;
      }
      GPRS_MEM_FREE(data_ptr);
    }
  }

  /* Close EFS */
  if (mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
  {
    MSG_GERAN_ERROR_2_G("PSM Write:: EFS close failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
===
===  FUNCTION      GLLC_READ_PSM_INFO()
===
===  DESCRIPTION
===
===   Read PSM info from the EFS file.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   TRUE - if EFS read is successful, FALSE - otherwise.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

boolean gllc_read_psm_info(void)
{
  int32   fs_handle;
  void   *data_ptr;
  uint32  data_len;
  uint8   sapi;

  MSG_GERAN_HIGH_0_G("Reading PSM info from EFS");
  
  /* Open EFS for read */
  fs_handle = mcfg_fopen(GERAN_EFS_LLC_PSM_INFO,
                         MCFG_FS_O_RDWR,
                         MCFG_FS_ALLPERMS,
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type) SYS_MODEM_AS_ID_1);

  if (fs_handle == MCFG_FS_ERROR)
  {
    MSG_GERAN_ERROR_2_G("PSM Read:: EFS open failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /* Read LLME info */
  data_len = sizeof(gllc_llme_psm_info_t);
  data_ptr = &gllc_psm_info.llme_info;
  if (mcfg_fread(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
  {
    MSG_GERAN_ERROR_2_G("PSM Read:: EFS read failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /* Read LLE info */
  for (sapi = GLLC_LL_SAPI_1_GMM; sapi <= GLLC_LL_LAST_SAPI; sapi++)
  {
    data_len = sizeof(boolean);
    data_ptr = &gllc_psm_info.lle_info[sapi].l2_info_valid;
    if (mcfg_fread(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
    {
      MSG_GERAN_ERROR_2_G("PSM Read:: EFS read failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }

    if (gllc_psm_info.lle_info[sapi].l2_info_valid)
    {
      data_len = sizeof(gllc_lle_l2_info_t);
      data_ptr = &gllc_psm_info.lle_info[sapi].l2_info;
      if (mcfg_fread(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
      {
        MSG_GERAN_ERROR_2_G("PSM Read:: EFS read failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        return FALSE;
      }
    }

    data_len = sizeof(uint8);
    data_ptr = &gllc_psm_info.lle_info[sapi].l3_xid_len;
    if (mcfg_fread(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
    {
      MSG_GERAN_ERROR_2_G("PSM Read:: EFS read failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }

    if (gllc_psm_info.lle_info[sapi].l3_xid_len)
    {
      data_len = gllc_psm_info.lle_info[sapi].l3_xid_len;
      gllc_psm_info.lle_info[sapi].l3_xid_info = GPRS_MEM_MALLOC(data_len);
      data_ptr = gllc_psm_info.lle_info[sapi].l3_xid_info;

      if (data_ptr)
      {
        if (mcfg_fread(fs_handle, data_ptr, data_len, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
        {
          MSG_GERAN_ERROR_2_G("PSM Read:: EFS read failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
          return FALSE;
        }
      }
      else
      {
        ERR_GERAN_FATAL_1_G("PSM Read:: memory allocation failed (size=%d)", data_len);
      }
    }
  }

  /* Close EFS */
  if (mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == MCFG_FS_ERROR)
  {
    MSG_GERAN_ERROR_2_G("PSM Read:: EFS close failed for %d errno=%d", fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  gllc_dump_psm_info();
  return TRUE;
}

/*===========================================================================
===
===  FUNCTION      GLLC_RESTORE_l2_PSM_INFO()
===
===  DESCRIPTION
===
===   Restores saved PSM info for a corresponding SAPI.
===
===  DEPENDENCIES
===
===   PSM Read should have been called before calling this function.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
LOCAL void gllc_restore_l2_psm_info(gllc_ll_sapi_t sapi)
{
  MSG_GERAN_LOW_2_G("GLC LL %d: Restoring PSM info (l2_info_valid=%d)", 
    gllc_sapi_addr_lookup[sapi], gllc_psm_info.lle_info[sapi].l2_info_valid);
  
  gllc_llme.llc_version         = gllc_psm_info.llme_info.llc_version;
  gllc_llme.input_offset_IOV_UI = gllc_psm_info.llme_info.input_offset_IOV_UI;

  if (gllc_psm_info.lle_info[sapi].l2_info_valid)
  {
     gllc_lle[sapi].unnum.xid_pending            = gllc_psm_info.lle_info[sapi].l2_info.xid_pending;
     gllc_lle[sapi].ack.input_offset_IOV_I       = gllc_psm_info.lle_info[sapi].l2_info.input_offset_IOV_I;
     gllc_lle[sapi].gen.retrx_interval_T200      = gllc_psm_info.lle_info[sapi].l2_info.retrx_interval_T200;
     gllc_lle[sapi].gen.max_retrx_cnt_N200       = gllc_psm_info.lle_info[sapi].l2_info.max_retrx_cnt_N200;
     gllc_lle[sapi].unack.max_info_octets_N201_U = gllc_psm_info.lle_info[sapi].l2_info.max_info_octets_N201_U;
     gllc_lle[sapi].ack.max_info_octets_N201_I   = gllc_psm_info.lle_info[sapi].l2_info.max_info_octets_N201_I;
     gllc_lle[sapi].ack.max_reseq_buf_frames_kD  = gllc_psm_info.lle_info[sapi].l2_info.max_reseq_buf_frames_kD;
     gllc_lle[sapi].ack.max_retrx_buf_frames_kU  = gllc_psm_info.lle_info[sapi].l2_info.max_retrx_buf_frames_kU;
     gllc_lle[sapi].ack.max_reseq_buf_octets_mD  = gllc_psm_info.lle_info[sapi].l2_info.max_reseq_buf_octets_mD;
     gllc_lle[sapi].ack.max_retrx_buf_octets_mU  = gllc_psm_info.lle_info[sapi].l2_info.max_retrx_buf_octets_mU;
     gllc_lle[sapi].unack.encipher_oc            = gllc_psm_info.lle_info[sapi].l2_info.unack_encipher_oc;
     gllc_lle[sapi].ack.encipher_oc              = gllc_psm_info.lle_info[sapi].l2_info.ack_encipher_oc;
     gllc_lle[sapi].unack.decipher_oc            = gllc_psm_info.lle_info[sapi].l2_info.unack_decipher_oc;
     gllc_lle[sapi].ack.decipher_oc              = gllc_psm_info.lle_info[sapi].l2_info.ack_decipher_oc;
  }

  gllc_psm_info.lle_info[sapi].l2_info_valid = FALSE;
}

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_RETRX_BUF()
===
===  DESCRIPTION
===
===   All I frame items in the LLE's retransmission buffer are flushed, and
===   all allocated memory resource is de-allocated.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_flush_retrx_buf( gllc_ll_sapi_t lle )
{
  register gllc_lle_retrx_buf_t *item_p, *next_p;

  /* --------------------------------------------------------------------- */

  item_p = gllc_lle[lle].ack.retrx_first_p;

  while ( item_p != NULL )
  {
    next_p = item_p -> next_p;
    GPRS_PDU_FREE( &item_p -> frame.i_s_frame.l3_pdu );
    GPRS_MEM_FREE( item_p );
    item_p = next_p;
  }

  gllc_lle[lle].ack.retrx_first_p         = NULL;
  gllc_lle[lle].ack.retrx_last_p          = NULL;

  gllc_lle[lle].ack.retrx_buf_occupancy_B = 0;
  gllc_lle[lle].ack.retrx_buf_num_frames  = 0;

  GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_I_FR_TRX_EVT );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_flush_retrx_buf() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_RESEQ_BUF()
===
===  DESCRIPTION
===
===   All I frame items in the LLE's resequencing buffer are flushed, and
===   all allocated memory resource is de-allocated.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_flush_reseq_buf( gllc_ll_sapi_t lle )
{
  register gllc_lle_reseq_buf_t *item_p, *next_p;

  /* --------------------------------------------------------------------- */

  item_p = gllc_lle[lle].ack.reseq_first_p;

  while ( item_p != NULL )
  {
    next_p = item_p -> next_p;
    GPRS_PDU_FREE( &item_p -> i_s_frame.l3_pdu );
    GPRS_MEM_FREE( item_p );
    item_p = next_p;
  }

  gllc_lle[lle].ack.reseq_first_p = NULL;
  gllc_lle[lle].ack.reseq_last_p  = NULL;

  gllc_lle[lle].ack.pdu_priority  = GPDU_LOW_PRIORITY;

  GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_S_FR_TRX_EVT );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_flush_reseq_buf() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_ENTER_STATE_ABM()
===
===  DESCRIPTION
===
===   Performs essential entry actions prior to entering the LLE to the ABM
===   state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_enter_state_abm( gllc_ll_sapi_t lle )
{
  gllc_lle[lle].gen.pdp_active                    = TRUE;
  gllc_lle[lle].gen.is_grr_purge_data_req_enabled = FALSE;
  gllc_lle[lle].ack.send_state_var_V_S            = 0; 
  gllc_lle[lle].ack.recv_state_var_V_R            = 0;
  gllc_lle[lle].ack.ackn_state_var_V_A            = 0;
  gllc_lle[lle].ack.highest_rcvd_N_S              = 511; /* None received yet! */
  gllc_lle[lle].ack.previous_rcvd_N_S             = 511; /* None received yet! */
  gllc_lle[lle].ack.encipher_oc                   = 0;
  gllc_lle[lle].ack.decipher_oc                   = 0;
  gllc_lle[lle].ack.peer_rcvr_busy                = FALSE;
  gllc_lle[lle].ack.own_rcvr_busy                 = FALSE;
  gllc_lle[lle].ack.cause                         = GRR_CAUSE_LAY3_DATA;

  gllc_lle[lle].ack.sack_bitmap_len_K             = 0;
  (void)memset( gllc_lle[lle].ack.sack_bitmap_R, 0, GLLC_SACK_BMP_LEN );

  (void)rex_clr_timer( &gllc_timer_t201[lle] );
  gllc_lle[lle].ack.T201_association = NULL;
  GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );

  /* Flush UL watermark and retransmission/resequencing buffers for this
     lle. */
  gllc_lle_flush_ll_ul_ack_q( lle );
  gllc_lle_flush_retrx_buf( lle );
  gllc_lle_flush_reseq_buf( lle );

  /* Make sure all relevent event masking schemes are cleared if they havn't
     already been so. */

  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_PEER_RCVR_BUSY );
  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_RETRX_BUF_FULL );
  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP );
  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );
  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_ABM       );

  /* If there are any layer 2 XID parameters for which a negotiation
     requirement is pending, then generate an XID request and transfer it
     to the peer LLE with an XID Command Message. */

  gllc_lle[lle].state = GLLC_LLE_ABM;

  if ( gllc_lle_generate_l2_xid_req(lle) )
  {
    gllc_lle_send_xid_cmd_mes( lle, NULL );
    gllc_lle[lle].substate = GLLC_LLE_L2_XID_EXCHANGE;
  }
  else
    gllc_lle[lle].substate = GLLC_LLE_NONE;

} /* end of gllc_lle_enter_state_abm() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_ENTER_STATE_ADM()
===
===  DESCRIPTION
===
===   Performs essential entry actions prior to entering the LLE to the ADM
===   state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_enter_state_adm( gllc_ll_sapi_t lle )
{
  /* Make sure the 'Wait peer response' and 'Wait layer3 response' event
     masking schemes are cleared if they havn't already been so, and set
     the 'Wait ABM' masking scheme. */

  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );
  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP );
  gllc_lle_set_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_ABM       );

  /* To cater for the case whereby ADM state is being entered as a result of
     an exit from the ABM state, then clear internal buffers associated with
     ABM operation. */

  gllc_lle_flush_retrx_buf( lle );
  gllc_lle_flush_reseq_buf( lle );

  /* If there are any layer 2 XID parameters for which a negotiation
     requirement is pending, and there is an active PDP associated with this
     LLE, then generate an XID request and transfer it to the peer LLE with
     an XID Command Message. */

  gllc_lle[lle].state = GLLC_LLE_ADM;
  gllc_lle[lle].substate = GLLC_LLE_NONE;

  if ( gllc_lle[lle].gen.pdp_active )
  {
    if ( gllc_lle_generate_l2_xid_req(lle) )
    {
      gllc_lle_send_xid_cmd_mes( lle, NULL );
      gllc_lle[lle].substate = GLLC_LLE_L2_XID_EXCHANGE;
    }
  }

  gllc_lle[lle].gen.is_grr_purge_data_req_enabled = TRUE;

} /* end of gllc_lle_enter_state_adm() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_XLATE_AND_SET_CIPHER_KEY()
===
===  DESCRIPTION
===
===   Performs translation of cipher key from uint8 to uint32 format and
===   relevant initialisations.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_xlate_and_set_cipher_key
(
  gmmllc_llgmm_assign_req_type  *assign_req_mes
)
{

  uint32  key_Kc[2];
  uint32  key_Kc128[4];

  if ( assign_req_mes -> gea != GMMLLC_GEA_UNDEFINED )
  {
    if (( assign_req_mes -> gea == GMMLLC_GEA_1    ) ||
        ( assign_req_mes -> gea == GMMLLC_GEA_2    ) ||
        ( assign_req_mes -> gea == GMMLLC_GEA_3    )
       )
    {

      GLLC_LLME_XLATE_KC( assign_req_mes -> kc, key_Kc );

      if ( (gllc_llme.key_Kc[0] != key_Kc[0]) ||
           (gllc_llme.key_Kc[1] != key_Kc[1])    )
      {
        gllc_llme_reset_IOV_xid_parameters();
      }

      gllc_llme.key_Kc[0] = key_Kc[0];
      gllc_llme.key_Kc[1] = key_Kc[1];
    }
    else if ( assign_req_mes -> gea == GMMLLC_GEA_4 )
    {

      GLLC_LLME_XLATE_KC128( assign_req_mes -> kc_128bit, key_Kc128 );

      if ( (gllc_llme.key_Kc128[0] != key_Kc128[0]) ||
           (gllc_llme.key_Kc128[1] != key_Kc128[1]) ||
           (gllc_llme.key_Kc128[2] != key_Kc128[2]) ||
           (gllc_llme.key_Kc128[3] != key_Kc128[3])
         )
      {
        gllc_llme_reset_IOV_xid_parameters();
      }

      gllc_llme.key_Kc128[0] = key_Kc128[0];
      gllc_llme.key_Kc128[1] = key_Kc128[1];
      gllc_llme.key_Kc128[2] = key_Kc128[2];
      gllc_llme.key_Kc128[3] = key_Kc128[3];
    }
    else if ( assign_req_mes -> gea == GMMLLC_GEA_NONE )
    {

      GLLC_LLME_XLATE_KC( assign_req_mes -> kc, key_Kc );
      GLLC_LLME_XLATE_KC128( assign_req_mes -> kc_128bit, key_Kc128 );

      if ( (gllc_llme.key_Kc[0] != key_Kc[0])       ||
           (gllc_llme.key_Kc[1] != key_Kc[1])       ||
           (gllc_llme.key_Kc128[0] != key_Kc128[0]) ||
           (gllc_llme.key_Kc128[1] != key_Kc128[1]) ||
           (gllc_llme.key_Kc128[2] != key_Kc128[2]) ||
           (gllc_llme.key_Kc128[3] != key_Kc128[3])
         )
      {
        gllc_llme_reset_IOV_xid_parameters();
      }

      gllc_llme.key_Kc[0] = key_Kc[0];
      gllc_llme.key_Kc[1] = key_Kc[1];
      gllc_llme.key_Kc128[0] = key_Kc128[0];
      gllc_llme.key_Kc128[1] = key_Kc128[1];
      gllc_llme.key_Kc128[2] = key_Kc128[2];
      gllc_llme.key_Kc128[3] = key_Kc128[3];
    }

    gllc_debug_print_llme_cipher_params();
  }
}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_RESET()
===
===  DESCRIPTION
===
===   Performs a full reset of the LLME.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_reset(void)
{
  gllc_llme.state                     = GLLC_LLME_TLLI_UNASSIGNED;
  gllc_llme.gan_state                 = GLLC_LLME_GAN_INACTIVE;
  gllc_llme.force_l3_flow_control     = FALSE;

  gllc_llme.tlli                      = GLLC_TLLI_UNASSIGNED;
  gllc_llme.tlli_rxd                  = FALSE;

  gllc_llme.fcs_errors_ctr            = 0;
  gllc_llme.short_errors_ctr          = 0;
  gllc_llme.addr_errors_ctr           = 0;
  gllc_llme.frmr_errors_ctr           = 0;
  gllc_llme.tlli_errors_ctr           = 0;

  gllc_llme.test_llc_pdu_num          = 0;
  gllc_llme.test_llc_pdu_size         = 0;
  gllc_llme.test_llc_pdu_p            = NULL;
  gllc_llme.active_test_mode_b_or_srb = FALSE;
  gllc_llme.xid_reset_in_progress     = GLLC_LLME_XID_RESET_INACTIVE;
  gllc_llme.is_resume_cnf_pending     = FALSE;

  gllc_llme_reset_l2_xid_parameters();

  gllc_gmm_ready_timer_active = FALSE;
  (void)rex_clr_timer( &gllc_gmm_ready_timer );
  (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_GMM_RDY_TMR_SIG );

  if ( gllc_llme_ul_evt_mask_scheme_flags[ GLLC_LLME_SCHEME_LLGMM_SUSPEND ] )
  {
    gllc_llme_send_grr_purge_suspended_data_req_mes();
  }
  else
  {
    gllc_llme_set_ul_event_mask_scheme( GLLC_LLME_SCHEME_LLGMM_SUSPEND );

    if ( gllc_initialised )
    {
      gllc_llme_send_grr_suspend_req_mes();
    }
  }

  gllc_llme_clr_ul_event_mask_scheme( GLLC_LLME_SCHEME_GRR_SUSPEND );
  gllc_llme_clr_ul_event_mask_scheme( GLLC_LLME_SCHEME_DSM_SUSPEND );
  gllc_llme_clr_ul_event_mask_scheme( GLLC_LLME_SCHEME_TEST_MODE   );

} /* end of gllc_llme_reset() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESET()
===
===  DESCRIPTION
===
===   Performs a full reset of an LLE, whilst leaving the the LLE's state set
===   as given.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_reset( gllc_ll_sapi_t lle, gllc_lle_state_t reset_state )
{
  /* Validity check the LLE index before use */

  if ( lle > GLLC_LL_LAST_SAPI )
  {
    ERR_GERAN_FATAL_1_G( "GLC LLE Reset: Invalid LLE: %d", lle);
  }

  /* --------------------------------------------------------------------- */

  /* Reset the LLE's state and substate variables. */

  gllc_lle[lle].state    = reset_state;
  gllc_lle[lle].substate = GLLC_LLE_NONE;

  /* --------------------------------------------------------------------- */

  /* Reset the LLE's PDU and ancilliary statistics for this attach
     session. */

  if ( gllc_lle[lle].state == GLLC_LLE_TLLI_UNASSIGNED )
  {
#ifndef FEATURE_GERAN_REDUCED_DEBUG
    (void)memset( &gllc_lle[lle].diag, 0, sizeof(gllc_lle_diag_t) );

    gllc_lle[lle].diag.pdu_stats.lle_sapi_addr =
        gllc_sapi_addr_lookup[lle];

    gllc_lle[lle].diag.reestablishment_cnt = 0;
    gllc_lle[lle].diag.general_a_cnt       = 0;
    gllc_lle[lle].diag.general_b_cnt       = 0;
    gllc_lle[lle].diag.general_c_cnt       = 0;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

    gllc_lle[lle].gen.pdp_active           = FALSE;
  }

  gllc_lle[lle].gen.is_grr_purge_data_req_enabled = TRUE;
  
  /* --------------------------------------------------------------------- */

  /* Reset the LLE's negotiable parameters to defaults if existing xid
     parameters are to be discarded. In the case of PS handover, lle needs
     resetting but existing xid parameters should be re-used. */

#ifdef FEATURE_GPRS_PS_HANDOVER
  if ( gllc_llme.use_old_xid_ind == FALSE )
  {
#endif /* FEATURE_GPRS_PS_HANDOVER */
    gllc_lle_reset_l2_xid_parameters( lle );
#ifdef FEATURE_GPRS_PS_HANDOVER
  }
#endif /* FEATURE_GPRS_PS_HANDOVER */

  /* --------------------------------------------------------------------- */

  /* Reset the LLE's unacknowledged mode information elements. */

  gllc_lle[lle].unack.send_state_var_V_U    = 0;
  gllc_lle[lle].unack.recv_state_var_V_UR   = 0;
  gllc_lle[lle].unack.recv_state_bitmap     = 0;
  gllc_lle[lle].unack.encipher_oc           = 0;
  gllc_lle[lle].unack.decipher_oc           = 0;
  gllc_lle[lle].unack.radio_priority        = 1;
  gllc_lle[lle].unack.peak_throughput       = 0;

  gllc_lle[lle].unack.pfi                   = 0;

  gllc_debug_print_lle_cipher_params( lle );

  /* --------------------------------------------------------------------- */

  /* Reset the LLE's acknowledged mode information elements and buffers. */

  gllc_lle[lle].ack.send_state_var_V_S      = 0;
  gllc_lle[lle].ack.recv_state_var_V_R      = 0;
  gllc_lle[lle].ack.ackn_state_var_V_A      = 0;
  gllc_lle[lle].ack.highest_rcvd_N_S        = 511; /* None received yet! */
  gllc_lle[lle].ack.previous_rcvd_N_S       = 511; /* None received yet! */
  gllc_lle[lle].ack.encipher_oc             = 0;
  gllc_lle[lle].ack.decipher_oc             = 0;
  gllc_lle[lle].ack.peer_rcvr_busy          = FALSE;
  gllc_lle[lle].ack.own_rcvr_busy           = FALSE;
  gllc_lle[lle].ack.radio_priority          = 1;
  gllc_lle[lle].ack.peak_throughput         = 0;

  gllc_lle[lle].ack.pfi                     = 0;

  gllc_lle[lle].ack.sack_bitmap_len_K       = 0;
  (void)memset( gllc_lle[lle].ack.sack_bitmap_R, 0, GLLC_SACK_BMP_LEN );

  (void)rex_clr_timer( &gllc_timer_t200[lle] );
  (void)rex_clr_timer( &gllc_timer_t201[lle] );
  gllc_lle[lle].ack.T201_association = NULL;
  GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );

  gllc_lle_flush_retrx_buf( lle );
  gllc_lle_flush_reseq_buf( lle );

  /* --------------------------------------------------------------------- */

  /* Reset the LLE's unnumbered information elements. */

  gllc_lle[lle].unnum.radio_priority        = 1;
  gllc_lle[lle].unnum.peak_throughput       = 0;

  gllc_lle[lle].unnum.pfi                   = 0;

  /* --------------------------------------------------------------------- */

  /* Flush all input queues to this LLE and clear any related event flags. */

  gllc_lle_flush_ll_ul_sig_q( lle );
  gllc_lle_flush_ll_ul_ack_q( lle );
  gllc_lle_flush_ll_ul_unack_q( lle );
  gllc_lle_flush_cr_ul_q( lle );

  /* --------------------------------------------------------------------- */

  /* Reset all LLE specific event masking schemes. */

  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_PEER_RCVR_BUSY );
  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_RETRX_BUF_FULL );
  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP );
  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );
  gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_ABM       );

  /* --------------------------------------------------------------------- */

  /* De-allocate any PDU resource associated with the LLE's unacknowledged
     and unnumbered frame transmission information elements. */

  if ( gllc_lle[lle].unack.frame.ui_frame.l3_pdu != NULL )
    GPRS_PDU_FREE( &gllc_lle[lle].unack.frame.ui_frame.l3_pdu );

  if
  (
    (gllc_lle[lle].unnum.cmd_frame.u_frame.xid != GLLC_XID_NONE) &&
    (
      gllc_lle[lle].unnum.cmd_frame.
      u_frame.info_field.xid_info.l3_xid.params != NULL
    )
  )
  {
    GPRS_PDU_FREE(
      &gllc_lle[lle].unnum.cmd_frame.
        u_frame.info_field.xid_info.l3_xid.params );
  }

  if
  (
    (gllc_lle[lle].unnum.sol_res_frame.u_frame.xid != GLLC_XID_NONE) &&
    (
      gllc_lle[lle].unnum.sol_res_frame.
      u_frame.info_field.xid_info.l3_xid.params != NULL
    )
  )
  {
    GPRS_PDU_FREE(
      &gllc_lle[lle].unnum.sol_res_frame.
        u_frame.info_field.xid_info.l3_xid.params );
  }

  if
  (
    (gllc_lle[lle].unnum.unsol_res_frame.u_frame.xid != GLLC_XID_NONE) &&
    (
      gllc_lle[lle].unnum.unsol_res_frame.
      u_frame.info_field.xid_info.l3_xid.params != NULL
    )
  )
  {
    GPRS_PDU_FREE(
      &gllc_lle[lle].unnum.unsol_res_frame.
        u_frame.info_field.xid_info.l3_xid.params );
  }

  /* Clear any outstanding transmission events. */

  GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TRX_EVT );

  /* --------------------------------------------------------------------- */

  /* Clear any outstanding timer events. */

  GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_EVT );

  /* --------------------------------------------------------------------- */



} /* end of gllc_lle_reset() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_ASSIGN_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LLGMM-ASSIGN-REQ Service Primitive from GMM as a function
===   of LLME state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_process_assign_req_mes
(
  gmmllc_llgmm_assign_req_type  *assign_req_mes
)
{
  uint32  lle;

  MSG_GERAN_MED_4_G("Rcvd LLGMM-ASSIGN-REQ, state:%d, tlli_old:%d, tlli_new:%d, Dbg:%d", 
    gllc_llme.state,
    assign_req_mes -> tlli_old,
    assign_req_mes -> tlli_new,
    geran_ciphering_logging_enabled);

  switch ( gllc_llme.state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_UNASSIGNED:

    if ( (assign_req_mes -> tlli_old == GLLC_TLLI_UNASSIGNED) &&
         (assign_req_mes -> tlli_new != GLLC_TLLI_UNASSIGNED)    )
    {
      /* For Test Purposes this represents a suitable trigger point for AMSS
         based GEA Test */

      #ifdef DEBUG_GSM_GPRS_GLLC_FRAME
      #error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */

      /* TLLI Assignment from GMM. Accept the TLLI and other parameters,
         ensure that all LLEs are moved to the ADM state, and the LLME to
         the TLLI Assigned state. */
      gllc_llme.gea = assign_req_mes -> gea;

      gllc_llme_xlate_and_set_cipher_key( assign_req_mes );

      gllc_llme.tlli      = assign_req_mes -> tlli_new;
      gllc_llme.tlli_old  = assign_req_mes -> tlli_old;
      gllc_llme.tlli_rxd  = FALSE;
      gllc_llme.state     = GLLC_LLME_TLLI_ASSIGNED;

      for
      (
        lle = (uint32)GLLC_LL_SAPI_1_GMM;
        lle <= (uint32)GLLC_LL_LAST_SAPI;
        lle++
      )
      {
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
        /* For unit test convenience purposes only, do not send LL-XID-IND
           message.*/

        if ( gllc_ll_client_record[lle].ll_dl_sig_get_mes_buf_fnc_ptr != NULL )
          gllc_lle_send_ll_xid_ind_mes( (gllc_ll_sapi_t)lle, NULL, GLLC_TLLI_UNASSIGNED );
#endif
        gllc_lle_enter_state_adm( (gllc_ll_sapi_t)lle );
      }

      /* Register the DSM Memory Event handler callbacks. */

      dsm_reg_mem_event_cb
      (
        DSM_DUP_ITEM_POOL, DSM_MEM_LEVEL_MANY,
        DSM_MEM_OP_FREE,   gllc_dsm_event_ind_1
      );
      
      dsm_reg_mem_event_cb
      (
        DSM_DUP_ITEM_POOL, DSM_MEM_LEVEL_FEW,
        DSM_MEM_OP_NEW,    gllc_dsm_event_ind_1
      );
    }

    /* ................................................................... */

    else if ( (assign_req_mes -> tlli_old == GLLC_TLLI_UNASSIGNED) &&
              (assign_req_mes -> tlli_new == GLLC_TLLI_UNASSIGNED)    )
    {
      /* Entertain this redundant transition from GMM by doing nothing. */
    }

    /* ................................................................... */

    else
    {
      /* Any other combination of TLLI old and new represents an error
         condition in this LLME state. */

      MSG_GERAN_LOW_3_G
      (
        "GLC ME: LLGMM-ASSIGN-REQ Err: S:%d O:0x%x N:0x%x",
        gllc_llme.state,
        assign_req_mes -> tlli_old,
        assign_req_mes -> tlli_new
      );
    }

    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_ASSIGNED:
    if ( (assign_req_mes -> tlli_old != GLLC_TLLI_UNASSIGNED) &&
         (assign_req_mes -> tlli_new == GLLC_TLLI_UNASSIGNED)    )
    {
      /* TLLI unassignment from GMM. Ensure that all LLEs are reset. Move
         to TLLI_UNASSIGNED_STATE. */

      gllc_llme.tlli      = assign_req_mes -> tlli_new;
      gllc_llme.tlli_old  = assign_req_mes -> tlli_old;

      gllc_llme_reset();

      for
      (
        lle = (uint32)GLLC_LL_SAPI_1_GMM;
        lle <= (uint32)GLLC_LL_LAST_SAPI;
        lle++
      )
      {
        gllc_lle_reset( (gllc_ll_sapi_t)lle, GLLC_LLE_TLLI_UNASSIGNED );

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
        /* For unit test convenience purposes only, do not send LL-XID-IND
           message relating to all LLEs. */

        if ( gllc_ll_client_record[lle].ll_dl_sig_get_mes_buf_fnc_ptr != NULL )
          gllc_lle_send_ll_xid_ind_mes( (gllc_ll_sapi_t)lle, NULL, GLLC_TLLI_UNASSIGNED );
#endif
      }

      /* De-register the DSM Memory Event handler callbacks. */

      dsm_unreg_mem_event_cb(
        DSM_DUP_ITEM_POOL, DSM_MEM_LEVEL_MANY, DSM_MEM_OP_FREE );

      dsm_unreg_mem_event_cb(
        DSM_DUP_ITEM_POOL, DSM_MEM_LEVEL_FEW, DSM_MEM_OP_NEW );
    }

    /* ................................................................... */

    else if ( (assign_req_mes -> tlli_old == gllc_llme.tlli_old) &&
              (assign_req_mes -> tlli_new == gllc_llme.tlli    )    )
    {
      /* Ciphering algorithm/key re-assignment only, from GMM. */
      gllc_llme.gea = assign_req_mes -> gea;
      gllc_llme_xlate_and_set_cipher_key( assign_req_mes );
    }

    /* ................................................................... */

    else if ( (assign_req_mes -> tlli_old != GLLC_TLLI_UNASSIGNED) &&
              (assign_req_mes -> tlli_new != GLLC_TLLI_UNASSIGNED)    )
    {
      /* TLLI reassignment (change) from GMM. Accept the TLLI and other
         parameters, and maintain current state. */
      gllc_llme.gea = assign_req_mes -> gea;
      gllc_llme_xlate_and_set_cipher_key( assign_req_mes );

      gllc_llme.tlli      = assign_req_mes -> tlli_new;
      gllc_llme.tlli_old  = assign_req_mes -> tlli_old;
      gllc_llme.tlli_rxd  = FALSE;
    }

    /* ................................................................... */

    else if ( (assign_req_mes -> tlli_old == GLLC_TLLI_UNASSIGNED) &&
              (assign_req_mes -> tlli_new == gllc_llme.tlli      )    )
    {
      /* TLLI reassignment (change) from GMM, but where old TLLI has been
         unassigned by GMM following old PTMSI invalidation as a result of
         LLC notification of frame receipt on new TLLI. Accept the changes
         and maintain current state. */
      gllc_llme.gea = assign_req_mes -> gea;
      gllc_llme_xlate_and_set_cipher_key( assign_req_mes );

      gllc_llme.tlli      = assign_req_mes -> tlli_new;
      gllc_llme.tlli_old  = assign_req_mes -> tlli_old;
    }
    else
    {
      /* Any other combination of TLLI old and new represents an error
         condition in this LLME state. */
      MSG_GERAN_LOW_3_G
      (
        "GLC ME: LLGMM-ASSIGN-REQ Err: S:%d O:0x%x N:0x%x",
        gllc_llme.state,
        assign_req_mes -> tlli_old,
        assign_req_mes -> tlli_new
      );
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TEST_MODE:
  default:
    break;
  }

  /* Check assignment GEA algorithm against that which is supported. */
  if ( !gllc_is_gea_supported(gllc_llme.gea) )
  {
    MSG_GERAN_ERROR_1_G( "GLC ME: LLGMM-ASSIGN-REQ GEA Err %d", gllc_llme.gea );
    gllc_llme.gea = GMMLLC_GEA_NONE;
  }
} /* end of gllc_llme_process_assign_req_mes() */

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_TRIGGER_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LLGMM-TRIGGER-REQ Service Primitive from GMM as a
===   function of LLME state.
===
===   NOTE: FEATURE_GSM_GPRS_LLC_TRIG_REQ_FORMAL forces the formal
===         processing of the LLGMM-TRIGGER-REQ primitive.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_process_trigger_req_mes
(
  gmmllc_llgmm_trigger_req_type *trigger_req_mes
)
{
#ifdef FEATURE_GSM_GPRS_LLC_TRIG_REQ_FORMAL
  register gllc_lle_retrx_buf_t  *item_p;
  register uint32                unmasked_trx_events;
#endif
           uint32                lle;
           grr_data_req_cause_t  grr_cause;
           boolean               triggered = FALSE;

  /* ===================================================================== */

  switch ( gllc_llme.state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_ASSIGNED:

    if ( trigger_req_mes -> tlli == gllc_llme.tlli )
    {
      /* ................................................................. */

      /* Translate the LLGMM Trigger Req cause into a GRR Data Req cause. */

      if ( trigger_req_mes -> cause == GMMLLC_PAGE_RESPONSE )
      {
        grr_cause = GRR_CAUSE_GMM_PAGE_RESPONSE;
      }
      else if ( trigger_req_mes -> cause == GMMLLC_CELL_UPDATE )
      {
        grr_cause = GRR_CAUSE_GMM_CELL_UPDATE;
      }
      else if ( trigger_req_mes -> cause == GMMLLC_CELL_NOTIFY )
      {
        grr_cause = GRR_CAUSE_GMM_CELL_NOTIFY;
      }
      else
      {
        grr_cause = GRR_CAUSE_GMM_SIGNALLING;
      }

      MSG_GERAN_MED_1_G( "GLC ME: Rcvd LLGMM-TRIG-REQ cause%d", grr_cause);

      /* ................................................................. */

#ifdef FEATURE_GSM_GPRS_LLC_TRIG_REQ_FORMAL

      /* A GMM trigger requirement shall be satisfied in the first instance
         by any frame which is currently awaiting transmission and can be
         transferred to RLC via a GRR-DATA-REQ primitive.

         Run through each LLE until a transmission event has been found
         which satisfies the GMM trigger requirement. */

      for ( lle = 0; (lle <= (uint32)GLLC_LL_LAST_SAPI) && !triggered; lle++ )
      {
        unmasked_trx_events =
          (uint32)(gllc_lle_ul_event_map[lle] & gllc_lle_ul_mask_map[lle]);

        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */

        /* A GMM trigger requirement can be satisfied by an I frame
           retransmission event ... */

        if ( unmasked_trx_events & GLLC_LLE_UL_I_FR_RETRX_EVT )
        {
          item_p = gllc_lle[lle].ack.retrx_first_p;
          while ( (item_p != NULL) && (!item_p -> retrx_flag) )
            item_p = item_p -> next_p;

          if ( item_p != NULL )
          {
            item_p -> cause = grr_cause;
            triggered       = TRUE;
          }
        }

        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */

        /* ... or an I frame transmission event ... */

        if ( (!triggered                                ) &&
             (unmasked_trx_events & GLLC_LLE_UL_I_FR_EVT)    )
        {
          item_p = gllc_lle[lle].ack.retrx_last_p;

          if ( item_p != NULL )
          {
            item_p -> cause = grr_cause;
            triggered       = TRUE;
          }
        }

        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */

        /* ... or an S frame transmission event ... */

        if ( (!triggered                                ) &&
             (unmasked_trx_events & GLLC_LLE_UL_S_FR_EVT)    )
        {
          gllc_lle[lle].ack.cause = grr_cause;
          triggered               = TRUE;
        }

        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */

        /* ... or a U frame transmission event ... */

        if
        (
          (!triggered)
          &&
          (
            unmasked_trx_events &
            (
              GLLC_LLE_UL_U_FR_CMD_EVT  |
              GLLC_LLE_UL_U_FR_SRES_EVT |
              GLLC_LLE_UL_U_FR_URES_EVT
            )
          )
        )
        {
          gllc_lle[lle].unnum.cause = grr_cause;
          triggered                 = TRUE;
        }

        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */

        /* ... or a UI frame transmission event on an LLE serving a SAPI
           other than SAPI 3, 5, 9, & 11. */

        if ( (!triggered                                 ) &&
             (unmasked_trx_events & GLLC_LLE_UL_UI_FR_EVT)    )
        {
          if ( !GLLC_LLE_IS_3_5_9_11(lle) )
          {
            gllc_lle[lle].unack.cause = grr_cause;
            triggered                 = TRUE;
          }
        }

        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
      }

#endif /* FEATURE_GSM_GPRS_LLC_TRIG_REQ_FORMAL */

      /* ................................................................. */

      /* If a GMM Trigger requirement has still not been satisfied, and the
         trigger cause is Cell Notify, then transmit a NULL command (P=0),
         with a Cell Update cause. */

      if ( !triggered )
      {
        if ( trigger_req_mes -> cause == GMMLLC_CELL_NOTIFY )
        {
          gllc_lle_send_null_cmd_mes( GLLC_LL_SAPI_1_GMM );
          gllc_lle[GLLC_LL_SAPI_1_GMM].unnum.cause = grr_cause;
          triggered = TRUE;
        }
      }

      /* ................................................................. */

#ifdef FEATURE_GSM_GPRS_LLC_TRIG_REQ_FORMAL

      /* If a GMM Trigger requirement has still not been satisfied, then run
         through each LLE to find which one if any is in the ABM state. If
         found then flag an S frame supervisory transmission event and set
         associated cause. */

      if ( !triggered )
      {
        for ( lle = 0; (lle <= (uint32)GLLC_LL_LAST_SAPI) && !triggered; lle++ )
        {
          if ( gllc_lle[lle].state == GLLC_LLE_ABM )
          {
            gllc_lle[lle].ack.cause = grr_cause;

            GLLC_SET_LLE_UL_EVT( lle, GLLC_LLE_UL_S_FR_EVT );
            triggered = TRUE;
          }
        }
      }

#endif /* FEATURE_GSM_GPRS_LLC_TRIG_REQ_FORMAL */

      /* ................................................................. */

      /* If a GMM Trigger requirement has still not been satisfied, then set
         cause and flag a zero length UI frame transmission event on the LLE
         serving the LL SAP GLLC_LL_SAPI_1_GMM. */

      if ( !triggered )
      {
        lle = (uint32)GLLC_LL_SAPI_1_GMM;

        /* Collate basic contents. */

        gllc_lle[lle].unack.frame.format                 = GLLC_UI_FORMAT;
        gllc_lle[lle].unack.frame.ui_frame.cmd_or_res_CR = GLLC_COMMAND;
        gllc_lle[lle].unack.frame.ui_frame.l3_pdu        = NULL;
        gllc_lle[lle].unack.l3_pdu_length                = 0;
        gllc_lle[lle].unack.frame.ui_frame.encrypted_E   =
          (
            (gllc_llme.gea != GMMLLC_GEA_NONE     ) &&
            (gllc_llme.gea != GMMLLC_GEA_UNDEFINED)
          ) ? TRUE:FALSE;

        gllc_lle[lle].unack.frame.ui_frame.send_seq_num_N_U =
          gllc_lle[lle].unack.send_state_var_V_U;

        gllc_lle[lle].unack.frame.ui_frame.protected_mode_PM = TRUE;

        gllc_lle[lle].unack.frame.ui_frame.cipher_oc =
          gllc_lle[lle].unack.encipher_oc;

        gllc_lle[lle].unack.cause = grr_cause;

        /* Check whether the unacknowledged send state variable V(U) is
           about to roll over when incremented, and increment the encipher
           OC by the V(U) modulo if so. */

        if ( gllc_lle[lle].unack.send_state_var_V_U == 511 )
        {
          gllc_lle[lle].unack.encipher_oc =
            (uint32)(gllc_lle[lle].unack.encipher_oc + 512);

          gllc_debug_print_lle_cipher_params ( lle );
        }

        /* Increment the unacknowledged send state variable V(U). */

        gllc_lle[lle].unack.send_state_var_V_U =
          GMODULO( 512, gllc_lle[lle].unack.send_state_var_V_U + 1 );

        /* Flag the transmission requirement to the MPEX trx handler. */

        GLLC_SET_LLE_UL_EVT( lle, GLLC_LLE_UL_UI_FR_EVT );
        triggered = TRUE;
      }

      /* ................................................................. */
    }

    else
    {
      MSG_GERAN_ERROR_1_G("GLC ME: LLGMM-TRIG-REQ TLLI Err 0x%x", gllc_llme.tlli);
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_UNASSIGNED:
  case GLLC_LLME_TEST_MODE:
  default:

    /* Illegal state identifier. */

    MSG_GERAN_ERROR_1_G("GLC ME: LLGMM-TRIG-REQ State Err %d", gllc_llme.state);
    break;

  /* --------------------------------------------------------------------- */
  }

  /* ===================================================================== */
} /* end of gllc_llme_process_trigger_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_SUSPEND_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LLGMM-SUSPEND-REQ Service Primitive from GMM as a
===   function of LLME state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_process_suspend_req_mes
(
  gmmllc_llgmm_suspend_req_type *suspend_req_mes
)
{
  uint32 lle;

  /* ===================================================================== */
  MSG_GERAN_MED_3_G( "Rcvd LLGMM-SUSPEND-REQ, state%d, sus_tlli:%d, tlli:%d", 
    gllc_llme.state, suspend_req_mes -> tlli, gllc_llme.tlli);

  /* ===================================================================== */

  switch ( gllc_llme.state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_ASSIGNED:

    if ( suspend_req_mes -> tlli == gllc_llme.tlli )
    {

      /* Suspend UL user data flow. */

      /* Suspend SAPI-3 */

      if ( gllc_lle[ GLLC_LL_SAPI_3_LL3 ].state == GLLC_LLE_ADM )
      {
        gsn_ll3_ul_unack_hiwater_nas_susp();
      }

      if ( gllc_lle[ GLLC_LL_SAPI_3_LL3 ].state == GLLC_LLE_ABM )
      {
        gsn_ll3_ul_ack_hiwater_nas_susp();
      }


      /* Suspend SAPI-5 */

      if ( gllc_lle[ GLLC_LL_SAPI_5_LL5 ].state == GLLC_LLE_ADM )
      {
        gsn_ll5_ul_unack_hiwater_nas_susp();
      }

      if ( gllc_lle[ GLLC_LL_SAPI_5_LL5 ].state == GLLC_LLE_ABM )
      {
        gsn_ll5_ul_ack_hiwater_nas_susp();
      }


      /* Suspend SAPI-9 */

      if ( gllc_lle[ GLLC_LL_SAPI_9_LL9 ].state == GLLC_LLE_ADM )
      {
        gsn_ll9_ul_unack_hiwater_nas_susp();
      }

      if ( gllc_lle[ GLLC_LL_SAPI_9_LL9 ].state == GLLC_LLE_ABM )
      {
        gsn_ll9_ul_ack_hiwater_nas_susp();
      }


      /* Suspend SAPI-11 */

      if ( gllc_lle[ GLLC_LL_SAPI_11_LL11 ].state == GLLC_LLE_ADM )
      {
        gsn_ll11_ul_unack_hiwater_nas_susp();
      }

      if ( gllc_lle[ GLLC_LL_SAPI_11_LL11 ].state == GLLC_LLE_ABM )
      {
        gsn_ll11_ul_ack_hiwater_nas_susp();
      }


      /* Extend the user channel suspension to the RLC UL process. */

      gllc_llme_send_grr_suspend_req_mes();

      /* If an 'LLGMM Suspend' event masking scheme is not already in force
         then invoke this scheme and reset any active T201 timers. */

      if ( !gllc_llme_ul_evt_mask_scheme_flags[GLLC_LLME_SCHEME_LLGMM_SUSPEND] )
      {
        for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
        {
          (void)rex_clr_timer( &gllc_timer_t201[lle] );
          GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );
        }
      }

      gllc_llme_set_ul_event_mask_scheme( GLLC_LLME_SCHEME_LLGMM_SUSPEND );

      /* Confirm to GMM that the suspend request has been completed. */

      gllc_llme_send_llgmm_suspend_cnf_mes();


      /* Suspension is now in force. Cancel any pending GRR-RESUME-CNF. */

      gllc_llme.is_resume_cnf_pending = FALSE;

    }

    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_UNASSIGNED:
  case GLLC_LLME_TEST_MODE:
  default:
    /* Illegal state identifier. */
    break;

  /* --------------------------------------------------------------------- */
  }

  /* ===================================================================== */

} /* end of gllc_llme_process_suspend_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_RESUME_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LLGMM-RESUME-REQ Service Primitive from GMM as a
===   function of LLME state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_process_resume_req_mes
(
  gmmllc_llgmm_resume_req_type *resume_req_mes
)
{
  /* ===================================================================== */

  MSG_GERAN_MED_3_G( "Rcvd LLGMM-RESUME-REQ, state%d, res_tlli%d, tlli%d", 
    gllc_llme.state, resume_req_mes -> tlli, gllc_llme.tlli);

  /* ===================================================================== */

  switch ( gllc_llme.state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_ASSIGNED:

    if ( resume_req_mes -> tlli == gllc_llme.tlli )
    {

      gllc_llme.is_resume_cnf_pending = TRUE;

      /* Extend the user channel resumption to the RLC UL process. */

      gllc_llme_send_grr_resume_req_mes();

      if (resume_req_mes->psm_restore && (gllc_psm_info.lle_info[GLLC_LL_SAPI_1_GMM].l2_info_valid))
      {
        gllc_restore_l2_psm_info(GLLC_LL_SAPI_1_GMM);
      }
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_UNASSIGNED:
  case GLLC_LLME_TEST_MODE:
  default:
    /* Illegal state identifier. */
    break;

  /* --------------------------------------------------------------------- */
  }

  /* ===================================================================== */
} /* end of gllc_llme_process_resume_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_STRT_RDY_TMR_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LLGMM-STRT-RDY-TMR-REQ proprietry Service Primitive from
===   GMM.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_process_strt_rdy_tmr_req_mes
(
  gmmllc_llgmm_strt_rdy_tmr_req_type *strt_rdy_tmr_req_mes
)
{
  if ( strt_rdy_tmr_req_mes -> tlli == gllc_llme.tlli )
  {
    MSG_GERAN_MED_1_G( "GLC ME: Rcvd STRT-RDY-TMR-REQ %dms",
      strt_rdy_tmr_req_mes -> timeout_ms);

    gllc_gmm_ready_timer_active = TRUE;
    gllc_gmm_ready_time         = strt_rdy_tmr_req_mes -> timeout_ms;
    (void)rex_set_timer( &gllc_gmm_ready_timer, gllc_gmm_ready_time );
    llc_log_ready_timer_status();
  }
  else
  {
    MSG_GERAN_LOW_1_G("GLC ME: STRT-RDY-TMR-REQ TLLI Err 0x%x", strt_rdy_tmr_req_mes -> tlli);
  }
} /* end of gllc_llme_process_strt_rdy_tmr_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_STOP_RDY_TMR_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LLGMM-STOP-RDY-TMR-REQ proprietry Service Primitive from
===   GMM.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_process_stop_rdy_tmr_req_mes
(
  gmmllc_llgmm_stop_rdy_tmr_req_type *stop_rdy_tmr_req_mes
)
{
  if ( stop_rdy_tmr_req_mes -> tlli == gllc_llme.tlli )
  {
    MSG_GERAN_MED_0_G("GLC ME: Rcvd STOP-RDY-TMR-REQ");

    gllc_gmm_ready_timer_active = FALSE;
    (void)rex_clr_timer( &gllc_gmm_ready_timer );
    (void)rex_clr_sigs( rex_self(), GLLC_GMM_RDY_TMR_SIG );
    llc_log_ready_timer_status();
  }
  else
  {
    MSG_GERAN_LOW_1_G("GLC ME: STOP-RDY-TMR-REQ TLLI Err 0x%x", stop_rdy_tmr_req_mes -> tlli);
  }
} /* end of gllc_llme_process_stop_rdy_tmr_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_TEST_MODE_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LLGMM-TEST-MODE-REQ proprietry Service Primitive from
===   GMM.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_process_test_mode_req_mes
(
  gmmllc_llgmm_test_mode_req_type *test_mode_req_mes
)
{
  uint32  lle;

  /* ===================================================================== */

  MSG_GERAN_HIGH_4_G( "Rcvd LLGMM-TEST-MODE-REQ - state=%d, suspended=%d, mode=%d PDUs=%d",
    gllc_llme.state,
    (gllc_llme_ul_evt_mask_scheme_flags[GLLC_LLME_SCHEME_LLGMM_SUSPEND]),
    test_mode_req_mes->test_mode,
    test_mode_req_mes->llc_pdu_num);

  /* ===================================================================== */


  /* ===================================================================== */

  switch ( gllc_llme.state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_ASSIGNED:
    gllc_llme.test_mode         = test_mode_req_mes -> test_mode;
    gllc_llme.loopback_mode     = test_mode_req_mes -> loopback_mode;
    gllc_llme.test_llc_pdu_num  = test_mode_req_mes -> llc_pdu_num;
    gllc_llme.test_llc_pdu_size = test_mode_req_mes -> llc_pdu_size;
    gllc_llme.ts_offset         = test_mode_req_mes -> ts_offset;

    if( gllc_llme.test_mode == 0 )
    {
      gllc_llme.test_llc_pdu_p =
        GPRS_MEM_MALLOC( gllc_llme.test_llc_pdu_size );

      if ( gllc_llme.test_llc_pdu_p != NULL )
      {
        /* Generate a dummy PDU containing pseudo-random data for use
           during the test procedure. */

        if (secapi_get_random( SECAPI_SECURE_RANDOM,
                               gllc_llme.test_llc_pdu_p,
                               gllc_llme.test_llc_pdu_size ) != E_SUCCESS)
        {
          ERR_GERAN_FATAL_0_G("GLC ME: secapi_get_random() returned FAILURE");
        }

        /* Clear T200 timer expiry events and stop T200 timers if active. T200 expiry
           events are not processed in Test Mode for any LLE. */

        for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
        {

          (void)rex_clr_timer( &gllc_timer_t200[lle] );
          GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T200_EVT );
          gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

        }

        /* Before entering Etsi Test Mode A, stop all UL processing. */

        gllc_llme_set_ul_event_mask_scheme( GLLC_LLME_SCHEME_TEST_MODE );

        (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_TEST_MODE_SIG );

        gllc_llme.state = GLLC_LLME_TEST_MODE;

#ifdef FEATURE_TC
        geran_send_cm_test_mode_on();
#endif /* FEATURE_TC */
      }
      else
        ERR_GERAN_FATAL_0_G( "GLC ME: Heap Exhaustion");
    }
    else if( (gllc_llme.test_mode == 0x01) || (gllc_llme.test_mode == 0x02) )
    {

      /* Clear T200 timer expiry events and stop T200 timers if active. T200 expiry
         events are not processed in Test Mode for any LLE. */

      for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
      {

        (void)rex_clr_timer( &gllc_timer_t200[lle] );
        GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T200_EVT );
        gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

      }

      /* Before entering Etsi Test Mode B or SRB, stop all UL processing. */

      gllc_llme_set_ul_event_mask_scheme( GLLC_LLME_SCHEME_TEST_MODE );

      (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_TEST_MODE_SIG );

      gllc_llme.state = GLLC_LLME_TEST_MODE;

#ifdef FEATURE_TC
      geran_send_cm_test_mode_on();
#endif /* FEATURE_TC */
    }
    else
    {
      MSG_GERAN_LOW_1_G("GLC ME: LLGMM-TEST-MODE-REQ Mode Err %d", gllc_llme.test_mode);
    }

    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLME_TLLI_UNASSIGNED:
  case GLLC_LLME_TEST_MODE:
  default:

    /* Illegal state. */
    break;

  /* --------------------------------------------------------------------- */
  }

  /* ===================================================================== */
} /* end of gllc_llme_process_test_mode_req_mes() */

#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_PSHO_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LLGMM-PSHO-REQ proprietry Service Primitive from
===   GMM.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gllc_llme_process_psho_req_mes
(
   gmmllc_llgmm_psho_req_type *psho_req_mes
)
{
  uint32 iov_ui;

  MSG_GERAN_MED_0_G( "GLC ME: Rcvd LLGMM-PSHO-REQ");

  {

  /* Convert IOV_UI into a 32-bit value for later use */
  iov_ui =
    (
      (uint32)(psho_req_mes -> nas_container.iov_ui[GLLC_IOV_UI_SIZE-4] << 24) |
      (uint32)(psho_req_mes -> nas_container.iov_ui[GLLC_IOV_UI_SIZE-3] << 16) |
      (uint32)(psho_req_mes -> nas_container.iov_ui[GLLC_IOV_UI_SIZE-2] <<  8) |
      (uint32)(psho_req_mes -> nas_container.iov_ui[GLLC_IOV_UI_SIZE-1] <<  0)
    );

  /* Update LLME with received old_xid_indicator parameter */
  gllc_llme.use_old_xid_ind = psho_req_mes -> nas_container.use_old_xid_ind;

  /* Update LLME with new ciphering algorithm */
  gllc_llme.gea             = psho_req_mes -> nas_container.gea;

  /* Check assignment GEA algorithm against that which is supported. */
  if ( !gllc_is_gea_supported( psho_req_mes -> nas_container.gea ) )
  {
    MSG_GERAN_LOW_1_G( "GLC ME: LLGMM-PSHO-REQ GEA Err %d",
      psho_req_mes -> nas_container.gea);
    gllc_llme.gea           = GMMLLC_GEA_NONE;
  }

    /* Prepare a pseudo XID command as if it was received from network. This
       XID command contains XID_RESET and IOV_UI parameters only */
  gllc_ul_xid_cmd_psho_put_mes( iov_ui );

  /* Send a psho cnf to GMM */
  gllc_llme_send_llgmm_psho_cnf_mes();
  }

} /* gllc_llme_process_psho_req_mes() */

#endif /* FEATURE_GPRS_PS_HANDOVER */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_GRR_STATUS_IND_MES()
===
===  DESCRIPTION
===
===   Processes an GRR-STATUS-IND Service Primitive from RLC.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_process_grr_status_ind_mes
(
  gprs_grr_dl_mes_t *status_ind_mes
)
{
    MSG_GERAN_HIGH_3_G( "GLC ME: Rcvd GRR-STAT-IND : pdu cause %d, TI %d tx cause=%d",
      status_ind_mes -> pdu_cause, status_ind_mes -> transaction_id, status_ind_mes -> cause );

  switch ( status_ind_mes -> pdu_cause )
  {

  case GRR_CAUSE_GMM_SIGNALLING:

    gllc_llme_send_llgmm_status_ind_mes
    (
      PD_GMM,
      status_ind_mes -> transaction_id,
      status_ind_mes -> cause
    );

    break;

  /* ..................................................................... */

  case GRR_CAUSE_SM_SIGNALLING:

    gllc_llme_send_llgmm_status_ind_mes
    (
      PD_GPRS_SM,
      status_ind_mes -> transaction_id,
      status_ind_mes -> cause
    );

    break;

  /* ..................................................................... */

  default:

    break;
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_llme_process_grr_status_ind_mes() */




/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_GRR_RESUME_CNF_MES()
===
===  DESCRIPTION
===
===   Processes an GRR-RESUME-CNF Service Primitive from RLC.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_process_grr_resume_cnf_mes (void)
{

  uint32   lle;
  boolean  b_gcf_flag = FALSE;
  boolean  b_qos_enabled_flag = FALSE;

  /* --------------------------------------------------------------------- */

  if ( gllc_llme.is_resume_cnf_pending )
  {


    gllc_llme.is_resume_cnf_pending = FALSE;


    /* RLC is now resumed and in data transfer mode. */

    /* Clear down the 'LLGMM Suspend' event masking scheme which is
       in force due to a previous suspend order from GMM. */

    gllc_llme_clr_ul_event_mask_scheme( GLLC_LLME_SCHEME_LLGMM_SUSPEND );


    /* Read gcf flag setting. */

    rr_read_public_store( RRPS_MS_ANITE_GCF_FLAG, &b_gcf_flag );
    
    /* Read QoS enabled flag setting. */

    b_qos_enabled_flag = ds3gpp_is_ue_init_qos_supported();


    MSG_GERAN_HIGH_2_G( "Rcvd GRR-RESUME-CNF: gcf_flag %d, qos_flag %d ",b_gcf_flag, b_qos_enabled_flag );


    /* Run through all the LLEs to determine if a timer T201 association
       is in force, thereby indicating that T201 for the LLE was running
       before the GMM suspension order. If so then restart the timer. */

    for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
    {
      /* The assumption is that if the LLE has an active T201 association,
         then it must be in the correct state to have the timer restarted. */

      if ( gllc_lle[lle].ack.T201_association != NULL )
      {
        (void)rex_set_timer( &gllc_timer_t201[lle], gllc_lle[lle].ack.retrx_interval_T201 );

        GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );
      }

      /* If this user plane suspension session was for an inter-SGSN
         RAU, then an XID Reset would have reset parameters back to defaults.

         If there are any layer 2 XID parameters for which a negotiation
         requirement is pending, and there is an active PDP associated
         with this LLE, then generate an XID request and transfer it to
         the peer LLE with an XID Command Message.

         After resuming, initiate negotiation for outstanding N201-U for SAPI-1
         if gcf flag is not set and qos enabled flag is set. */

      if ( ( ( gllc_lle[lle].state            == GLLC_LLE_ADM ) &&
             ( gllc_lle[lle].gen.pdp_active                   )    ) ||
           ( ( gllc_lle[lle].state            == GLLC_LLE_ADM ) &&
             ( lle == GLLC_LL_SAPI_1_GMM                              ) &&
             ( !b_gcf_flag   &&   b_qos_enabled_flag                  )    )
         )
      {
        if ( gllc_lle_generate_l2_xid_req( (gllc_ll_sapi_t)lle ) )
        {
          MSG_GERAN_LOW_3_G("GLC LL %d: psm_restore=%d, l2_info_valid=%d", 
            gllc_sapi_addr_lookup[lle], gllc_lle[lle].gen.psm_restore, gllc_psm_info.lle_info[lle].l2_info_valid);

          /* If PSM restore is pending then do not send XID cmd to the n/w,
           * rather create XID rsp frame by retrieving data from EFS */
          if (gllc_lle[lle].gen.psm_restore && gllc_psm_info.lle_info[lle].l2_info_valid)
          {
            gllc_lle_internal_u_frame_rsp(lle, GLLC_XID);
  
            /* restore encipher_oc/decipher_oc */
            gllc_lle[lle].unack.encipher_oc = gllc_psm_info.lle_info[lle].l2_info.unack_encipher_oc;
            gllc_lle[lle].ack.encipher_oc   = gllc_psm_info.lle_info[lle].l2_info.ack_encipher_oc;
            gllc_lle[lle].unack.decipher_oc = gllc_psm_info.lle_info[lle].l2_info.unack_decipher_oc;
            gllc_lle[lle].ack.decipher_oc   = gllc_psm_info.lle_info[lle].l2_info.ack_decipher_oc;
  
            gllc_psm_info.lle_info[lle].l2_info_valid = FALSE;
            gllc_lle[lle].gen.psm_restore = FALSE;
          }
          else
          {
            gllc_lle_send_xid_cmd_mes( lle, NULL );
          }
          gllc_lle[lle].substate = GLLC_LLE_L2_XID_EXCHANGE;
        }
      }
    }


    /* Resume UL user data flow. */

    /* Resume SAPI-3 */

    if ( gllc_lle[ GLLC_LL_SAPI_3_LL3 ].gen.pdp_active == TRUE )
    {
      gsn_ll3_ul_unack_lowater_nas_susp();
      gsn_ll3_ul_ack_lowater_nas_susp();
    }


    /* Suspend SAPI-5 */

    if ( gllc_lle[ GLLC_LL_SAPI_5_LL5 ].gen.pdp_active == TRUE )
    {
      gsn_ll5_ul_unack_lowater_nas_susp();
      gsn_ll5_ul_ack_lowater_nas_susp();
    }


    /* Suspend SAPI-9 */

    if ( gllc_lle[ GLLC_LL_SAPI_9_LL9 ].gen.pdp_active == TRUE )
    {
      gsn_ll9_ul_unack_lowater_nas_susp();
      gsn_ll9_ul_ack_lowater_nas_susp();
    }


    /* Suspend SAPI-11 */

    if ( gllc_lle[ GLLC_LL_SAPI_11_LL11 ].gen.pdp_active == TRUE )
    {
      gsn_ll11_ul_unack_lowater_nas_susp();
      gsn_ll11_ul_ack_lowater_nas_susp();
    }
  }
  else
  {

    MSG_GERAN_LOW_0_G( "GLC ME: GRR-RESUME-CNF is unexpected" );

  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_llme_process_grr_resume_cnf_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_XID_RESET_PROCEDURES()
===
===  DESCRIPTION
===
===   For use on receipt of the XID Reset parameter. Resets all LLEs whilst
===   and leaves the LLE with the state set as ADM.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_llme_xid_reset_procedures( uint32 rxd_tlli )
{
  uint32 lle;

  /* Suspend UL flow immediately. SNDCP should not send any new UL PDUs to
     LLC. UL flow will be enabled after receiving LL_RESET_RESP from SNDCP.
     UL PDU, if any, that SNDCP is dealing with currently, should be
     discarded when it arrives in LLC as it will have header info prior to
     reset. */

  gllc_llme.xid_reset_in_progress = GLLC_LLME_XID_RESET_INACTIVE;

  /* Suspend SAPI-3 */
  if ( gllc_lle[ GLLC_LL_SAPI_3_LL3 ].gen.pdp_active == TRUE )
  {
    gllc_llme.xid_reset_in_progress |= GLLC_LLME_XID_RESET_SAPI_3;

    if ( gllc_lle[ GLLC_LL_SAPI_3_LL3 ].state == GLLC_LLE_ADM )
    {
      gsn_ll3_ul_unack_hiwater_xid_reset();
    }

    if ( gllc_lle[ GLLC_LL_SAPI_3_LL3 ].state == GLLC_LLE_ABM )
    {
      gsn_ll3_ul_ack_hiwater_xid_reset();
    }
  }

  /* Suspend SAPI-5 */
  if ( gllc_lle[ GLLC_LL_SAPI_5_LL5 ].gen.pdp_active == TRUE )
  {
    gllc_llme.xid_reset_in_progress |= GLLC_LLME_XID_RESET_SAPI_5;

    if ( gllc_lle[ GLLC_LL_SAPI_5_LL5 ].state == GLLC_LLE_ADM )
    {
      gsn_ll5_ul_unack_hiwater_xid_reset();
    }

    if ( gllc_lle[ GLLC_LL_SAPI_5_LL5 ].state == GLLC_LLE_ABM )
    {
      gsn_ll5_ul_ack_hiwater_xid_reset();
    }
  }

  /* Suspend SAPI-9 */
  if ( gllc_lle[ GLLC_LL_SAPI_9_LL9 ].gen.pdp_active == TRUE )
  {
    gllc_llme.xid_reset_in_progress |= GLLC_LLME_XID_RESET_SAPI_9;

    if ( gllc_lle[ GLLC_LL_SAPI_9_LL9 ].state == GLLC_LLE_ADM )
    {
      gsn_ll9_ul_unack_hiwater_xid_reset();
    }

    if ( gllc_lle[ GLLC_LL_SAPI_9_LL9 ].state == GLLC_LLE_ABM )
    {
      gsn_ll9_ul_ack_hiwater_xid_reset();
    }
  }

  /* Suspend SAPI-11 */
  if ( gllc_lle[ GLLC_LL_SAPI_11_LL11 ].gen.pdp_active == TRUE )
  {
    gllc_llme.xid_reset_in_progress |= GLLC_LLME_XID_RESET_SAPI_11;

    if ( gllc_lle[ GLLC_LL_SAPI_11_LL11 ].state == GLLC_LLE_ADM )
    {
      gsn_ll11_ul_unack_hiwater_xid_reset();
    }

    if ( gllc_lle[ GLLC_LL_SAPI_11_LL11 ].state == GLLC_LLE_ABM )
    {
      gsn_ll11_ul_ack_hiwater_xid_reset();
    }
  }

  MSG_GERAN_LOW_1_G( "GLC ME: xid reset in progress: %d",
    gllc_llme.xid_reset_in_progress );


  for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
  {
    gllc_lle_reset( (gllc_ll_sapi_t)lle, GLLC_LLE_ADM );

    /* Set the 'Wait ABM' masking scheme as required in the ADM state. */

    gllc_lle_set_ul_event_mask_scheme( (gllc_ll_sapi_t)lle, GLLC_LLE_SCHEME_WAIT_ABM );

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
    /* For unit test convenience purposes only, do not send LL-RESET-IND
       and LL-XID-IND message relating to all LLEs. */

    if ( gllc_ll_client_record[lle].ll_dl_sig_get_mes_buf_fnc_ptr != NULL )
    {
      gllc_lle_send_ll_rst_ind_mes( (gllc_ll_sapi_t)lle, rxd_tlli );
      gllc_lle_send_ll_xid_ind_mes( (gllc_ll_sapi_t)lle, NULL, rxd_tlli );
    }
#endif
  }

  gllc_llme_reset_l2_xid_parameters();

  if ( gllc_llme_ul_evt_mask_scheme_flags[ GLLC_LLME_SCHEME_LLGMM_SUSPEND ] )
    gllc_llme_send_grr_purge_suspended_data_req_mes();

#ifdef FEATURE_GPRS_PS_HANDOVER
  /* Set use_old_xid_ind in LLME to FALSE. This is to avoid a RESET with
     current XID values for a subsequent RESET request. */

  gllc_llme.use_old_xid_ind = FALSE;
#endif /* FEATURE_GPRS_PS_HANDOVER */

} /* end of gllc_llme_xid_reset_procedures() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_EST_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LL-ESTABLISH-REQ Service Primitive from SNDCP as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_est_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_establish_req_t  *est_req_mes
)
{
  /* ===================================================================== */

  if ( !GLLC_LLE_IS_3_5_9_11(lle) )
  {
    MSG_GERAN_LOW_1_G( "GLC LL %d: LL-EST-REQ SAPI Err",
      gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    MSG_GERAN_MED_2_G( "GLC LL %d: Rcvd LL-EST-REQ ADM, state:%d",
      gllc_sapi_addr_lookup[lle],
      gllc_lle[lle].state);

    switch (gllc_lle[lle].state)
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    /* LL Est Req invalid in state TLLI Unassigned. */

    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:

    /* Valid layer 3 establishment request.  */


    if ( gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE )
    {
      /* Layer 3 peer to peer collision between this LL Est Req and an
         LL XID Ind has occured. Layer 3 contention resolution procedures
         require that the LL XID Ind sent by LLC be ignored, so this
         LL Est Req must be acted upon. Due to abnormal message sequencing,
         the 'Wait Layer 3 Resp' event masking scheme be cleared explicitly
         before proceeding. */

      gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP);
    }

    gllc_lle[lle].ack.peak_throughput   = est_req_mes -> peak_throughput;
    gllc_lle[lle].ack.radio_priority    = est_req_mes -> radio_priority;
    gllc_lle[lle].unnum.peak_throughput = est_req_mes -> peak_throughput;
    gllc_lle[lle].unnum.radio_priority  = est_req_mes -> radio_priority;
    gllc_lle[lle].ack.pfi               = est_req_mes -> pfi;
    gllc_lle[lle].unnum.pfi             = est_req_mes -> pfi;

    /* Generate an XID request for all XID parameters that can be sent on
       the uplink and send this in a SABM command to peer LLE, with or
       without layer 3 XID parameters. Move to the loc est state to await a
       UA response from the peer LLE. */

    gllc_lle[lle].state                      = GLLC_LLE_LOC_ESTABLISH;
    gllc_lle[lle].substate                   = GLLC_LLE_L3_ESTABLISH;
    gllc_lle[lle].unnum.disc_rcvd_in_loc_est = FALSE;

    gllc_lle[lle].unnum.xid_pending |= GLLC_ALL_UL_XID;
    (void)gllc_lle_generate_l2_xid_req( lle );
    gllc_lle_send_sabm_cmd_mes( lle, &est_req_mes -> l3_xid_req );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:
        MSG_GERAN_LOW_2_G( "GLC LL %d: Rcvd LL-EST-REQ %d",
          gllc_sapi_addr_lookup[lle],
          gllc_lle[lle].substate);

    if ( gllc_lle[lle].substate == GLLC_LLE_L2_ESTABLISH )
    {
      /* Layer 3 has requested a link re-establishment during a pending
         layer 2 initiated re-establishment. Change substate to reflect
         this and allow the process to continue without worrying about
         any layer 3 XID. */

      gllc_lle[lle].substate = GLLC_LLE_L3_ESTABLISH;
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* LL Est Req invalid in state Rem Est. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ABM:

    /* Layer 3 initiated re-establishment request. */

    if ( gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE )
    {
      /* Layer 3 peer to peer collision between this LL Est Req and an
         LL XID Ind has occured. Layer 3 contention resolution procedures
         require that the LL XID Ind sent by LLC be ignored, so this
         LL Est Req must be acted upon. Due to abnormal message sequencing,
         the 'Wait Layer 3 Resp' event masking scheme be cleared explicitly
         before proceeding. */

      gllc_lle_clr_ul_event_mask_scheme(lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP);
    }

    gllc_lle[lle].ack.peak_throughput   = est_req_mes -> peak_throughput;
    gllc_lle[lle].ack.radio_priority    = est_req_mes -> radio_priority;
    gllc_lle[lle].unnum.peak_throughput = est_req_mes -> peak_throughput;
    gllc_lle[lle].unnum.radio_priority  = est_req_mes -> radio_priority;
    gllc_lle[lle].ack.pfi               = est_req_mes -> pfi;
    gllc_lle[lle].unnum.pfi             = est_req_mes -> pfi;
    /* Initiate L3 reestablishment procedures. */

    gllc_lle_initiate_reestablishment( lle, &est_req_mes -> l3_xid_req );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* LL Est Req invalid in state Loc Rel. */

    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */

    break;
    }
  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */

} /* end of gllc_lle_process_est_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_EST_RES_MES()
===
===  DESCRIPTION
===
===   Processes an LL-ESTABLISH-RES Service Primitive from SNDCP as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_est_res_mes
(
  gllc_ll_sapi_t      lle,
  ll_establish_res_t  *est_res_mes
)
{
  /* ===================================================================== */

  if ( !GLLC_LLE_IS_3_5_9_11(lle) )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: LL-EST-RES SAPI Err",
      gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    MSG_GERAN_MED_2_G( "GLC LL %d: Rcvd LL-EST-RES state: %d",
      gllc_sapi_addr_lookup[lle],
      gllc_lle[lle].state);

    switch (gllc_lle[lle].state)
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:
      case GLLC_LLE_ABM:
      case GLLC_LLE_ADM:
      case GLLC_LLE_LOC_ESTABLISH:
      case GLLC_LLE_LOC_RELEASE:
        break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* Valid layer 3 remote establishment response. Set any negotiated
       layer 2 parameters, inform layer 3 of any change in N201, and send a
       UA response to the peer LLE and enter ABM. */
    gllc_lle[lle].ack.peak_throughput   = est_res_mes -> peak_throughput;
    gllc_lle[lle].ack.radio_priority    = est_res_mes -> radio_priority;
    gllc_lle[lle].unnum.peak_throughput = est_res_mes -> peak_throughput;
    gllc_lle[lle].unnum.radio_priority  = est_res_mes -> radio_priority;
    gllc_lle[lle].ack.pfi               = est_res_mes -> pfi;
    gllc_lle[lle].unnum.pfi             = est_res_mes -> pfi;

    gllc_lle_set_l2_xid_parameters( lle, NULL );
    gllc_lle_send_ll_xid_ind_mes( lle, NULL, GLLC_TLLI_UNASSIGNED );
    gllc_lle_send_ua_res_mes( lle, &est_res_mes -> l3_xid_neg );
    gllc_lle_enter_state_abm( lle );

    /* Entered ABM. Now clear ul suspension due to est_susp. */
    gllc_lle_clr_est_susp( lle );

    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */
    break;
    }
  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */

} /* end of gllc_lle_process_est_res_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_REL_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LL-RELEASE-REQ Service Primitive from SNDCP as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_rel_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_release_req_t    *rel_req_mes
)
{
  /* ===================================================================== */

  if ( !GLLC_LLE_IS_3_5_9_11(lle) )
  {
    MSG_GERAN_LOW_1_G( "GLC LL %d: LL-REL-REQ SAPI Err",
      gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    MSG_GERAN_MED_2_G( "GLC LL %d: Rcvd LL-REL-REQ state: %d",
      gllc_sapi_addr_lookup[lle],
      gllc_lle[lle].state);

    switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    /* LL Rel Req invalid in state TLLI Unassigned, unless a local
       operation. If the release request local parameter is TRUE then
       prepare to send LL_RELEASE_CNF to layer 3, else report error. */

    if (rel_req_mes->local == TRUE)
    {
      if (rel_req_mes->cnf_needed)
      {
        gllc_lle_send_ll_rel_cnf_mes(lle, gllc_llme.tlli);
      }
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:

    /* LL Rel Req is generally invalid in state ADM, but may need to be
       confirmed to cater for race conditions between local layer 3 PDP
       Deactivation and peer layer 2 activity. */
    if (rel_req_mes->cnf_needed)
    {
      gllc_lle_send_ll_rel_cnf_mes(lle, gllc_llme.tlli);
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* LL Rel Req is generally invalid in state Rem Est, but may need to be
       confirmed to cater for race conditions between local layer 3 PDP
       Deactivation and peer layer 2 activity. */
    if (rel_req_mes->cnf_needed)
    {
      gllc_lle_send_ll_rel_cnf_mes(lle, gllc_llme.tlli);
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:
  case GLLC_LLE_ABM:

    /* Valid layer 3 release request. */

      if ( gllc_lle[lle].state == GLLC_LLE_ABM )
    {
      if ( gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE )
      {
        /* Layer 3 peer to peer collision between this LL Rel Req and an
           LL XID Ind has occured. Layer 3 contention resolution procedures
           require that the LL XID Ind sent by LLC be ignored, so this
           LL Rel Req must be acted upon. Due to abnormal message sequencing,
           the 'Wait Layer 3 Resp' event masking scheme be cleared explicitly
           before proceeding. */

        gllc_lle_clr_ul_event_mask_scheme(lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP);
      }
    }

    /* If the release request local parameter is TRUE then prepare to send
       LL_RELEASE_CNF to layer 3, else prepare to send a DISC command to
       peer LLE. */

    if ( rel_req_mes -> local == TRUE )
    {
      /* Move directly to the ADM State and signal layer 3. */

      (void)rex_clr_timer( &gllc_timer_t201[lle] );
      gllc_lle[lle].ack.T201_association = NULL;
      GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );

      if (rel_req_mes->cnf_needed)
      {
        gllc_lle_send_ll_rel_cnf_mes(lle, gllc_llme.tlli);
      }
      gllc_lle_enter_state_adm( lle );
    }
    else
    {
      /* Move to the Local Release State. */

      (void)rex_clr_timer( &gllc_timer_t201[lle] );
      gllc_lle[lle].ack.T201_association = NULL;
      GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );

      gllc_lle_send_disc_cmd_mes( lle );

      gllc_lle_set_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_ABM );

      gllc_lle[lle].state    = GLLC_LLE_LOC_RELEASE;
      gllc_lle[lle].substate = GLLC_LLE_NONE;
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* LL Rel Req invalid in state Loc Rel. */

    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */

    break;
    }
  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */

} /* end of gllc_lle_process_rel_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_ACT_IND_MES()
===
===  DESCRIPTION
===
===   Processes an LL-ACTIVATE-REQ Service Primitive from Layer 3 as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_act_ind_mes
(
  gllc_ll_sapi_t      lle,
  ll_activate_ind_t   *act_ind_mes
)
{
  /* ===================================================================== */

  /* ===================================================================== */

  if ( !GLLC_LLE_IS_3_5_9_11(lle) )
  {
    MSG_GERAN_LOW_1_G( "GLC LL %d: LL-ACT-IND SAPI Err",
      gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    MSG_GERAN_MED_2_G( "GLC LL %d: Rcvd LL-ACT-IND state %d",
      gllc_sapi_addr_lookup[lle],
      gllc_lle[lle].state);

    switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:

    /* LL Act Ind received in state ADM. Record PDP state and initiate L2
       XID negotiation of any pending parameters. */
    if (act_ind_mes != NULL)
    {
      gllc_lle[lle].unack.peak_throughput = act_ind_mes -> peak_throughput;
      gllc_lle[lle].unack.radio_priority  = act_ind_mes -> radio_priority;
      gllc_lle[lle].unnum.peak_throughput = act_ind_mes -> peak_throughput;
      gllc_lle[lle].unnum.radio_priority  = act_ind_mes -> radio_priority;
      gllc_lle[lle].unack.pfi             = act_ind_mes -> pfi;
      gllc_lle[lle].unnum.pfi             = act_ind_mes -> pfi;
  
      gllc_lle[lle].gen.pdp_active  = TRUE;
      gllc_lle[lle].gen.psm_restore = act_ind_mes->psm_restore;
  
      gllc_lle[lle].gen.is_grr_purge_data_req_enabled = TRUE;
  
      /* If an 'LLGMM Suspend' event masking scheme is in force then delay
         L2 negotiation of pending parameters until LLGMM Resume is issued. */
  
      if (!gllc_llme_ul_evt_mask_scheme_flags[GLLC_LLME_SCHEME_LLGMM_SUSPEND])
      {
        if (gllc_lle_generate_l2_xid_req(lle))
        {
          MSG_GERAN_LOW_3_G("GLC LL %d: psm_restore=%d, l2_info_valid=%d", 
            gllc_sapi_addr_lookup[lle], act_ind_mes->psm_restore, gllc_psm_info.lle_info[lle].l2_info_valid);
  
          /* If ACTIVATION is being done as part of PSM exit then do not send XID cmd to the n/w,
           * rather create XID rsp frame by retrieving data from EFS */
          if (act_ind_mes->psm_restore && gllc_psm_info.lle_info[lle].l2_info_valid)
          {
            gllc_lle_internal_u_frame_rsp(lle, GLLC_XID);
  
            /* restore encipher_oc/decipher_oc */
            gllc_lle[lle].unack.encipher_oc = gllc_psm_info.lle_info[lle].l2_info.unack_encipher_oc;
            gllc_lle[lle].ack.encipher_oc   = gllc_psm_info.lle_info[lle].l2_info.ack_encipher_oc;
            gllc_lle[lle].unack.decipher_oc = gllc_psm_info.lle_info[lle].l2_info.unack_decipher_oc;
            gllc_lle[lle].ack.decipher_oc   = gllc_psm_info.lle_info[lle].l2_info.ack_decipher_oc;
  
            gllc_psm_info.lle_info[lle].l2_info_valid = FALSE;
            gllc_lle[lle].gen.psm_restore = FALSE;
          }
          else
          {
            gllc_lle_send_xid_cmd_mes(lle, NULL);
          }
          gllc_lle[lle].substate = GLLC_LLE_L2_XID_EXCHANGE;
        }
      }

      /* Print llme and lle cipher parameters. */
      gllc_debug_print_llme_cipher_params();
      gllc_debug_print_lle_cipher_params (lle);
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:
  case GLLC_LLE_LOC_ESTABLISH:
  case GLLC_LLE_REM_ESTABLISH:
  case GLLC_LLE_ABM:
  case GLLC_LLE_LOC_RELEASE:
    /* LL Act Ind invalid these states. */
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */
    break;
    }
  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */

} /* end of gllc_lle_process_act_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_DEACT_IND_MES()
===
===  DESCRIPTION
===
===   Processes an LL-DEACTIVATE-REQ Service Primitive from Layer 3 as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_deact_ind_mes
(
  gllc_ll_sapi_t      lle,
  ll_deactivate_ind_t *deact_ind_mes
)
{
  /* ===================================================================== */

  /* ===================================================================== */

  if ( !GLLC_LLE_IS_3_5_9_11(lle) )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: LL-DEACT-IND SAPI Err",
      gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    MSG_GERAN_HIGH_2_G("GLC LL %d: Rcvd LL-DEACT-IND State %d",
      gllc_sapi_addr_lookup[lle],
      gllc_lle[lle].state);

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:

    /* LL Deact Ind received in state ADM. Record PDP state and flush
       appropriate data queues. */
    gllc_lle[lle].gen.pdp_active = FALSE;

    gllc_lle_flush_ll_ul_unack_q( lle );
    gllc_lle_flush_ll_ul_ack_q( lle );

    /* Clear any XID send event. */

    if ( gllc_lle_chk_ul_event_mask_scheme ( lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP ) )
    {
 
      /* Clear U Frame buffer. */

      if ( gllc_lle[lle].unnum.cmd_frame.u_frame.
             info_field.xid_info.l3_xid.params != NULL )
      {
        GPRS_PDU_FREE(
          &gllc_lle[lle].unnum.cmd_frame.u_frame.
            info_field.xid_info.l3_xid.params );
      }

      (void)memset
      (
        &gllc_lle[lle].unnum.cmd_frame,
        0,
        ( sizeof(gllc_frame_t) / sizeof(uint8) )
      );
      
      GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_U_FR_CMD_EVT );          

      MSG_GERAN_HIGH_1_G( "GLC LL %d: XID PDU deleted",
                  gllc_sapi_addr_lookup[lle] );

    }

    /* Stop T200 timer if running. */

    (void)rex_clr_timer( &gllc_timer_t200[lle] );
    GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

    /* Send GRR-PURGE-DATA-REQ to GRLC which will purge all PDUs that
       belong to the SAPI indicated. Purge is only required if flag is TRUE. */

    if ( gllc_lle[lle].gen.is_grr_purge_data_req_enabled )
    {
      gllc_llme_send_grr_purge_data_req_mes( lle );
    }

    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:
  case GLLC_LLE_LOC_ESTABLISH:
  case GLLC_LLE_REM_ESTABLISH:
  case GLLC_LLE_ABM:
  case GLLC_LLE_LOC_RELEASE:
    /* LL Deact Ind invalid these states. */
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */
    break;
    }
  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */

} /* end of gllc_lle_process_deact_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_XID_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LL-XID-REQ Service Primitive from Layer 3 as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_xid_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_xid_req_t        *xid_req_mes
)
{
    MSG_GERAN_MED_2_G( "GLC LL %d: LL-XID-REQ State %d",
      gllc_sapi_addr_lookup[lle], gllc_lle[lle].state );

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    if ( lle != GLLC_LL_SAPI_1_GMM )
    {
      /* LL XID Res invalid in state TLLI Unassigned for other LL SAPIs. */
      break;
    }

    /* Drop through if lle == GLLC_LL_SAPI_1_GMM. */

  /* ..................................................................... */

  case GLLC_LLE_ADM:
  case GLLC_LLE_ABM:

    /* For SAPIs 3,5,9 and 11 this request can be implicitly taken as an
       indication from SNDCP that a PDP Context has been activated. */

    if ( GLLC_LLE_IS_3_5_9_11(lle) )
      gllc_lle[lle].gen.pdp_active = TRUE;

    if ( gllc_lle[lle].substate != GLLC_LLE_L3_XID_EXCHANGE )
    {
      /* Valid layer 3 LL XID Req so generate any pending layer 2 XID
         negotiation request which can be piggybacked with the same
         command and send to the peer LLE. */
      gllc_lle[lle].unack.peak_throughput = xid_req_mes -> peak_throughput;
      gllc_lle[lle].unack.radio_priority  = xid_req_mes -> radio_priority;
      gllc_lle[lle].unnum.peak_throughput = xid_req_mes -> peak_throughput;
      gllc_lle[lle].unnum.radio_priority  = xid_req_mes -> radio_priority;
      gllc_lle[lle].unack.pfi             = xid_req_mes -> pfi;
      gllc_lle[lle].unnum.pfi             = xid_req_mes -> pfi;

      (void)gllc_lle_generate_l2_xid_req( lle );
      gllc_lle_send_xid_cmd_mes( lle, &xid_req_mes -> l3_xid_req );
      gllc_lle[lle].substate = GLLC_LLE_L3_XID_EXCHANGE;
    }
    else
    {
      /* Substate is GLLC_LLE_L3_XID_EXCHANGE and the LL SAP Signalling
         queue event is unmasked whilst awaiting an LL XID Res from layer
         3. This represents a layer 3 peer to peer XID collision. Ignore
         as layer 3 XID contention resolution procedures will handle. */

      MSG_GERAN_HIGH_2_G( "GLC LL %d: LL XID Req/Ind Collision in State %d",
        gllc_sapi_addr_lookup[lle], gllc_lle[lle].state );
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:

    /* LL XID Req invalid in state Loc Est. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* LL XID Req invalid in state Rem Est. Represents a layer 3 peer to
       peer collision between an LL XID Req and an LL Est Ind primitive.
       Layer 3 contention resolution procedures should manage this. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* LL XID Req invalid in state Loc Rel. */
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */
    break;

  /* --------------------------------------------------------------------- */
  }

} /* end of gllc_lle_process_xid_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_XID_RES_MES()
===
===  DESCRIPTION
===
===   Processes an LL-XID-RES Service Primitive from Layer 3 as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_xid_res_mes
(
  gllc_ll_sapi_t      lle,
  ll_xid_res_t        *xid_res_mes
)
{
    MSG_GERAN_MED_2_G("GLC LL %d: LL-XID-RES State: %d",
      gllc_sapi_addr_lookup[lle], gllc_lle[lle].state);

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    if ( lle != GLLC_LL_SAPI_1_GMM )
    {
      /* LL XID Res invalid in state TLLI Unassigned for other LL SAPIs. */
      break;
    }

    /* Drop through if lle == GLLC_LL_SAPI_1_GMM. */

  /* ..................................................................... */

  case GLLC_LLE_ADM:
  case GLLC_LLE_ABM:

    /* Valid layer 3 LL Xid Res so send an XID response to the peer LLE
       whilst combining the layer 2 XID negotiation response generated
       earlier with the layer 3 negotiated response. */

    if ( gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE )
    {
      gllc_lle[lle].unack.peak_throughput = xid_res_mes -> peak_throughput;
      gllc_lle[lle].unack.radio_priority  = xid_res_mes -> radio_priority;
      gllc_lle[lle].unnum.peak_throughput = xid_res_mes -> peak_throughput;
      gllc_lle[lle].unnum.radio_priority  = xid_res_mes -> radio_priority;
      gllc_lle[lle].unack.pfi             = xid_res_mes -> pfi;
      gllc_lle[lle].unnum.pfi             = xid_res_mes -> pfi;

      gllc_lle_set_l2_xid_parameters( lle, NULL );
      gllc_lle_send_ll_xid_ind_mes( lle, NULL, GLLC_TLLI_UNASSIGNED );
      gllc_lle_send_xid_res_mes( lle, &xid_res_mes -> l3_xid_neg );

      /* If there are pending (outstanding) parameters awaiting negotiation
         then initiate the negotiation now. */

      if ( gllc_lle_generate_l2_xid_req( lle) )
      {
        gllc_lle_send_xid_cmd_mes( lle, NULL );
        gllc_lle[lle].substate = GLLC_LLE_L2_XID_EXCHANGE;
      }
      else
        gllc_lle[lle].substate = GLLC_LLE_NONE;
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:
  case GLLC_LLE_REM_ESTABLISH:
  case GLLC_LLE_LOC_RELEASE:
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */

    break;

  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */

} /* end of gllc_lle_process_xid_res_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_INACT_SAPI_RES_MES()
===
===  DESCRIPTION
===
===   Processes an LL-INACT-SAPI-RES Service Primitive from Layer 3 as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_inact_sapi_res_mes
(
  gllc_ll_sapi_t      lle
)
{
  /* ===================================================================== */
  MSG_GERAN_MED_2_G("GLC LL %d: LL_INACT_SAPI_RES State: %d",
    gllc_sapi_addr_lookup[lle], gllc_lle[lle].state);

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    /* LL INACT SAPI Res invalid in state TLLI Unassigned. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:
  case GLLC_LLE_ABM:

    if ( gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE )
    {
      /* The LL INACT SAPI Res has been received in response to an
         LL XID Ind which was sent with layer 3 content, and for which
         the indicated SAPI was considered inactive by layer 3. */
      /* Abort the XID exchange. */

      gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP );

      gllc_lle[lle].unnum.xid_pending = 0;
      gllc_lle[lle].substate          = GLLC_LLE_NONE;
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:

    /* LL INACT SAPI Res invalid in state Loc Est. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* The LL INACT SAPI Res has been received in response to an LL EST Ind
       and for which the indicated SAPI was considered inactive by layer 3 */
    /* Abort the remote establishment. */

    gllc_lle[lle].unnum.xid_pending = 0;
    gllc_lle_enter_state_adm( lle );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* LL INACT SAPI Res invalid in state Loc Rel. */
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */
    break;

  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */

} /* end of gllc_lle_process_inact_sapi_res_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_RESET_RES_MES()
===
===  DESCRIPTION
===
===   Processes an LL-RESET-RES Service Primitive from SNDCP as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_reset_res_mes
(
  gllc_ll_sapi_t      lle
)
{
  MSG_GERAN_MED_2_G( "GLC LL %d: LL-RESET-RES State: %d",
    gllc_sapi_addr_lookup[lle], gllc_lle[lle].state);

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    /* LL RESET Res invalid in state TLLI Unassigned. */
    break;

  /* ..................................................................... */

  case GLLC_LLE_ADM:
  case GLLC_LLE_ABM:

    /* Reset LLME reset progress state for each SAPI as LL_RESET_RES is
       received for each SAPI. */

    if( ( lle == GLLC_LL_SAPI_3_LL3 ) &&
        ( gllc_lle[ GLLC_LL_SAPI_3_LL3 ].gen.pdp_active == TRUE ) )
    {
      gllc_llme.xid_reset_in_progress &= ~GLLC_LLME_XID_RESET_SAPI_3;
    }

    if( ( lle == GLLC_LL_SAPI_5_LL5 ) &&
        ( gllc_lle[ GLLC_LL_SAPI_5_LL5 ].gen.pdp_active == TRUE ) )
    {
      gllc_llme.xid_reset_in_progress &= ~GLLC_LLME_XID_RESET_SAPI_5;
    }

    if( ( lle == GLLC_LL_SAPI_9_LL9 ) &&
        ( gllc_lle[ GLLC_LL_SAPI_9_LL9 ].gen.pdp_active == TRUE ) )
    {
      gllc_llme.xid_reset_in_progress &= ~GLLC_LLME_XID_RESET_SAPI_9;
    }

    if( ( lle == GLLC_LL_SAPI_11_LL11 ) &&
        ( gllc_lle[ GLLC_LL_SAPI_11_LL11 ].gen.pdp_active == TRUE ) )
    {
      gllc_llme.xid_reset_in_progress &= ~GLLC_LLME_XID_RESET_SAPI_11;
    }

    MSG_GERAN_MED_2_G( "GLC LL %d: xid reset in progress %d.",
      gllc_sapi_addr_lookup[lle], gllc_llme.xid_reset_in_progress );



    /* LL_RESET_RES received for all SAPIs. Resume UL flow. */

    if( gllc_llme.xid_reset_in_progress == GLLC_LLME_XID_RESET_INACTIVE )
    {

      /* Resume SAPI-3 */
      if ( gllc_lle[ GLLC_LL_SAPI_3_LL3 ].gen.pdp_active == TRUE )
      {
        gsn_ll3_ul_unack_lowater_xid_reset();
        gsn_ll3_ul_ack_lowater_xid_reset();
      }

      /* Resume SAPI-5 */
      if ( gllc_lle[ GLLC_LL_SAPI_5_LL5 ].gen.pdp_active == TRUE )
      {
        gsn_ll5_ul_unack_lowater_xid_reset();
        gsn_ll5_ul_ack_lowater_xid_reset();
      }

      /* Resume SAPI-9 */
      if ( gllc_lle[ GLLC_LL_SAPI_9_LL9 ].gen.pdp_active == TRUE )
      {
        gsn_ll9_ul_unack_lowater_xid_reset();
        gsn_ll9_ul_ack_lowater_xid_reset();
      }

      /* Suspend SAPI-11 */
      if ( gllc_lle[ GLLC_LL_SAPI_11_LL11 ].gen.pdp_active == TRUE )
      {
        gsn_ll11_ul_unack_lowater_xid_reset();
        gsn_ll11_ul_ack_lowater_xid_reset();
      }
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:
  case GLLC_LLE_REM_ESTABLISH:
  case GLLC_LLE_LOC_RELEASE:
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */
    break;

  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */

} /* end of gllc_lle_process_reset_res_mes() */

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_ENTER_PSM_IND_MES()
===
===  DESCRIPTION
===
===   Processes an LL-ENTER-PSM-IND Service Primitive from SNDCP.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_enter_psm_ind_mes
(
  ll_enter_psm_ind_t  *enter_psm_ind
)
{
  uint8 sapi;
  const gsn_sapi_t gsn_sapi_lookup[6] =
  {
    GSN_SAPI_MAX,
    GSN_SAPI_3,
    GSN_SAPI_5,
    GSN_SAPI_MAX,
    GSN_SAPI_9,
    GSN_SAPI_11,
  };
  
  MSG_GERAN_LOW_0_G("GLC LL : Rcvd LL-ENTER-PSM-IND");

  /* Update PSM info in the database */
  gllc_psm_info.llme_info.llc_version         = gllc_llme.llc_version;
  gllc_psm_info.llme_info.input_offset_IOV_UI = gllc_llme.input_offset_IOV_UI;

  for (sapi = GLLC_LL_SAPI_1_GMM; sapi <= GLLC_LL_LAST_SAPI; sapi++)
  {
    if ((sapi == GLLC_LL_SAPI_1_GMM) || (sapi == GLLC_LL_SAPI_7_SMS))
    {
      gllc_psm_info.lle_info[sapi].l2_info_valid   = TRUE;
      gllc_psm_info.lle_info[sapi].l3_xid_len      = 0;
    }
    else
    {
      gsn_sapi_t gsn_sapi = gsn_sapi_lookup[sapi];

      gllc_psm_info.lle_info[sapi].l2_info_valid   = enter_psm_ind->psm_info[gsn_sapi].sapi_active;
      
      if (enter_psm_ind->psm_info[gsn_sapi].l3_xid_info != NULL)
      {
        gllc_psm_info.lle_info[sapi].l3_xid_len    = GPRS_PDU_LENGTH(enter_psm_ind->psm_info[gsn_sapi].l3_xid_info);
        gllc_psm_info.lle_info[sapi].l3_xid_info   = GPRS_MEM_MALLOC(gllc_psm_info.lle_info[sapi].l3_xid_len);
        
        if (gllc_psm_info.lle_info[sapi].l3_xid_info)
        {
          (void) gprs_pdu_strip_head
          (
            &enter_psm_ind->psm_info[gsn_sapi].l3_xid_info,
            gllc_psm_info.lle_info[sapi].l3_xid_info,
            gllc_psm_info.lle_info[sapi].l3_xid_len
          );
        }
      }
    }
    
    if (gllc_psm_info.lle_info[sapi].l2_info_valid)
    {
      gllc_psm_info.lle_info[sapi].l2_info.xid_pending             = gllc_lle[sapi].unnum.xid_pending;
      gllc_psm_info.lle_info[sapi].l2_info.input_offset_IOV_I      = gllc_lle[sapi].ack.input_offset_IOV_I;
      gllc_psm_info.lle_info[sapi].l2_info.retrx_interval_T200     = gllc_lle[sapi].gen.retrx_interval_T200;
      gllc_psm_info.lle_info[sapi].l2_info.max_retrx_cnt_N200      = gllc_lle[sapi].gen.max_retrx_cnt_N200;
      gllc_psm_info.lle_info[sapi].l2_info.max_info_octets_N201_U  = gllc_lle[sapi].unack.max_info_octets_N201_U;
      gllc_psm_info.lle_info[sapi].l2_info.max_info_octets_N201_I  = gllc_lle[sapi].ack.max_info_octets_N201_I;
      gllc_psm_info.lle_info[sapi].l2_info.max_reseq_buf_frames_kD = gllc_lle[sapi].ack.max_reseq_buf_frames_kD;
      gllc_psm_info.lle_info[sapi].l2_info.max_retrx_buf_frames_kU = gllc_lle[sapi].ack.max_retrx_buf_frames_kU;
      gllc_psm_info.lle_info[sapi].l2_info.max_reseq_buf_octets_mD = gllc_lle[sapi].ack.max_reseq_buf_octets_mD;
      gllc_psm_info.lle_info[sapi].l2_info.max_retrx_buf_octets_mU = gllc_lle[sapi].ack.max_retrx_buf_octets_mU;
      gllc_psm_info.lle_info[sapi].l2_info.unack_encipher_oc       = gllc_lle[sapi].unack.encipher_oc;
      gllc_psm_info.lle_info[sapi].l2_info.ack_encipher_oc         = gllc_lle[sapi].ack.encipher_oc;
      gllc_psm_info.lle_info[sapi].l2_info.unack_decipher_oc       = gllc_lle[sapi].unack.decipher_oc;
      gllc_psm_info.lle_info[sapi].l2_info.ack_decipher_oc         = gllc_lle[sapi].ack.decipher_oc;
    }
  }

  /* write context to EFS */
  (void) gllc_write_psm_info();
}

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_DATA_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LL-DATA-REQ Service Primitive from SNDCP as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_data_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_data_req_t       *data_req_mes,
  uint32              pending_data_req_cnt
)
{
  register gllc_lle_retrx_buf_t *item_p;
  register ll_data_req_t        *mes_p           = data_req_mes;
           boolean              ack_request_reqd = FALSE;
           uint16               l3_pdu_length =
                                  GPRS_PDU_LENGTH( mes_p -> l3_pdu );

  /* ===================================================================== */

  if ( !GLLC_LLE_IS_3_5_9_11(lle) )
  {
    MSG_GERAN_LOW_1_G( "GLC LL %d: Rcvd LL-DATA-REQ",
      gllc_sapi_addr_lookup[lle]);
  }

  /* ===================================================================== */

  else if ( (l3_pdu_length >  gllc_lle[lle].ack.max_info_octets_N201_I) ||
            (l3_pdu_length == 0                                       )    )
  {
    MSG_GERAN_ERROR_2_G( "GLC LL %d: Rcvd LL-DATA-REQ Len %d",
      gllc_sapi_addr_lookup[lle], l3_pdu_length);
  }
  else
  {
  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:
  case GLLC_LLE_ADM:
  case GLLC_LLE_LOC_ESTABLISH:
  case GLLC_LLE_REM_ESTABLISH:
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ABM:

    /* Report. */

    switch ( lle )
    {
    case GLLC_LL_SAPI_3_LL3:
    case GLLC_LL_SAPI_5_LL5:
    case GLLC_LL_SAPI_9_LL9:
    case GLLC_LL_SAPI_11_LL11:
      MSG_GERAN_LOW_4_G
      (
        "GLC LL %d: Rcvd LL-DATA-REQ %d:%d:%d",
        lle,
        mes_p -> sndcp_ref.nsapi,
        mes_p -> sndcp_ref.npdu_num,
        mes_p -> sndcp_ref.seg_num
      );
      break;

    default:
      break;
    }

    if ( !gllc_lle_ul_evt_mask_scheme_flags[GLLC_LLE_SCHEME_RETRX_BUF_FULL][lle] )
    {
      /* Allocate storage for the retransmission buffer element. */

      item_p = GPRS_MEM_MALLOC( sizeof(gllc_lle_retrx_buf_t) / sizeof(uint8) );

      if ( item_p == NULL )
      {
        ERR_GERAN_FATAL_1_G( "GLC LL %d: Proc LL-DATA-REQ Heap Ex",
          gllc_sapi_addr_lookup[lle]);
      }
      else
      {
        /* ............................................................... */

        /* Collate retransmission buffer item I frame contents. */

        item_p -> l3_pdu_length          = l3_pdu_length;
        item_p -> peak_throughput        = mes_p -> peak_throughput;
        item_p -> radio_priority         = mes_p -> radio_priority;
        item_p-> pfi                     = mes_p -> pfi;
        item_p -> sndcp_ref              = mes_p -> sndcp_ref;
        item_p -> cause                  = GRR_CAUSE_LAY3_DATA;
        item_p -> retrx_flag             = FALSE;
        item_p -> retrx_ctr              = 0;
        item_p -> frame.format           = GLLC_I_S_FORMAT;

        item_p -> frame.i_s_frame.l3_pdu = mes_p -> l3_pdu;
        mes_p -> l3_pdu                  = NULL; /* De-link */

        item_p -> pdu_priority           = mes_p -> pdu_priority;

        item_p -> frame.i_s_frame.send_seq_num_N_S =
          gllc_lle[lle].ack.send_state_var_V_S;

        item_p -> frame.i_s_frame.cipher_oc =
          gllc_lle[lle].ack.encipher_oc;

        /* ............................................................... */

        /* Supervisory frame parameters are set under the control of the
           MPEX trx handler based on the supervisory and acknowledgement
           transmission requirements of the time, and may be different under
           retransmission conditions. Set to some arbitrary values for now.*/

        item_p -> frame.i_s_frame.ack_req_A         = FALSE;
        item_p -> frame.i_s_frame.cmd_or_res_CR     = GLLC_COMMAND;
        item_p -> frame.i_s_frame.cmd_res_S1S2      = GLLC_RR;
        item_p -> frame.i_s_frame.sack_bitmap_len_K = 0;

        /* ............................................................... */

        /* Copy the data request's QoS and Radio priority parameters to the
           LLE's data store elements associated with standalone S frame and
           U frame transmission. These parameters are to assume the values
           used by any layer 3 entity using this SAPI. */

        gllc_lle[lle].ack.peak_throughput = item_p -> peak_throughput;
        gllc_lle[lle].ack.radio_priority  = item_p -> radio_priority;
        gllc_lle[lle].ack.pfi             = item_p -> pfi;

        gllc_lle[lle].ack.pdu_priority    = item_p -> pdu_priority;

        gllc_lle[lle].unnum.peak_throughput = gllc_lle[lle].ack.peak_throughput;
        gllc_lle[lle].unnum.radio_priority  = gllc_lle[lle].ack.radio_priority;
        gllc_lle[lle].unnum.pfi             = gllc_lle[lle].ack.pfi;


        /* ............................................................... */

        /* Append new buffer element to the end of the retransmission buffer,
           update occupancy record, and increment the acknowledged send state
           variable V(S). If incrementing the V(S) variable causes it to
           roll over it's modulo, then increment the encipher OC by the V(S)
           modulo. */

        GLLC_RETRX_BUF_APPEND_ITEM( lle, item_p );

        if ( gllc_lle[lle].ack.send_state_var_V_S == 511 )
          gllc_lle[lle].ack.encipher_oc = (uint32)(gllc_lle[lle].ack.encipher_oc + 512);

        gllc_lle[lle].ack.send_state_var_V_S =
          GMODULO( 512, gllc_lle[lle].ack.send_state_var_V_S + 1 );

        /* ............................................................... */

        /* Check and act on retransmission buffer limits. */

        if
        (
          (
            gllc_lle[lle].ack.retrx_buf_num_frames >=
            gllc_lle[lle].ack.max_retrx_buf_frames_kU
          )
          ||
          (
            (gllc_lle[lle].ack.max_retrx_buf_octets_mU != 0)
            &&
            (
              gllc_lle[lle].ack.retrx_buf_occupancy_B >
              (
                gllc_lle[lle].ack.max_retrx_buf_octets_mU -
                gllc_lle[lle].ack.max_info_octets_N201_I
              )
            )
          )
        )
        {
          /* Either the kU maximum outstanding frames limit has been
             reached, or the retransmission buffer octet occupancy
             B > mU - N201-I, indicating that a maximal length PDU can
             no longer be accomodated. Invoke the appropriate event masking
             scheme for this LLE to prevent any more data request messages
             from being processed, and request an acknowledgment from
             the peer LLE. */

          gllc_lle_set_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_RETRX_BUF_FULL );

          ack_request_reqd = TRUE;
        }

        /* ............................................................... */

        /* If this L3 PDU transmission request represents the last in the
           current sequence from layer 3, then request an acknowledgement
           from the peer LLE. */

        if ( (mes_p -> final_seqment) && (pending_data_req_cnt == 0) )
          ack_request_reqd = TRUE;

        /* ............................................................... */

        if ( ack_request_reqd )
        {
          /* An acknowledgement request must be transmitted to the peer
             LLE. (Re)associate the T201 timer with the last frame in the
             retransmission buffer (ie. this new frame) */

          (void)rex_clr_timer( &gllc_timer_t201[lle] );
          gllc_lle[lle].ack.T201_association = item_p;
          (void)rex_set_timer( &gllc_timer_t201[lle], gllc_lle[lle].ack.retrx_interval_T201 );
          GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );

          /* Flag the ACK request and I Frame transmission requirements to
             the MPEX trx handler. */

          GLLC_SET_LLE_UL_EVT( lle, (GLLC_LLE_UL_S_FR_ABIT_EVT | GLLC_LLE_UL_I_FR_EVT) );
        }
        else
        {
          /* If T201 is currently unassociated, then associate the timer
             with this frame. */

          if ( gllc_lle[lle].ack.T201_association == NULL )
          {
            gllc_lle[lle].ack.T201_association = item_p;
            (void)rex_set_timer( &gllc_timer_t201[lle], gllc_lle[lle].ack.retrx_interval_T201 );
          }

          /* Flag the I Frame transmission requirement to the MPEX trx
             handler. */

          GLLC_SET_LLE_UL_EVT( lle, GLLC_LLE_UL_I_FR_EVT );
        }

        /* ............................................................... */
      }
    }
    else
    {
      /* The LLE's LL Uplink Queue event should have been masked off until
         such time as the retransmission buffer occupancy has been reduced.*/

      MSG_GERAN_ERROR_1_G("GLC LL %d: LL-DATA-REQ Q Msk Err", gllc_sapi_addr_lookup[lle]);
      gllc_lle_set_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_RETRX_BUF_FULL );
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* LL Data Req invalid in state Loc Rel. */

    MSG_GERAN_LOW_1_G( "GLC LL %d: Rcvd LL-DATA-REQ L.Rel",
      gllc_sapi_addr_lookup[lle]);
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */

    MSG_GERAN_LOW_2_G( "GLC LL %d: LL-DATA-REQ State Err %d",
      gllc_sapi_addr_lookup[lle], gllc_lle[lle].state );
    break;

  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */
  }
} /* end of gllc_lle_process_data_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_UNITDATA_REQ_MES()
===
===  DESCRIPTION
===
===   Processes an LL-UNITDATA-REQ Service Primitive from layer 3 as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_unitdata_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_unitdata_req_t   *unitdata_req_mes
)
{
  register ll_unitdata_req_t *mes_p        = unitdata_req_mes;
           uint16            l3_pdu_length = GPRS_PDU_LENGTH(mes_p->l3_pdu);

  if ( (l3_pdu_length >  gllc_lle[lle].unack.max_info_octets_N201_U) ||
       (l3_pdu_length == 0                                         )    )
  {
    MSG_GERAN_ERROR_2_G( "GLC LL %d: Rcvd LL-UDATA-REQ Len %d",
      gllc_sapi_addr_lookup[lle], l3_pdu_length);
  }

  /* ===================================================================== */

  else switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    /* LL Unitdata Req invalid in state TLLI Unassigned for all SAPIs
       except GLLC_LL_SAPI_1_GMM. */

    if ( lle != GLLC_LL_SAPI_1_GMM )
    {
      MSG_GERAN_ERROR_1_G( "GLC LL %d: Rcvd LL-UDAT-REQ Unass",
        gllc_sapi_addr_lookup[lle]);
      break;
    }
    /* Drop through if lle == GLLC_LL_SAPI_1_GMM. */

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:
  case GLLC_LLE_LOC_ESTABLISH:
  case GLLC_LLE_REM_ESTABLISH:
  case GLLC_LLE_ABM:
  case GLLC_LLE_LOC_RELEASE:

    MSG_GERAN_LOW_2_G
    (
      "GLC LL %d: Rcvd LL-UDAT-REQ U:%d",
      gllc_sapi_addr_lookup[lle], gllc_lle[lle].unack.send_state_var_V_U
    );

    /* Collate basic contents. */

    gllc_lle[lle].unack.frame.format                 = GLLC_UI_FORMAT;
    gllc_lle[lle].unack.frame.ui_frame.cmd_or_res_CR = GLLC_COMMAND;

    /* Attach payload and associated parameters. */

    gllc_lle[lle].unack.frame.ui_frame.l3_pdu = mes_p -> l3_pdu;
    mes_p -> l3_pdu                           = NULL; /* De-link */

    gllc_lle[lle].unack.l3_pdu_length = l3_pdu_length;

    gllc_lle[lle].unack.frame.ui_frame.encrypted_E =
      (
        (gllc_llme.gea != GMMLLC_GEA_NONE     ) &&
        (gllc_llme.gea != GMMLLC_GEA_UNDEFINED) &&
        (mes_p -> ciphered                    )
      ) ? TRUE:FALSE;

    gllc_lle[lle].unack.frame.ui_frame.protected_mode_PM =
      (mes_p -> reliability_class == 0x5) ? FALSE:TRUE;

    gllc_lle[lle].unack.frame.ui_frame.send_seq_num_N_U =
      gllc_lle[lle].unack.send_state_var_V_U;

    gllc_lle[lle].unack.frame.ui_frame.cipher_oc =
      gllc_lle[lle].unack.encipher_oc;

    gllc_lle[lle].unack.reliability_class  = mes_p -> reliability_class;
    gllc_lle[lle].unack.peak_throughput    = mes_p -> peak_throughput;
    gllc_lle[lle].unack.radio_priority     = mes_p -> radio_priority;
    gllc_lle[lle].unack.pfi                = mes_p -> pfi;
    gllc_lle[lle].unack.sndcp_ref          = mes_p -> sndcp_ref;

    gllc_lle[lle].unack.pdu_priority       = mes_p -> pdu_priority;

    if ( lle == GLLC_LL_SAPI_1_GMM )
    {
      if( mes_p -> protocol_disc == PD_GPRS_SM )
      {
        /* SM signalling */
        gllc_lle[lle].unack.cause = GRR_CAUSE_SM_SIGNALLING;
      }
      else if ( mes_p -> protocol_disc == PD_GMM )
      {
        /* GMM signalling */
        gllc_lle[lle].unack.cause = GRR_CAUSE_GMM_SIGNALLING;
      }
      else
      {
        MSG_GERAN_ERROR_2_G("GLLC LL %d: Rcvd Invalid PD for UL NAS signalling msg %d",
                                                 gllc_sapi_addr_lookup[lle],
                                                 mes_p -> protocol_disc );
      }

      /* Transaction Identifier in NAS signalling PDU */
      gllc_lle[lle].unack.transaction_id = mes_p -> transaction_id;
    }
    else
    {
      gllc_lle[lle].unack.cause = GRR_CAUSE_LAY3_DATA;
    }

    /* Copy the data request's QoS and Radio priority parameters to the
       LLE's data store elements associated with U frame transmission.
       These parameters are to assume the values of any layer 3 entity
       using this SAPI. */

    gllc_lle[lle].unnum.peak_throughput = gllc_lle[lle].unack.peak_throughput;
    gllc_lle[lle].unnum.radio_priority = gllc_lle[lle].unack.radio_priority;
    gllc_lle[lle].unnum.pfi = gllc_lle[lle].unack.pfi;

    /* Check whether the unacknowledged send state variable V(U) is about
       to roll over when incremented, and increment the encipher OC by the
       V(U) modulo if so. */

    if ( gllc_lle[lle].unack.send_state_var_V_U == 511 )
    {
      gllc_lle[lle].unack.encipher_oc = (uint32)(gllc_lle[lle].unack.encipher_oc + 512);

      gllc_debug_print_lle_cipher_params ( lle );
    }

    /* Increment the unacknowledged send state variable V(U). */

    gllc_lle[lle].unack.send_state_var_V_U =
      GMODULO( 512, gllc_lle[lle].unack.send_state_var_V_U + 1 );

    /* Flag the transmission requirement to the MPEX trx handler. */

    GLLC_SET_LLE_UL_EVT( lle, GLLC_LLE_UL_UI_FR_EVT );
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */

    MSG_GERAN_LOW_2_G( "GLC LL %d: LL-UDATA-REQ State Err %d",
      gllc_sapi_addr_lookup[lle], gllc_lle[lle].state );
    break;

  /* --------------------------------------------------------------------- */
  }
  /* ===================================================================== */

} /* end of gllc_lle_process_unitdata_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_U_FORMAT_CR_XID_CMD()
===
===  DESCRIPTION
===
===   Processes a received U Format XID command frame from RLC as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   The U Frame is expected to have been checked to ensure that it is an
===   XID command frame with Poll=1.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_process_u_format_cr_xid_cmd
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         rxd_tlli
)
{
  /* --------------------------------------------------------------------- */

  /* Verify any layer 2 XID embodiment. */

  if ( gllc_lle_validate_u_format_cr_xid_req( lle, u_frame) )
  {
    /* Layer 2 XID negotiation request (if present) is deemed
       valid. */

    /* Check for XID Cmd collision on the radio link. */
    if
    (
      (
        (gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE      ) &&
       !(u_frame -> info_field.xid_info.contents & GLLC_RESET_XID)
      )
      ||
      (
        (gllc_lle[lle].substate == GLLC_LLE_L2_XID_EXCHANGE      ) &&
       !(u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID ) &&
       !(u_frame -> info_field.xid_info.contents & GLLC_RESET_XID)
      )
    )
    {
      /* XID Cmd collision condition. Resolution procedures require that if:

         EITHER:

         (a) the txd XID cmd DOES contain L3 params and the rxd XID cmd
             DOES NOT contain a reset param,

         OR:

         (b) the txd XID cmd DOES NOT contain L3 and reset params, and the
             rxd XID cmd DOES NOT contain L3 params,

         THEN:

         the SGSN originated XID Cmd shall be ignored, and the
         unnegotiated params shall be held pending for renegotiation ASAP. */

      gllc_lle[lle].unnum.xid_pending |=
        (u_frame -> info_field.xid_info.contents & ~GLLC_LAY3_XID);
    }

    /* ................................................................... */

    else
    {
      /* No XID exchange in progress, or L3 XID exchange in progress and
         received XID Cmd contains a reset parameter (conceded contention
         resolution), or L2 XID exchange in progress and received XID Cmd
         contains L3 or reset parameters (conceded contention resolution). */

      if ( (gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE) ||
           (gllc_lle[lle].substate == GLLC_LLE_L2_XID_EXCHANGE)    )
      {
        /* Conceded contention resolution. Due to the abnormal message
           sequencing, the 'Wait Peer Resp' event masking scheme must be
           explicitly cleared. */

        gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );
      }

      if ( u_frame -> info_field.xid_info.contents & GLLC_RESET_XID )
      {
        /* Perform LLME reset procedures. */

        gllc_llme_xid_reset_procedures( rxd_tlli );
      }

      /* Store the XID negotiation request, generate an XID negotiation
         response and place in the LLE's unnumbered parameters store in
         anticipation of the impending transmission of an XID response. */

      gllc_lle_store_u_format_cr_xid_req( lle, u_frame );
      gllc_lle_generate_l2_xid_neg( lle );

      if ( (u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID) &&
           (u_frame -> info_field.xid_info.l3_xid.params != NULL   )    )
      {
        /* Layer 3 XID negotiation request is present so inform layer 3 and
           await a response. */

        gllc_lle_send_ll_xid_ind_mes( lle, u_frame, rxd_tlli );
        gllc_lle[lle].substate = GLLC_LLE_L3_XID_EXCHANGE;
      }

      else if ( (u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID) &&
                (u_frame -> info_field.xid_info.l3_xid.params == NULL   )    )
      {
        dsm_item_type *l3_xid_neg = NULL;

        /* Layer 3 XID negotiation request is absent, so set layer 2
           negotiated parameters now, send XID response to peer LLE, and
           inform layer 3 of N201-I and N201-U. */

        gllc_lle_set_l2_xid_parameters( lle, u_frame );
        gllc_lle_send_ll_xid_ind_mes( lle, NULL, rxd_tlli );
        gllc_lle_send_xid_res_mes( lle, &l3_xid_neg );

        /* A conceded contention resolution may have resulted in a
           pending L2 XID negotiation requirement, which can be initiated
           now that the current negotiation is complete.

           Do not negotiate N201-U for SAPI-1 here due to possible
           collision with commands from network. */

        if ( gllc_lle_generate_l2_xid_req( lle ) )
        {
          if ( !( ( gllc_lle[ lle ].unnum.xid_req.contents & GLLC_N201_U_XID ) &&
                  ( lle == GLLC_LL_SAPI_1_GMM ) ) )
          {
            gllc_lle_send_xid_cmd_mes( lle, NULL );
            gllc_lle[lle].substate = GLLC_LLE_L2_XID_EXCHANGE;
          }
          else
          {
            gllc_lle[lle].substate = GLLC_LLE_NONE;
          }
        }
        else
        {
          gllc_lle[lle].substate = GLLC_LLE_NONE;
        }
      }
      else
      {
        /* Layer 3 XID negotiation request is absent, so set layer 2
           negotiated parameters now, send XID response to peer LLE, and
           inform layer 3 of N201-I and N201-U. */

        gllc_lle_set_l2_xid_parameters( lle, u_frame );
        gllc_lle_send_ll_xid_ind_mes( lle, NULL, rxd_tlli );
        gllc_lle_send_xid_res_mes( lle, NULL );

        /* A conceded contention resolution may have resulted in a
           pending L2 XID negotiation requirement, which can be initiated
           now that the current negotiation is complete.

           Do not negotiate N201-U for SAPI-1 here due to possible
           collision with commands from network. */

        if ( gllc_lle_generate_l2_xid_req( lle ) )
        {
          if ( !( ( gllc_lle[ lle ].unnum.xid_req.contents & GLLC_N201_U_XID ) &&
                  ( lle == GLLC_LL_SAPI_1_GMM ) ) )
          {
            gllc_lle_send_xid_cmd_mes( lle, NULL );
            gllc_lle[lle].substate = GLLC_LLE_L2_XID_EXCHANGE;
          }
          else
          {
            gllc_lle[lle].substate = GLLC_LLE_NONE;
          }
        }
        else
        {
          gllc_lle[lle].substate = GLLC_LLE_NONE;
        }
      }
    }
  }

  /* --------------------------------------------------------------------- */

    MSG_GERAN_HIGH_3_G( "GLC LL%d: Rcvd XID Cmd L3:%d State:%d",
      gllc_sapi_addr_lookup[lle],
      (u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID),
      gllc_lle[lle].state );

    gllc_lle_report_l2_xid_content(lle, &u_frame -> info_field.xid_info );
  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_process_u_format_cr_xid_cmd() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_U_FORMAT_CR_XID_RES()
===
===  DESCRIPTION
===
===   Processes a received U Format XID response frame from RLC as a
===   function of the given LLE's state.
===
===  DEPENDENCIES
===
===   The U Frame is expected to have been checked to ensure that it is an
===   XID response frame with Final=1.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_process_u_format_cr_xid_res
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         rxd_tlli
)
{
  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE) ||
       (gllc_lle[lle].substate == GLLC_LLE_L2_XID_EXCHANGE)    )
  {
    /* XID Res received in response to the XID command sent earlier. */

    if ( gllc_lle_validate_u_format_cr_xid_neg( lle, u_frame) )
    {
      /* Layer 2 XID negotiation response (if present) is deemed valid, so
         store the XID negotiation response and set the negotiated
         parameters. */

      gllc_lle_store_u_format_cr_xid_neg( lle, u_frame );
      gllc_lle_set_l2_xid_parameters( lle, u_frame );

      if ( gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE )
      {
        /* Layer 3 XID negotiation in progress so send confirmation to
           layer 3. */
        gllc_lle_send_ll_xid_cnf_mes( lle, u_frame, rxd_tlli );
      }
      else
      {
        /* Layer 2 XID negotiation in progress, so send XID response
           to peer LLE, and inform layer 3 of N201-I and N201-U. */
        gllc_lle_send_ll_xid_ind_mes( lle, NULL, rxd_tlli );
      }

      /* Any remaining pending parameters which have yet to be negotiated,
         can be negotiated now that the current negotiation is complete. */

      if ( gllc_lle_generate_l2_xid_req(lle) )
      {
        gllc_lle_send_xid_cmd_mes( lle, NULL );
        gllc_lle[lle].substate = GLLC_LLE_L2_XID_EXCHANGE;
      }
      else
        gllc_lle[lle].substate = GLLC_LLE_NONE;
    }

    /* ................................................................... */

    else
    {
      /* Received XID Res contains an invalid layer 2 XID negotiation
         response, so retransmit the XID cmd. */
      if ( !gllc_lle_resend_xid_cmd_mes( lle ) )
      {
        /* Number of XID resend attempts has exceeded limit. Inform GMM
           about the invalid XID response. */

        gllc_lle_send_llgmm_status_ind_mes(lle, rxd_tlli, GLLCMM_INVALID_XID_RESP);

        if ( gllc_lle[lle].state == GLLC_LLE_ABM )
        {
          /* Release this LLE from the ABM state and inform layer 3. */

          gllc_lle_send_ll_rel_ind_mes(lle, rxd_tlli, GPRS_LL_INVALID_XID_RESP);
          gllc_lle_enter_state_adm( lle );
        }
        else
        {
          /* Inform layer 3 about the invalid XID response. */

          gllc_lle_send_ll_status_ind_mes( lle, rxd_tlli, GPRS_LL_INVALID_XID_RESP );
          gllc_lle[lle].substate = GLLC_LLE_NONE;
        }
      }
    }
  }

  /* --------------------------------------------------------------------- */
    /* Unsolicited but with F=1 - Ignore. */

  MSG_GERAN_MED_3_G("GLC LL %d: Rcvd XID Res, State: %d, substate %d",
    gllc_sapi_addr_lookup[lle],
    gllc_lle[lle].state,
    gllc_lle[lle].substate);

  gllc_lle_report_l2_xid_content(
    lle, &u_frame -> info_field.xid_info );
} /* end of gllc_lle_process_u_format_cr_xid_res() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_REPORT_FRMR_CONTENT()
===
===  DESCRIPTION
===
===   Generates a debug messaging report with an entry for each FRMR
===   parameter contained in the given information element.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_report_frmr_content
(
  gllc_ll_sapi_t   lle,
  gllc_frmr_info_t *frmr_info_p
)
{
  MSG_GERAN_HIGH_7_G( "GLC LL %d: Info FRMR CF1 : 0x%x, CF2 : 0x%x, CF3 : 0x%x, CF4 : 0x%x, CF5 : 0x%x, CF6 : 0x%x",
    gllc_sapi_addr_lookup[lle],
    frmr_info_p -> rej_fr_ctrl_field[0],
    frmr_info_p -> rej_fr_ctrl_field[1],
    frmr_info_p -> rej_fr_ctrl_field[2],
    frmr_info_p -> rej_fr_ctrl_field[3],
    frmr_info_p -> rej_fr_ctrl_field[4],
    frmr_info_p -> rej_fr_ctrl_field[5]);

  MSG_GERAN_HIGH_4_G( "GLC LL %d: Info FRMR V(S): %d, V(R): %d, C/R : %d, W4W1: 0x%x",
    frmr_info_p -> send_state_var_V_S,
    frmr_info_p -> recv_state_var_V_R,
    frmr_info_p -> rejected_frame_CR,
    frmr_info_p -> reject_status_W4_W1);
} /* end of gllc_lle_report_frmr_content */

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_U_FRAME_IN_UNASSIGNED()
===
===  DESCRIPTION
===
===   Processes a received U Format frame from RLC in the given LLE's
===   'TLLI Unassigned' state.
===
===  DEPENDENCIES
===
===   LLE state must be TLLI Unassigned.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_process_u_frame_in_unassigned
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
)
{
  MSG_GERAN_MED_4_G( "GLC LL %d: Rcvd U Frame CMD:%d, type:%d, Poll:%d",
    gllc_sapi_addr_lookup[lle],
    u_frame -> cmd_or_res_CR,
    u_frame -> cmd_res_M4_M1,
    u_frame -> poll_final_PF);

  if ( lle != GLLC_LL_SAPI_1_GMM )
  {
    MSG_GERAN_LOW_1_G( "GLC LL %d: U CR Unass SAPI Err",
      gllc_sapi_addr_lookup[lle]);
  }

  /* ===================================================================== */

  else if ( u_frame -> cmd_or_res_CR == GLLC_COMMAND )
  {
    /* Unnumbered command received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_SABM: /* Set Asynchronous Balanced Mode (ABM) */
      gllc_lle_report_l2_xid_content(lle, &u_frame -> info_field.xid_info);
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DISC: /* set DISConnected mode. */
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        /* Valid XID Cmd frame (so far). */

        gllc_lle_process_u_format_cr_xid_cmd( lle, u_frame, rxd_tlli );
      }
      else
      {
        /* Unsolicited, so ignore. */
        gllc_lle_report_l2_xid_content( lle, &u_frame -> info_field.xid_info );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_NULL:

      /* A NULL command is not expected from the SGSN peer LLE. */
      break;

    /* ------------------------------------------------------------------- */

    default:
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */

  else
  {
    /* Unsolicited unnumbered response received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_UA:   /* Unumbered Acknowledgement. */
      gllc_lle_report_l2_xid_content( lle, &u_frame -> info_field.xid_info );
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_FRMR: /* FRaMe Reject. */

      /* Inform GMM. */
      gllc_lle_report_frmr_content( lle, &u_frame ->info_field.frmr_info );
      gllc_lle_send_llgmm_status_ind_mes(lle, rxd_tlli, GLLCMM_FRAME_REJECT_RESP);
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DM:   /* Disconnected Mode. */
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */

      if ( u_frame -> poll_final_PF == GLLC_PF_FINAL )
      {
        /* Valid XID Res frame (so far). */
        gllc_lle_process_u_format_cr_xid_res( lle, u_frame, rxd_tlli );
      }
      else
      {
        /* Unsolicited, so ignore. */
        gllc_lle_report_l2_xid_content( lle, &u_frame -> info_field.xid_info );
      }
      break;

    /* ------------------------------------------------------------------- */

    default:
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */
} /* end of gllc_lle_process_u_frame_in_unassigned() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_U_FRAME_IN_ADM()
===
===  DESCRIPTION
===
===   Processes a received U Format frame from RLC in the given LLE's
===   'ADM' state.
===
===  DEPENDENCIES
===
===   LLE state must be ADM.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_process_u_frame_in_adm
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
)
{
  dsm_item_type *l3_xid_neg = NULL;

  MSG_GERAN_HIGH_4_G( "GLC LL %d: Rcvd U Frame ADM CMD:%d, type:%d, poll:%d",
    gllc_sapi_addr_lookup[lle],
    u_frame -> cmd_or_res_CR,
    u_frame -> cmd_res_M4_M1,
    u_frame -> poll_final_PF);

  if ( u_frame -> cmd_or_res_CR == GLLC_COMMAND )
  {
    /* Unnumbered command received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_SABM: /* Set Asynchronous Balanced Mode (ABM) */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        if ( GLLC_LLE_IS_3_5_9_11(lle) )
        {
          /* Valid SABM command received from the peer LLE. Firstly verify
             any layer 2 XID embodiment. */

          if ( gllc_lle_validate_u_format_cr_xid_req(lle, u_frame) )
          {
            /* Layer 2 XID negotiation request (if present) is deemed
               valid. */

            if ( (gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE) ||
                 (gllc_lle[lle].substate == GLLC_LLE_L2_XID_EXCHANGE)    )
            {
              /* Conceded contention resolution. Due to the abnormal message
                 sequencing, the 'Wait Peer Resp' event masking scheme must
                 be explicitly cleared. */

              gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );
            }

            /* Store the XID negotiation request, generate an XID negotiation
               response and place in the LLE's unnumbered parameters store in
               anticipation of the impending transmission of a UA response. */

            gllc_lle_store_u_format_cr_xid_req( lle, u_frame );
            gllc_lle_generate_l2_xid_neg( lle );

            MSG_GERAN_MED_2_G( "GLC LL %d: Rcvd SABM L3 %d ADM",
              gllc_sapi_addr_lookup[lle],
              (u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID));

            gllc_lle_report_l2_xid_content(
              lle, &u_frame -> info_field.xid_info );

            if ( u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID )
            {
              if ( u_frame -> info_field.xid_info.l3_xid.params != NULL )
              {
                /* Layer 3 XID negotiation request is present so inform layer
                   3 and await a response. */

                gllc_lle_send_ll_est_ind_mes( lle, u_frame, rxd_tlli );
                gllc_lle[lle].state    = GLLC_LLE_REM_ESTABLISH;
                gllc_lle[lle].substate = GLLC_LLE_NONE;
              }
              else
              {
                /* Layer 3 XID negotiation request is absent, so set layer 2
                   XID parameters now, send UA to peer LLE, inform layer 3 and
                   enter ABM state. */

                gllc_lle_set_l2_xid_parameters( lle, NULL );
                gllc_lle_send_ll_est_ind_mes( lle, NULL, rxd_tlli );
                gllc_lle_send_ua_res_mes( lle, &l3_xid_neg );
                gllc_lle_enter_state_abm( lle );
                /* Entered ABM. Now clear ul suspension due to est_susp. */
                gllc_lle_clr_est_susp( lle );
              }
            }
            else
            {
              /* Layer 3 XID negotiation request is absent, so set layer 2
                 XID parameters now, send UA to peer LLE, inform layer 3 and
                 enter ABM state. */
              gllc_lle_set_l2_xid_parameters( lle, NULL );
              gllc_lle_send_ll_est_ind_mes( lle, NULL, rxd_tlli );
              gllc_lle_send_ua_res_mes( lle, NULL );
              gllc_lle_enter_state_abm( lle );
              /* Entered ABM. Now clear ul suspension due to est_susp. */
              gllc_lle_clr_est_susp( lle );
            }
          }
          else
          {
            /* Layer 2 XID negotiation request is deemed invalid so
               ignore. */

            MSG_GERAN_HIGH_1_G( "GLC LL %d: Rcvd SABM L2-Err ADM",
              gllc_sapi_addr_lookup[lle]);

            gllc_lle_report_l2_xid_content(lle, &u_frame -> info_field.xid_info );
          }
        }

        /* ............................................................... */

        else
        {
          /* ABM not supported for this LL SAPI address. Respond to the peer
             LLE with an unnumbered DM response message. */

          MSG_GERAN_LOW_1_G( "GLC LL %d: SABM ADM Sapi Err",
            gllc_sapi_addr_lookup[lle]);
          gllc_lle_send_dm_res_mes( lle, GLLC_PF_FINAL );
        }
      }
      else
      {
        /* Unsolicited, so ignore. */
        MSG_GERAN_LOW_1_G( "GLC LL %d: Rcvd SABM P=0 ADM",
          gllc_sapi_addr_lookup[lle]);

        gllc_lle_report_l2_xid_content( lle, &u_frame -> info_field.xid_info );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DISC: /* set DISConnected mode. */

      /* Respond to the peer LLE with an unnumbered DM response message. */
      gllc_lle_send_dm_res_mes( lle, GLLC_PF_FINAL );
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        /* Valid XID Cmd frame (so far). */
        gllc_lle_process_u_format_cr_xid_cmd( lle, u_frame, rxd_tlli );
      }
      else
      {
        /* Unsolicited, so ignore. */
        gllc_lle_report_l2_xid_content( lle, &u_frame -> info_field.xid_info );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_NULL:

      /* A NULL command is not expected from the SGSN peer LLE. */
      break;

    /* ------------------------------------------------------------------- */

    default:
      MSG_GERAN_LOW_1_G( "GLC LL %d: Rcvd invalid U Cmd ADM",
        gllc_sapi_addr_lookup[lle]);
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */

  else
  {
    /* Unsolicited unnumbered response received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_UA:   /* Unumbered Acknowledgement. */

      /* Unsolicited response. Possible multiple assignment of same
         TLLI, so inform GMM. */
      gllc_lle_send_llgmm_status_ind_mes(
        lle, rxd_tlli, GLLCMM_MULTIPLE_TLLI_USERS);
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_FRMR: /* FRaMe Reject. */

      /* Inform GMM. */
      gllc_lle_report_frmr_content( lle, &u_frame ->info_field.frmr_info );
      gllc_lle_send_llgmm_status_ind_mes(
        lle, rxd_tlli, GLLCMM_FRAME_REJECT_RESP);
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DM:   /* Disconnected Mode. */

      /* Ignore. */
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */

      if ( u_frame -> poll_final_PF == GLLC_PF_FINAL )
      {
        /* Valid XID Res frame (so far). */
        gllc_lle_process_u_format_cr_xid_res( lle, u_frame, rxd_tlli );
      }
      else
      {
        /* Unsolicited, so ignore. */
        gllc_lle_report_l2_xid_content(
          lle, &u_frame -> info_field.xid_info );
      }
      break;

    /* ------------------------------------------------------------------- */

    default:
      MSG_GERAN_LOW_1_G( "GLC LL %d: Rcvd invalid U Res ADM",
        gllc_sapi_addr_lookup[lle]);
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */
} /* end of gllc_lle_process_u_frame_in_adm() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SABM_CONTENTION_PROCEDURES()
===
===  DESCRIPTION
===
===   Processes a received U Format SABM command frame from RLC in the
===   given LLE's 'Local Establishment' state, as per specified SABM
===   contention procedures.
===
===  DEPENDENCIES
===
===   LLE state must be Local Establishment and the U Frame is expected to
===   have been pre-checked to ensure that it is a SABM command with Poll=1.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_sabm_contention_procedures
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         rxd_tlli
)
{
  dsm_item_type *l3_xid_neg = NULL;

  MSG_GERAN_MED_4_G( "GLC LL %d: Rcvd SABM, substate:%d, type%d, L3:%d",
    gllc_sapi_addr_lookup[lle],
    gllc_lle[lle].substate,
    u_frame -> xid,
    (u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID));
  /* --------------------------------------------------------------------- */
  gllc_lle_report_l2_xid_content(
    lle, &u_frame -> info_field.xid_info );

  if ( gllc_lle[lle].substate == GLLC_LLE_L3_ESTABLISH )
  {
    /* Layer 3 initiated establishment in progress. */

    if ( u_frame -> xid == GLLC_XID_REQ )
    {
      /* Hold any outstanding XID items which are resulting from
         contention resolution as pending to be dealt with ASAP. */

      gllc_lle[lle].unnum.xid_pending |=
        (u_frame -> info_field.xid_info.contents & ~GLLC_LAY3_XID);
    }
    else
    {
      /* Received SABM collision does not contain XID parameters, indicating
         a layer 2 peer initiated re-establishment. Ignore as layer 3 takes
         precedence. */
    }
  }

  /* --------------------------------------------------------------------- */

  else
  {
    /* Layer 2 initiated re-establishment in progress. */

    if ( u_frame -> xid == GLLC_XID_REQ )
    {
      if ( gllc_lle_validate_u_format_cr_xid_req( lle, u_frame ) )
      {
        /* Any layer 2 XID contents are verified. */

        if ( u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID )
        {
          /* Received SABM collision contains layer 3 XID marking,
             indicating a layer 3 initiated establishment at the peer LLE.
             This takes precedence over layer 2 re-establishment. */

          /* Due to the abnormal message sequencing, the 'Wait Peer Resp'
             event masking scheme must be explicitly cleared. */

          gllc_lle_clr_ul_event_mask_scheme(
            lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

          /* Store the XID negotiation request, generate an XID
             negotiation response and place in the LLE's unnumbered
             parameters store in anticipation of the impending
             transmission of a UA response. */

          gllc_lle_store_u_format_cr_xid_req( lle, u_frame );
          gllc_lle_generate_l2_xid_neg( lle );

          if ( u_frame -> info_field.xid_info.l3_xid.params != NULL )
          {
            /* Layer 3 XID negotiation request is present so inform layer
               3 and await a response. */

            gllc_lle_send_ll_est_ind_mes( lle, u_frame, rxd_tlli );
            gllc_lle[lle].state    = GLLC_LLE_REM_ESTABLISH;
            gllc_lle[lle].substate = GLLC_LLE_NONE;
          }
          else
          {
            /* Layer 3 XID negotiation request is absent, so send UA to
               peer LLE, inform layer 3 and re-enter ABM state without
               waiting for a reply. */

            gllc_lle_set_l2_xid_parameters( lle, NULL );
            gllc_lle_send_ll_est_ind_mes( lle, NULL, rxd_tlli );
            gllc_lle_send_ua_res_mes( lle, &l3_xid_neg );
            gllc_lle_enter_state_abm( lle );
            /* Entered ABM. Now clear ul suspension due to est_susp. */
            gllc_lle_clr_est_susp( lle );
          }
        }
        else
        {
          /* Received SABM collision does not contain layer 3 XID marking,
             indicating collided layer 2 re-establishments. In this case the
             SGSN originated SABM shall be ignored. Hold any outstanding XID
             items which are resulting from contention resolution as pending
             to be dealt with ASAP. */
          gllc_lle[lle].unnum.xid_pending |=
            (u_frame -> info_field.xid_info.contents & ~GLLC_LAY3_XID);
        }
      }
      else
      {
        /* Layer 2 XID negotiation request content is deemed invalid so
           ignore. */
      }
    }
    else
    {
      /* Received SABM collision does not contain XID parameters, indicating
         a layer 2 peer initiated re-establishment. Ignore as MS takes
         precedence. */
    }
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_sabm_contention_procedures() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_U_FRAME_IN_LOC_EST()
===
===  DESCRIPTION
===
===   Processes a received U Format frame from RLC in the given LLE's
===   'Local Establishment' state.
===
===  DEPENDENCIES
===
===   LLE state must be Local Establishment.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_process_u_frame_in_loc_est
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
)
{
  MSG_GERAN_MED_4_G("GLC LL %d: Rcvd U Frame L.Est, CMD:%d, type:%d, Poll:%d",
    gllc_sapi_addr_lookup[lle],
    u_frame -> cmd_or_res_CR,
    u_frame -> cmd_res_M4_M1,
    u_frame -> poll_final_PF);

  if ( u_frame -> cmd_or_res_CR == GLLC_COMMAND )
  {
    /* Unnumbered command received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_SABM: /* Set Asynchronous Balanced Mode (ABM). */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        /* SABM commands have crossed in the radio link. SABM contention
           resolution procedures are required. */

        gllc_lle_sabm_contention_procedures( lle, u_frame, rxd_tlli );
      }
      else
      {
        gllc_lle_report_l2_xid_content(
          lle, &u_frame -> info_field.xid_info );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DISC: /* set DISConnected mode. */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        /* A contending transmitted SABM command and a received DISC command
           requires that a DM response be transmitted, whilst continuing to
           await a UA or DM response in this state. Record the DISC
           reception as this will determine the layer 3 release cause on
           DM response receipt. */
        gllc_lle[lle].unnum.disc_rcvd_in_loc_est = TRUE;
        gllc_lle_send_dm_res_mes( lle, GLLC_PF_FINAL );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */

      gllc_lle_report_l2_xid_content(
        lle, &u_frame -> info_field.xid_info );

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        /* A contending transmitted SABM command and a received XID command
           requires that the XID command be ignored. Hold any outstanding
           XID items which are resulting from contention resolution as
           pending to be dealt with ASAP. */

        gllc_lle[lle].unnum.xid_pending |=
          (u_frame -> info_field.xid_info.contents & ~GLLC_LAY3_XID);
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_NULL:

      /* A NULL command is not expected from the SGSN peer LLE. */

      break;

    /* ------------------------------------------------------------------- */

    default:
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */

  else
  {
    /* Unnumbered response received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_UA:   /* Unumbered Acknowledgement. */

      if ( u_frame -> poll_final_PF == GLLC_PF_FINAL )
      {
        /* UA received in response to the SABM command which was sent in
           state ADM. */

        if ( gllc_lle_validate_u_format_cr_xid_neg( lle, u_frame) )
        {
          /* Layer 2 XID negotiation response (if present) is deemed
             valid. */
          gllc_lle_report_l2_xid_content(
            lle, &u_frame -> info_field.xid_info );

          /* Store XID negotiated response, set negotiated parameters,
             signal layer 3 as dependent on whether this is a
             re-establishment procedure or not, and enter the ABM state. */

          gllc_lle_store_u_format_cr_xid_neg( lle, u_frame );
          gllc_lle_set_l2_xid_parameters( lle, u_frame );

          if ( gllc_lle[lle].substate == GLLC_LLE_L2_ESTABLISH )
            gllc_lle_send_ll_est_ind_mes( lle, NULL, rxd_tlli );
          else
            gllc_lle_send_ll_est_cnf_mes( lle, u_frame, rxd_tlli );

          gllc_lle_enter_state_abm( lle );
          /* Entered ABM. Now clear ul suspension due to est_susp. */
          gllc_lle_clr_est_susp( lle );
        }
        else
        {
          /* Received UA contains an invalid layer 2 XID negotiation
             response, so retransmit the SABM. */

          if ( !gllc_lle_resend_sabm_cmd_mes( lle ) )
          {
            /* Number of SABM resend attempts has exceeded limit. Inform
               layer 3 and GMM about the invalid XID response and return
               to the ADM state. */

            gllc_lle_send_llgmm_status_ind_mes(
              lle, rxd_tlli, GLLCMM_INVALID_XID_RESP );
            gllc_lle_send_ll_rel_ind_mes(
              lle, rxd_tlli, GPRS_LL_INVALID_XID_RESP );
            gllc_lle_enter_state_adm( lle );
          }
        }
      }
      else
      {
        /* Unsolicited UA response. Possible multiple assignment of same
           TLLI, so inform GMM. */
        gllc_lle_report_l2_xid_content(
          lle, &u_frame -> info_field.xid_info );

        gllc_lle_send_llgmm_status_ind_mes(
          lle, rxd_tlli, GLLCMM_MULTIPLE_TLLI_USERS );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_FRMR: /* FRaMe Reject. */

      /* Inform GMM. */
      gllc_lle_report_frmr_content( lle, &u_frame ->info_field.frmr_info );
      gllc_lle_send_llgmm_status_ind_mes(
        lle, rxd_tlli, GLLCMM_FRAME_REJECT_RESP);
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DM:   /* Disconnected Mode. */

      if ( u_frame -> poll_final_PF == GLLC_PF_FINAL )
      {
        /* DM response from peer LLE received so inform layer 3 and return
           to the ADM state. */
        if ( gllc_lle[lle].unnum.disc_rcvd_in_loc_est )
          gllc_lle_send_ll_rel_ind_mes( lle, rxd_tlli, GPRS_LL_NORMAL_RELEASE );
        else
          gllc_lle_send_ll_rel_ind_mes( lle, rxd_tlli, GPRS_LL_DM_RECEIVED );

        gllc_lle_enter_state_adm( lle );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */

        /* Unsolicited, so ignore */
      gllc_lle_report_l2_xid_content(
        lle, &u_frame -> info_field.xid_info );
      break;

    /* ------------------------------------------------------------------- */

    default:
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */
} /* end of gllc_lle_process_u_frame_in_loc_est() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_U_FRAME_IN_ABM()
===
===  DESCRIPTION
===
===   Processes a received U Format frame from RLC in the given LLE's
===   'ABM' state.
===
===  DEPENDENCIES
===
===   LLE state must be ABM.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_process_u_frame_in_abm
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
)
{
  MSG_GERAN_MED_4_G("GLC LL %d: Rcvd U Frame ABM, CMD:%d, type:%d, Poll:%d",
    gllc_sapi_addr_lookup[lle],
    u_frame -> cmd_or_res_CR,
    u_frame -> cmd_res_M4_M1,
    u_frame -> poll_final_PF);

  if ( u_frame -> cmd_or_res_CR == GLLC_COMMAND )
  {
    /* Unnumbered command received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_SABM: /* Set Asynchronous Balanced Mode (ABM). */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        /* SABM received in ABM requires that a re-establishment procedure
           be implemented. Firstly verify any XID content before deciding
           to proceed. */

        if ( gllc_lle_validate_u_format_cr_xid_req( lle, u_frame) )
        {
          /* Any layer 2 XID contents (if present) are verified. */

          if ( (gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE) ||
               (gllc_lle[lle].substate == GLLC_LLE_L2_XID_EXCHANGE)    )
          {
            /* Conceded contention resolution. Due to the abnormal message
               sequencing, the 'Wait Peer Resp' event masking scheme must
               be explicitly cleared. */

            gllc_lle_clr_ul_event_mask_scheme(
              lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );
          }

          /* Store the XID negotiation request, generate an XID
             negotiation response and place in the LLE's unnumbered
             parameters store in anticipation of the impending
             transmission of a UA response. */

          gllc_lle_store_u_format_cr_xid_req( lle, u_frame );
          gllc_lle_generate_l2_xid_neg( lle );

          gllc_lle_report_l2_xid_content(
            lle, &u_frame -> info_field.xid_info );

          if ( u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID )
          {
            if ( u_frame -> info_field.xid_info.l3_xid.params != NULL )
            {
              /* Layer 3 XID negotiation request is present so inform layer
                 3 and await a response. */

              gllc_lle_send_ll_est_ind_mes( lle, u_frame, rxd_tlli );
              gllc_lle[lle].state    = GLLC_LLE_REM_ESTABLISH;
              gllc_lle[lle].substate = GLLC_LLE_NONE;
            }
            else
            {
              dsm_item_type *l3_xid_neg = NULL;

              /* Layer 3 XID negotiation request is absent, so send UA to
                 peer LLE, inform layer 3 and re-enter ABM state without
                 waiting for a reply. */

              gllc_lle_set_l2_xid_parameters( lle, NULL );
              gllc_lle_send_ll_est_ind_mes( lle, NULL, rxd_tlli );
              gllc_lle_send_ua_res_mes( lle, &l3_xid_neg );
              gllc_lle_enter_state_abm( lle );
              /* Entered ABM. Now clear ul suspension due to est_susp. */
              gllc_lle_clr_est_susp( lle );
            }
          }
          else
          {
            /* Layer 3 XID negotiation request is absent, so send UA to
               peer LLE, inform layer 3 and re-enter ABM state without
               waiting for a reply. */
            gllc_lle_set_l2_xid_parameters( lle, NULL );
            gllc_lle_send_ll_est_ind_mes( lle, NULL, rxd_tlli );
            gllc_lle_send_ua_res_mes( lle, NULL );
            gllc_lle_enter_state_abm( lle );
            /* Entered ABM. Now clear ul suspension due to est_susp. */
            gllc_lle_clr_est_susp( lle );

          }
        }
        else
        {
          /* Layer 2 XID negotiation request is deemed invalid so
             ignore. */

          gllc_lle_report_l2_xid_content(
            lle, &u_frame -> info_field.xid_info );
        }
      }
      else
      {
        gllc_lle_report_l2_xid_content(
          lle, &u_frame -> info_field.xid_info );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DISC: /* set DISConnected mode. */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        /* DISC command received from peer LLE. Send UA response without
           layer 2 XID, inform layer 3 and enter ADM state. */
        (void)memset( &gllc_lle[lle].unnum.xid_neg, 0, sizeof(gllc_xid_info_t) );
        gllc_lle_send_ll_rel_ind_mes( lle, rxd_tlli, GPRS_LL_NORMAL_RELEASE );
        gllc_lle_send_ua_res_mes( lle, NULL );
        gllc_lle_enter_state_adm( lle );
      }
      else
      {
        MSG_GERAN_HIGH_1_G( "GLC LL %d: Rcvd DISC P=0 ABM",
          gllc_sapi_addr_lookup[lle]);
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        /* Valid XID Cmd frame (so far). */

        gllc_lle_process_u_format_cr_xid_cmd( lle, u_frame, rxd_tlli );
      }
      else
      {
        /* Unsolicited, so ignore. */
        gllc_lle_report_l2_xid_content(
          lle, &u_frame -> info_field.xid_info );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_NULL: /* NULL Command. */

      /* A NULL command is not expected from the SGSN peer LLE. */
      break;

    /* ------------------------------------------------------------------- */
    default:
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */

  else
  {
    /* Unnumbered response received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_UA:   /* Unnumbered Acknowledgement. */

      /* Unsolicited UA response. Possible multiple assignment of same
         TLLI, so inform GMM. */
      gllc_lle_send_llgmm_status_ind_mes(
        lle, rxd_tlli, GLLCMM_MULTIPLE_TLLI_USERS);
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_FRMR: /* FRaMe Reject. */

      /* Inform GMM. */
      gllc_lle_report_frmr_content( lle, &u_frame ->info_field.frmr_info );
      gllc_lle_send_llgmm_status_ind_mes(
        lle, rxd_tlli, GLLCMM_FRAME_REJECT_RESP);
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DM:   /* Disconnected Mode. */

      if ( u_frame -> poll_final_PF == GLLC_PF_FINAL )
      {
        /* Unsolicited DM response with F=1. Possible multiple assignment
           of same TLLI, so inform GMM. */
        gllc_lle_send_llgmm_status_ind_mes(
          lle, rxd_tlli, GLLCMM_MULTIPLE_TLLI_USERS );
      }
      else
      {
        /* Unsolicited DM response with F=0. Initiate re-establishment and
           inform GMM. */
        gllc_lle_initiate_reestablishment( lle, NULL );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */

      if ( u_frame -> poll_final_PF == GLLC_PF_FINAL )
      {
        /* Valid XID Res frame (so far). */
        gllc_lle_process_u_format_cr_xid_res( lle, u_frame, rxd_tlli );
      }
      else
      {
        /* Unsolicited, so ignore. */
        gllc_lle_report_l2_xid_content(
          lle, &u_frame -> info_field.xid_info );
      }
      break;

    /* ------------------------------------------------------------------- */
    default:
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */
} /* end of gllc_lle_process_u_frame_in_abm() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_U_FRAME_IN_LOC_REL()
===
===  DESCRIPTION
===
===   Processes a received U Format frame from RLC in the given LLE's
===   'Local Release' state.
===
===  DEPENDENCIES
===
===   LLE state must be Local Release.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_process_u_frame_in_loc_rel
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
)
{
  MSG_GERAN_MED_4_G("GLC LL %d: Rcvd U Frame L.Rel, CMD:%d, type:%d, Poll:%d",
    gllc_sapi_addr_lookup[lle],
    u_frame -> cmd_or_res_CR,
    u_frame -> cmd_res_M4_M1,
    u_frame -> poll_final_PF);

  if ( u_frame -> cmd_or_res_CR == GLLC_COMMAND )
  {
    /* Unnumbered command received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_SABM: /* Set Asynchronous Balanced Mode (ABM). */

      /* A contending transmitted DISC command and a received SABM command
         have crossed in the radio link. Contention resolution procedures
         require the transmission of a DM response, whilst continuing to
         await a UA or DM response from the peer LLE. */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        gllc_lle_send_dm_res_mes( lle, GLLC_PF_FINAL );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DISC: /* set DISConnected mode. */

      /* DISC commands have crossed in the radio link. DISC contention
         resolution procedures require the transmission of a UA response
         without layer 2 XID, whilst continuing to await a UA or DM response
         from the peer LLE. */

      if ( u_frame -> poll_final_PF == GLLC_PF_POLL )
      {
        (void)memset( &gllc_lle[lle].unnum.xid_neg, 0, sizeof(gllc_xid_info_t) );
        gllc_lle_send_ua_res_mes( lle, NULL );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */

      /* A transmitted DISC command and a received XID command have crossed
         in the radio link. Ignore for now and hold a pending record of the
         parameters which the peer wants to negotiate, for re-negotiation at
         the next opportunity. */

      gllc_lle_report_l2_xid_content(
        lle, &u_frame -> info_field.xid_info );

      gllc_lle[lle].unnum.xid_pending |=
        (u_frame -> info_field.xid_info.contents & ~GLLC_LAY3_XID);
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_NULL: /* NULL Command. */

      /* A NULL command is not expected from the SGSN peer LLE. */
      break;

    /* ------------------------------------------------------------------- */

    default:
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */

  else
  {
    /* Unnumbered response received. */

    switch ( u_frame -> cmd_res_M4_M1 )
    {
    /* ------------------------------------------------------------------- */

    case GLLC_UA:   /* Unumbered Acknowledgement. */

      gllc_lle_report_l2_xid_content(
        lle, &u_frame -> info_field.xid_info );

      if ( u_frame -> poll_final_PF == GLLC_PF_FINAL )
      {
        if ( u_frame -> xid == GLLC_XID_NONE )
        {
          /* UA received in response to the DISC command which was sent in
             state ABM, so signal layer 3. */
        }
        else
        {
          /* The UA unnumbered acknowledgement is not expected to contain
             an XID element when used in response to a DISC command. However,
             assume peer wants to negotiate some parameters, so hold these
             pending until they can be negotiated at the next oppotunity. */
          gllc_lle[lle].unnum.xid_pending |=
            (u_frame -> info_field.xid_info.contents & ~GLLC_LAY3_XID);
        }

        gllc_lle_send_ll_rel_cnf_mes( lle, rxd_tlli );
        gllc_lle_enter_state_adm( lle );
      }
      else
      {
        /* Unsolicited UA response. Possible multiple assignment of same
           TLLI, so inform GMM. */
        gllc_lle_send_llgmm_status_ind_mes(
          lle, rxd_tlli, GLLCMM_MULTIPLE_TLLI_USERS );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_FRMR: /* FRaMe Reject. */

      /* Inform GMM. */
      gllc_lle_report_frmr_content( lle, &u_frame ->info_field.frmr_info );
      gllc_lle_send_llgmm_status_ind_mes(
        lle, rxd_tlli, GLLCMM_FRAME_REJECT_RESP);
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_DM:   /* Disconnected Mode. */

      if ( u_frame -> poll_final_PF == GLLC_PF_FINAL )
      {
        /* DM response from peer LLE received so inform layer 3 and return
           to the ADM state. */
        gllc_lle_send_ll_rel_cnf_mes( lle, rxd_tlli );
        gllc_lle_enter_state_adm( lle );
      }
      break;

    /* ------------------------------------------------------------------- */

    case GLLC_XID:  /* eXchange of IDentification. */
      gllc_lle_report_l2_xid_content(
        lle, &u_frame -> info_field.xid_info );
      break;

    /* ------------------------------------------------------------------- */

    default:
      break;

    /* ------------------------------------------------------------------- */
    }
  }

  /* ===================================================================== */
} /* end of gllc_lle_process_u_frame_in_loc_rel() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_U_FRAME()
===
===  DESCRIPTION
===
===   Processes a received U Format frame from RLC as a function of the
===   given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_u_frame
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
)
{
  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    gllc_lle_process_u_frame_in_unassigned( lle, u_frame, rxd_tlli );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:

    gllc_lle_process_u_frame_in_adm( lle, u_frame, rxd_tlli );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:

    gllc_lle_process_u_frame_in_loc_est( lle, u_frame, rxd_tlli );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* CR message events are masked off during Remote Establishment as this
       is a local transitory state, where an LL_ESTABLISH_RES message is
       awaited from layer 3. */

    MSG_GERAN_ERROR_1_G( "GLC LL %d: Rcvd U R.Est",
      gllc_sapi_addr_lookup[lle]);
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ABM:

    gllc_lle_process_u_frame_in_abm( lle, u_frame, rxd_tlli );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    gllc_lle_process_u_frame_in_loc_rel( lle, u_frame, rxd_tlli );
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */

    MSG_GERAN_LOW_2_G( "GLC LL %d: Rcvd U State Err %d",
      gllc_sapi_addr_lookup[lle], gllc_lle[lle].state );
    break;

  /* --------------------------------------------------------------------- */
  }

} /* end of gllc_lle_process_u_frame() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_REJ_FRAME()
===
===  DESCRIPTION
===
===   Processes a received but rejected frame format as a function of the
===   given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_rej_frame
(
  gllc_ll_sapi_t      lle,
  gllc_rej_frame_t    *rej_frame,
  uint32              rxd_tlli
)
{
  MSG_GERAN_MED_3_G( "GLC LL %d: Rcvd REJECT Frame: 0x%x, state:%d",
    gllc_sapi_addr_lookup[lle], rej_frame -> fr_dis_err_map, gllc_lle[lle].state);

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:
    /* Reject Format CR invalid in state TLLI Unassigned. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:

    /* A frame rejection condition has occured with respect to a received
       frame. Inform GMM and transmit a FRMR response message. */
    gllc_lle_send_llgmm_status_ind_mes(
      lle, rxd_tlli, GLLCMM_FRAME_REJECT_COND );
    gllc_lle_send_frmr_res_mes( lle, rej_frame );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:

    /* A frame rejection condition has occured with respect to a received
       frame. Inform GMM and transmit a FRMR response message. */
    gllc_lle_send_llgmm_status_ind_mes(
      lle, rxd_tlli, GLLCMM_FRAME_REJECT_COND );
    gllc_lle_send_frmr_res_mes( lle, rej_frame );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* CR message events are masked off during Remote Establishment as this
       is a local transitory state, where an LL_ESTABLISH_RES message is
       awaited from layer 3. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ABM:

    /* A frame rejection condition has occured with respect to a received
       frame. Inform GMM, transmit a FRMR response message, and initiate
       re-establishment. */
    gllc_lle_send_llgmm_status_ind_mes(
      lle, rxd_tlli, GLLCMM_FRAME_REJECT_COND );
    gllc_lle_send_frmr_res_mes( lle, rej_frame );
    gllc_lle_initiate_reestablishment( lle, NULL );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* A frame rejection condition has occured with respect to a received
       frame. Inform GMM and transmit a FRMR response message. */
    gllc_lle_send_llgmm_status_ind_mes(
      lle, rxd_tlli, GLLCMM_FRAME_REJECT_COND );
    gllc_lle_send_frmr_res_mes( lle, rej_frame );
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */
    break;

  /* --------------------------------------------------------------------- */
  }

} /* end of gllc_lle_process_rej_frame() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_UPDATE_SACK_BITMAP()
===
===  DESCRIPTION
===
===   Maintains an up to date selective acknowledgement bitmap for the given
===   LLE by traversing the resequencing buffer and creating a bitmap entry
===   for each received frame's N(S) as referenced to V(R) + 1.
===
===  DEPENDENCIES
===
===   Must be called only after resequencing procedures have been performed
===   and V(R) is representative.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_update_sack_bitmap_R( gllc_ll_sapi_t lle )
{
  register gllc_lle_reseq_buf_t *item_p;
  register uint8                *sack_bitmap_R_p;
  register uint32               sack_bitmap_n;
  register uint32               sack_bitmap_octet;

  /* ---------------------------------------------------------------------- */

  /* Clear down the current bitmap in preparation for generating an
     updated bitmap. */

  gllc_lle[lle].ack.sack_bitmap_len_K = 0;
  memset( gllc_lle[lle].ack.sack_bitmap_R, 0, GLLC_SACK_BMP_LEN );

  /* ---------------------------------------------------------------------- */

  /* Traverse the resequencing buffer and create a bitmap entry for each
     received frame. */

  item_p          = gllc_lle[lle].ack.reseq_first_p;
  sack_bitmap_R_p = gllc_lle[lle].ack.sack_bitmap_R;

  while ( item_p != NULL )
  {
    /* First bit of SACK bitmap is referenced to V(R) + 1. */

    if ( item_p -> i_s_frame.send_seq_num_N_S <
         gllc_lle[lle].ack.recv_state_var_V_R   )
    {
      sack_bitmap_n = (512 + item_p -> i_s_frame.send_seq_num_N_S) -
                      gllc_lle[lle].ack.recv_state_var_V_R;
    }
    else
    {
      sack_bitmap_n = item_p -> i_s_frame.send_seq_num_N_S -
                      gllc_lle[lle].ack.recv_state_var_V_R;
    }

    /* The 1 to 256 bit index must now be re-referenced to 0 to 255. */

    if ( sack_bitmap_n != 0 )
      sack_bitmap_n--;

    else
    {
      MSG_GERAN_ERROR_0_G( "GLC_LL %d: SACK Bmp Err");
      break;
    }

    sack_bitmap_octet = sack_bitmap_n >> 3;

    if ( sack_bitmap_octet < GLLC_SACK_BMP_LEN )
    {
      gllc_lle[lle].ack.sack_bitmap_len_K  = (uint8)sack_bitmap_octet;
      sack_bitmap_R_p[sack_bitmap_octet]  |= (0x80 >> (sack_bitmap_n & 0x7));
    }
    else
    {
      MSG_GERAN_ERROR_2_G( "GLC LL %d: SACK Bmp Err %d",
        gllc_sapi_addr_lookup[lle], sack_bitmap_octet);
      break;
    }

    item_p = item_p -> next_p;
  }

  /* ---------------------------------------------------------------------- */
} /* end of gllc_lle_update_sack_bitmap_R() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESEQUENCING_PROCEDURES()
===
===  DESCRIPTION
===
===   Resequencing procedures relating to the receipt of confirmed
===   information frames, are concerned with optionally inserting an out of
===   sequence frame into the resequencing buffer, and then mandatorially
===   checking the buffer for frame sequences for which the N(S) is >= V(R).
===
===   Identified frame sequences are removed from the resequencing buffer
===   and sent to the appropriate layer 3 entity as LL_DATA_IND primitives.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_resequencing_procedures
(
  gllc_ll_sapi_t    lle,
  gllc_i_s_frame_t  *i_s_frame,
  uint16            l3_pdu_length,
  uint32            rxd_tlli
)
{
  register gllc_lle_reseq_buf_t *item_p, *next_p, *insert_p;

  /* ---------------------------------------------------------------------- */

  /* Determine whether a new item is to be inserted before the resequencing
     buffer is checked for complete sequences. */

  if ( i_s_frame != NULL )
  {
    /* Allocate storage for the resequencing buffer element and enter the
       relevent frame parameters to the element. */

    item_p = GPRS_MEM_MALLOC( sizeof(gllc_lle_reseq_buf_t) / sizeof(uint8) );

    if ( item_p == NULL )
    {
      ERR_GERAN_FATAL_1_G( "GLC LL %d: Reseq Proc: Heap Ex",
        gllc_sapi_addr_lookup[lle]);
    }
    else
    {
      item_p -> rxd_tlli                   = rxd_tlli;
      item_p -> l3_pdu_length              = l3_pdu_length;
      item_p -> i_s_frame.send_seq_num_N_S = i_s_frame -> send_seq_num_N_S;
      item_p -> i_s_frame.l3_pdu           = i_s_frame -> l3_pdu;

      i_s_frame -> l3_pdu = NULL;  /* Delink L3 PDU from the source frame. */

      /* ................................................................. */

      /* Insert the resequencing buffer element in the buffer (queue) at the
         correct position based on the frame's send sequence number N(S). */

      insert_p = gllc_lle[lle].ack.reseq_first_p;

      while
      (
        ( insert_p != NULL )
        &&
        GMODULO_GT
        (
          512,                                     /* mod */
          item_p   -> i_s_frame.send_seq_num_N_S,  /* val */
          insert_p -> i_s_frame.send_seq_num_N_S   /* lim */
        )
      )
      {
        insert_p = insert_p -> next_p;
      }

      if ( insert_p == NULL )
      {
        GLLC_RESEQ_BUF_APPEND_ITEM( lle, item_p );
      }
      else if ( item_p   -> i_s_frame.send_seq_num_N_S !=
                insert_p -> i_s_frame.send_seq_num_N_S    )
      {
        GLLC_RESEQ_BUF_INSERT_ITEM( lle, item_p, insert_p );
      }
      else
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Rcvd Dup S:%d",
          gllc_sapi_addr_lookup[lle], item_p -> i_s_frame.send_seq_num_N_S);
        GPRS_PDU_FREE( &item_p -> i_s_frame.l3_pdu );
        GPRS_MEM_FREE( item_p );
      }
    }
  }

  /* ---------------------------------------------------------------------- */

  /* Examine the resequencing buffer to check for complete frame sequences
     with an N(S) number in the range of current V(R) to the inserted frames
     N(S) value minus one, and send each L3 PDU to the appropriate layer 3
     entity. */

  item_p = gllc_lle[lle].ack.reseq_first_p;

  while
  (
    ( item_p != NULL )
    &&
    (
      item_p -> i_s_frame.send_seq_num_N_S ==
      gllc_lle[lle].ack.recv_state_var_V_R
    )
  )
  {
    GLLC_RESEQ_BUF_REMOVE_ITEM( lle, item_p )

    /* Check whether the acknowledged receive state variable V(R)
       modulo is about to roll over when updated, and increment the
       decipher OC by the V(R) modulo if so. */

    if ( gllc_lle[lle].ack.recv_state_var_V_R == 511 )
    {
      gllc_lle[lle].ack.decipher_oc =
        (uint32)(gllc_lle[lle].ack.decipher_oc + 512);
    }

    gllc_lle[lle].ack.recv_state_var_V_R =
      GMODULO( 512, gllc_lle[lle].ack.recv_state_var_V_R + 1 );

    gllc_lle_send_ll_data_ind_mes(
      lle, &item_p -> i_s_frame, item_p -> rxd_tlli );

#ifndef FEATURE_GERAN_REDUCED_DEBUG
    gllc_lle[lle].diag.pdu_stats.ack_l3pdu_rx_cnt += 1;
    gllc_lle[lle].diag.pdu_stats.ack_octet_rx_cnt += item_p -> l3_pdu_length;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

    next_p = item_p -> next_p;
    GPRS_MEM_FREE( item_p );
    item_p = next_p;
  }

  /* ---------------------------------------------------------------------- */
} /* end of gllc_lle_resequencing_procedures() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_N_R_ACKNOWLEDGEMENT_PROCEDURES()
===
===  DESCRIPTION
===
===   The N(R) value of a received frame is assumed to represent a valid
===   indication of the next in-sequence I_S frame which the peer LLE
===   expects to receive. Therefore, all frames in the retransmission buffer
===   with an N(S) < rcvd N(R) are considered as acknowledged.
===
===   In each case the associated retransmission buffer element is removed
===   from the buffer, layer 3 is informed of the acknowledgement, and the
===   buffer element resource is de-allocated.
===
===   On completion, V(A) is set to the value of the received N(R).
===
===  DEPENDENCIES
===
===   The given N(R) value must have been validated as being within the
===   range V(A) <= N(R) <= V(S).
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_N_R_acknowledgement_procedures
(
  gllc_ll_sapi_t       lle,
  uint16               recv_seq_num_N_R
)
{
  register gllc_lle_retrx_buf_t *item_p, *next_p;

  /* --------------------------------------------------------------------- */

  /* N(R) is assumed to represent a valid indication of the last in-sequence
     I_S frame received by the peer LLE. All frames in the retransmission
     buffer with an N(S) < rcvd N(R) can be considered as acknowledged. */

  item_p = gllc_lle[lle].ack.retrx_first_p;

  while
  (
    ( item_p != NULL )
    &&
    (
      GMODULO_LT
      (
        512,                                          /* mod */
        item_p -> frame.i_s_frame.send_seq_num_N_S,   /* val */
        recv_seq_num_N_R                              /* lim */
      )
    )
  )
  {
    /* Acknowledged frame. Remove buffer item, adjust occupancy and reset
       timer T201 if it has been associated with this buffer item. */

    GLLC_RETRX_BUF_REMOVE_ITEM( lle, item_p );

    if ( item_p == gllc_lle[lle].ack.T201_association )
    {
      gllc_lle[lle].ack.T201_association = NULL;
      (void)rex_clr_timer( &gllc_timer_t201[lle] );
      GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );
    }

    /* Notify layer 3 about frame delivery. */

    gllc_lle_send_ll_data_cnf_mes( lle, &item_p -> sndcp_ref );

    /* Free retransmission buffer element resource and examine next item. */

    GPRS_PDU_FREE( &item_p -> frame.i_s_frame.l3_pdu );
    next_p = item_p -> next_p;
    GPRS_MEM_FREE( item_p );
    item_p = next_p;
  }

  gllc_lle[lle].ack.ackn_state_var_V_A = recv_seq_num_N_R;

  /* If there are outstanding I_S frames remaining in the retransmission
     buffer and there is currently no T201 association, then associate
     T201 with the item having lowest N(S) (first item in the buffer). */

  if ( gllc_lle[lle].ack.T201_association == NULL )
  {
    item_p = gllc_lle[lle].ack.retrx_first_p;
    if ( item_p != NULL )
    {
      gllc_lle[lle].ack.T201_association = item_p;
      (void)rex_set_timer(
        &gllc_timer_t201[lle], gllc_lle[lle].ack.retrx_interval_T201 );
    }
  }

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_N_R_acknowledgement_procedures() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_ACK_ACKNOWLEDGEMENT_PROCEDURES()
===
===  DESCRIPTION
===
===   The ACK supervisory indication implies the loss of frame with N(S) =
===   N(R) (or V(A) assuming N(R) ack procedures have been performed), and
===   implies the receipt of frame N(S) = N(R) + 1 (or V(A) + 1).
===
===   Runs through the retransmission buffer for the given LLE, marking the
===   first item for retransmission, and searching for the acknowledged
===   element with N(S) = V(A) + 1. If the acknowledged element is found
===   then any T201 timer association is reset.
===
===   If the item marked for retransmission has been retransmitted more than
===   N200 times, indicating that ABM re-establishment is required, then
===   FALSE is returned.
===
===  DEPENDENCIES
===
===   Assumes that N(R) acknowledgement procedures have been performed, and
===   that V(A) has been set as equal to N(R).
===
===  RETURN VALUE
===
===   TRUE  if ACK acknowledgement procedures were successfull.
===   FALSE if ABM re-establishment is to be initiated.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL boolean gllc_lle_ACK_acknowledgement_procedures( gllc_ll_sapi_t lle )
{
  register  gllc_lle_retrx_buf_t *item_p;
  register  boolean              re_establishment_required = FALSE;
            uint16               ackn_state_var_V_A_plus_one;

  /* --------------------------------------------------------------------- */

  ackn_state_var_V_A_plus_one =
    GMODULO(512, gllc_lle[lle].ack.ackn_state_var_V_A + 1);

  /* --------------------------------------------------------------------- */

  item_p = gllc_lle[lle].ack.retrx_first_p;

  if ( item_p != NULL )
  {
    /* The ACK supervisory indication implies the loss of frame with
       N(S) = V(A) (assuming N(R) ack procedures have been performed).
       Mark this for retransmission. */

    if ( item_p -> frame.i_s_frame.send_seq_num_N_S ==
         gllc_lle[lle].ack.ackn_state_var_V_A          )
    {
      if
      (
        (!item_p -> retrx_flag                                      ) &&
        (item_p -> retrx_ctr >= gllc_lle[lle].gen.max_retrx_cnt_N200)
      )
      {
        /* Retransmission count on this element has exceeded the N200
           limit, so a re-establishment procedure is required. */

        MSG_GERAN_HIGH_2_G
        (
          "GLC LL %d: Info IS S:%d N200 Limit",
          gllc_sapi_addr_lookup[lle],
          item_p ->frame.i_s_frame.send_seq_num_N_S
        );

        re_establishment_required = TRUE;
      }
      else if ( !item_p -> retrx_flag )
      {
        /* Only the lowest N(S) item must have it's retransmission count
           incremented. This item represents the one with lowest N(S). */

        item_p -> retrx_flag = TRUE;
        item_p -> retrx_ctr++;
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G( "GLC LL %d: ACK Proc - Retrx Buf Corrupt",
        gllc_sapi_addr_lookup[lle]);
    }
  }

  /* --------------------------------------------------------------------- */

  if ( item_p != NULL )
    item_p = item_p -> next_p;

  while ( item_p != NULL )
  {
    /* The ACK supervisory indication implies the correct receipt of
       frame with N(S) = V(A) + 1 (assuming N(R) ack procedures have
       been performed). */

    if ( item_p -> frame.i_s_frame.send_seq_num_N_S ==
         ackn_state_var_V_A_plus_one                   )
    {
      /* Acknowledged frame. Reset timer T201 if it has been associated
         with this buffer item. */

      if ( item_p == gllc_lle[lle].ack.T201_association )
      {
        gllc_lle[lle].ack.T201_association = NULL;
        (void)rex_clr_timer( &gllc_timer_t201[lle] );
        GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );
      }

      break;                                                      /* BREAK */
    }
    item_p = item_p -> next_p;
  }

  /* --------------------------------------------------------------------- */

  if ( !re_establishment_required )
  {
    /* Flag the re-transmission requirement to the MPEX transmission
       handler. */

    GLLC_SET_LLE_UL_EVT( lle, GLLC_LLE_UL_I_FR_RETRX_EVT );

    /* If there are outstanding I_S frames remaining in the retransmission
       buffer and there is currently no T201 association, then associate
       T201 with the item having lowest N(S) (first item in the buffer). */

    if ( gllc_lle[lle].ack.T201_association == NULL )
    {
      item_p = gllc_lle[lle].ack.retrx_first_p;
      if ( item_p != NULL )
      {
        gllc_lle[lle].ack.T201_association = item_p;
        (void)rex_set_timer(
          &gllc_timer_t201[lle], gllc_lle[lle].ack.retrx_interval_T201 );
      }
    }
  }

  return ( re_establishment_required ? FALSE : TRUE );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_ACK_acknowledgement_procedures() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SACK_ACKNOWLEDGEMENT_PROCEDURES()
===
===  DESCRIPTION
===
===   Runs through the retransmission buffer for the given LLE, and compares
===   the transmitted frames with the given SACK bitmap. If acknowledged,
===   then any T201 timer association is reset, otherwise the element is
===   marked for retransmission.
===
===   If an item marked for retransmission has been retransmitted more than
===   N200 times, indicating that ABM re-establishment is required, then
===   FALSE is returned.
===
===  DEPENDENCIES
===
===   Assumes that N(R) acknowledgement procedures have been performed, and
===   that V(A) has been set as equal to N(R).
===
===  RETURN VALUE
===
===   TRUE  if SACK acknowledgement procedures were successfull.
===   FALSE if ABM re-establishment has been initiated.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL boolean gllc_lle_SACK_acknowledgement_procedures
(
  gllc_ll_sapi_t lle,
  uint8          *sack_bitmap_R,
  uint8          sack_bitmap_len_K
)
{
  register gllc_lle_retrx_buf_t *item_p;
           boolean              re_establishment_required = FALSE;

  register uint32               sack_bitmap_n;
  register uint32               sack_bitmap_octet;
  register uint8                sack_bitmap_mask;
  register uint16               highest_ackd_N_S;


  /* --------------------------------------------------------------------- */

  /* Validity check on input parameters to satisfy KW. */

  if ( sack_bitmap_len_K > GLLC_SACK_BMP_LEN )
  {

    MSG_GERAN_ERROR_1_G( "GLC LL %d: SACK Proc - Incorrect bit map length ",
        gllc_sapi_addr_lookup[lle]);

    return FALSE;
  }


  /* --------------------------------------------------------------------- */

  /* Starting from the top, work through the bitmap until the highest
     acknowledged frame indication is found. This determines the scope of
     the SACK bitmap and permits the highest acknowledged N(S) to be
     determined. */

  sack_bitmap_n = (sack_bitmap_len_K + 1) << 3;

  do
  {
    sack_bitmap_n     -= 1;
    sack_bitmap_octet  = sack_bitmap_n >> 3;
    sack_bitmap_mask   = (0x80 >> (sack_bitmap_n & 0x7));
  }
  while
  (
    !(sack_bitmap_R[sack_bitmap_octet] & sack_bitmap_mask) &&
     (sack_bitmap_n != 0                                 )
  );

  highest_ackd_N_S = GMODULO(
    512, (uint16)(gllc_lle[lle].ack.ackn_state_var_V_A + 1 + sack_bitmap_n) );

  /* --------------------------------------------------------------------- */

  item_p = gllc_lle[lle].ack.retrx_first_p;

  if ( item_p != NULL )
  {
    /* The first frame in the retransmission buffer is not even referenced
       by the SACK bitmap, because if it were received by the peer at the
       point that the bitmap was compiled, then it would be 'in-sequence',
       and covered by the N(R) value. It must therefore be considered lost
       and marked for retransmission by default. */

    if ( item_p -> frame.i_s_frame.send_seq_num_N_S ==
         gllc_lle[lle].ack.ackn_state_var_V_A          )
    {
      if
      (
        (!item_p -> retrx_flag                                      ) &&
        (item_p -> retrx_ctr >= gllc_lle[lle].gen.max_retrx_cnt_N200)
      )
      {
        /* Retransmission count on this element has exceeded the N200
           limit, so a re-establishment procedure is required. */

        MSG_GERAN_HIGH_2_G
        (
          "GLC LL %d: Info IS S:%d N200 Limit",
          gllc_sapi_addr_lookup[lle],
          item_p ->frame.i_s_frame.send_seq_num_N_S
        );

        re_establishment_required = TRUE;
      }
      else if ( !item_p -> retrx_flag )
      {
        /* Only the lowest N(S) item must have it's retransmission count
           incremented. This item represents the one with lowest N(S). */

        item_p -> retrx_flag = TRUE;
        item_p -> retrx_ctr++;
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G( "GLC LL %d: SACK Proc - Retrx Buf Corrupt",
        gllc_sapi_addr_lookup[lle]);
    }
  }

  /* --------------------------------------------------------------------- */

  if ( item_p != NULL )
    item_p = item_p -> next_p;

  while ( item_p != NULL )
  {
    /* Only consider items which are in the scope of the SACK bitmap;
       i.e. for which the N(S) is less than or equal to the highest N(S)
       acknowledged frame as indicated by the SACK bitmap. */

    if
    (
      GMODULO_LTE
      (
        512,                                          /* mod */
        item_p -> frame.i_s_frame.send_seq_num_N_S,   /* val */
        highest_ackd_N_S                              /* lim */
      )
    )
    {
      /* Examine the corresponding SACK bitmap element. */

      if ( item_p -> frame.i_s_frame.send_seq_num_N_S <
           gllc_lle[lle].ack.ackn_state_var_V_A         )
      {
        sack_bitmap_n = (512 + item_p -> frame.i_s_frame.send_seq_num_N_S) -
                        gllc_lle[lle].ack.ackn_state_var_V_A;
      }
      else
      {
        sack_bitmap_n = item_p -> frame.i_s_frame.send_seq_num_N_S -
                        gllc_lle[lle].ack.ackn_state_var_V_A;
      }

      /* The 1 to 256 bit index must now be re-referenced to 0 to 255. */

      if ( sack_bitmap_n != 0 )
        sack_bitmap_n--;

      else
      {
        MSG_GERAN_ERROR_1_G("GLC_LL %d: SACK Ack Proc Err", gllc_sapi_addr_lookup[lle]);
        re_establishment_required = TRUE;
        break;
      }

      sack_bitmap_octet  = sack_bitmap_n >> 3;
      sack_bitmap_n     &= 0x7;
      sack_bitmap_mask   = (0x80 >> sack_bitmap_n);

      /* ................................................................. */

      if ( sack_bitmap_R[sack_bitmap_octet] & sack_bitmap_mask )
      {
        /* Acknowledged frame. Reset timer T201 if it has been associated
           with this buffer item. */

        if ( item_p == gllc_lle[lle].ack.T201_association )
        {
          gllc_lle[lle].ack.T201_association = NULL;
          (void)rex_clr_timer( &gllc_timer_t201[lle] );
          GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );
        }
        item_p = item_p -> next_p;
      }

      /* ................................................................. */

      else if
      (
        (!item_p -> retrx_flag                                      ) &&
        (item_p -> retrx_ctr >= gllc_lle[lle].gen.max_retrx_cnt_N200)
      )
      {
        /* Retransmission count on this element has exceeded the N200
           limit, so a re-establishment procedure is required. */

        re_establishment_required = TRUE;
        item_p = item_p -> next_p;
      }

      /* ................................................................. */

      else
      {
        /* Frame has NOT been acknowledged, so mark for retransmission.
           Only the lowest N(S) item must have it's retransmission count
           incremented. This item does not qualify for an increment.*/

        item_p -> retrx_flag = TRUE;
        item_p = item_p -> next_p;
      }
    }

    /* ................................................................... */

    else
    {
      /* Stop traversing the retransmission buffer because further elements
         are outside the scope of the SACK bitmap. */

      break;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( !re_establishment_required )
  {
    /* Flag the re-transmission requirement to the MPEX transmission
       handler. */

    GLLC_SET_LLE_UL_EVT( lle, GLLC_LLE_UL_I_FR_RETRX_EVT );

    /* If there are outstanding I_S frames remaining in the retransmission
       buffer and there is currently no T201 association, then associate
       T201 with the item having lowest N(S) (first item in the buffer). */

    if ( gllc_lle[lle].ack.T201_association == NULL )
    {
      item_p = gllc_lle[lle].ack.retrx_first_p;
      if ( item_p != NULL )
      {
        gllc_lle[lle].ack.T201_association = item_p;
        (void)rex_set_timer(
          &gllc_timer_t201[lle], gllc_lle[lle].ack.retrx_interval_T201 );
      }
    }
  }

  return ( re_establishment_required ? FALSE:TRUE );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_sack_acknowledgement_procedures() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PEER_RCVR_BUSY_PROCEDURES()
===
===  DESCRIPTION
===
===   Prepares the given LLE to handle a 'Peer Receiver Busy' condition. The
===   LLE is configured for T201 timer scheduled enquiry of the peer's busy
===   condition.
===
===  DEPENDENCIES
===
===   Assumes that N(R) acknowledgement procedures have been performed, and
===   that V(A) has been set as equal to N(R).
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_peer_rcvr_busy_procedures( gllc_ll_sapi_t lle )
{
  register gllc_lle_retrx_buf_t *item_p;

  /* --------------------------------------------------------------------- */

  if ( !gllc_lle[lle].ack.peer_rcvr_busy )
  {
    /* Peer LLE has signalled a 'peer receiver busy condition'. All I
       frame transmission and retransmission must cease. Invoke the 'Peer
       Rcvr Busy' LLE event masking scheme to prevent transmission of
       further I frames. */

    gllc_lle[lle].ack.peer_rcvr_busy = TRUE;
    gllc_lle_set_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_PEER_RCVR_BUSY );
  }

  /* --------------------------------------------------------------------- */

  /* Start/continue the peer LLE enquiry process. Use Timer T201's
     associated I frame to provide a 'reference point' for the enquiry
     process. */

  if ( gllc_lle[lle].ack.T201_association == NULL )
  {
    /* Timer T201 is reset and not currently associated with an I frame.
       An association with the last frame transmitted must be established.
       Use the last item in the re-transmission buffer. */

    item_p = gllc_lle[lle].ack.retrx_last_p;

    if ( item_p != NULL )
    {
      gllc_lle[lle].ack.T201_association              = item_p;
      gllc_lle[lle].ack.T201_association -> retrx_ctr = 0;
    }
    else
    {
      /* For the peer LLE to signal RNR (peer receiver busy) without there
         being any outstanding (unacknowledged) frames in the retransmission
         buffer represents a curious condition! */

      MSG_GERAN_HIGH_1_G( "GLC LL %d: Proc RNR - Empty retrx",
        gllc_sapi_addr_lookup[lle]);

      /* Associate timer T201 with a dummy retransmission element to provide
         a 'reference point' for the enquiry process. */

      gllc_lle[lle].ack.T201_association =
        &gllc_lle[lle].ack.retrx_dummy;
      gllc_lle[lle].ack.T201_association -> retrx_flag   = FALSE;
      gllc_lle[lle].ack.T201_association -> retrx_ctr    = 0;
      gllc_lle[lle].ack.T201_association -> frame.format = GLLC_NO_FORMAT;
    }
  }
  else
  {
    gllc_lle[lle].ack.T201_association -> retrx_ctr = 0;
  }

  (void)rex_set_timer(
    &gllc_timer_t201[lle], gllc_lle[lle].ack.retrx_interval_T201 );

  GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_peer_rcvr_busy_procedures() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PEER_RCVR_BUSY_CLEARANCE_PROCEDURES()
===
===  DESCRIPTION
===
===   Restores the given LLE's operation as before the 'Peer Receiver Busy'
===   condition occured.
===
===  DEPENDENCIES
===
===   Assumes that N(R) acknowledgement procedures have been performed, and
===   that V(A) has been set as equal to N(R).
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_peer_rcvr_busy_clearance_procedures
(
  gllc_ll_sapi_t lle
)
{
  /* --------------------------------------------------------------------- */

  if ( gllc_lle[lle].ack.peer_rcvr_busy )
  {
    /* Peer LLE has signalled a CLEARED 'peer receiver busy condition'.
       All I frame transmission and retransmission may now continue.
       Clear down the 'Peer Rcvr Busy' LLE event masking scheme. */

    gllc_lle[lle].ack.peer_rcvr_busy = FALSE;
    gllc_lle_clr_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_PEER_RCVR_BUSY );

    /* ................................................................... */

    if ( gllc_lle[lle].ack.T201_association != NULL )
    {
      /* Timer T201 has been set and associated with an I Frame
         throughout the previous 'peer receiver busy' condition, and
         the frame has yet to be acknowledged. Reset the timer and
         maintain the association. */

      (void)rex_set_timer(
        &gllc_timer_t201[lle], gllc_lle[lle].ack.retrx_interval_T201 );
      GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );
    }
  }

  /* --------------------------------------------------------------------- */

  else
  {
    /* Examine the LLE's retransmission buffer occupancy to determine
       whether it can accept new frames. */

    if ( gllc_lle[lle].ack.retrx_buf_num_frames <
         gllc_lle[lle].ack.max_retrx_buf_frames_kU )
    {
      if ( gllc_lle[lle].ack.max_retrx_buf_octets_mU == 0 )
      {
        gllc_lle_clr_ul_event_mask_scheme(
          lle, GLLC_LLE_SCHEME_RETRX_BUF_FULL );
      }
      else if
      (
        gllc_lle[lle].ack.retrx_buf_occupancy_B <=
        (
          gllc_lle[lle].ack.max_retrx_buf_octets_mU -
          gllc_lle[lle].ack.max_info_octets_N201_I
        )
      )
      {
        gllc_lle_clr_ul_event_mask_scheme(
          lle, GLLC_LLE_SCHEME_RETRX_BUF_FULL );
      }
    }
  }

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_peer_rcvr_busy_clearance_procedures() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_INITIATE_REESTABLISHMENT()
===
===  DESCRIPTION
===
===   Prepares the given LLE for ABM re-establishment following a condition
===   which warrants it.
===
===   This may be LLC initiated (l3_xid_req == NULL) or layer 3 initiated
===   (l3_xid_req != NULL).
===
===  DEPENDENCIES
===
===   Must be called from the ABM state only.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_initiate_reestablishment
(
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_req
)
{
  uint32 lle_ul_scheme;

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  gllc_lle[lle].diag.reestablishment_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

  /* Clear down all transmission events except for any U frame solicited
     response trx event, as a FRMR response might be awaiting transmission */

  GLLC_CLR_LLE_UL_EVT(
    lle, (GLLC_LLE_UL_TRX_EVT & ~GLLC_LLE_UL_U_FR_SRES_EVT) );

  for
  (
    lle_ul_scheme = 0;
    lle_ul_scheme < (uint32)GLLC_LLE_NUM_SCHEMES;
    lle_ul_scheme++
  )
  {
    gllc_lle_clr_ul_event_mask_scheme(
      lle, (gllc_lle_ul_evt_mask_scheme_t)lle_ul_scheme );
  }

  (void)rex_clr_timer( &gllc_timer_t200[lle] );
  (void)rex_clr_timer( &gllc_timer_t201[lle] );
  gllc_lle[lle].ack.T201_association = NULL;

  GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_EVT );

  /* Generate an XID request based on any pending parameters that havn't
     been negotiated yet and send this in a SABM command to peer LLE.
     Move to the loc est state to await a UA response from the peer LLE. */

  gllc_lle[lle].state = GLLC_LLE_LOC_ESTABLISH;

  (void)gllc_lle_generate_l2_xid_req( lle );

  MSG_GERAN_HIGH_2_G( "GLC LL %d: Info L3:%d Reestablishment",
    gllc_sapi_addr_lookup[lle],
    (l3_xid_req != NULL));
  if ( l3_xid_req == NULL )
  {
    gllc_lle_send_llgmm_status_ind_mes(
      lle, gllc_llme.tlli, GLLCMM_L2_REESTABLISHMENT );
    gllc_lle_send_sabm_cmd_mes( lle, NULL );
    gllc_lle[lle].substate = GLLC_LLE_L2_ESTABLISH;
  }
  else
  {
    gllc_lle_send_sabm_cmd_mes( lle, l3_xid_req );
    gllc_lle[lle].substate = GLLC_LLE_L3_ESTABLISH;
  }

  gllc_lle_set_ul_event_mask_scheme( lle, GLLC_LLE_SCHEME_WAIT_ABM );

} /* end of gllc_lle_initiate_reestablishment() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_I_S_FRAME()
===
===  DESCRIPTION
===
===   Processes a received I+S Format frame from RLC as a function of the
===   given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_i_s_frame
(
  gllc_ll_sapi_t      lle,
  gllc_i_s_frame_t    *i_s_frame,
  uint16              l3_pdu_length,
  uint32              rxd_tlli
)
{
  boolean send_seq_num_N_S_seq_error = FALSE;
  boolean duplicate_send_seq_num_N_S = TRUE;
  boolean re_establishment_required  = FALSE;

  MSG_GERAN_HIGH_3_G( "GLC LL %d: Rcvd IS N(S):%d, state:%d",
    gllc_sapi_addr_lookup[lle], i_s_frame -> send_seq_num_N_S, gllc_lle[lle].state);

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    /* Down link event handler should have filtered this out at TLLI
       validation. */
    GPRS_PDU_FREE( &i_s_frame -> l3_pdu );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:

    /* IS frame receipt is not expected in the ADM state. Respond to the
       peer LLE with an unsolicited unnumbered DM response message. */
    GPRS_PDU_FREE( &i_s_frame -> l3_pdu );
    gllc_lle_send_dm_res_mes( lle, GLLC_PF_UNSOL );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:

    /* IS frame receipt is not expected in the local establishment state.
       Could be because the unnumbered UA response from the peer was
       missed. */
    GPRS_PDU_FREE( &i_s_frame -> l3_pdu );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* IS frame receipt not expected in the remote establishment state.
       This is a local transitory state, where an LL_ESTABLISH_RES message
       is awaited from layer 3. */
    GPRS_PDU_FREE( &i_s_frame -> l3_pdu );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ABM:

    /* I_S Frame N(R) must be validated to ensure it is within the range
       V(A) <= N(R) <= V(S). */

    if
    (
      GMODULO_IN_RGE
      (
        512,                                    /* mod */
        i_s_frame -> recv_seq_num_N_R,          /* val */
        gllc_lle[lle].ack.send_state_var_V_S,   /* max */
        gllc_lle[lle].ack.ackn_state_var_V_A    /* min */
      )
    )
    {
      /* ................................................................. */

      MSG_GERAN_MED_5_G
      (
        "GLC LL %d: Rcvd IS S1S2%d NS:%d NR:%d A%d",
        gllc_sapi_addr_lookup[lle],
        i_s_frame -> cmd_res_S1S2,
        i_s_frame -> send_seq_num_N_S,
        i_s_frame -> recv_seq_num_N_R,
        i_s_frame -> ack_req_A
      );

      if ( i_s_frame -> cmd_res_S1S2 == GLLC_ACK )
      {
        /* Perform acknowledgement procedures based on the frame's receive
           sequence number N(R), and the ACK supervisory content. If the
           ACK processing indicates that ABM re-establishment was not
           required, then perform peer receiver busy clearance procedures. */
        gllc_lle_N_R_acknowledgement_procedures(
          lle, i_s_frame -> recv_seq_num_N_R );

        if ( gllc_lle_ACK_acknowledgement_procedures( lle ) )
          gllc_lle_peer_rcvr_busy_clearance_procedures( lle );
        else
          re_establishment_required = TRUE;
      }

      /* ................................................................. */

      else if ( i_s_frame -> cmd_res_S1S2 == GLLC_SACK )
      {
        /* Perform acknowledgement procedures based on the frame's receive
           sequence number N(R), and the selective acknowledgement SACK
           bitmap. If the SACK processing indicates that ABM
           re-establishment was not required, then perform peer receiver
           busy clearance procedures. */

        gllc_lle_N_R_acknowledgement_procedures(
          lle, i_s_frame -> recv_seq_num_N_R );

        if ( gllc_lle_SACK_acknowledgement_procedures(
               lle, i_s_frame->sack_bitmap_R, i_s_frame->sack_bitmap_len_K) )
        {
          gllc_lle_peer_rcvr_busy_clearance_procedures( lle );
        }
        else
          re_establishment_required = TRUE;
      }

      /* ................................................................. */

      else if ( i_s_frame -> cmd_res_S1S2 == GLLC_RR )
      {
        /* Perform acknowledgement procedures based on the frame's receive
           sequence number N(R). Also perform peer receiver busy clearance
           procedures. */
        gllc_lle_N_R_acknowledgement_procedures(
          lle, i_s_frame -> recv_seq_num_N_R );

        gllc_lle_peer_rcvr_busy_clearance_procedures( lle );
      }

      /* ................................................................. */

      else if ( i_s_frame -> cmd_res_S1S2 == GLLC_RNR )
      {
        /* Perform acknowledgement procedures based on the frame's receive
           sequence number N(R). */
        gllc_lle_N_R_acknowledgement_procedures(
          lle, i_s_frame -> recv_seq_num_N_R );

        gllc_lle_peer_rcvr_busy_procedures( lle );
      }

      /* ................................................................. */
    }
    else
    {
      MSG_GERAN_HIGH_2_G( "GLC LL %d: Rcvd IS Invld R:%d",
        gllc_sapi_addr_lookup[lle], i_s_frame -> recv_seq_num_N_R);
    }

    /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

    /* Confirmed information (IS) frame receipt in the ABM state is
       considered irrespective of whether the 'own receiver busy' condition
       is set or not. If the peer fails to respond to RNR flow control
       orders, then eventually the L3 PDUs will be discarded at the LL
       Downlink DSM watermark. */

    /* If the confirmed information frame is correctly sequenced then
       immediately send the L3 PDU to the appropriate layer 3 entity,
       increment V(R), and update the SACK bitmap record. */

    if ( i_s_frame -> send_seq_num_N_S ==
         gllc_lle[lle].ack.recv_state_var_V_R )
    {
      duplicate_send_seq_num_N_S = FALSE;

      /* Check whether the acknowledged receive state variable V(R)
         modulo is about to roll over when updated, and increment the
         decipher OC by the V(R) modulo if so. */

      if ( gllc_lle[lle].ack.recv_state_var_V_R == 511 )
      {
        gllc_lle[lle].ack.decipher_oc =
          (uint32)(gllc_lle[lle].ack.decipher_oc + 512);
      }

      gllc_lle[lle].ack.recv_state_var_V_R =
        GMODULO( 512, gllc_lle[lle].ack.recv_state_var_V_R + 1 );

      gllc_lle_send_ll_data_ind_mes( lle, i_s_frame, rxd_tlli );

      /* See if there are any complete sequences in the reseq buffer that
         can be sent to layer 3 now that V(R) has been incremented. */

      gllc_lle_resequencing_procedures( lle, NULL, 0, rxd_tlli );
      gllc_lle_update_sack_bitmap_R( lle );

#ifndef FEATURE_GERAN_REDUCED_DEBUG
      /* If received  N(S) == V(R) and N(S) < highest_rcvd_N_S, then
         this N(S) is a retransmission from peer LLC. Increment
         repeated DL frame counter. */
      if
      (
        GMODULO_LT
        (
          512,                                                   /* mod */
          i_s_frame -> send_seq_num_N_S,                         /* val */
          GMODULO(512, gllc_lle[lle].ack.highest_rcvd_N_S)       /* lim */
        )
      )
      {
        gllc_lle[lle].diag.pdu_stats.ack_l3pdu_rerx_cnt++;
        gllc_lle[lle].diag.pdu_stats.ack_octet_rerx_cnt += l3_pdu_length;

      }
        /* In sequence delivery, no frames are missing currently. Increment
           DL received frame counter. */
      else
      {
        gllc_lle[lle].diag.pdu_stats.ack_l3pdu_rx_cnt++;
        gllc_lle[lle].diag.pdu_stats.ack_octet_rx_cnt += l3_pdu_length;
      }
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
    }

    /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

    /* else if the confirmed information frame send sequence number N(S)
       is in the range V(R) < N(S) < (V(R) + kD), then the frame shall be
       considered out of sequence but within the resequencing window, so
       initiate resequencing procedures. */

    else if
    (
      GMODULO_IN_RGE
      (
        512,                                                      /* mod */
        i_s_frame -> send_seq_num_N_S,                            /* val */
        GMODULO_ABS
        ( 512, (gllc_lle[lle].ack.recv_state_var_V_R +
                gllc_lle[lle].ack.max_reseq_buf_frames_kD - 1) ), /* max */
        GMODULO( 512, gllc_lle[lle].ack.recv_state_var_V_R + 1 )  /* min */
      )
    )
    {
      /* If the I frame is received with an N(S) greater than the
         previously received N(S), but differing by more than 1, then
         lost frames must be presumed and a sequence error flagged.
         Note that this could occur during selective retransmission by
         the peer LLE, but in these cases N(S) = V(R) and these would not
         be handled here. */

      if
      (
        GMODULO_GT
        (
          512,                                                    /* mod */
          i_s_frame -> send_seq_num_N_S,                          /* val */
          GMODULO(512, gllc_lle[lle].ack.previous_rcvd_N_S + 1)   /* lim */
        )
      )
      {
        send_seq_num_N_S_seq_error = TRUE;
      }

      duplicate_send_seq_num_N_S = FALSE;
      gllc_lle_resequencing_procedures( lle, i_s_frame, l3_pdu_length, rxd_tlli );
      gllc_lle_update_sack_bitmap_R( lle );

#ifndef FEATURE_GERAN_REDUCED_DEBUG
      /* If the I frame received has an N(S) which is less than the highest
         N(S) previously received, then received N(S) is a retransmition from
         peer. In this case, increment DL repeated frame counter. */

      if
      (
        GMODULO_LT
        (
          512,                                                   /* mod */
          i_s_frame -> send_seq_num_N_S,                         /* val */
          GMODULO(512, gllc_lle[lle].ack.highest_rcvd_N_S)       /* lim */
        )
      )
      {
        /* Increment DL repeated frame counter */
        gllc_lle[lle].diag.pdu_stats.ack_l3pdu_rerx_cnt++;
        gllc_lle[lle].diag.pdu_stats.ack_octet_rerx_cnt += l3_pdu_length;
      }
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
    }

    /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

    if ( !duplicate_send_seq_num_N_S )
    {
      /* Update record of previously rcvd N(S) and highest rcvd N(S). */

      if
      (
        GMODULO_GT
        (
          512,                                                  /* mod */
          i_s_frame -> send_seq_num_N_S,                        /* val */
          gllc_lle[lle].ack.highest_rcvd_N_S                    /* lim */
        )
      )
      {
        gllc_lle[lle].ack.highest_rcvd_N_S = i_s_frame -> send_seq_num_N_S;
      }

      gllc_lle[lle].ack.previous_rcvd_N_S  = i_s_frame -> send_seq_num_N_S;
    }
#ifndef FEATURE_GERAN_REDUCED_DEBUG
    /* Received frames with N(S) which do not satisfy condition
       V(R) <= N(S) <= V(R) + K are duplicate frames. Increment DL repeated
       frame counter. */

    else
    {
      gllc_lle[lle].diag.pdu_stats.ack_l3pdu_rerx_cnt++;
      gllc_lle[lle].diag.pdu_stats.ack_octet_rerx_cnt += l3_pdu_length;
    }
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

    /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

    /* Acknowledgement procedures may have determined that it is necessary
       to reestablish the link ... */

    if ( re_establishment_required )
    {
      gllc_lle_initiate_reestablishment( lle, NULL );
    }

    /* ... else if a supervisory frame transmission requirement has been
       identified, then flag the requirement to the MPEX transmission
       handler. */

    else if ( (gllc_lle[lle].ack.own_rcvr_busy) ||
              (send_seq_num_N_S_seq_error     ) ||
              (i_s_frame -> ack_req_A         )    )
    {
      GLLC_SET_LLE_UL_EVT( lle, GLLC_LLE_UL_S_FR_EVT );
    }

    /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

    /* If the frame has been discarded then free PDU resource. */

    if ( i_s_frame -> l3_pdu != NULL )
      GPRS_PDU_FREE( &i_s_frame -> l3_pdu );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* IS frame receipt is not expected in the local release state.
       Could be because the DISC command was not received by the peer LLE,
       or overlap of signalling and data. Specification requires that this
       frame be ignored. */
    GPRS_PDU_FREE( &i_s_frame -> l3_pdu );
    break;

  /* --------------------------------------------------------------------- */

  default:
    GPRS_PDU_FREE( &i_s_frame -> l3_pdu );
    break;

  /* --------------------------------------------------------------------- */
  }

} /* end of gllc_lle_process_i_s_frame() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_S_FRAME()
===
===  DESCRIPTION
===
===   Processes a received S Format frame from RLC as a function of the
===   given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_s_frame
(
  gllc_ll_sapi_t      lle,
  gllc_s_frame_t      *s_frame,
  uint32              rxd_tlli
)
{
  boolean re_establishment_required = FALSE;

  NOTUSED(rxd_tlli); /* unreferenced var. */

  MSG_GERAN_ERROR_2_G( "GLC LL %d: Rcvd S Frame state:%d",
    gllc_sapi_addr_lookup[lle],
    gllc_lle[lle].state);

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    /* Down link event handler should have filtered this out at TLLI
       validation. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:

    /* S frame receipt is not expected in the ADM state. Respond to the
       peer LLE with an unsolicited unnumbered DM response message. */
    gllc_lle_send_dm_res_mes( lle, GLLC_PF_UNSOL );
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:

    /* S frame receipt is not expected in the local establishment state. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* S frame receipt is not expected in the remote establishment state.
       This is a local transitory state, where an LL_ESTABLISH_RES message
       is awaited from layer 3. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ABM:

    /* S Frame N(R) must be validated to ensure it is within the range
       V(A) <= N(R) <= V(S). */

    if
    (
      GMODULO_IN_RGE
      (
        512,                                    /* mod */
        s_frame -> recv_seq_num_N_R,            /* val */
        gllc_lle[lle].ack.send_state_var_V_S,   /* max */
        gllc_lle[lle].ack.ackn_state_var_V_A    /* min */
      )
    )
    {
      /* ................................................................. */
      MSG_GERAN_MED_4_G("GLC LL %d: Rcvd S S1S2%d NR:%d, A:%d",
        gllc_sapi_addr_lookup[lle],
        s_frame -> cmd_res_S1S2,
        s_frame -> recv_seq_num_N_R,
        s_frame -> ack_req_A);

      if ( s_frame -> cmd_res_S1S2 == GLLC_ACK )
      {
        /* Perform acknowledgement procedures based on the frame's receive
           sequence number N(R), and the ACK supervisory content. If the
           ACK processing indicates that ABM re-establishment was not
           required, then perform peer receiver busy clearance procedures. */
        gllc_lle_N_R_acknowledgement_procedures(
          lle, s_frame -> recv_seq_num_N_R );

        if ( gllc_lle_ACK_acknowledgement_procedures( lle ) )
          gllc_lle_peer_rcvr_busy_clearance_procedures( lle );
        else
          re_establishment_required = TRUE;
      }

      /* ................................................................. */

      else if ( s_frame -> cmd_res_S1S2 == GLLC_SACK )
      {
        /* Perform acknowledgement procedures based on the frame's receive
           sequence number N(R), and the selective acknowledgement SACK
           bitmap. If the SACK processing indicates that ABM
           re-establishment was not required, then perform peer receiver
           busy clearance procedures. */
        gllc_lle_N_R_acknowledgement_procedures(
          lle, s_frame -> recv_seq_num_N_R );

        if ( gllc_lle_SACK_acknowledgement_procedures(
               lle, s_frame->sack_bitmap_R, s_frame->sack_bitmap_len_K) )
        {
          gllc_lle_peer_rcvr_busy_clearance_procedures( lle );
        }
        else
          re_establishment_required = TRUE;
      }

      /* ................................................................. */

      else if ( s_frame -> cmd_res_S1S2 == GLLC_RR )
      {
        /* Perform acknowledgement procedures based on the frame's receive
           sequence number N(R). Also perform peer receiver busy clearance
           procedures. */
        gllc_lle_N_R_acknowledgement_procedures(
          lle, s_frame -> recv_seq_num_N_R );

        gllc_lle_peer_rcvr_busy_clearance_procedures( lle );
      }

      /* ................................................................. */

      else if ( s_frame -> cmd_res_S1S2 == GLLC_RNR )
      {
        /* Perform acknowledgement procedures based on the frame's receive
           sequence number N(R), and peer receiver busy condition
           procedures. */
        gllc_lle_N_R_acknowledgement_procedures(
          lle, s_frame -> recv_seq_num_N_R );

        gllc_lle_peer_rcvr_busy_procedures( lle );
      }

      /* ................................................................. */

      /* Acknowledgement procedures may have determined that it is necessary
         to reestablish the link ... */

      if ( re_establishment_required )
      {
        gllc_lle_initiate_reestablishment( lle, NULL );
      }

      /* ... else if a supervisory frame transmission requirement has been
         requested, then flag the requirement to the MPEX transmission
         handler. */

      else if ( s_frame -> ack_req_A )
      {
        GLLC_SET_LLE_UL_EVT( lle, GLLC_LLE_UL_S_FR_EVT );
      }

      /* ................................................................. */
    }
    else
    {
      MSG_GERAN_HIGH_2_G( "GLC LL %d: S invalid R:%d",
        gllc_sapi_addr_lookup[lle], s_frame -> recv_seq_num_N_R);
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* S frame receipt is not expected in the local release state.
       Could be because the DISC command was not received by the peer LLE,
       or overlap of signalling and data. Specification requires that this
       frame be ignored. */
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */
    break;

  /* --------------------------------------------------------------------- */
  }

} /* end of gllc_lle_process_s_frame() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RCVD_EMPTY_GMM_INFO_MSG()
===
===  DESCRIPTION
===
===   The Agilent 8960 employs empty GMM Information messages for
===   transmission when engaged in BLER test. These can be extremely
===   frequent and carry a high processing overhead, only to be discarded
===   prior to being delivered to GMM.
===
===   This function allows the message to be detected and subsequently
===   discarded much earlier in the processing thread. This permits an
===   essential saving of CPU whilst running this test.
===
===  DEPENDENCIES
===
===   It is expected that the LL PDU is contained in a single DSM item,
===   for the detection to be valid. With a small PDU this would be the case.
===
===  RETURN VALUE
===
===   TRUE if an empty GMM Information Message has been detected, FALSE
===   otherwise.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL boolean gllc_lle_rcvd_empty_gmm_info_msg
(
  gllc_ll_sapi_t      lle,
  gllc_ui_frame_t     *ui_frame,
  uint16              l3_pdu_length
)
{
  /* --------------------------------------------------------------------- */

  /* Test for a GMM Information Message ID (0x21) */

  return
  (
    (lle                               == GLLC_LL_SAPI_1_GMM ) &&
    (l3_pdu_length                     == 2                  ) &&
    (ui_frame -> l3_pdu -> used        == 2                  ) &&
    (ui_frame -> l3_pdu -> data_ptr[0] == 0x08               ) &&
    (ui_frame -> l3_pdu -> data_ptr[1] == 0x21               )
  );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_rcvd_empty_gmm_info_msg() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_UI_FRAME_IS_DUPLICATED()
===
===  DESCRIPTION
===
===   If the unconfirmed information frame send sequence number is in the
===   range (V(UR) - 32) <= N(U) < V(UR), and the frame has been received
===   already, then the frame must be considered as a duplicate.
===
===  DEPENDENCIES
===
===   Bit position 0 of the bitmap must be aligned with (V(UR) - 1) Mod 512.
===
===  RETURN VALUE
===
===   TRUE if the given UI frame is represented in the UI Frame receive
===   bitmap, FALSE otherwise.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL boolean gllc_lle_ui_frame_is_duplicated
(
  gllc_ll_sapi_t      lle,
  gllc_ui_frame_t     *ui_frame
)
{
  boolean duplicate_ui_frame = FALSE;
  uint16  bitmap_index;

  /* --------------------------------------------------------------------- */

  if
  (
    GMODULO_IN_RGE
    (
      512,                                                           /*mod*/
      ui_frame -> send_seq_num_N_U,                                  /*val*/
      GMODULO_ABS(512, gllc_lle[lle].unack.recv_state_var_V_UR -  1),/*max*/
      GMODULO_ABS(512, gllc_lle[lle].unack.recv_state_var_V_UR - 32) /*min*/
    )
  )
  {
    /* Send sequence number falls in range (V(UR) - 32) <= N(U) < V(UR), so
       check the bitmap to see whether the frame has been received before. */

    bitmap_index =
      GMODULO_ABS
      (
        512,
        (
          gllc_lle[lle].unack.recv_state_var_V_UR -
          ui_frame -> send_seq_num_N_U - 1
        )
      );

    if ( gllc_lle[lle].unack.recv_state_bitmap & (1 << bitmap_index) )
    {
      /* UI frame is received as a duplicate. */

      if ( lle == GLLC_LL_SAPI_1_GMM )
      {
        /* The LL SAPI is GLLC_LL_SAPI_1_GMM, so disregard the fact that
           this is a duplicate frame, as this may be due to MM context
           deletion in the SGSN due to mobile reachable timer expiry whilst
           the MS was out of service. Set V(UR) to N(U) and clear the bitmap
           record. */

        gllc_lle[lle].unack.recv_state_var_V_UR =
          ui_frame -> send_seq_num_N_U;

        gllc_lle[lle].unack.recv_state_bitmap = 0;
      }
      else
        duplicate_ui_frame = TRUE;
    }
  }

  /* --------------------------------------------------------------------- */

  return ( duplicate_ui_frame );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_ui_frame_is_duplicated() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_UPDATE_UNACKNOWLEDGED_RECV_STATE()
===
===  DESCRIPTION
===
===   Updates the receive state variable V(UR), receive state bitmap, and
===   decipher overflow counter to reflect the correct reception of the
===   given UI frame.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_update_unacknowledged_recv_state
(
  gllc_ll_sapi_t      lle,
  gllc_ui_frame_t     *ui_frame
)
{
  uint16 bitmap_index, bitmap_shift;

  /* --------------------------------------------------------------------- */

  if
  (
    GMODULO_IN_RGE
    (
      512,                                                           /*mod*/
      ui_frame -> send_seq_num_N_U,                                  /*val*/
      GMODULO_ABS(512, gllc_lle[lle].unack.recv_state_var_V_UR -  1),/*max*/
      GMODULO_ABS(512, gllc_lle[lle].unack.recv_state_var_V_UR - 32) /*min*/
    )
  )
  {
    /* Send sequence number falls in range (V(UR) - 32) <= N(U) < V(UR), so
       make an entry in the receive state bitmap. */

    bitmap_index =
      GMODULO_ABS
      (
        512,
        (
          gllc_lle[lle].unack.recv_state_var_V_UR -
          ui_frame -> send_seq_num_N_U - 1
        )
      );

    /* bitmap_index will always be less than 32 according to the checks above but to appease
    ** LLVM, an extra check is made here
    */
    if (bitmap_index < 32)
    gllc_lle[lle].unack.recv_state_bitmap |= (1 << bitmap_index);
  }

  /* --------------------------------------------------------------------- */

  else
  {
    /* Send sequence number falls outside range (V(UR) - 32) <= N(U) < V(UR),
       so the receive state bitmap must be shifted and bit position 0 set to
       reflect the reception of the current UI frame. */

    bitmap_shift =
      GMODULO_ABS
      (
        512,                                                         /*mod*/
        ( 1 + ui_frame -> send_seq_num_N_U -
          gllc_lle[lle].unack.recv_state_var_V_UR)                   /*val*/
      );

    if ( bitmap_shift >= 32 )
      gllc_lle[lle].unack.recv_state_bitmap = (1 << 0);
    else
      gllc_lle[lle].unack.recv_state_bitmap =
        (gllc_lle[lle].unack.recv_state_bitmap << bitmap_shift) | (1 << 0);


    /* If unacknowledged receive state variable V(UR) modulo is about to roll
       over when updated and ciphering is not enabled, increment the decipher
       OC by 512. This is to maintain the synchronisation of DL OC in UE and
       SGSN.

       If ciphering is enabled, the decipher OC is not updated here and is
       instead updated upon successful decipher during initial frame decode. */

    if ( ( GMODULO(512, ui_frame -> send_seq_num_N_U + 1) <
           gllc_lle[lle].unack.recv_state_var_V_UR          ) &&
         ( ( gllc_llme.gea == GMMLLC_GEA_NONE           ) ||
           ( gllc_llme.gea == GMMLLC_GEA_UNDEFINED      )   )    )
    {
      gllc_lle[lle].unack.decipher_oc =
        (uint32)(gllc_lle[lle].unack.decipher_oc + 512);

       gllc_debug_print_lle_cipher_params ( lle );

    }


    /* Update the unacknowledged receive state variable V(UR). */

    gllc_lle[lle].unack.recv_state_var_V_UR =
      GMODULO( 512, ui_frame -> send_seq_num_N_U + 1 );
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_update_unacknowledged_recv_state() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_UI_FRAME()
===
===  DESCRIPTION
===
===   Processes a received UI Format frame from RLC as a function of the
===   given LLE's state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_ui_frame
(
  gllc_ll_sapi_t      lle,
  gllc_ui_frame_t     *ui_frame,
  uint16              l3_pdu_length,
  uint32              rxd_tlli
)
{
  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    /* UI format frames from the peer LLE are invalid in state TLLI
       Unassigned, for all LL SAPIs except GLLC_LL_SAPI_1_GMM. */

    if ( lle != GLLC_LL_SAPI_1_GMM )
    {
      MSG_GERAN_LOW_1_G( "GLC LL %d: Rcvd UI Fr Unass",
        gllc_sapi_addr_lookup[lle]);
      GPRS_PDU_FREE( &ui_frame -> l3_pdu );
      break;
    }
    /* Drop through if lle == GLLC_LL_SAPI_1_GMM. */

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:
  case GLLC_LLE_LOC_ESTABLISH:
  case GLLC_LLE_REM_ESTABLISH:
  case GLLC_LLE_ABM:
  case GLLC_LLE_LOC_RELEASE:

    /* Unconfirmed information (UI) frame receipt in all states except the
       TLLI unassigned state is considered. */

    /* Firstly check for an Empty GMM Information Message and and discard
       as Agilent 8960 BLER test residue. */

    if ( gllc_lle_rcvd_empty_gmm_info_msg(lle, ui_frame, l3_pdu_length) )
    {
      GPRS_PDU_FREE( &ui_frame -> l3_pdu );
      break;
    }

    /* If the unconfirmed information frame send sequence number is a
       duplicate then discard, otherwise update receive state variables
       and send the L3 PDU to the appropriate layer 3 entity. */

    else if ( !gllc_lle_ui_frame_is_duplicated( lle, ui_frame) )
    {
      MSG_GERAN_MED_4_G
      (
        "GLC LL %d: Rcvd UI U:%d Cipher Len:%d, E:%d",
        gllc_sapi_addr_lookup[lle],
        ui_frame -> send_seq_num_N_U,
        l3_pdu_length,
        ui_frame -> encrypted_E
      );

      if ( ui_frame -> send_seq_num_N_U !=
           gllc_lle[lle].unack.recv_state_var_V_UR )
      {
        MSG_GERAN_HIGH_3_G
        (
          "GLC LL %d: Info UI U:%d Frame Seq Jump %d",
          gllc_sapi_addr_lookup[lle], ui_frame -> send_seq_num_N_U,
          GMODULO_ABS
          (
            512,
            ui_frame->send_seq_num_N_U - gllc_lle[lle].unack.recv_state_var_V_UR
          )
        );
      }

      gllc_lle_update_unacknowledged_recv_state( lle, ui_frame );
      gllc_lle_send_ll_unitdata_ind_mes( lle, ui_frame, rxd_tlli );

#ifndef FEATURE_GERAN_REDUCED_DEBUG
      gllc_lle[lle].diag.pdu_stats.unack_l3pdu_rx_cnt += 1;
      gllc_lle[lle].diag.pdu_stats.unack_octet_rx_cnt += l3_pdu_length;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
    }
    else
    {
      MSG_GERAN_HIGH_3_G
      (
        "GLC LL %d: Rcvd UI Dup U:%d V_UR:%d",
        gllc_sapi_addr_lookup[lle],
        ui_frame -> send_seq_num_N_U,
        gllc_lle[lle].unack.recv_state_var_V_UR
      );

      GPRS_PDU_FREE( &ui_frame -> l3_pdu );
    }

    llc_log_recv_ui_frame
    (
      ui_frame -> send_seq_num_N_U,
      gllc_sapi_addr_lookup[lle],
      l3_pdu_length,
      ui_frame -> encrypted_E
    );
    break;

  /* --------------------------------------------------------------------- */

  default:

    MSG_GERAN_ERROR_2_G( "GLC LL %d: Rcvd UI State Err %d",
      gllc_sapi_addr_lookup[lle], gllc_lle[lle].state );
    GPRS_PDU_FREE( &ui_frame -> l3_pdu );
    break;

  /* --------------------------------------------------------------------- */
  }

} /* end of gllc_lle_process_ui_frame() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_TIMER_T200_EXPIRY()
===
===  DESCRIPTION
===
===   Processes a T200 timer expiry event as a function of the given LLE's
===   state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_timer_t200_expiry( gllc_ll_sapi_t lle )
{
  MSG_GERAN_HIGH_2_G( "GLC LL %d: Expy T200 state:%d",
    gllc_sapi_addr_lookup[lle],
    gllc_lle[lle].state);

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    if ( lle == GLLC_LL_SAPI_1_GMM )
    {
      if ( (gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE) ||
           (gllc_lle[lle].substate == GLLC_LLE_L2_XID_EXCHANGE)    )
      {
        if ( !gllc_lle_resend_xid_cmd_mes( lle ) )
        {
          /* The number of XID command retransmission attempts has now met
             the N200 limit, so inform layer 3 and GMM about the lack of
             peer LLE response. */

          gllc_lle_send_llgmm_status_ind_mes(
            lle, gllc_llme.tlli, GLLCMM_NO_PEER_RESPONSE );
          gllc_lle_send_ll_status_ind_mes(
            lle, gllc_llme.tlli, GPRS_LL_NO_PEER_RESPONSE );
          gllc_lle[lle].substate = GLLC_LLE_NONE;
        }
      }
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:
    if ( (gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE) ||
         (gllc_lle[lle].substate == GLLC_LLE_L2_XID_EXCHANGE)    )
    {
      if ( !gllc_lle_resend_xid_cmd_mes( lle ) )
      {
        /* The number of XID command retransmission attempts has now met
           the N200 limit, so inform layer 3 and GMM about the lack of peer
           LLE response. */

        gllc_lle_send_llgmm_status_ind_mes(
          lle, gllc_llme.tlli, GLLCMM_NO_PEER_RESPONSE );
        gllc_lle_send_ll_status_ind_mes(
          lle, gllc_llme.tlli, GPRS_LL_NO_PEER_RESPONSE );
        gllc_lle[lle].substate = GLLC_LLE_NONE;
      }
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:

    /* A layer 3 establishment request which resulted in the sending of a
       SABM unnumbered command to the peer LLE, has not been acknowledged
       with the receipt of a UA unnumbered acknowledgement from the peer
       LLE within the T200 time limit. */
    if ( !gllc_lle_resend_sabm_cmd_mes( lle ) )
    {
      /* The number of SABM command retransmission attempts has now met
         the N200 limit, so inform layer 3 and GMM about the lack of peer
         LLE response, and return to the ADM state. */

      gllc_lle_send_llgmm_status_ind_mes(
        lle, gllc_llme.tlli, GLLCMM_NO_PEER_RESPONSE );
      gllc_lle_send_ll_rel_ind_mes( lle, gllc_llme.tlli, GPRS_LL_NO_PEER_RESPONSE );
      gllc_lle_enter_state_adm( lle );
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* T200 Expiry invalid in state Rem Est. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ABM:

    if ( (gllc_lle[lle].substate == GLLC_LLE_L3_XID_EXCHANGE) ||
         (gllc_lle[lle].substate == GLLC_LLE_L2_XID_EXCHANGE)    )
    {
      if ( !gllc_lle_resend_xid_cmd_mes( lle ) )
      {
        /* The number of XID command retransmission attempts has now met
           the N200 limit, so inform layer 3 and GMM about the lack of peer
           LLE response, and return to the ADM state. */

        gllc_lle_send_llgmm_status_ind_mes(
          lle, gllc_llme.tlli, GLLCMM_NO_PEER_RESPONSE );
        gllc_lle_send_ll_rel_ind_mes(
          lle, gllc_llme.tlli, GPRS_LL_NO_PEER_RESPONSE );
        gllc_lle_enter_state_adm( lle );
      }
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* A layer 3 release request, with parameter Local = FALSE, which
       resulted in the sending of a DISC unnumbered command to the peer LLE,
       has not been acknowledged with the receipt of a DM unnumbered
       response from the peer LLE within the T200 time limit. */
    if ( !gllc_lle_resend_disc_cmd_mes( lle ) )
    {
      /* The number of DISC command retransmission attempts has now met
         the N200 limit, so inform GMM about the lack of peer LLE response,
         confirm release to layer 3, and return to the ADM state. */

      gllc_lle_send_llgmm_status_ind_mes(
        lle, gllc_llme.tlli, GLLCMM_NO_PEER_RESPONSE );
      gllc_lle_send_ll_rel_cnf_mes( lle, gllc_llme.tlli );
      gllc_lle_enter_state_adm( lle );
    }
    break;

  /* --------------------------------------------------------------------- */

  default:
    /* Illegal state identifier. */
    break;

  /* --------------------------------------------------------------------- */
  }

} /* end of gllc_lle_process_timer_t200_expiry() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_TIMER_T201_EXPIRY()
===
===  DESCRIPTION
===
===   Processes a T201 timer expiry event as a function of the given LLE's
===   state.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_process_timer_t201_expiry(  gllc_ll_sapi_t lle )
{
  MSG_GERAN_HIGH_2_G( "GLC LL %d: Expy T201, state:%d",
    gllc_sapi_addr_lookup[lle],
    gllc_lle[lle].state);

  switch ( gllc_lle[lle].state )
  {
  /* --------------------------------------------------------------------- */

  case GLLC_LLE_TLLI_UNASSIGNED:

    /* T201 Expiry invalid in state TLLI Unassigned. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ADM:

    /* T201 Expiry invalid in state ADM. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_ESTABLISH:

    /* T201 Expiry invalid in state Loc Est. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_REM_ESTABLISH:

    /* T201 Expiry invalid in state Rem Est. */
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_ABM:

    if ( gllc_lle[lle].ack.T201_association != NULL )
    {
      if ( gllc_lle[lle].ack.T201_association -> frame.format !=
           GLLC_NO_FORMAT                                        )
      {
        MSG_GERAN_MED_2_G
        (
          "GLC LL %d: Expy T201 S:%d ABM",
          gllc_sapi_addr_lookup[lle],
          gllc_lle[lle].ack.T201_association ->
            frame.i_s_frame.send_seq_num_N_S
        );
      }

      if
      (
        (!gllc_lle[lle].ack.T201_association -> retrx_flag)
        &&
        (
          gllc_lle[lle].ack.T201_association -> retrx_ctr >=
          gllc_lle[lle].gen.max_retrx_cnt_N200
        )
      )
      {
        /* Retransmission count on this element has exceeded the N200
           limit. */

        MSG_GERAN_HIGH_2_G
        (
          "GLC LL %d: Info IS S:%d N200 Limit",
          gllc_sapi_addr_lookup[lle],
          gllc_lle[lle].ack.T201_association->frame.i_s_frame.send_seq_num_N_S
        );
        gllc_lle_initiate_reestablishment( lle, NULL );
      }
      else
      {
        /* An acknowledgement for the associated retransmission buffer I
           frame has not been forthcoming, so initiate retransmission and
           include an acknowledgement request. Set the T201 timer again with
           the existing association. */

        if ( !gllc_lle[lle].ack.T201_association -> retrx_flag )
        {
          gllc_lle[lle].ack.T201_association -> retrx_flag = TRUE;
          gllc_lle[lle].ack.T201_association -> retrx_ctr++;
        }
        (void)rex_set_timer(
          &gllc_timer_t201[lle], gllc_lle[lle].ack.retrx_interval_T201 );

        GLLC_CLR_LLE_UL_EVT( lle, GLLC_LLE_UL_TIMER_T201_EVT );

        if ( gllc_lle[lle].ack.T201_association -> frame.format ==
             GLLC_NO_FORMAT                                        )
        {
          /* Associated Retrx buffer element is 'dummy'.
             Only acknowledgement request transmission required. */

          GLLC_SET_LLE_UL_EVT( lle, GLLC_LLE_UL_S_FR_ABIT_EVT );
        }
        else
        {
          /* Associated Retrx buffer element is 'real'. */

          GLLC_SET_LLE_UL_EVT( 
            lle, (GLLC_LLE_UL_I_FR_RETRX_EVT | GLLC_LLE_UL_S_FR_ABIT_EVT) );
        }
      }
    }
    else
    {
      MSG_GERAN_LOW_1_G( "GLC LL %d: Expy T201 S:None ABM",
        gllc_sapi_addr_lookup[lle]);
    }
    break;

  /* --------------------------------------------------------------------- */

  case GLLC_LLE_LOC_RELEASE:

    /* T201 Expiry invalid in state Loc Rel. */
    break;

  /* --------------------------------------------------------------------- */

  default:

    /* Illegal state identifier. */
    break;

  /* --------------------------------------------------------------------- */
  }

} /* end of gllc_lle_process_timer_t201_expiry() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SET_OWN_RCVR_BUSY_COND()
===
===  DESCRIPTION
===
===   Assert an 'own receiver busy' condition for all LLE's which are in
===   the ABM state, and request a supervisory frame transmission to signal
===   RNR to the associated peer.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

#ifdef FEATURE_DSM_WM_CB
void gllc_llme_set_own_rcvr_busy_cond_dyn
(
  dsm_watermark_type *wm_p,
  void *context
)
#else
void gllc_llme_set_own_rcvr_busy_cond_dyn(void)
#endif /* FEATURE_DSM_WM_CB */
{
  MSG_GERAN_LOW_0("GLC LL: Info DL Suspend");

  {
    if ( gllc_lle[GLLC_LL_SAPI_3_LL3].state == GLLC_LLE_ABM )
    {
      gllc_lle[GLLC_LL_SAPI_3_LL3].ack.own_rcvr_busy = TRUE;
      GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_S_FR_EVT );
    }

    if ( gllc_lle[GLLC_LL_SAPI_5_LL5].state == GLLC_LLE_ABM )
    {
      gllc_lle[GLLC_LL_SAPI_5_LL5].ack.own_rcvr_busy = TRUE;
      GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_S_FR_EVT );
    }

    if ( gllc_lle[GLLC_LL_SAPI_9_LL9].state == GLLC_LLE_ABM )
    {
      gllc_lle[GLLC_LL_SAPI_9_LL9].ack.own_rcvr_busy = TRUE;
      GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_S_FR_EVT );
    }

    if ( gllc_lle[GLLC_LL_SAPI_11_LL11].state == GLLC_LLE_ABM )
    {
      gllc_lle[GLLC_LL_SAPI_11_LL11].ack.own_rcvr_busy = TRUE;
      GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_S_FR_EVT );
    }
  }

} /* end of gllc_llme_set_own_rcvr_busy_cond() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_CLR_OWN_RCVR_BUSY_COND()
===
===  DESCRIPTION
===
===   Negate any asserted 'own receiver busy' conditions and request a
===   supervisory frame transmission to signal RR to the associated peer.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

#ifdef FEATURE_DSM_WM_CB
void gllc_llme_clr_own_rcvr_busy_cond_dyn
(
  dsm_watermark_type *wm_p,
  void *context
)
#else
void gllc_llme_clr_own_rcvr_busy_cond_dyn(void)
#endif /* FEATURE_DSM_WM_CB */
{

  MSG_GERAN_LOW_0_G( "GLC LL: Info DL Resume");

  {
    if ( gllc_lle[GLLC_LL_SAPI_3_LL3].ack.own_rcvr_busy )
    {
      gllc_lle[GLLC_LL_SAPI_3_LL3].ack.own_rcvr_busy = FALSE;
      GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_S_FR_EVT );
    }

    if ( gllc_lle[GLLC_LL_SAPI_5_LL5].ack.own_rcvr_busy )
    {
      gllc_lle[GLLC_LL_SAPI_5_LL5].ack.own_rcvr_busy = FALSE;
      GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_S_FR_EVT );
    }

    if ( gllc_lle[GLLC_LL_SAPI_9_LL9].ack.own_rcvr_busy )
    {
      gllc_lle[GLLC_LL_SAPI_9_LL9].ack.own_rcvr_busy = FALSE;
      GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_S_FR_EVT );
    }

    if ( gllc_lle[GLLC_LL_SAPI_11_LL11].ack.own_rcvr_busy )
    {
      gllc_lle[GLLC_LL_SAPI_11_LL11].ack.own_rcvr_busy = FALSE;
      GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_S_FR_EVT );
    }
  }

} /* end of gllc_llme_clr_own_rcvr_busy_cond() */



/*** EOF: don't remove! ***/

