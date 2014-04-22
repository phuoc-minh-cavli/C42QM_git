/*==============================================================================

                M O D E M   C O N F I G   U I M

GENERAL DESCRIPTION
  Module supporting accessing the UIM for the purposes of automatically
  selecting an associated configuration. This module is confined to reading the
  identifying parameters from the UIM - the actual configuration selection is
  done in mcfg_sel.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_sel/src/mcfg_uim.c#1 $
$DateTime: 2020/02/19 23:07:19 $
$Author: pwbldsvc $
$Change: 22644646 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
09/07/12 bd   Created module

==============================================================================*/

#include "comdef.h"

#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"

#include "mcfg_int.h"
#include "mcfg_nv.h"
#include "mcfg_osal.h"
#include "mcfg_sel.h"
#include "mcfg_uim.h"
#include "mcfg_utils.h"
#include "mcfg_proc.h"

#include "mcfg_setting.h"
#include "mcfg_map.h"

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/

#define MNC_LEN_BYTE  (4)
#define MCC_LEN_BYTE  (3)

/*==============================================================================
  Typedefs
==============================================================================*/

#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
#define MCFG_NUM_OF_EXT_SESSIONS_PER_SLOT  3	//(CARD_SESSION_SLOT | GW_SESSION_SUB | 1X_SESSION_SUB)
#else
#define MCFG_NUM_OF_EXT_SESSIONS_PER_SLOT  2	//(CARD_SESSION_SLOT | GW_SESSION_SUB)
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/

typedef struct
{
  //mmgsdi_session_type_enum_type               session_type;  
  mcfg_sub_id_type_e_type                       sub_id;        
  mmgsdi_session_id_type                        session_id;    
  mmgsdi_app_enum_type                          app_type;      
  size_t                                        num_mnc_digits;
  uint8                                         imsi[MMGSDI_IMSI_LEN];
} mcfg_mmgsdi_ctx_t;

/*==============================================================================
  Variables
==============================================================================*/

static struct {
  mmgsdi_client_id_type  client_id;

  struct {
    /* session id (for slot session)*/
    mmgsdi_session_id_type        session_id;

#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
    /* session id for non-provisioned ISIM APP slot session*/
    mmgsdi_session_id_type        session_id_isim;

	mmgsdi_static_data_type       isim_app_id;

    mcfg_trl_carrier_mcc_mnc_s_type impi_plmn;

    mcfg_uim_EF_read_state_e_type impi_read_status;

    /* Set to TRUE if IMPI selection needed for the card. Decided based on the country. */
    boolean                       impi_sel_needed;

    /* Set to TRUE once client ID & session ID successfully allocated for ISIM app */
    boolean 					  isim_ready;
#endif

    /* Set to TRUE once client ID & session ID successfully allocated */
    boolean                       ready;

    mcfg_uim_card_state_e_type    card_state;

    mcfg_sub_id_type_e_type       sub_id;

    uint8                          autoselect_nv;
  }slot[MCFG_NUM_OF_UIM_SLOT];

  struct {
    /* ext session id (for subscriptions)*/
    mmgsdi_session_id_type        ext_session_id[MCFG_UIM_NUM_OF_EXT_SESSION];
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
    mcfg_trl_carrier_mcc_mnc_s_type imsi_plmn;
#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
    mcfg_trl_carrier_mcc_mnc_s_type imsi1x_plmn;
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/
#endif
    mmgsdi_app_enum_type          ext_app_type[MCFG_UIM_NUM_OF_EXT_SESSION];
  }sub[MCFG_NUM_OF_SUB_ID];

} mcfg_uim_state;

static timer_type       mcfg_uim_flexmap_timer;  
static timer_group_type mcfg_uim_flexmap_timer_group;
#define MCFG_UIM_FLEXMAP_TIMER_TIMEOUT (2) 
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
/* Spec ETSI TS 101 220: 3GPP RID (A000000087) + ISIM APP ID (1004) + Bytes for application provider
    - 3GPP ISIM app ID in the card always should start with this */
static uint8 mcfg_uim_isim_aid[] = { 0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x04 };
#endif

/*==============================================================================

                 PRIVATE FUNCTION FORWARD DECLARATIONS

==============================================================================*/

static uint8 mcfg_uim_map_session_id_to_slot_index
(
  mmgsdi_session_id_type session_id
);

mcfg_slot_index_type_e_type mcfg_uim_map_sub_id_to_slot_index
(
  mcfg_sub_id_type_e_type sub_id
);

static mcfg_sub_id_type_e_type mcfg_uim_map_session_id_to_sub_id
(
  mmgsdi_session_id_type session_id
);

static mcfg_slot_index_type_e_type mcfg_uim_map_mmgsdi_slot_to_slot_index
(
  mmgsdi_slot_id_enum_type slot_id
);

static void mcfg_uim_mmgsdi_evt_cb
(
  const mmgsdi_event_data_type *event
);

static void mcfg_uim_mmgsdi_evt_proc
(
  const mmgsdi_event_data_type *event
);

static void mcfg_uim_mmgsdi_read_cnf_proc
(
  mmgsdi_return_enum_type     status,
  const mmgsdi_read_cnf_type *cnf_data
);

static void mcfg_uim_read_iccid_if_ready
(
  uint8 slot_index
);

static void mcfg_uim_mmgsdi_rsp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

static void mcfg_uim_mmgsdi_rsp_proc
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

static void mcfg_uim_mmgsdi_session_open_ext_sync_cnf_proc
(
  uint8                 num_sessions,
  const mmgsdi_session_open_info_type *session_info_ptr
);

#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
static boolean mcfg_uim_is_isim_aid
(
  const mmgsdi_static_data_type *aid_ptr
);
static void mcfg_uim_open_mmgsdi_session_for_isim
(
  mcfg_slot_index_type_e_type slot_index
);
static void mcfg_uim_mmgsdi_session_open_cnf_proc
(
  mmgsdi_return_enum_type             status,
  const mmgsdi_session_open_cnf_type *cnf_data
);
static void mcfg_uim_read_impi_if_ready
(
  uint8 slot_index
);
static boolean mcfg_uim_wait_for_IMPI
(
  mcfg_sub_id_type_e_type sub_id,
  mcfg_trl_carrier_mcc_mnc_s_type **impi_plmn  
);
#endif

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
static void mcfg_uim_read_imsi_if_ready
(
  uint8 slot_index,
  mmgsdi_app_enum_type app_type
);
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/
/*===========================================================================

  FUNCTION  mcfg_uim_read_autoselect_nv

  DESCRIPTION
    This function is used to get mcfg refresh autoenabled setting

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
uint8 mcfg_uim_nv_read
(
  mcfg_slot_index_type_e_type slot_index
)
{
  uint8 autoselect_enabled;
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index <= (MCFG_REFRESH_INDEX_MAX));

  status = mcfg_nv_read(MCFG_NV_ITEM_AUTOSELECT_BY_UIM, 0, slot_index,
                        &autoselect_enabled, sizeof(autoselect_enabled));
  if (status != MCFG_NV_STATUS_OK)
  {
    autoselect_enabled = MCFG_UIM_AUTOSELECT_DEFAULT;
    if (status != MCFG_NV_STATUS_NOT_ACTIVE)
    {
      MCFG_MSG_MED_2("Couldn't read autoselect feature NV item: status %d slot %d",
                       status, slot_index);
    }
  }
  else if (((autoselect_enabled & MCFG_UIM_NV_HYBRIDSELECT_MASK) > 0) &&
    ((autoselect_enabled & MCFG_UIM_NV_HYBRIDSELECT_MASK) != MCFG_UIM_NV_HYBRIDSELECT_MASK))
  {
    autoselect_enabled = autoselect_enabled | MCFG_UIM_NV_HYBRIDSELECT_MASK;
    MCFG_MSG_MED_1("selection enabled, force iccid and imsi selection: %x", autoselect_enabled);

    status = mcfg_nv_write(MCFG_NV_ITEM_AUTOSELECT_BY_UIM, 0, slot_index,
                           &autoselect_enabled, sizeof(autoselect_enabled));
    if (status != MCFG_NV_STATUS_OK)
    {
      MCFG_MSG_ERROR_2("Couldn't write autoselect feature NV item: status %d slot %d",
                       status, slot_index);
    }
  }

  MCFG_MSG_LOW_2("read_autoselect_nv sloti%d NV=%x", slot_index, autoselect_enabled);

  return autoselect_enabled;
}

/*===========================================================================

  FUNCTION mcfg_uim_nv_autoselect_enabled

  DESCRIPTION
    Checks whether UIM-based automatic MCFG_SW selection is enabled.

  DEPENDENCIES
    None

  PARAMETERS
    sub_id  subscription ID

  RETURN VALUE
    TRUE if enabled, FALSE otherwise;
    TURE if enabled for any subscription with MCFG_NUM_OF_SUB_ID as input.

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_nv_autoselect_enabled
(
  mcfg_slot_index_type_e_type slot_index
)
{
  uint8 autoselect_enabled;
  mcfg_nv_status_e_type status;
  mcfg_slot_index_type_e_type start, end, i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index <= MCFG_NUM_OF_UIM_SLOT);

  if (slot_index < MCFG_NUM_OF_UIM_SLOT)
  {
    start = slot_index;
    end = (slot_index+1);
    MCFG_MSG_MED_1("mcfg_uim_nv_autoselect_enabled for sloti %d", slot_index);
  }
  else
  {
    start = MCFG_SLOT_INDEX_FIRST;
    end = MCFG_NUM_OF_UIM_SLOT;
    MCFG_MSG_MED("mcfg_uim_nv_autoselect_enabled for any slot");
  }

  for (i = start; i < end; i++)
  {
    MCFG_MSG_HIGH_2("autoselect_nv 0x%02x for slot %d", mcfg_uim_state.slot[i].autoselect_nv, i);
    if(mcfg_uim_state.slot[i].autoselect_nv == MCFG_UIM_NV_UNINIT)
    {
      autoselect_enabled = mcfg_uim_nv_read(i);
      mcfg_uim_state.slot[i].autoselect_nv = autoselect_enabled;
    }
    else
    {
      autoselect_enabled = mcfg_uim_state.slot[i].autoselect_nv;
    }

    if ((autoselect_enabled & MCFG_UIM_NV_AUTOSELECT_MASK) > 0)
    {
      break;
    }
  }

  MCFG_MSG_MED_1("mcfg_uim_nv_autoselect_enabled NV=%x", autoselect_enabled);
  return ((autoselect_enabled & MCFG_UIM_NV_AUTOSELECT_MASK) > 0);
} /* mcfg_uim_nv_autoselect_enabled() */

/*===========================================================================

  FUNCTION mcfg_uim_nv_flexmap_enabled

  DESCRIPTION
    Checks whether UIM-based flexmapping selection is enabled.

  DEPENDENCIES
    None

  PARAMETERS
    sub_id  subscription ID

  RETURN VALUE
    TRUE if enabled, FALSE otherwise;

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_nv_flexmap_enabled
(
  mcfg_slot_index_type_e_type slot_index
)
{
  uint8 nv_value;
  boolean flexmap_enabled = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index < MCFG_NUM_OF_UIM_SLOT);

  if(mcfg_uim_state.slot[slot_index].autoselect_nv == MCFG_UIM_NV_UNINIT)
  {
    nv_value = mcfg_uim_nv_read(slot_index);
    mcfg_uim_state.slot[slot_index].autoselect_nv = nv_value;
  }
  else
  {
    nv_value = mcfg_uim_state.slot[slot_index].autoselect_nv;
  }

  if((nv_value != MCFG_UIM_NV_UNINIT) && (nv_value & MCFG_UIM_NV_FLEXMAP_MASK))
  {
    if(nv_value & (MCFG_UIM_NV_AUTOSELECT_MASK | MCFG_UIM_NV_IMSISELECT_MASK))
    {
      flexmap_enabled = TRUE;
    }
    else
    {
      mcfg_uim_state.slot[slot_index].autoselect_nv = nv_value & (~MCFG_UIM_NV_FLEXMAP_MASK);
      MCFG_MSG_MED_2("Disable Flex mapping with auto selection disabled nv from %x to %x", 
                     nv_value, mcfg_uim_state.slot[slot_index].autoselect_nv);
      mcfg_uim_nv71546_write(slot_index);

      return FALSE;
    }
  }

  MCFG_MSG_MED_2("nv_flexmap_enabled %d NV=%x", flexmap_enabled, nv_value);
  return flexmap_enabled;
} /* mcfg_uim_nv_flexmap_enabled() */

/*===========================================================================

  FUNCTION  mcfg_uim_nv_refresh_enabled

  DESCRIPTION
    This function is used to get refresh setting

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_nv_refresh_enabled
(
  mcfg_slot_index_type_e_type slot_index
)
{
  boolean ret = FALSE;
#ifdef FEATURE_MCFG_REFRESH
  uint8 nv_value;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index <= MCFG_REFRESH_INDEX_MAX);

  if(mcfg_uim_state.slot[slot_index].autoselect_nv == MCFG_UIM_NV_UNINIT)
  {
    nv_value = mcfg_uim_nv_read(slot_index);
    mcfg_uim_state.slot[slot_index].autoselect_nv = nv_value;
  }
  else
  {
    nv_value = mcfg_uim_state.slot[slot_index].autoselect_nv;
  }

  if ((nv_value & MCFG_UIM_NV_REFRESH_MASK) > 0)
  {
    MCFG_MSG_LOW_1("mcfg_refresh_disabled sloti %d", slot_index);
    ret = FALSE;
  }
  else
  {
    MCFG_MSG_LOW_1("mcfg_refresh_enabled sloti %d", slot_index);
    ret = TRUE;
  }
#endif

  return ret;
}

/*===========================================================================

  FUNCTION mcfg_uim_nv_updateonly_enabled

  DESCRIPTION
    Checks whether updateonly is enabled.

  DEPENDENCIES
    None

  PARAMETERS
    sub_id  subscription ID

  RETURN VALUE
    TRUE if enabled, FALSE otherwise;

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_nv_updateonly_enabled
(
  mcfg_slot_index_type_e_type slot_index
)
{
  uint8 nv_value;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index < MCFG_NUM_OF_UIM_SLOT);

  if(mcfg_uim_state.slot[slot_index].autoselect_nv == MCFG_UIM_NV_UNINIT)
  {
    nv_value = mcfg_uim_nv_read(slot_index);
    mcfg_uim_state.slot[slot_index].autoselect_nv = nv_value;
  }
  else
  {
    nv_value = mcfg_uim_state.slot[slot_index].autoselect_nv;
  }

  MCFG_MSG_MED_1("updateonly_enabled NV=%x", nv_value);
  return ((nv_value & MCFG_UIM_NV_UPDATEONLY_MASK) > 0);
} /* mcfg_uim_nv_updateonly_enabled() */

/*===========================================================================

  FUNCTION mcfg_uim_nv_imsiselect_enabled

  DESCRIPTION
    Checks whether imsi based auto selection is enabled.

  DEPENDENCIES
    None

  PARAMETERS
    sub_id  subscription ID

  RETURN VALUE
    TRUE if enabled, FALSE otherwise;

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_nv_imsiselect_enabled
(
  mcfg_slot_index_type_e_type slot_index
)
{
  uint8 imsiselect_enabled;
  mcfg_nv_status_e_type status;
  mcfg_slot_index_type_e_type start, end, i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index <= MCFG_NUM_OF_UIM_SLOT);

  if (slot_index < MCFG_NUM_OF_UIM_SLOT)
  {
    start = slot_index;
    end = (slot_index+1);
    MCFG_MSG_MED_1("mcfg_uim_nv_imsiselect_enabled for sloti %d", slot_index);
  }
  else
  {
    start = MCFG_SLOT_INDEX_FIRST;
    end = MCFG_NUM_OF_UIM_SLOT;
    MCFG_MSG_MED("mcfg_uim_nv_imsiselect_enabled for any slot");
  }

  for (i = start; i < end; i++)
  {
    MCFG_MSG_HIGH_2("imsiselect_nv 0x%02x for slot %d", mcfg_uim_state.slot[i].autoselect_nv, i);
    if(mcfg_uim_state.slot[i].autoselect_nv == MCFG_UIM_NV_UNINIT)
    {
      imsiselect_enabled = mcfg_uim_nv_read(i);
      mcfg_uim_state.slot[i].autoselect_nv = imsiselect_enabled;
    }
    else
    {
      imsiselect_enabled = mcfg_uim_state.slot[i].autoselect_nv;
    }

    if ((imsiselect_enabled & MCFG_UIM_NV_IMSISELECT_MASK) > 0)
    {
      break;
    }
  }

  MCFG_MSG_MED_1("mcfg_uim_nv_imsiselect_enabled NV=%x", imsiselect_enabled);
  return ((imsiselect_enabled & MCFG_UIM_NV_IMSISELECT_MASK) > 0);
} /* mcfg_uim_nv_imsiselect_enabled() */


boolean mcfg_uim_nv71546_update
(
  mcfg_slot_index_type_e_type sloti,
  uint8   value,
  uint8   mask
)
{
  uint8 nv;
  boolean changed = FALSE; 
  MCFG_ASSERT_COND_RET_FALSE(sloti < MCFG_NUM_OF_UIM_SLOT);

  nv = mcfg_uim_state.slot[sloti].autoselect_nv;
  changed = !((nv & mask) == (value & mask));

  nv = nv & (~mask);
  nv = nv | (value & mask);

  mcfg_uim_state.slot[sloti].autoselect_nv = nv;
  return changed;
}

/*===========================================================================

  FUNCTION  mcfg_uim_nv71546_write

  DESCRIPTION
    This function is used to write mcfg autoenabled setting into EFS

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
uint8 mcfg_uim_nv71546_write
(
  mcfg_slot_index_type_e_type sloti
)
{
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(sloti < MCFG_NUM_OF_UIM_SLOT);

  MCFG_MSG_LOW_2("write autoselect_nv 0x%x for sloti %d", 
                 mcfg_uim_state.slot[sloti].autoselect_nv,
                 sloti);
  if (mcfg_uim_state.slot[sloti].autoselect_nv == MCFG_UIM_NV_UNINIT || 
      mcfg_uim_state.slot[sloti].autoselect_nv == MCFG_UIM_AUTOSELECT_DEFAULT)
  {
    status = mcfg_nv_delete(MCFG_NV_ITEM_AUTOSELECT_BY_UIM, 0, sloti);
  }
  else
  {
    status = mcfg_nv_write(MCFG_NV_ITEM_AUTOSELECT_BY_UIM, 0, sloti,
                           &mcfg_uim_state.slot[sloti].autoselect_nv, 
                           sizeof(mcfg_uim_state.slot[sloti].autoselect_nv));
  }

  if (status != MCFG_NV_STATUS_OK)
  {
    MCFG_MSG_ERROR_2("Couldn't write or delete autoselect feature NV item: status %d sloti %d",
                     status, sloti);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

  FUNCTION  mcfg_uim_get_flexmap_timer

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
uint8 mcfg_uim_get_flexmap_timer
(
  void
)
{
  uint8 value;
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = mcfg_nv_read(MCFG_NV_ITEM_UIM_SLOT_MAPPING_TIMER, 0, 
                        MCFG_SLOT_INDEX_FIRST,
                        &value, sizeof(value));
  if (status != MCFG_NV_STATUS_OK)
  {
    value = MCFG_UIM_FLEXMAP_TIMER_TIMEOUT;
    if (status != MCFG_NV_STATUS_NOT_ACTIVE)
    {
      MCFG_MSG_HIGH_1("Couldn't read flexmap timer NV item: status %d", status);
    }
  }

  MCFG_MSG_MED_1("mcfg_uim_get_flexmap_timer NV=%d", value);
  return value;
} /* mcfg_uim_get_flexmap_timer() */


/*===========================================================================

  FUNCTION mcfg_uim_decode_iccid

  DESCRIPTION
    Decodes the ICCID from its SIM encoding (binary coded decimal) into an
    ASCII string.

  DEPENDENCIES
    None

  PARAMETERS
    iccid_bcd    [in] input ICCID in EF-ICCID format (BCD)
    iccid_ascii [out] array to store decoded ICCID; buffer must be at least
                      MCFG_UIM_ICCID_STR_BUF_LEN bytes long

  RETURN VALUE
    TRUE if ICCID successfully decoded, FALSE otherwise (e.g. bad ICCID)

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_decode_iccid
(
  const uint8 *iccid_bcd,
  char        *iccid_ascii
)
{
  uint32 i;
  uint8 digit;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(iccid_bcd);
  MCFG_CHECK_NULL_PTR_RET_FALSE(iccid_ascii);

  memset(iccid_ascii, 0, MCFG_UIM_ICCID_STR_BUF_LEN);
  for (i = 0; i < MCFG_UIM_ICCID_LEN; i++)
  {
    digit = (iccid_bcd[i] & 0x0F);
    if (digit < 0xA)
    {
      iccid_ascii[i * 2] = (char) (digit + 0x30);
    }
    else
    {
      /* A~F: 65~70 */
      iccid_ascii[i * 2] = (char) (digit + 55);
    }

    digit = ((iccid_bcd[i] >> 4) & 0x0F);
    if (digit < 0xA)
    {
      iccid_ascii[(i * 2) + 1] = (char) (digit + 0x30);
    }
    else
    {
      /* A~F: 65~70 */
      iccid_ascii[(i * 2) + 1] = (char) (digit + 55);
    }
  }

  return TRUE;
} /* mcfg_uim_decode_iccid() */

#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
/*===========================================================================

  FUNCTION mcfg_uim_decode_impi

  DESCRIPTION
       Decodes the IMPI and format into PLMN/MCC_MNC.

  DEPENDENCIES
       None
	
  PARAMETERS
       impi        [in]   input impi buffer in EF-impi format
       impi_len  [in]   input impi length
       plmn       [out] MCC/MNC

  RETURN VALUE
       TRUE if IMPI successfully decoded, FALSE otherwise

  SIDE EFFECTS
       None

===========================================================================*/
static boolean mcfg_uim_decode_impi
(
  const uint8 *impi,
  mmgsdi_len_type impi_len,
  mcfg_trl_carrier_mcc_mnc_s_type *plmn
)
{
  char current_impi[MCFG_UIM_IMPI_LEN];
  char *mnc_start = NULL;
  char* mnc_end = NULL;
  char *mcc_start = NULL;
  char *mcc_end = NULL;
  char *p = NULL;
  boolean status = 0;
  const char *mnc_start_pattern = "@ims.mnc";
  const char *mcc_start_pattern = ".mcc";
  const char *mcc_end_pattern = ".3gppnetwork.org";
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(impi);
  MCFG_CHECK_NULL_PTR_RET_FALSE(plmn);
  MCFG_ASSERT_COND_RET_FALSE(impi_len > 0);
  memset(plmn, 0, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
  memset(current_impi, 0, MCFG_UIM_IMPI_LEN);

  memscpy(current_impi, (MCFG_UIM_IMPI_LEN - 1), impi, impi_len);
  current_impi[MCFG_UIM_IMPI_LEN - 1] = '\0';
  MCFG_MSG_SPRINTF_1("Current IMPI: %s", current_impi);

  mnc_start = strstr(current_impi, mnc_start_pattern);
  if(mnc_start != NULL)
  {
    mcc_start = strstr(mnc_start, mcc_start_pattern);
	if(mcc_start!= NULL)
    {
      mcc_end = strstr(mcc_start, mcc_end_pattern);
    }
  }

  if((mnc_start == NULL) || (mcc_start == NULL) || (mcc_end == NULL))
  {
    MCFG_MSG_ERROR("Decode IMPI failed: Pattern not matched");
    return FALSE;
  }

  mnc_start += strlen(mnc_start_pattern);
  mnc_end = mcc_start;
  mcc_start += strlen(mcc_start_pattern);

  if((mcc_end > mcc_start) && 
     (mnc_end > mnc_start) &&
     ((mnc_end-mnc_start) <= (MNC_LEN_BYTE - 1)) &&
     ((mcc_end-mcc_start) <= MCC_LEN_BYTE))
  {
    status = TRUE;
    /* convert MNC string into numeric */
    for(p=mnc_start; p<mnc_end; p++)
    {
      if((*p < '0') || (*p > '9')) /* is not a digit */
      {
        status = FALSE; 
        MCFG_MSG_ERROR("Decode IMPI failed: MNC is not a number");
        break;
      }
      plmn->mnc = (plmn->mnc * 10) + (*p - '0');
    }

    /* convert MCC string into numeric */
    for(p=mcc_start; p<mcc_end; p++)
    {
      if((*p < '0') || (*p > '9')) /* is not a digit */
      {
        status = FALSE;
        MCFG_MSG_ERROR("Decode IMPI failed: MCC is not a number");
        break;
      }
      plmn->mcc = (plmn->mcc * 10) + (*p - '0');
    }
  }
  else
  {
    MCFG_MSG_ERROR_2("Decode IMPI failed: Either MNCLength:%d OR MCCLength:%d is wrong", 
              mnc_end-mnc_start, mcc_end-mcc_start);
  }

  if((plmn->mnc == 0) && (plmn->mcc == 0))
  {
    MCFG_MSG_ERROR("Decode IMPI failed: MCC and MNC are NULL");
    status = FALSE;
  }

  return status;
}/* mcfg_uim_decode_impi() */
#endif

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
/*===========================================================================

  FUNCTION mcfg_uim_iccid_blacklisted

  DESCRIPTION
    Check ICCID is blacklisted for selection, if yes, fall back to imsi selection

  DEPENDENCIES
    None

  PARAMETERS
    iccid_ascii [in] array to store decoded ICCID; buffer must be at least
                      MCFG_UIM_ICCID_STR_BUF_LEN bytes long

  RETURN VALUE
    TRUE if ICCID is blacklisted, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_uim_iccid_blacklisted
(
  char *iccid
)
{
   /* TODO: add new EFS items for a list of blacklisted iccid */
  return TRUE;
} /* mcfg_uim_iccid_blacklisted */

/*===========================================================================

  FUNCTION mcfg_uim_decode_imsi

  DESCRIPTION
    Decodes the EF-IMSI format into PLMN/MCC_MNC.

  DEPENDENCIES
    None

  PARAMETERS
    imsi  [in]  input imsi in EF-imsi format
    plmn  [out] MCC/MNC

  RETURN VALUE
    TRUE if imsi successfully decoded, FALSE otherwise (e.g. bad imsi)

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_uim_decode_imsi
(
  const uint8 *imsi,
  mcfg_trl_carrier_mcc_mnc_s_type *plmn,
  uint8 num_of_mnc
)
{
  uint8 i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(imsi);
  MCFG_CHECK_NULL_PTR_RET_FALSE(plmn);
  
  plmn->mcc = (imsi[1] >>  4)*100 + (imsi[2] & 0xF)*10 + (imsi[2] >>  4);

  if (num_of_mnc == 3)
  {
    plmn->mnc = (imsi[3] & 0xF)*100 + (imsi[3] >> 4)*10 + (imsi[4] & 0xF);
  }
  else
  {
    plmn->mnc = (imsi[3] & 0xF)*10 + (imsi[3] >> 4);
  }

  MCFG_MSG_HIGH_3("mcfg_uim_decode_imsi %d digit mcc=%03d mnc=%03d", num_of_mnc, plmn->mcc, plmn->mnc);
  return TRUE;
} /* mcfg_uim_decode_imsi() */

#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
static boolean mcfg_uim_decode_cdma_imsi
(
  const uint8 *cdma_imsi,
  mcfg_trl_carrier_mcc_mnc_s_type *plmn
)
{
  uint8  tens = 0;
  uint8  ones = 0;
  uint8  hundreds = 0;
  uint16 temp = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(cdma_imsi);
  MCFG_CHECK_NULL_PTR_RET_FALSE(plmn);
  
  //MCC
  temp =  (cdma_imsi[MMGSDI_CDMA_IMSI_LEN - 1] & 0x3);
  temp =  (temp << 8);
  temp |=  cdma_imsi[MMGSDI_CDMA_IMSI_LEN - 2];
  MCFG_MSG_LOW_1("mcfg_uim_decode_cdma_imsi mcc 0x%03x", temp);
  temp += 111;

  ones = temp - 10 * (temp / 10);
  tens = (temp / 10) - 10 * (temp / 100);
  hundreds = temp / 100;
  MCFG_MSG_LOW_3("mcfg_uim_decode_cdma_imsi mcc %d hundreds %d tens %d ones", hundreds, tens, ones);

  if (ones == 0) {tens--;}
  if (tens == 0) {hundreds--;}
  plmn->mcc = 100 * hundreds + 10 * tens + ones;

  //MNC
  temp = cdma_imsi[MMGSDI_CDMA_IMSI_LEN - 4];
  MCFG_MSG_LOW_1("mcfg_uim_decode_cdma_imsi mcc 0x%02x", temp);
  temp += 11;

  ones = temp - 10 * (temp / 10);
  tens = (temp / 10) - 10 * (temp / 100);
  hundreds = temp / 100;
  MCFG_MSG_LOW_3("mcfg_uim_decode_cdma_imsi mnc %d hundreds %d tens %d ones", hundreds, tens, ones);

  if (ones == 0) {tens--;}
  if (tens == 0) {hundreds--;}
  plmn->mnc = 10 * tens + ones;

  MCFG_MSG_HIGH_2("mcfg_uim_decode_cdma_imsi mcc=%03d mnc=%02d", plmn->mcc, plmn->mnc);
  return TRUE;
} /* mcfg_uim_decode_cdma_imsi() */
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

/*===========================================================================

  FUNCTION mcfg_uim_map_session_id_to_slot_index

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS
    session id

  RETURN VALUE
    slot index

  SIDE EFFECTS
    None

===========================================================================*/
static uint8 mcfg_uim_map_session_id_to_slot_index
(
  mmgsdi_session_id_type session_id
)
{
  uint8 slot_index;
  uint8 ext_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (slot_index = 0; slot_index < MCFG_NUM_OF_UIM_SLOT; slot_index++)
  {
    if (mcfg_uim_state.slot[slot_index].session_id == session_id)
    {
      return slot_index;
    }

#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
    if (mcfg_uim_state.slot[slot_index].session_id_isim == session_id)
    {
      return slot_index;
    }
#endif

    for (ext_index = 0; ext_index < MCFG_UIM_NUM_OF_EXT_SESSION; ext_index++)
    {
      if (mcfg_uim_state.sub[slot_index].ext_session_id[ext_index] == session_id)
      {
        return mcfg_uim_map_sub_id_to_slot_index(slot_index);
      }
    }
  }

  return MCFG_NUM_OF_UIM_SLOT;
} /* mcfg_uim_map_session_id_to_slot_index() */

/*===========================================================================

  FUNCTION mcfg_uim_map_session_id_to_sub_id

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS
    session id

  RETURN VALUE
    slot index

  SIDE EFFECTS
    None

===========================================================================*/
static mcfg_sub_id_type_e_type mcfg_uim_map_session_id_to_sub_id
(
  mmgsdi_session_id_type session_id
)
{
  mcfg_sub_id_type_e_type sub_id;
  uint8 ext_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (sub_id = 0; sub_id < MCFG_NUM_OF_SUB_ID; sub_id++)
  {
    for (ext_index = 0; ext_index < MCFG_UIM_NUM_OF_EXT_SESSION; ext_index++)
    {
      MCFG_MSG_LOW_3("sub %d gw/1x %d %x", sub_id, ext_index, mcfg_uim_state.sub[sub_id].ext_session_id[ext_index]);
      if (mcfg_uim_state.sub[sub_id].ext_session_id[ext_index] == session_id)
      {
        return sub_id;
      }
    }
  }

  MCFG_MSG_ERROR_1("Session ID not found %llx", session_id);
  return MCFG_NUM_OF_SUB_ID;
} /* mcfg_uim_map_session_id_to_sub_id() */

/*===========================================================================

  FUNCTION mcfg_uim_map_mmgsdi_slot_to_slot_index

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS
    slot id

  RETURN VALUE
    slot index

  SIDE EFFECTS
    None

===========================================================================*/
static mcfg_slot_index_type_e_type mcfg_uim_map_mmgsdi_slot_to_slot_index
(
  mmgsdi_slot_id_enum_type slot_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_ERROR(slot_id > 0, MCFG_NUM_OF_UIM_SLOT);

  return (slot_id - 1);
} /* mcfg_uim_map_mmgsdi_slot_to_slot_index() */

/*===========================================================================

  FUNCTION mcfg_uim_map_sub_id_to_slot_index

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS
    slot id

  RETURN VALUE
    slot index

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_slot_index_type_e_type mcfg_uim_map_sub_id_to_slot_index
(
  mcfg_sub_id_type_e_type sub_id
)
{
  mcfg_slot_index_type_e_type slot_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_ERROR(sub_id < MCFG_NUM_OF_SUB_ID, MCFG_NUM_OF_UIM_SLOT);

  for (slot_index = 0; slot_index < MCFG_NUM_OF_UIM_SLOT; slot_index++ )
  {
    if (mcfg_uim_state.slot[slot_index].sub_id == sub_id)
    {
      return slot_index;
    }
  }


  return MCFG_NUM_OF_UIM_SLOT;
} /* mcfg_uim_map_sub_id_to_slot_index() */

/*===========================================================================

  FUNCTION mcfg_uim_map_sub_id_to_slot_index

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS
    slot id

  RETURN VALUE
    slot index

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_sub_id_type_e_type mcfg_uim_map_slot_index_to_sub_id
(
  mcfg_slot_index_type_e_type slot_index
)
{
  mcfg_sub_id_type_e_type sub_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_ERROR(slot_index < MCFG_NUM_OF_UIM_SLOT, MCFG_NUM_OF_SUB_ID);

  return mcfg_uim_state.slot[slot_index].sub_id;
} /* mcfg_uim_map_sub_id_to_slot_index() */

/*===========================================================================

  FUNCTION mcfg_uim_write_mapping

  DESCRIPTION
    Write UIM mapping table into EFS

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_write_mapping
(
  void
)
{
  mcfg_nv_status_e_type       status;
  mcfg_slot_index_type_e_type slot_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_MSG_LOW("Write mapping table");

  for (slot_index = MCFG_SLOT_INDEX_FIRST; 
        slot_index < MCFG_NUM_OF_UIM_SLOT; slot_index++)
  {
    if (mcfg_uim_state.slot[slot_index].sub_id == 
        (mcfg_sub_id_type_e_type)slot_index)
    {
      status = mcfg_nv_delete(MCFG_NV_ITEM_UIM_SLOT_MAPPING, 0, slot_index);
      if (status != MCFG_NV_STATUS_OK)
      {
        MCFG_MSG_ERROR_2("Couldn't delete uim mapping slot %d NV item: status %d",
                         slot_index, status);
        return FALSE;
      }
    }
    else
    {
      status = mcfg_nv_write(MCFG_NV_ITEM_UIM_SLOT_MAPPING, 0, slot_index, 
                             (void *) &mcfg_uim_state.slot[slot_index].sub_id, 
                             sizeof(mcfg_uim_state.slot[slot_index].sub_id));
      if (status != MCFG_NV_STATUS_OK)
      {
        MCFG_MSG_ERROR_2("Couldn't write uim mapping slot %d NV item: status %d",
                         slot_index, status);
        return FALSE;
      }
    }
  }

  return TRUE;
} /* mcfg_uim_write_mapping() */

/*===========================================================================

  FUNCTION mcfg_uim_validate_mapping

  DESCRIPTION
    Validate UIM mapping table, if it's not validate, set as 1:1 mapping

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_validate_mapping
(
  void
)
{
  mcfg_slot_index_type_e_type slot_index;
  uint8                       sub_mask = 0;
  boolean                     valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (slot_index = 0; slot_index < MCFG_NUM_OF_UIM_SLOT; slot_index++)
  {
    if ((mcfg_uim_state.slot[slot_index].sub_id >= MCFG_NUM_OF_SUB_ID) ||
        (sub_mask & (1 << mcfg_uim_state.slot[slot_index].sub_id)))
    {
      MCFG_MSG_ERROR_3("Invalid mapping slot %d to sub_id %d with submask %x",
                       slot_index, mcfg_uim_state.slot[slot_index].sub_id, sub_mask);
      valid = FALSE;
      break;
    }
  }

  if (!valid)
  {
    for (slot_index = 0; slot_index < MCFG_NUM_OF_UIM_SLOT; slot_index++)
    {
      mcfg_uim_state.slot[slot_index].sub_id = (mcfg_sub_id_type_e_type)slot_index;
    }
    mcfg_uim_write_mapping();
  }

  return valid;
} /* mcfg_uim_validate_mapping() */

/*===========================================================================

  FUNCTION mcfg_uim_read_mapping

  DESCRIPTION
    Read UIM mapping table from EFS and validate it

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_read_mapping
(
  void
)
{
  mcfg_slot_index_type_e_type slot_index;
  mcfg_sub_id_type_e_type     sub_id;
  mcfg_nv_status_e_type       status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (slot_index = 0; slot_index < MCFG_NUM_OF_UIM_SLOT; slot_index++)
  {
    status = mcfg_nv_read(MCFG_NV_ITEM_UIM_SLOT_MAPPING, 0, slot_index, 
                          &sub_id, sizeof(sub_id));
    if (status == MCFG_NV_STATUS_NOT_ACTIVE)
    {
      mcfg_uim_state.slot[slot_index].sub_id = (mcfg_sub_id_type_e_type)slot_index;
    }
    else if (status == MCFG_NV_STATUS_OK && sub_id < MCFG_NUM_OF_SUB_ID)
    {
      mcfg_uim_state.slot[slot_index].sub_id = sub_id;
    }
    else
    {
      mcfg_uim_state.slot[slot_index].sub_id = MCFG_SUB_ID_INVALID;
    }
  }

  return mcfg_uim_validate_mapping();
} /* mcfg_uim_read_mapping() */

/*===========================================================================

  FUNCTION mcfg_uim_process_mapping

  DESCRIPTION
    Process UIM mapping table for flex mapping

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    TRUE if MBN is switched, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_process_mapping
(
  mcfg_slot_index_type_e_type slot_index,
  mcfg_sub_id_type_e_type     sub_id,
  boolean                     refresh
)
{
  mcfg_config_id_s_type       active_id;
  mcfg_config_id_s_type       oppo_active_id;
  static mcfg_slot_index_type_e_type oppo_slot_index = 0;
  static mcfg_sub_id_type_e_type     oppo_sub_id = 0;
  boolean config_matches = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index < MCFG_NUM_OF_UIM_SLOT);
  MCFG_ASSERT_COND_RET_FALSE(sub_id < MCFG_NUM_OF_SUB_ID);

  MCFG_MSG_LOW_3("Update mapping slot %d to sub %d from %d",slot_index, sub_id,
                 mcfg_uim_state.slot[slot_index].sub_id);
  if(mcfg_uim_state.slot[slot_index].sub_id == sub_id)
  {
    MCFG_MSG_MED_2("Same mapping, ignore slot %d to sub %d",slot_index, sub_id);
    return FALSE;
  }

  memset(&active_id,      0, sizeof(active_id));
  memset(&oppo_active_id, 0, sizeof(oppo_active_id));

  oppo_sub_id = mcfg_uim_state.slot[slot_index].sub_id;

  for (oppo_slot_index = MCFG_SLOT_INDEX_FIRST; 
        oppo_slot_index < MCFG_NUM_OF_UIM_SLOT; oppo_slot_index++)
  {
    if(mcfg_uim_state.slot[oppo_slot_index].sub_id == sub_id)
    {
      MCFG_MSG_MED_1("Get active ID for sub %d", sub_id);
      if(!mcfg_utils_get_active_config(MCFG_TYPE_SW, &active_id, sub_id))
      {
        MCFG_MSG_MED_1("No active ID for sub %d, get pending instead", sub_id);
        if(!mcfg_utils_get_selected_config(MCFG_TYPE_SW, &active_id, sub_id))
        {
          MCFG_MSG_MED_1("No pending ID for sub %d either", sub_id);
        }
      }

      MCFG_MSG_MED_1("Get active ID for oppo sub %d", oppo_sub_id);
      if(!mcfg_utils_get_active_config(MCFG_TYPE_SW, &oppo_active_id, oppo_sub_id))
      {
        MCFG_MSG_MED_1("No active ID for oppo sub %d, get pending instead", oppo_sub_id);
        if(!mcfg_utils_get_selected_config(MCFG_TYPE_SW, &oppo_active_id, oppo_sub_id))
        {
          MCFG_MSG_MED_1("No pending ID for oppo sub %d either", oppo_sub_id);
        }
      }

      config_matches = mcfg_utils_config_id_matches(&active_id, &oppo_active_id);
      if (!config_matches)
      {
        MCFG_MSG_MED_1("Deactivate sub items for sub %d", sub_id);
        mcfg_utils_deactivate_config_by_type(MCFG_TYPE_SW, sub_id, FALSE, MCFG_REFRESH_TYPE_SUBS);

        MCFG_MSG_MED_1("Deactivate sub items for oppo sub %d", oppo_sub_id);
        mcfg_utils_deactivate_config_by_type(MCFG_TYPE_SW, oppo_sub_id, FALSE, MCFG_REFRESH_TYPE_SUBS);

        if (oppo_active_id.id_len > 0)
        {
          MCFG_MSG_MED_1("Select for sub %d", sub_id);
          mcfg_utils_set_selected_config(MCFG_TYPE_SW, &oppo_active_id, sub_id);
        }

        if (active_id.id_len > 0)
        {
          MCFG_MSG_MED_1("Select for oppo sub %d", oppo_sub_id);
          mcfg_utils_set_selected_config(MCFG_TYPE_SW, &active_id, oppo_sub_id);
        }
      }

      mcfg_uim_state.slot[oppo_slot_index].sub_id = oppo_sub_id;
      mcfg_uim_state.slot[slot_index].sub_id = sub_id;

      mcfg_uim_write_mapping();
      MCFG_MSG_MED("clean up saved iccid for flex mapping");
      mcfg_sel_sw_config_store_updated(MCFG_NUM_OF_UIM_SLOT);

      if (!config_matches)
      {
        if (!refresh)
        {
          MCFG_MSG_HIGH_4("Activate after flex mapping slot%d-sub%d slot%d-sub%d", 
                          slot_index, sub_id, oppo_slot_index, oppo_sub_id);
          mcfg_utils_activate_config(MCFG_TYPE_SW, MCFG_ACTIVATION_CROSSMAPPING);
        }
#ifdef FEATURE_MCFG_REFRESH
        else
        {
          MCFG_MSG_MED_4("MBN pending for refresh after flex mapping slot%d-sub%d slot%d-sub%d", 
                          slot_index, sub_id, oppo_slot_index, oppo_sub_id);
        }
#endif
      }
      else
      {
        MCFG_MSG_MED_4("No need to reset after flex mapping slot%d-sub%d slot%d-sub%d for matching config", 
                        slot_index, sub_id, oppo_slot_index, oppo_sub_id);
      }

      return (!config_matches);
    }
  }

  MCFG_MSG_ERROR_2("Can't find matching pair for flex mapping slot %d to sub %d",
                   slot_index, sub_id);
  return FALSE;
} /* mcfg_uim_process_mapping() */

/*===========================================================================

  FUNCTION mcfg_uim_update_mapping

  DESCRIPTION
    Update UIM mapping table for flex mapping

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_uim_update_mapping
(
  mcfg_slot_index_type_e_type slot_index,
  mcfg_sub_id_type_e_type     sub_id,
  boolean                     timeout
)
{
  static mcfg_slot_index_type_e_type cached_slot_index = 0;
  static mcfg_sub_id_type_e_type     cached_sub_id = 0;
  static boolean              holding = FALSE;
  boolean                     ret = TRUE;
  uint8                       timer_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index < MCFG_NUM_OF_UIM_SLOT);
  MCFG_ASSERT_COND_RET_FALSE(sub_id < MCFG_NUM_OF_SUB_ID);

  if(timeout)
  {
    if (holding)
    {
      mcfg_uim_process_mapping(cached_slot_index, cached_sub_id, FALSE);
      cached_slot_index = 0;
      cached_sub_id = 0;
      holding = FALSE;
    }
    else
    {
      MCFG_MSG_ERROR("Ignore timeout cb without holding");
    }
  }
  else
  {
    MCFG_MSG_MED_3("Update mapping slot %d to sub %d from %d", slot_index, sub_id,
                   mcfg_uim_state.slot[slot_index].sub_id);
    if(mcfg_uim_state.slot[slot_index].sub_id == sub_id)
    {
      MCFG_MSG_LOW_2("Same mapping, ignore slot %d to sub %d", slot_index, sub_id);
    }
    else if (holding == FALSE)
    {
      timer_len = mcfg_uim_get_flexmap_timer();
      if (timer_len == 0)
      {
        mcfg_uim_process_mapping(slot_index, sub_id, FALSE);
      }
      else
      { 
        MCFG_MSG_HIGH_2("Deboucing flex mapping slot%d-sub%d", slot_index, sub_id);
        timer_set(&mcfg_uim_flexmap_timer, timer_len, 0, T_SEC);
        cached_slot_index = slot_index;
        cached_sub_id = sub_id;
        holding = TRUE;
      }
    }
    else if (cached_slot_index == slot_index && cached_sub_id == sub_id)
    {
      MCFG_MSG_LOW_2("Same mapping as cached, ignore slot %d to sub %d", slot_index, sub_id);
      ret = FALSE;
    }
    else
    {
      timer_clr(&mcfg_uim_flexmap_timer, T_SEC);
      cached_slot_index = 0;
      cached_sub_id = 0;
      holding = FALSE;
      mcfg_uim_process_mapping(slot_index, sub_id, FALSE);
    }
  }

  return ret;
} /* mcfg_uim_update_mapping() */

/*===========================================================================

  FUNCTION mcfg_uim_flexmap_timer_cb

  DESCRIPTION
    Flexmap Timer callback. 

  DEPENDENCIES
    None

  PARAMETERS
    data    [in]  timer callback data

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_flexmap_timer_cb(timer_cb_data_type data)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_uim_cmd_data_s_type *uim_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_MSG_HIGH("flex mapping cb");
  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_UIM, sizeof(mcfg_uim_cmd_data_s_type));
  if (cmd != NULL)
  {
    uim_cmd_data = (mcfg_uim_cmd_data_s_type *) cmd->data;
    uim_cmd_data->cmd_id = MCFG_UIM_CMD_MAPPING_TIMEOUT;

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for MMGSDI event!");
      mcfg_task_cmd_free(cmd);
    }
  }
} /* mcfg_utils_cmd_reset_timer_cb() */

/*===========================================================================

  FUNCTION mcfg_uim_proc_mapping_timeout

  DESCRIPTION
    Update UIM mapping table for flex mapping

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_proc_mapping_timeout
(
  void
)
{
  mcfg_uim_update_mapping(MCFG_SLOT_INDEX_FIRST, MCFG_SUB_ID_FIRST, TRUE);
}

/*===========================================================================

  FUNCTION mcfg_uim_reselect

  DESCRIPTION
    Reselect

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_reselect
(
  mcfg_slot_index_type_e_type sloti
)
{
  pdc_get_feature_ind_msg_v01 feature;
  mcfg_sub_id_type_e_type sub;
  mcfg_uim_ext_session_e_type ext_session;

  MCFG_ASSERT_COND_RET(sloti < MCFG_NUM_OF_UIM_SLOT);

  MCFG_MSG_LOW_1("mcfg_uim_reselect sloti %d", sloti);
  if (MCFG_ERR_NONE != mcfg_get_feature(sloti, &feature))
  {
    MCFG_MSG_ERROR_1("mcfg_uim_reselect sloti %d fail to get feature", sloti);
    return;
  }

  if (feature.selection_mode == PDC_SELECTION_MODE_DISABLED_V01)
  {
    MCFG_MSG_MED_2("mcfg_uim_reselect sloti %d select disabled %d cleanup saved iccid/imsi", sloti, feature.selection_mode);
    mcfg_sel_sw_config_store_updated(MCFG_NUM_OF_UIM_SLOT);
  }

  if (feature.carrier != PDC_SELECTION_OPEN_MARKET_V01)
  {
    MCFG_MSG_MED_2("mcfg_uim_reselect sloti %d select by carrier %d", sloti, feature.carrier);
    mcfg_sel_sw_by_carrier(mcfg_uim_map_slot_index_to_sub_id(sloti), 
                           feature.carrier);
  }
  else
  {
    if(!mcfg_uim_state.slot[sloti].ready)
    {
      MCFG_MSG_MED_2("mcfg_uim_reselect sloti %d not ready %d", sloti, mcfg_uim_state.slot[sloti].ready);
    }
    else
    {
      if (feature.selection_mode == PDC_SELECTION_MODE_IIN_V01)
      {
        MCFG_MSG_MED_2("mcfg_uim_reselect iin sloti %d select mode %d", sloti, feature.selection_mode);
        mcfg_uim_read_iccid_if_ready(sloti);
      }
      else
      {
        MCFG_MSG_MED_2("mcfg_uim_reselect imsi sloti %d select mode %d", sloti, feature.selection_mode);
        sub = mcfg_uim_map_slot_index_to_sub_id(sloti);
        MCFG_ASSERT_COND_RET(sub < MCFG_NUM_OF_SUB_ID);

#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
        if(mcfg_uim_state.slot[sloti].impi_sel_needed)
        {
          mcfg_uim_read_impi_if_ready(sloti);
        }
#endif
        mcfg_uim_read_imsi_if_ready(sloti, mcfg_uim_state.sub[sub].ext_app_type[MCFG_UIM_EXT_SESSION_GW]);
#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
        mcfg_uim_read_imsi_if_ready(sloti, mcfg_uim_state.sub[sub].ext_app_type[MCFG_UIM_EXT_SESSION_1X]);
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/
      }
    }
  }
}


/*===========================================================================

  FUNCTION mcfg_uim_update_app_type

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_uim_update_app_type
(
      mmgsdi_session_id_type session_id, 
      mmgsdi_app_enum_type   app_type
)
{
  mcfg_sub_id_type_e_type sub_id;
  uint8 ext_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (sub_id = 0; sub_id < MCFG_NUM_OF_SUB_ID; sub_id++)
  {
    for (ext_index = 0; ext_index < MCFG_UIM_NUM_OF_EXT_SESSION; ext_index++)
    {
      MCFG_MSG_LOW_3("sub %d gw/1x %d %x", sub_id, ext_index, mcfg_uim_state.sub[sub_id].ext_session_id[ext_index]);
      if (mcfg_uim_state.sub[sub_id].ext_session_id[ext_index] == session_id)
      {
        mcfg_uim_state.sub[sub_id].ext_app_type[ext_index] = app_type;

        return TRUE;
      }
    }
  }

  MCFG_MSG_ERROR_2("Session ID not found %llx for app type %d", session_id, app_type);
  return FALSE;

}

/*===========================================================================

  FUNCTION mcfg_uim_mmgsdi_evt_cb

  DESCRIPTION
    MMGSDI event callback. Posts command to MCFG task for processing.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_mmgsdi_evt_cb
(
  const mmgsdi_event_data_type *event
)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_uim_cmd_data_s_type *uim_cmd_data;
  uint32 slot = MMGSDI_MAX_SLOT_ID_ENUM;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (event != NULL)
  {
    if (event->evt == MMGSDI_CARD_INSERTED_EVT)
    {
      slot = event->data.card_inserted.slot;
    }
    else if (event->evt == MMGSDI_CARD_REMOVED_EVT)
    {
      slot = event->data.card_removed.slot;
    }
    else if (event->evt == MMGSDI_CARD_ERROR_EVT)
    {
      slot = event->data.card_error.slot;
    }
    else if (event->evt == MMGSDI_SUBSCRIPTION_READY_EVT)
    {
      slot = event->data.subscription_ready.app_info.slot;
    }
    else if (event->evt == MMGSDI_SESSION_CHANGED_EVT)
    {
      slot = event->data.session_changed.app_info.slot;
    }

    if (slot <= MMGSDI_SLOT_3)
    {
      MCFG_MSG_MED_2("Got card event %x on slot index %x; sending cmd", 
                      event->evt, mcfg_uim_map_mmgsdi_slot_to_slot_index(slot));
      cmd = mcfg_task_cmd_alloc(
        MCFG_TASK_CMD_UIM, sizeof(mcfg_uim_cmd_data_s_type));
      if (cmd != NULL)
      {
        uim_cmd_data = (mcfg_uim_cmd_data_s_type *) cmd->data;
        uim_cmd_data->cmd_id = MCFG_UIM_CMD_MMGSDI_EVT;
        uim_cmd_data->data.evt = *event;

        if (!mcfg_task_cmd_send(cmd))
        {
          MCFG_MSG_ERROR("Couldn't send command for MMGSDI event!");
          mcfg_task_cmd_free(cmd);
        }
      }
    }
  }
} /* mcfg_uim_mmgsdi_evt_cb() */

/*===========================================================================

  FUNCTION mcfg_uim_mmgsdi_evt_proc

  DESCRIPTION
    Handles MMGSDI events from the context of the MCFG task.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_mmgsdi_evt_proc
(
  const mmgsdi_event_data_type *event
)
{
  mcfg_slot_index_type_e_type slot_index = 0;
  mcfg_sub_id_type_e_type  sub_id = 0;
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
  uint8 i = 0;
  boolean isim_found = FALSE;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(event);

  MCFG_MSG_MED_2("Processing MMGSDI event %d for session %x", 
                 event->evt, event->session_id);
  switch (event->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      slot_index = mcfg_uim_map_mmgsdi_slot_to_slot_index(event->data.card_inserted.slot);
      MCFG_MSG_MED_2("Card inserted slot %d index %d", 
                   event->data.card_inserted.slot, slot_index);
      MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);

      mcfg_uim_state.slot[slot_index].card_state = MCFG_UIM_CARD_STATE_INSERTED;

#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
      if(mcfg_uim_state.slot[slot_index].impi_read_status != MCFG_UIM_EF_READ_SUCCESS)
      {
        mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_NOT_READ;
        memset(&mcfg_uim_state.slot[slot_index].impi_plmn, 0, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
      }

      /* Read all supported apps info and open non-prov session for ISIM app if it is available */
      memset(&mcfg_uim_state.slot[slot_index].isim_app_id, 0, sizeof(mcfg_uim_state.slot[slot_index].isim_app_id));      
      MCFG_MSG_LOW_1("Card inserted - num_aids_avail %lu", event->data.card_inserted.num_aids_avail);
      for(i=0; i < event->data.card_inserted.num_aids_avail; i++)
      {
        MCFG_MSG_LOW_2("Card inserted - app_type[%u] : %d",
                i, event->data.card_inserted.aid_info[i].app_type);
        if(event->data.card_inserted.aid_info[i].app_type == MMGSDI_APP_UNKNOWN)
        {
          if(mcfg_uim_is_isim_aid(&event->data.card_inserted.aid_info[i].aid))
          {
            MCFG_MSG_MED("Card inserted - Found ISIM APP");
            isim_found = TRUE;
            mcfg_uim_state.slot[slot_index].isim_app_id.data_len = event->data.card_inserted.aid_info[i].aid.data_len;
            memscpy(mcfg_uim_state.slot[slot_index].isim_app_id.data_ptr,
                    MMGSDI_MAX_AID_LEN,
                    event->data.card_inserted.aid_info[i].aid.data_ptr,
                    event->data.card_inserted.aid_info[i].aid.data_len);
            mcfg_uim_open_mmgsdi_session_for_isim(slot_index);
            break;
          }
        }
      }

      if(!isim_found)
      {
        MCFG_MSG_MED("Card inserted - ISIM APP not found");
        mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_APP_NOT_FOUND;
      }
#endif

#ifdef FEATURE_MCFG_REFRESH
      if(mcfg_refresh_autoselect_enabled((mcfg_refresh_index_type)slot_index))
      {
        MCFG_MSG_MED("Ignore card inserted for refresh");
      }
      else
#endif
      {
        mcfg_uim_read_iccid_if_ready(slot_index);
      }
      break;

    case MMGSDI_CARD_REMOVED_EVT:
      slot_index = mcfg_uim_map_mmgsdi_slot_to_slot_index(event->data.card_removed.slot);
      MCFG_MSG_MED_2("Card is not available slot %d index %d", 
                   event->data.card_removed.slot, slot_index);
      MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);

      mcfg_uim_state.slot[slot_index].card_state = MCFG_UIM_CARD_STATE_NO_CARD;
      break;

    case MMGSDI_CARD_ERROR_EVT:
      slot_index = mcfg_uim_map_mmgsdi_slot_to_slot_index(event->data.card_error.slot);
      MCFG_MSG_MED_2("Card is not available slot %d index %d", 
                   event->data.card_error.slot, slot_index);
      MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);

      mcfg_uim_state.slot[slot_index].card_state = MCFG_UIM_CARD_STATE_NO_CARD;
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      slot_index = 
        mcfg_uim_map_mmgsdi_slot_to_slot_index(event->data.session_changed.app_info.slot);
      MCFG_MSG_MED_3("mmgsdi_session_changed_evt slot %d activate %d app type %d", 
                     slot_index, event->data.session_changed.activated,
                     event->data.session_changed.app_info.app_data.app_type);

      mcfg_uim_update_app_type(event->session_id, event->data.session_changed.app_info.app_data.app_type);
      if(event->data.session_changed.activated && mcfg_uim_nv_flexmap_enabled(slot_index))
      {
        sub_id = mcfg_uim_map_session_id_to_sub_id(event->session_id);
        if (sub_id < MCFG_NUM_OF_SUB_ID)
        {
          mcfg_uim_update_mapping(slot_index, sub_id, FALSE);
        }
      }
      break;

    case MMGSDI_SUBSCRIPTION_READY_EVT:
      MCFG_MSG_MED_2("mmgsdi_subscription_ready_evt mmgsdi slot %d app %d", 
                      event->data.subscription_ready.app_info.slot,
                      event->data.subscription_ready.app_info.app_data.app_type);
      mcfg_map_print_map(FALSE);
#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
      slot_index = mcfg_uim_map_mmgsdi_slot_to_slot_index(event->data.subscription_ready.app_info.slot);
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
      if(mcfg_uim_state.slot[slot_index].impi_sel_needed)
      {
        /* IMPI selection needed for the card. Read IMPI */
        if((mcfg_uim_state.slot[slot_index].impi_read_status == MCFG_UIM_EF_NOT_READ) ||
           ((mcfg_uim_state.slot[slot_index].impi_read_status == MCFG_UIM_EF_READ_SUCCESS) && 
            (mcfg_uim_state.slot[slot_index].impi_plmn.mnc == 0) &&
            (mcfg_uim_state.slot[slot_index].impi_plmn.mcc == 0)))
        {
          mcfg_uim_read_impi_if_ready(slot_index);
        }
        else if(mcfg_uim_state.slot[slot_index].impi_read_status == MCFG_UIM_EF_READ_SUCCESS)
        {
          MCFG_MSG_MED_3("IMPI already available with MCC=%03d MNC=%03d for slot %d", 
                       mcfg_uim_state.slot[slot_index].impi_plmn.mcc, 
                       mcfg_uim_state.slot[slot_index].impi_plmn.mnc, slot_index);
        }
        else if(mcfg_uim_state.slot[slot_index].impi_read_status == MCFG_UIM_EF_READ_INPROGRESS)
        {
          MCFG_MSG_MED_1("IMPI READ in progress for slot %d", slot_index);
        }
        else
        {
          MCFG_MSG_MED_2("IMPI cannot be read for slot %d, impi_read_status %d", 
                       slot_index, mcfg_uim_state.slot[slot_index].impi_read_status);
        }
      }
      else
      {
        /* Card doesn't need IMPI selection, skip IMPI read */
        MCFG_MSG_MED_1("IMPI based selection not needed for the card for slot %d", slot_index);
      }
#endif
      mcfg_uim_read_imsi_if_ready(slot_index,
                                  event->data.subscription_ready.app_info.app_data.app_type);
#endif /*FEATURE_MCFG_SELECTION_BY_IMSI*/
      break;

    default:
      MCFG_MSG_ERROR_1("Unexpected event %d", event->evt);
  }
} /* mcfg_uim_mmgsdi_evt_proc() */

/*===========================================================================

  FUNCTION mcfg_uim_mmgsdi_init

  DESCRIPTION
    Starts the MCFG_UIM MMGSDI initialization procedure by requesting a
    client ID and registering for card events.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_uim_mmgsdi_init
(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status;  
  mmgsdi_session_type_enum_type slot_session[] = 
  {
    MMGSDI_CARD_SESSION_SLOT_1_MASK
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
    ,MMGSDI_CARD_SESSION_SLOT_2_MASK
#ifdef FEATURE_MCFG_MULTISIM_TSTS_SUPPORT
    ,MMGSDI_CARD_SESSION_SLOT_3_MASK
#endif /*FEATURE_MCFG_MULTISIM_TSTS_SUPPORT*/
#endif /*FEATURE_MCFG_MULTISIM_SUPPORT*/
  };
#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
  mmgsdi_session_type_enum_type sub_session[] = 
  {
    MMGSDI_GW_PROV_PRI_SESSION_MASK | MMGSDI_1X_PROV_PRI_SESSION_MASK
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
    ,MMGSDI_GW_PROV_SEC_SESSION_MASK | MMGSDI_1X_PROV_SEC_SESSION_MASK
#ifdef FEATURE_MCFG_MULTISIM_TSTS_SUPPORT
    ,MMGSDI_GW_PROV_TER_SESSION_MASK | MMGSDI_1X_PROV_TER_SESSION_MASK
#endif /*FEATURE_MCFG_MULTISIM_TSTS_SUPPORT*/
#endif /*FEATURE_MCFG_MULTISIM_SUPPORT*/
  };
#else
  mmgsdi_session_type_enum_type sub_session[] = 
  {
    MMGSDI_GW_PROV_PRI_SESSION_MASK
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
    ,MMGSDI_GW_PROV_SEC_SESSION_MASK
#ifdef FEATURE_MCFG_MULTISIM_TSTS_SUPPORT
    ,MMGSDI_GW_PROV_TER_SESSION_MASK
#endif /*FEATURE_MCFG_MULTISIM_TSTS_SUPPORT*/
#endif /*FEATURE_MCFG_MULTISIM_SUPPORT*/
  };
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/
 
  uint32 session_type_mask = 0;
  uint8 slot_index;
  uint8 num_sessions = 0;
  mmgsdi_session_open_info_type *session_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_MSG_MED("Starting MCFG_UIM MMGSDI initialization");

  for (slot_index = MCFG_SLOT_INDEX_FIRST; 
        slot_index < MCFG_NUM_OF_UIM_SLOT; slot_index++)
  {
    session_type_mask |= slot_session[slot_index];
    session_type_mask |= sub_session[slot_index];
	num_sessions += MCFG_NUM_OF_EXT_SESSIONS_PER_SLOT;
  }

  mcfg_uim_state.client_id = mmgsdi_client_id_and_evt_reg_sync(
    mcfg_uim_mmgsdi_evt_cb, "MCFG", strlen("MCFG"));

  if (!mcfg_uim_state.client_id)
  {
    MCFG_MSG_ERROR("Allocating MMGSDI client ID failed!");
    return;
  }
  MCFG_MSG_HIGH_3("Successfully allocated MMGSDI client ID %llx and opening %d ext sync session(s) with mask %04lX",
  	              mcfg_uim_state.client_id, num_sessions, session_type_mask);

  session_info_ptr = (mmgsdi_session_open_info_type *)mcfg_malloc(sizeof(mmgsdi_session_open_info_type) * num_sessions);
  if (!session_info_ptr)
  {
    MCFG_MSG_ERROR("Couldn't alloc memory for 'session_info_ptr'");
    return;
  }
  memset(session_info_ptr, 0, (sizeof(mmgsdi_session_open_info_type) * num_sessions));
  
  mmgsdi_status = mmgsdi_session_open_ext_sync(mcfg_uim_state.client_id,
                                   session_type_mask,
                                   mcfg_uim_mmgsdi_evt_cb,
                                   session_info_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MCFG_MSG_ERROR_1("Opening MMGSDI ext sync sessions failed with status %d", mmgsdi_status);
  }
  else
  {
    mcfg_uim_mmgsdi_session_open_ext_sync_cnf_proc(num_sessions, session_info_ptr);
  }
  mcfg_free(session_info_ptr);
  
} /* mcfg_uim_mmgsdi_init() */

/*===========================================================================

  FUNCTION mcfg_uim_init

  DESCRIPTION
    Starts the MCFG_UIM initialization .

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_uim_init
(
  void
)
{
  mcfg_slot_index_type_e_type i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_MSG_MED("Starting MCFG_UIM initialization");
  memset(&mcfg_uim_state, 0, sizeof(mcfg_uim_state));

  for (i = MCFG_SLOT_INDEX_FIRST; i < MCFG_NUM_OF_UIM_SLOT; i++ )
  {
    mcfg_uim_state.slot[i].autoselect_nv = MCFG_UIM_NV_UNINIT;

    if (mcfg_uim_nv_autoselect_enabled(i))
    {
      MCFG_MSG_MED_2("NV71546 for slot %d set to %d", i, mcfg_uim_state.slot[i].autoselect_nv);
    }
  }

  /* define timer for flex mapping */
  timer_def(&mcfg_uim_flexmap_timer,
            &mcfg_uim_flexmap_timer_group,
            NULL,
            0,
            mcfg_uim_flexmap_timer_cb,
            0);

  mcfg_uim_read_mapping();
} /* mcfg_uim_init() */

static void mcfg_uim_read_num_of_mnc
(
  mcfg_mmgsdi_ctx_t         *p_ctx
)
{
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_access_type access_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(p_ctx);
  MCFG_ASSERT_COND_RET(p_ctx->sub_id < MCFG_NUM_OF_SUB_ID);
  MCFG_MSG_MED_3("Attempting to read num of mnc sub %d app %d session %x", 
                 p_ctx->sub_id, p_ctx->app_type, p_ctx->session_id);

  access_type.access_method  = MMGSDI_EF_ENUM_ACCESS;
  if (p_ctx->app_type == MMGSDI_APP_SIM)
  {
    access_type.file.file_enum = MMGSDI_GSM_AD;
  }
  else if (p_ctx->app_type == MMGSDI_APP_USIM)
  {
    access_type.file.file_enum = MMGSDI_USIM_AD;
  }
  else
  {
    //access_type.file.file_enum = MMGSDI_CSIM_IMSI_T;
    MCFG_MSG_LOW_1("mcfg_uim_read_num_of_mnc n/a for CSIM/RUIM app type %d", 
                   p_ctx->app_type);
    return;
  }

  mmgsdi_status = mmgsdi_session_read_transparent(
    p_ctx->session_id, access_type, 0, MNC_LEN_BYTE, 
    mcfg_uim_mmgsdi_rsp_cb, (mmgsdi_client_data_type)p_ctx);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MCFG_MSG_ERROR_2("Couldn't read num_of_mnc sub %d: status %d", p_ctx->sub_id, 
                     mmgsdi_status);
  }

  return;
} /* mcfg_uim_read_num_of_mnc() */

/*===========================================================================

  FUNCTION mcfg_uim_mmgsdi_read_cnf_proc

  DESCRIPTION
    Handles a read confirmation from MMGSDI. Decodes the ICCID and initiates
    the autoselection procedure.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_mmgsdi_read_cnf_proc
(
  mmgsdi_return_enum_type     status,
  const mmgsdi_read_cnf_type *cnf_data
)
{
  char card_iccid[MCFG_UIM_ICCID_STR_BUF_LEN];
#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
  mcfg_trl_carrier_mcc_mnc_s_type plmn;
  mcfg_mmgsdi_ctx_t *p_ctx;
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
  mcfg_trl_carrier_mcc_mnc_s_type *impi_plmn = NULL;
#endif
  uint8 *data_ptr;
  uint8 slot_index;
  uint8 sub_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(cnf_data);
  memset(&plmn, 0, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
  slot_index = mcfg_uim_map_session_id_to_slot_index(cnf_data->response_header.session_id);
  MCFG_MSG_HIGH_2("mcfg_uim_mmgsdi_read_cnf_proc: status %d session_id %llx", status, 
                   cnf_data->response_header.session_id);
  MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);
  sub_id = mcfg_uim_map_slot_index_to_sub_id(slot_index);
  MCFG_ASSERT_COND_RET(sub_id < MCFG_NUM_OF_SUB_ID);

  switch (cnf_data->access.file.file_enum)
  {
#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
    case MMGSDI_GSM_IMSI:
    case MMGSDI_USIM_IMSI:
      p_ctx = (mcfg_mmgsdi_ctx_t*)cnf_data->response_header.client_data;
      if (!p_ctx)
      {
        MCFG_MSG_ERROR("invalid ctx");
      }
      else if (status != MMGSDI_SUCCESS || 
          cnf_data->read_data.data_len != MMGSDI_IMSI_LEN ||
          cnf_data->read_data.data_ptr == NULL)
      {
        MCFG_MSG_ERROR_3("Couldn't read IMSI: status %d len %ld p=%p", 
                         status, cnf_data->read_data.data_len, cnf_data->read_data.data_ptr);
        mcfg_free(p_ctx);
      }
      else
      {
        memscpy(p_ctx->imsi, MMGSDI_IMSI_LEN,
                cnf_data->read_data.data_ptr, cnf_data->read_data.data_len);
        mcfg_uim_read_num_of_mnc(p_ctx);
      }
      break;

    case MMGSDI_USIM_AD:
    case MMGSDI_GSM_AD:
      p_ctx = (mcfg_mmgsdi_ctx_t*)cnf_data->response_header.client_data;
      if (!p_ctx)
      {
        MCFG_MSG_ERROR("invalid ctx");
      }
      else
      {
        if (status != MMGSDI_SUCCESS ||
            cnf_data->read_data.data_len < MNC_LEN_BYTE ||
            cnf_data->read_data.data_ptr == NULL)
        {
          MCFG_MSG_ERROR_3("Couldn't read num of mnc: status %d len %ld p=%p defaulting to 2 digits", 
                           status, cnf_data->read_data.data_len, cnf_data->read_data.data_ptr);
          p_ctx->num_mnc_digits = 2;
        }
        else
        { 
          p_ctx->num_mnc_digits = cnf_data->read_data.data_ptr[MNC_LEN_BYTE-1] & 0x0F;
        }

        if (!mcfg_uim_decode_imsi(p_ctx->imsi, &plmn, p_ctx->num_mnc_digits))
        {
          MCFG_MSG_ERROR_1("Couldn't decode IMSI: status %d", status);
        }
        else
        {
          MCFG_MSG_HIGH_2("Selecting config using IMSI MCC=%d MNC=%d", plmn.mcc, plmn.mnc);
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
          memscpy(&mcfg_uim_state.sub[sub_id].imsi_plmn, sizeof(mcfg_trl_carrier_mcc_mnc_s_type),
                  &plmn, sizeof(plmn));
          if(!mcfg_uim_wait_for_IMPI(sub_id, &impi_plmn))
#endif
          {
            /* IMPI read is not in progress, proceed with selection */
            mcfg_sel_sw_by_plmn(sub_id,
                                &plmn,
                                MCFG_UIM_EXT_SESSION_GW
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
                              , impi_plmn
#endif
                              );
          }
        }

        mcfg_free(p_ctx);
      }

      break;
#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
  case MMGSDI_CDMA_IMSI_M:
  case MMGSDI_CSIM_IMSI_M:
    if (status != MMGSDI_SUCCESS ||
        cnf_data->read_data.data_len != MMGSDI_CDMA_IMSI_LEN ||
        cnf_data->read_data.data_ptr == NULL)
    {
      MCFG_MSG_ERROR_3("Couldn't read CDMA IMSI: status %d len %lu p=%p",
                       status, cnf_data->read_data.data_len, cnf_data->read_data.data_ptr);
    }
    else if (!mcfg_uim_decode_cdma_imsi(cnf_data->read_data.data_ptr, &plmn))
    {
      MCFG_MSG_ERROR_1("Couldn't decode cdma IMSI: status %d", status);
    }
    else if (plmn.mcc != 460 || plmn.mnc != 3)
    {
      MCFG_MSG_HIGH_2("Ignore non-CT CDMA IMSI MCC=%d MNC=%d", plmn.mcc, plmn.mnc);
    }
    else
    {
      MCFG_MSG_HIGH_2("Selecting config using CDMA IMSI MCC=%d MNC=%d", plmn.mcc, plmn.mnc);
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
      memscpy(&mcfg_uim_state.sub[sub_id].imsi1x_plmn, 
              sizeof(mcfg_trl_carrier_mcc_mnc_s_type),
              &plmn, sizeof(plmn));
      if(!mcfg_uim_wait_for_IMPI(sub_id, &impi_plmn))
#endif
      {
        /* IMPI read is not in progress, proceed with selection */
        mcfg_sel_sw_by_plmn(sub_id,
                            &plmn,
                            MCFG_UIM_EXT_SESSION_1X
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
                          , impi_plmn
#endif
                          );
      }
    }
    break;
#endif /* FEATURE_MCFG_SELECTION_BY_1X_IMSI */
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
  case MMGSDI_ISIM_IMPI:
    if(!mcfg_uim_state.slot[slot_index].impi_sel_needed)
    {
      MCFG_MSG_ERROR("mcfg_uim_mmgsdi_read_cnf_proc for IMPI selection not needed for this card! ");
    }
    else
    {
      impi_plmn = NULL;
      if ((status != MMGSDI_SUCCESS) ||
          (cnf_data->read_data.data_len <= 0) ||
          (cnf_data->read_data.data_ptr == NULL))
      {
        MCFG_MSG_ERROR_3("Couldn't read IMPI: status %d len %ld p=%p",
                        status, cnf_data->read_data.data_len, cnf_data->read_data.data_ptr);
        mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_READ_FAILED;
      }
      else if (!mcfg_uim_decode_impi(cnf_data->read_data.data_ptr, cnf_data->read_data.data_len, &plmn))
      {
        MCFG_MSG_ERROR("Failed to decode IMPI");
        mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_PARSE_FAILED;
      }
      else
      {
        mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_READ_SUCCESS;
        memscpy(&mcfg_uim_state.slot[slot_index].impi_plmn, sizeof(mcfg_trl_carrier_mcc_mnc_s_type),
                &plmn, sizeof(plmn));
        impi_plmn = &mcfg_uim_state.slot[slot_index].impi_plmn;
      }
#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
      if(mcfg_uim_state.sub[sub_id].imsi1x_plmn.mcc != 0)
      {
        /* 1x IMSI is ready, Trigger MBN selection based on IMPI(If decode success), 1XIMSI */
        mcfg_sel_sw_by_plmn(sub_id,
                            &mcfg_uim_state.sub[sub_id].imsi1x_plmn,
                            MCFG_UIM_EXT_SESSION_1X,
                            impi_plmn);
      }
      else 
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/
      if(mcfg_uim_state.sub[sub_id].imsi_plmn.mcc != 0)
      {
        /* 1x IMSI is not present and GW IMSI is ready, Trigger MBN selection based on  IMPI(If decode success), GWIMSI */
        mcfg_sel_sw_by_plmn(sub_id, 
                            &mcfg_uim_state.sub[sub_id].imsi_plmn,
                            MCFG_UIM_EXT_SESSION_GW,
                            impi_plmn);
      }
      else
      {
        MCFG_MSG_MED_2("IMPI MCC=%03d MNC=%03d, IMSI is not available for MBN selection, wait for it",
                     plmn.mcc, plmn.mnc);
      }
    }
    break;
#endif

  case MMGSDI_ICCID:
      if (status != MMGSDI_SUCCESS || 
          cnf_data->read_data.data_len < MCFG_UIM_ICCID_LEN ||
          cnf_data->read_data.data_ptr == NULL)
      {
        MCFG_MSG_ERROR_1("Couldn't read ICCID: status %d", status);
      }
      else if (!mcfg_uim_decode_iccid(cnf_data->read_data.data_ptr, card_iccid))
      {
        MCFG_MSG_ERROR_1("Couldn't decode ICCID: status %d", status);
      }
      else
      {
        MCFG_MSG_SPRINTF_1("ICCID %s", card_iccid); 
        mcfg_sel_sw_by_iccid(mcfg_uim_state.slot[slot_index].sub_id, card_iccid);
      }
      break;

    default:
      MCFG_MSG_ERROR_1("Unknown UIM File %x", cnf_data->access.file.file_enum);
      break;
  }

  if(cnf_data->read_data.data_ptr)
  {
    data_ptr = cnf_data->read_data.data_ptr;
    mcfg_free(data_ptr);
  }
} /* mcfg_uim_mmgsdi_read_cnf_proc() */

/*===========================================================================

  FUNCTION mcfg_uim_read_iccid_if_ready

  DESCRIPTION
    Issues a request to read the ICCID if we are ready (session is opened
    and card is inserted).

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_read_iccid_if_ready
(
  uint8 slot_index
)
{
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_access_type access_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);

  if (!mcfg_uim_state.slot[slot_index].ready)
  {
    MCFG_MSG_MED_1("Not ready to query ICCID %d: waiting on client/session init",
                   slot_index);
  }
  else if (mcfg_uim_state.slot[slot_index].card_state != 
             MCFG_UIM_CARD_STATE_INSERTED)
  {
    MCFG_MSG_MED_1("Not ready to query ICCID %d: waiting on card inserted event",
                   slot_index);
  }
  else
  {
    MCFG_MSG_MED_1("Attempting to read ICCID %d", slot_index);
    access_type.access_method  = MMGSDI_EF_ENUM_ACCESS;
    access_type.file.file_enum = MMGSDI_ICCID;
    mmgsdi_status = mmgsdi_session_read_transparent(
      mcfg_uim_state.slot[slot_index].session_id, access_type, 0, 
      MCFG_UIM_ICCID_LEN, mcfg_uim_mmgsdi_rsp_cb, 0);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MCFG_MSG_ERROR_2("Couldn't read ICCID %d: status %d", slot_index, 
                       mmgsdi_status);
    }
  }
} /* mcfg_uim_read_iccid_if_ready() */

#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
/*===========================================================================

  FUNCTION mcfg_uim_read_impi_if_ready

  DESCRIPTION
    Issues a request to read the IMPI from ISIM app if we are ready (session is opened
    and card is inserted).

  DEPENDENCIES
    None

  PARAMETERS
    slot_index

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_read_impi_if_ready
(
  uint8 slot_index
)
{
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_access_type access_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);
  memset(&mcfg_uim_state.slot[slot_index].impi_plmn, 0, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
  if (mcfg_uim_state.slot[slot_index].card_state == MCFG_UIM_CARD_STATE_INSERTED)
  {
    if (mcfg_uim_state.slot[slot_index].isim_ready)
    {
      MCFG_MSG_MED_2("Attempting to read IMPI slot %d from ISIM session 0x%llx", 
                    slot_index, mcfg_uim_state.slot[slot_index].session_id_isim);

      mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_READ_INPROGRESS;

      access_type.access_method  = MMGSDI_EF_ENUM_ACCESS;
      access_type.file.file_enum = MMGSDI_ISIM_IMPI;
      mmgsdi_status = mmgsdi_session_read_transparent(
                         mcfg_uim_state.slot[slot_index].session_id_isim,
                         access_type, 
                         0, 
                         MCFG_UIM_IMPI_LEN, 
                         mcfg_uim_mmgsdi_rsp_cb, 
                         0);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MCFG_MSG_ERROR_2("Couldn't read IMPI for slot %d: status %d", slot_index, mmgsdi_status);
        mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_READ_FAILED;
      }
    }
    else
    {
      MCFG_MSG_ERROR_1("Not ready to query IMPI for slot %d: ISIM session is not present", slot_index);
      mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_READ_FAILED;
    }
  }
  else
  {
    MCFG_MSG_ERROR_1("Not ready to query IMPI for slot %d: waiting on card inserted event", slot_index);
    mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_READ_FAILED;
  }

} /* mcfg_uim_read_impi_if_ready() */

/*===========================================================================

  FUNCTION mcfg_uim_wait_for_IMPI

  DESCRIPTION
    Function checks whether MBN selection logic should wait for IMPI or not.

  DEPENDENCIES
    None

  PARAMETERS
      sub_id   - in
      impi_plmn - out

  RETURN VALUE
    TRUE  -Wait for IMPI to arrive
    FALSE -Do not wait for IMPI, Either IMPI is available OR Failed to read OR not required

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_uim_wait_for_IMPI
(
  mcfg_sub_id_type_e_type sub_id,
  mcfg_trl_carrier_mcc_mnc_s_type **impi_plmn  
)
{
  mcfg_slot_index_type_e_type sloti;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(impi_plmn);
  *impi_plmn = NULL;
  sloti = mcfg_uim_map_sub_id_to_slot_index(sub_id);
  MCFG_ASSERT_COND_RET_FALSE(sloti < MCFG_NUM_OF_UIM_SLOT);

  if(mcfg_uim_state.slot[sloti].impi_sel_needed)
  {
    /* Trigger IMPI based selection if it is available */
    if((mcfg_uim_state.slot[sloti].impi_read_status == MCFG_UIM_EF_NOT_READ) ||
       ((mcfg_uim_state.slot[sloti].impi_read_status == MCFG_UIM_EF_READ_SUCCESS) && 
        (mcfg_uim_state.slot[sloti].impi_plmn.mnc == 0) &&
        (mcfg_uim_state.slot[sloti].impi_plmn.mcc == 0)))
    {
      MCFG_MSG_MED_1("IMPI is not available, lets read it for slot %d", sloti);
      mcfg_uim_read_impi_if_ready(sloti);
    }

    switch(mcfg_uim_state.slot[sloti].impi_read_status)
    {
      case MCFG_UIM_EF_READ_INPROGRESS:
          /* IMPI is not read yet, wait for it */
          MCFG_MSG_MED("IMPI read in progress for MBN selection, wait for it");
          return TRUE;
      case MCFG_UIM_EF_APP_NOT_FOUND:
      case MCFG_UIM_EF_SESSION_OPEN_FAILED:
      case MCFG_UIM_EF_READ_FAILED:
      case MCFG_UIM_EF_PARSE_FAILED:
          /* Fall back to IMSI based selection if IMSI is ready */
          MCFG_MSG_MED_1("Fallback to IMSI based selection. impi_read_status:%d",
                         mcfg_uim_state.slot[sloti].impi_read_status);
          break;
      case MCFG_UIM_EF_READ_SUCCESS:
          /* Trigger both IMPI and IMSI based selection if IMSI is ready */
          *impi_plmn = &mcfg_uim_state.slot[sloti].impi_plmn;
          break;
      default:
          /* Code flow should not reach here */
          MCFG_MSG_ERROR_1("mcfg_uim_wait_for_IMPI failed to handle. impi_read_status:%d",
                        mcfg_uim_state.slot[sloti].impi_read_status);
    }
  }
  else
  {
    /* Proceed with IMSI based selection if IMSI is ready for the cards that doesn't require IMPI selection */
    MCFG_MSG_LOW("IMPI selection not needed for this card.");
  }
  return FALSE;
}

/*===========================================================================

  FUNCTION mcfg_uim_reset_current_plmns

  DESCRIPTION
    Resets global variables maitained for current IMSI and IMPI PLMNs

  DEPENDENCIES
    None

  PARAMETERS
    slot_index

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_uim_reset_current_plmns
(
  uint8 slot_index
)
{
  uint8 sub_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);
  sub_id = mcfg_uim_map_slot_index_to_sub_id(slot_index);
  MCFG_ASSERT_COND_RET(sub_id < MCFG_NUM_OF_SUB_ID);

  MCFG_MSG_MED_2("Reset current PLMNs for slot %d, sub %d", slot_index, sub_id);
  mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_NOT_READ;
  memset(&mcfg_uim_state.slot[slot_index].impi_plmn, 0, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
  memset(&mcfg_uim_state.sub[sub_id].imsi_plmn, 0, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
  memset(&mcfg_uim_state.sub[sub_id].imsi1x_plmn, 0, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/
}

/*===========================================================================

  FUNCTION mcfg_uim_update_impi_sel_needed

  DESCRIPTION
    Updates the global based on the country of the inserted card belongs to.
    IMPI selection would be restricted to specific conuntries using this.

  DEPENDENCIES
    None

  PARAMETERS
    slot_index
    value

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_uim_update_impi_sel_needed
(
  uint8 slot_index,
  boolean value
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);
  mcfg_uim_state.slot[slot_index].impi_sel_needed = value;
}

#endif /* FEATURE_MCFG_SELECTION_BY_IMPI */

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
/*===========================================================================

  FUNCTION mcfg_uim_read_imsi_if_ready

  DESCRIPTION
    Issues a request to read the imsi if we are ready (session is opened
    and card is inserted).

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_read_imsi_if_ready
(
  uint8 slot_index,
  mmgsdi_app_enum_type app_type_input
)
{
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_access_type access_type;
  mcfg_uim_ext_session_e_type session_index;
  uint8 sub_id;
  mmgsdi_app_enum_type app_type;
  mcfg_mmgsdi_ctx_t *p_ctx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);
  if (!mcfg_uim_state.slot[slot_index].ready)
  {
    MCFG_MSG_MED_1("Not ready to query IMSI %d: waiting on client/session init",
                   slot_index);
  }
  else if (mcfg_uim_state.slot[slot_index].card_state != 
             MCFG_UIM_CARD_STATE_INSERTED)
  {
    MCFG_MSG_MED_1("Not ready to query IMSI %d: waiting on card inserted event",
                   slot_index);
  }
  else
  {
    sub_id = mcfg_uim_map_slot_index_to_sub_id(slot_index);
    MCFG_ASSERT_COND_RET(sub_id < MCFG_NUM_OF_SUB_ID);
    MCFG_MSG_MED_3("Attempting to read IMSI slot %d sub %d app %d", 
                   slot_index, sub_id, app_type_input);

    for (session_index = MCFG_UIM_EXT_SESSION_GW; 
          session_index < MCFG_UIM_NUM_OF_EXT_SESSION; session_index++)
    {
      app_type = mcfg_uim_state.sub[sub_id].ext_app_type[session_index];
      MCFG_MSG_LOW_2("app_type %d gw1x %d", app_type, session_index);
      if (app_type_input != app_type)
      {
        continue;
      }

      access_type.access_method  = MMGSDI_EF_ENUM_ACCESS;
      if (app_type == MMGSDI_APP_SIM)
      {
        access_type.file.file_enum = MMGSDI_GSM_IMSI;
      }
      else if (app_type == MMGSDI_APP_USIM)
      {
        access_type.file.file_enum = MMGSDI_USIM_IMSI;
      }
#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
      else if (app_type == MMGSDI_APP_RUIM)
      {
        access_type.file.file_enum = MMGSDI_CDMA_IMSI_M;
      }
      else if (app_type == MMGSDI_APP_CSIM)
      {
        access_type.file.file_enum = MMGSDI_CSIM_IMSI_M;
      }
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/
      else
      {
        MCFG_MSG_LOW_1("mmgsdi_session_read_transparent unknown app type %d", 
                       app_type_input);
        continue;
      }

      p_ctx = mcfg_malloc(sizeof(mcfg_mmgsdi_ctx_t));
      if (!p_ctx)
      {
        MCFG_MSG_ERROR_1("fail to malloc ctx size %d", sizeof(mcfg_mmgsdi_ctx_t));
        continue;
      }

#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
      if(session_index == MCFG_UIM_EXT_SESSION_GW)
      {
        memset(&mcfg_uim_state.sub[sub_id].imsi_plmn, 0, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
      }
#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
      else if(session_index == MCFG_UIM_EXT_SESSION_1X)
      {
        memset(&mcfg_uim_state.sub[sub_id].imsi1x_plmn, 0, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
      }
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/
#endif

      p_ctx->session_id = mcfg_uim_state.sub[sub_id].ext_session_id[session_index];
      p_ctx->app_type = app_type;
      p_ctx->sub_id = sub_id;
      p_ctx->num_mnc_digits = 0;

      MCFG_MSG_MED_2("mmgsdi_session_read_transparent IMSI session %x gw1x %d", 
                     mcfg_uim_state.sub[sub_id].ext_session_id[session_index], 
                     session_index);

      mmgsdi_status = mmgsdi_session_read_transparent(
        p_ctx->session_id, access_type, 0,
#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
        (access_type.file.file_enum == MMGSDI_CDMA_IMSI_M || access_type.file.file_enum == MMGSDI_CSIM_IMSI_M) ? MMGSDI_CDMA_IMSI_LEN : MMGSDI_IMSI_LEN,
#else
        MMGSDI_IMSI_LEN,
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/
        mcfg_uim_mmgsdi_rsp_cb, (mmgsdi_client_data_type)p_ctx);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MCFG_MSG_ERROR_2("Couldn't read imsi sub %d: status %d", sub_id, 
                         mmgsdi_status);
      }
    }
  }
} /* mcfg_uim_read_imsi_if_ready() */
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

/*===========================================================================

  FUNCTION mcfg_uim_mmgsdi_rsp_cb

  DESCRIPTION
    MMGSDI command response callback. Posts command to MCFG task for
    processing.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_mmgsdi_rsp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_uim_cmd_data_s_type *uim_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_UIM, sizeof(mcfg_uim_cmd_data_s_type));
  if (cmd != NULL)
  {
    uim_cmd_data = (mcfg_uim_cmd_data_s_type *) cmd->data;
    uim_cmd_data->cmd_id = MCFG_UIM_CMD_MMGSDI_RSP;
    uim_cmd_data->data.rsp.status = status;
    uim_cmd_data->data.rsp.cnf = cnf;
    if (cnf_ptr != NULL)
    {
      uim_cmd_data->data.rsp.cnf_data = *cnf_ptr;
      if (cnf == MMGSDI_READ_CNF)
      {
        uim_cmd_data->data.rsp.cnf_data.read_cnf.read_data.data_ptr = 
          mcfg_malloc(cnf_ptr->read_cnf.read_data.data_len);
        if (!uim_cmd_data->data.rsp.cnf_data.read_cnf.read_data.data_ptr)
        {
          MCFG_MSG_HIGH_1("Couldn't alloc memory for MMGSDI_READ_CNF! len=%ld", 
                           cnf_ptr->read_cnf.read_data.data_len);
          uim_cmd_data->data.rsp.cnf_data.read_cnf.read_data.data_len = 0;
          mcfg_task_cmd_free(cmd);

          return;
        }
        else
        {
          memscpy(uim_cmd_data->data.rsp.cnf_data.read_cnf.read_data.data_ptr,
                  uim_cmd_data->data.rsp.cnf_data.read_cnf.read_data.data_len,
                  cnf_ptr->read_cnf.read_data.data_ptr,
                  cnf_ptr->read_cnf.read_data.data_len);
        }
      }
    }

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for MMGSDI response!");
      mcfg_task_cmd_free(cmd);
    }
  }
} /* mcfg_uim_mmgsdi_rsp_cb() */

/*===========================================================================

  FUNCTION mcfg_uim_mmgsdi_rsp_proc

  DESCRIPTION
    Dispatches MMGSDI response data to the respective handler function.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_mmgsdi_rsp_proc
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  MCFG_CHECK_NULL_PTR_RET(cnf_ptr);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_MSG_MED_1("Processing MMGSDI cnf %d", cnf);
  switch (cnf)
  {
#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
    case MMGSDI_SESSION_OPEN_CNF:
      mcfg_uim_mmgsdi_session_open_cnf_proc(status, &cnf_ptr->session_open_cnf);
      break;
#endif

    case MMGSDI_READ_CNF:
      mcfg_uim_mmgsdi_read_cnf_proc(status, &cnf_ptr->read_cnf);
      break;

    default:
      MCFG_MSG_ERROR_1("Unexpected cnf %d", cnf);
  }
} /* mcfg_uim_mmgsdi_rsp_proc() */

/*===========================================================================

  FUNCTION mcfg_uim_mmgsdi_session_open_ext_sync_cnf_proc

  DESCRIPTION
    Handles a session open_ext_sync response.

  DEPENDENCIES
    None

  PARAMETERS
    [IN] num_sessions - Number of MMGSDI sessions got opened successfully.
    [IN] session_info_ptr - Session info buffer(filled by UIM).

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_mmgsdi_session_open_ext_sync_cnf_proc
(
  uint8                 num_sessions,
  const mmgsdi_session_open_info_type *session_info_ptr
)
{  
  mcfg_slot_index_type_e_type slot_index = MCFG_NUM_OF_UIM_SLOT;
  mcfg_sub_id_type_e_type sub_index = MCFG_NUM_OF_SUB_ID;
  mcfg_uim_ext_session_e_type session_index;
  uint16 i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(session_info_ptr);

  MCFG_MSG_MED_1("Successfully opened card ext sync sessions with %d session(s)", 
                 num_sessions);
  for (i = 0; i < num_sessions; i++)
  {
    MCFG_MSG_MED_4("Open session index %d slot_id %08x session_id %llx session_type %d", i, 
                    session_info_ptr[i].slot_id,
                    session_info_ptr[i].session_id,
                    session_info_ptr[i].session_type);
    
    slot_index = MCFG_NUM_OF_UIM_SLOT;
    sub_index = MCFG_NUM_OF_SUB_ID;
    switch (session_info_ptr[i].session_type)
    {
      case MMGSDI_GW_PROV_PRI_SESSION:
        sub_index = MCFG_SUB_ID_FIRST;
        session_index = MCFG_UIM_EXT_SESSION_GW;
        break;

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
      case MMGSDI_GW_PROV_SEC_SESSION:
        sub_index = MCFG_SUB_ID_SECOND;
        session_index = MCFG_UIM_EXT_SESSION_GW;
        break;

#ifdef FEATURE_MCFG_MULTISIM_TSTS_SUPPORT
      case MMGSDI_GW_PROV_TER_SESSION:
        sub_index = MCFG_SUB_ID_THIRD;
        session_index = MCFG_UIM_EXT_SESSION_GW;
        break;
#endif /*FEATURE_MCFG_MULTISIM_TSTS_SUPPORT*/
#endif /*FEATURE_MCFG_MULTISIM_SUPPORT*/

#ifdef FEATURE_MCFG_SELECTION_BY_1X_IMSI
      case MMGSDI_1X_PROV_PRI_SESSION:
        sub_index = MCFG_SUB_ID_FIRST;
        session_index = MCFG_UIM_EXT_SESSION_1X;
        break;

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
      case MMGSDI_1X_PROV_SEC_SESSION:
        sub_index = MCFG_SUB_ID_SECOND;
        session_index = MCFG_UIM_EXT_SESSION_1X;
        break;

#ifdef FEATURE_MCFG_MULTISIM_TSTS_SUPPORT
      case MMGSDI_1X_PROV_TER_SESSION:
        sub_index = MCFG_SUB_ID_THIRD;
        session_index = MCFG_UIM_EXT_SESSION_1X;
        break;
#endif /*FEATURE_MCFG_MULTISIM_TSTS_SUPPORT*/
#endif /*FEATURE_MCFG_MULTISIM_SUPPORT*/
#endif /*FEATURE_MCFG_SELECTION_BY_1X_IMSI*/

      case MMGSDI_CARD_SESSION_SLOT_1:
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
      case MMGSDI_CARD_SESSION_SLOT_2:
#ifdef FEATURE_MCFG_MULTISIM_TSTS_SUPPORT
      case MMGSDI_CARD_SESSION_SLOT_3:
#endif /*FEATURE_MCFG_MULTISIM_TSTS_SUPPORT*/
#endif /*FEATURE_MCFG_MULTISIM_SUPPORT*/
        slot_index = mcfg_uim_map_mmgsdi_slot_to_slot_index(
          session_info_ptr[i].slot_id);
        break;
      default:
        slot_index = MCFG_NUM_OF_UIM_SLOT;
        break;
    }
    if (slot_index < MCFG_NUM_OF_UIM_SLOT)
    {
      mcfg_uim_state.slot[slot_index].session_id = 
        session_info_ptr[i].session_id;
      mcfg_uim_state.slot[slot_index].ready = TRUE;
      MCFG_MSG_MED_3("Slot %d session type %d id %llx", 
                      slot_index,
                      session_info_ptr[i].session_type, 
                      mcfg_uim_state.slot[slot_index].session_id);
      mcfg_uim_read_iccid_if_ready(slot_index);
    }
    else if (sub_index < MCFG_NUM_OF_SUB_ID)
    {
      mcfg_uim_state.sub[sub_index].ext_session_id[session_index] = 
        session_info_ptr[i].session_id;
      mcfg_uim_state.sub[sub_index].ext_app_type[session_index]
         = session_info_ptr[i].app_info.app_type;
      MCFG_MSG_MED_5("Sub %d session gw/1x %d type %d id %llx app_type %d", 
                      sub_index, session_index,
                      session_info_ptr[i].session_type, 
                      mcfg_uim_state.sub[sub_index].ext_session_id[session_index],
                      mcfg_uim_state.sub[sub_index].ext_app_type[session_index]);
    }
    else
    {
      MCFG_MSG_MED_1("Invalid session type for mmgsdislot_id %08x", 
                     session_info_ptr[i].slot_id);
    }
  }
} /* mcfg_uim_mmgsdi_session_open_ext_sync_cnf_proc() */


#ifdef FEATURE_MCFG_SELECTION_BY_IMPI
/*===========================================================================

  FUNCTION mcfg_uim_is_isim_aid

  DESCRIPTION
    Checks whether given aid is for ISIM APP or not.

  DEPENDENCIES
    None

  PARAMETERS
    aid_ptr

  RETURN VALUE
    TRUE/FALSE - Returns whether aid belongs to ISIM app

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_uim_is_isim_aid
(
  const mmgsdi_static_data_type *aid_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(aid_ptr);

  if (aid_ptr->data_len >= sizeof(mcfg_uim_isim_aid))
  {
    if (memcmp(aid_ptr->data_ptr, mcfg_uim_isim_aid, sizeof(mcfg_uim_isim_aid)) == 0)
    {
      return TRUE;
    }
  }
  MCFG_MSG_HIGH_1("mcfg_uim_is_isim_aid: not an ISIM app, data_len %ld ", aid_ptr->data_len);
  return FALSE;
} /* mcfg_uim_is_isim_aid() */

/*===========================================================================

  FUNCTION mcfg_uim_open_mmgsdi_session_for_isim

  DESCRIPTION
    Opens MMGSDI non-provisioned session for ISIM app on a given slot.

  DEPENDENCIES
    None

  PARAMETERS
    slot_index

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_open_mmgsdi_session_for_isim
(
  mcfg_slot_index_type_e_type slot_index
)
{
  mmgsdi_non_prov_app_info_type non_prov_app_data;
  mmgsdi_return_enum_type status;
  mmgsdi_session_type_enum_type nonprov_slot_session[] =
  {
    MMGSDI_NON_PROV_SESSION_SLOT_1
    #ifdef FEATURE_MCFG_MULTISIM_SUPPORT
    ,MMGSDI_NON_PROV_SESSION_SLOT_2
    #ifdef FEATURE_MCFG_MULTISIM_TSTS_SUPPORT
    ,MMGSDI_NON_PROV_SESSION_SLOT_3
    #endif
    #endif
  };
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);
  if(!mcfg_uim_is_isim_aid(&(mcfg_uim_state.slot[slot_index].isim_app_id)))
  {
    MCFG_MSG_ERROR_2("Unable to open ISIM session for slot %d, Invalid app id - length %ld", 
              slot_index, mcfg_uim_state.slot[slot_index].isim_app_id.data_len);
    return;
  }

  /* Open Non-Provisioned sessions with MMGSDI for ISIM app on all slots */
  memset(&non_prov_app_data,0,sizeof(non_prov_app_data));
  non_prov_app_data.app_id_data.data_len = mcfg_uim_state.slot[slot_index].isim_app_id.data_len;
  non_prov_app_data.exclusive_channel    = FALSE;
  non_prov_app_data.app_id_data.data_ptr = mcfg_uim_state.slot[slot_index].isim_app_id.data_ptr;
	
  MCFG_MSG_MED_1("Opening non-provisioned session for ISIM app on slot: %d", slot_index);
  status = mmgsdi_session_open(mcfg_uim_state.client_id,
                               nonprov_slot_session[slot_index],&non_prov_app_data,
                               NULL, 0, mcfg_uim_mmgsdi_rsp_cb, 0);
  if (status != MMGSDI_SUCCESS)
  {
    MCFG_MSG_ERROR_2("Opening ISIM session on slot %d failed with status %d", slot_index, status);
    mcfg_uim_state.slot[slot_index].impi_read_status = MCFG_UIM_EF_SESSION_OPEN_FAILED;
  }
}/* mcfg_uim_open_mmgsdi_session_for_isim() */

/*===========================================================================

  FUNCTION mcfg_uim_mmgsdi_session_open_cnf_proc

  DESCRIPTION
    Handles a session open response.

  DEPENDENCIES
    None

  PARAMETERS
    status [in]
    cnf_data [in] 

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_uim_mmgsdi_session_open_cnf_proc
(
  mmgsdi_return_enum_type             status,
  const mmgsdi_session_open_cnf_type *cnf_data
)
{
  mcfg_slot_index_type_e_type slot_index = MCFG_NUM_OF_UIM_SLOT;
  uint8 *data_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(cnf_data);
  
  if (status == MMGSDI_SUCCESS)
  {
    slot_index = mcfg_uim_map_mmgsdi_slot_to_slot_index(cnf_data->response_header.slot_id);
    MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);

    if((cnf_data->app_info.app_type == MMGSDI_APP_UNKNOWN) &&
       (mcfg_uim_is_isim_aid(&(cnf_data->app_info.aid))))
    {
      MCFG_MSG_MED_2("Successfully opened MMGSDI session for ISIM app, slot %d session 0x%llx", 
                   slot_index, cnf_data->session_id);
      mcfg_uim_state.slot[slot_index].session_id_isim = cnf_data->session_id;
      mcfg_uim_state.slot[slot_index].isim_ready = TRUE;
    }
    else
    {
      MCFG_MSG_ERROR_2("Unexpected session open cnf received from MMGSDI, apptype: %d, slot %d",
                   cnf_data->app_info.app_type, slot_index);
    }
  }
  else
  {
    MCFG_MSG_ERROR_1("Opening MMGSDI session failed with status %d", status);
  }
} /* mcfg_uim_mmgsdi_session_open_cnf_proc() */
#endif

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/*===========================================================================

  FUNCTION mcfg_uim_get_mmgsdi_client_id

  DESCRIPTION
    This function to get the registered client id with MMGSDI.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mmgsdi_client_id_type mcfg_uim_get_mmgsdi_client_id()
{
  return mcfg_uim_state.client_id;
}

/*===========================================================================

  FUNCTION mcfg_uim_cmd_proc

  DESCRIPTION
    Processes a mcfg_uim command in the context of the MCFG task.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_uim_cmd_proc
(
  mcfg_task_cmd_s_type *cmd
)
{
  mcfg_uim_cmd_data_s_type *uim_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cmd == NULL || cmd->data == NULL)
  {
    MCFG_MSG_ERROR("Unexpected NULL pointer");
  }
  else
  {
    uim_cmd_data = (mcfg_uim_cmd_data_s_type *) cmd->data;
    switch (uim_cmd_data->cmd_id)
    {
      case MCFG_UIM_CMD_MMGSDI_RSP:
        mcfg_uim_mmgsdi_rsp_proc(
          uim_cmd_data->data.rsp.status, uim_cmd_data->data.rsp.cnf,
          &uim_cmd_data->data.rsp.cnf_data);
        break;

      case MCFG_UIM_CMD_MMGSDI_EVT:
        mcfg_uim_mmgsdi_evt_proc(&uim_cmd_data->data.evt);
        break;

      case MCFG_UIM_CMD_MAPPING_TIMEOUT:
        mcfg_uim_proc_mapping_timeout();
        break;

      case MCFG_UIM_CMD_RESELECT:
        mcfg_uim_reselect(uim_cmd_data->data.sloti);
        break;

      default:
        MCFG_MSG_ERROR_1("Unexpected UIM cmd_id %d", uim_cmd_data->cmd_id);
    }
  }
} /* mcfg_uim_cmd_proc() */

/*===========================================================================

  FUNCTION mcfg_uim_cmd_send

  DESCRIPTION
    This function sends mcfg uim commands for processing. 

  DEPENDENCIES
    None

  PARAMETERS
    uim_cmd    [in]  uim commands

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_uim_cmd_send
(
  mcfg_uim_cmd_e_type uim_cmd, 
  mcfg_uim_cmd_payload_s_type *data
)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_uim_cmd_data_s_type *uim_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd = mcfg_task_cmd_alloc(MCFG_TASK_CMD_UIM, sizeof(mcfg_uim_cmd_data_s_type));
  if (cmd != NULL)
  {
    uim_cmd_data = (mcfg_uim_cmd_data_s_type *) cmd->data;
    uim_cmd_data->cmd_id = uim_cmd;

    if (data)
    {
      memscpy(&uim_cmd_data->data, sizeof(mcfg_uim_cmd_payload_s_type),
              data, sizeof(mcfg_uim_cmd_payload_s_type));
    }

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR_1("Couldn't send UIM command %d", uim_cmd_data->cmd_id);
      mcfg_task_cmd_free(cmd);
    }
    else
    {
      MCFG_MSG_HIGH_1("Uim command sent %d", uim_cmd_data->cmd_id);
    }
  }
} /* mcfg_uim_cmd_send() */
