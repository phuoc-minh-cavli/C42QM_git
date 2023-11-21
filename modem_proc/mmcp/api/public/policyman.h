#ifndef _POLICYMAN_H_
#define _POLICYMAN_H_

/**
  @file policyman.h

  @brief Public interface to Policy Manager
*/

/*
    Copyright (c) 2013-2017 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/api/public/policyman.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include <sys/types.h>
#include "msgr_types.h"
#include "cm.h"
#include "sys.h"
#include "ref_cnt_obj.h"

#ifdef __cplusplus
}
#endif



/* Event to indicate that PM task is ready to process commands */
#define POLICYMAN_RCEVT_READY "policyman:ready"



#define POLICYMAN_ERR_BASE  0x80000000


/** Status code returned by Policy Management functions.
*/
typedef enum
{
  POLICYMAN_STATUS_SUCCESS = 0,               /**< call succeeded */

  POLICYMAN_STATUS_WRN_DEFAULT_USED,          /**< default value used (not provisioned) */
  POLICYMAN_STATUS_WRN_MODIFIED,              /**< provisioned value modified to fit capabilities */
  POLICYMAN_STATUS_WRN_SUSPENDED,             /**< policy manager is in suspended state */
  
  POLICYMAN_STATUS_ERR = POLICYMAN_ERR_BASE,  /**< call failed for unspecified reason */
  POLICYMAN_STATUS_ERR_NOT_READY,             /**< policyman is not ready */
  POLICYMAN_STATUS_ERR_INVALID_CFG,           /**< provisioned value not supported by capabilities */
  POLICYMAN_STATUS_ERR_INVALID_ARGS,          /**< call was made with invalid arguments */
  POLICYMAN_STATUS_ERR_POLICY_VIOLATION,      /**< call failed because of policy currently in place */
  POLICYMAN_STATUS_ERR_NOT_WRITABLE,          /**< configuration item is not writeable */
  POLICYMAN_STATUS_ERR_INVALID_ITEM_ID,       /**< Configuration item id in call is invalid */
  POLICYMAN_STATUS_ERR_INVALID_CLIENT_ID,     /**< Client id in call is invalid */
  POLICYMAN_STATUS_ERR_MALFORMED_XML,         /**< XML in config file for policy is not well-formed */
  POLICYMAN_STATUS_ERR_INVALID_ACTION,        /**< XML for action is well-formed but not correct */
  POLICYMAN_STATUS_ERR_INVALID_CONDITION,     /**< XML for condition is well-formed but not correct */
  POLICYMAN_STATUS_ERR_NOT_PRESENT,           /**< item not present */
  POLICYMAN_STATUS_ERR_POLICY_NOT_ENABLED,    /**< a policy-specific API was called, but policy is not enabled */
  POLICYMAN_STATUS_ERR_INVALID_VERSION,       /**< version of XML file for policy is invalid */
  POLICYMAN_STATUS_ERR_CONFIG_FAILURE,        /**< configuration of policy failed */
} policyman_status_t;


#define POLICYMAN_FAILED(s) (((s) & POLICYMAN_ERR_BASE) != 0)
#define POLICYMAN_SUCCEEDED(s)  (!POLICYMAN_FAILED(s))



/**  IDs for configuration items.
*/
typedef enum
{
  POLICYMAN_ITEM_INVALID                = 0,
  POLICYMAN_ITEM_DEVICE_CONFIGURATION   = 1,
  POLICYMAN_ITEM_RAT_CAPABILITY         = 2,
  POLICYMAN_ITEM_RF_BANDS               = 3,
  POLICYMAN_ITEM_SVC_MODE               = 4,
} policyman_item_id_t;



typedef struct policyman_item_t policyman_item_t;


/*-------- policyman_item_collection_t --------*/
/**
@brief  Collection of items returned to clients.
*/
typedef struct
{
  _REF_CNT_OBJ;
  
  size_t                  seqno;      /**< The current sequence number */
  size_t                  numItems;   /**< The number of items in the collection */
  policyman_item_t const  *pItems[];  /**< Array of pointers to policyman_item_t */
} policyman_item_collection_t;



/*=============================================================================
  Basic policyman APIs
=============================================================================*/


/*-------- policyman_get_items --------*/
/**
@brief Get the configuration items specified by a list of configuration ids.

The items returned have had a reference added to them, so once the caller is
done with the item it should call policyman_item_release() on the pointer to
the items.

@param[in]  pIds    Pointer to an array of item IDs desired.
@param[in]  numIds  Number of IDs pointed to by pIds.
@param[out] ppItem  Pointer to storage for numIds pointers to the configuration
                    items.
                    The pointers are to a policyman_item, which are reference-
                    counted objects.  The caller must call policyman_item_release()
                    on the items when it no longer wants an item.

@return
  - POLICYMAN_STATUS_SUCCESS        -- Call succeeded.
  - POLICYMAN_STATUS_ERR_INVALID_ID -- The list of IDs contained an invalid ID.
  - POLICYMAN_STATUS_ERR_NOT_READY  -- Items are not available because policyman
                                       is still intializing.
*/
policyman_status_t
policyman_get_items(
  policyman_item_id_t const *pIds,
  size_t                    numIds,
  policyman_item_t const    **ppItem
  );


/*-------- policyman_get_items_msim --------*/
/**
@brief Get the configuration items specified by a list of configuration ids for
       all subscriptions.

The items returned have had a reference added to them, so once the caller is
done with the item it should call policyman_item_release() on the pointer to
the items.

@param[in]  pIds    Pointer to an array of item IDs desired.
@param[in]  numIds  Number of IDs pointed to by pIds.
@param[out] ppItems Pointer to storage for a pointer to a
                    policyman_item_collection_t.  The client should call
                    policyman_item_collection_release() in order to release
                    this collection.

@return
  - POLICYMAN_STATUS_SUCCESS        -- Call succeeded.
  - POLICYMAN_STATUS_ERR_INVALID_ID -- The list of IDs contained an invalid ID.
  - POLICYMAN_STATUS_ERR_NOT_READY  -- Items are not available because policyman
                                       is still intializing.
*/
policyman_status_t
policyman_get_items_msim(
  policyman_item_id_t const         *pIds,
  size_t                            numIds,
  policyman_item_collection_t const **ppItems
  );


/*-------- policyman_get_items_block --------*/
/**
@brief Get the configuration items specified by a list of configuration ids.
       If called while policyman is still initializing, this call blocks until
       initialization completes.

The items returned have had a reference added to them, so once the caller is
done with the item it should call policyman_item_release() on the pointer to
the items.

@param[in]  pIds    Pointer to an array of item IDs desired.
@param[in]  numIds  Number of IDs pointed to by pIds.
@param[out] ppItem  Pointer to storage for numIds pointers to the configuration
                    items.
                    The pointers are to a policyman_item, which are reference-
                    counted objects.  The caller must call policyman_item_release()
                    on the items when it no longer wants an item.

@return
  - POLICYMAN_STATUS_SUCCESS        -- Call succeeded.
  - POLICYMAN_STATUS_ERR_INVALID_ID -- The list of IDs contained an invalid ID.    
*/
policyman_status_t
policyman_get_items_block(
  policyman_item_id_t const *pIds,
  size_t                    numIds,
  policyman_item_t const    **ppItem
  );


/*-------- policyman_get_items_block_msim --------*/
/**
@brief Get the configuration items specified by a list of configuration ids for
       all subscriptions.  If called while policyman is still initializing, this
       call blocks until initialization completes.

The items returned have had a reference added to them, so once the caller is
done with the item it should call policyman_item_release() on the pointer to
the items.

@param[in]  pIds    Pointer to an array of item IDs desired.
@param[in]  numIds  Number of IDs pointed to by pIds.
@param[out] ppItems Pointer to storage for a pointer to a
                    policyman_item_collection_t.  The client should call
                    policyman_item_collection_release() in order to release
                    this collection.

@return
  - POLICYMAN_STATUS_SUCCESS        -- Call succeeded.
  - POLICYMAN_STATUS_ERR_INVALID_ID -- The list of IDs contained an invalid ID.
*/
policyman_status_t
policyman_get_items_block_msim(
  policyman_item_id_t const         *pIds,
  size_t                            numIds,
  policyman_item_collection_t const **ppItems
  );


/*-------- policyman_item_get_id --------*/
/**
@brief  Get the ID of a configuration item.

@param[in]  pItem   Pointer to a policyman configuration item.

@return
  The configuration item ID of the item.
*/
policyman_item_id_t
policyman_item_get_id(
  policyman_item_t const  *pItem
  );


/*-------- policyman_item_get_id_msim --------*/
/**
@brief  Get the policyman item ID and subscription ID of a configuration item.

@param[in]  pItem   Pointer to a policyman configuration item.
@param[out] pSubsId Pointer to storage for the subscription ID to which
                    this item corresponds.

@return
  The configuration item ID of the item.
*/
policyman_item_id_t
policyman_item_get_id_msim(
  policyman_item_t const  *pItem,
  sys_modem_as_id_e_type  *pSubsId
  );


/*-------- policyman_item_addref --------*/
/**
@brief  Add a reference to a policyman configuration item.

@param[in]  pItem   Pointer to a policyman configuration item.

@return
  Nothing.
*/
void
policyman_item_addref(
  policyman_item_t const  *pItem
  );



/*-------- policyman_item_release --------*/
/**
@brief  Release a reference on a policyman configuration item.

This is called when an entity with a reference on the configuration item is
done with the item.

@param[in]  pItem   Pointer to a policyman configuration item.

@return
  Nothing.
*/
void
policyman_item_release(
  policyman_item_t const  *pItem
  );



/*-------- policyman_msgr_attachments_release --------*/
/**
@brief  Release all reference-counted attachments in a message.

This is an easier way to release all configuration items received in a message
from policyman than calling policyman_item_release() on each attachment.
NOTE: If you call this function on a MSGR message, YOU MUST NOT call
policyman_item_release() on the individual items or you will over-release them!

@param[in]  pMsg  Pointer to a MSGR message received from policyman.

@return
  None
*/
void
policyman_msgr_attachments_release(
  msgr_hdr_s  *pMsg
  );


/*-------- policyman_item_collection_release --------*/
/**
@brief  Release an item collection obtained from a call to
        policyman_get_items_msim() or policyman_get_items_block_msim().

@param[in]  pCollection Pointer to the collection to release.

@return
  None
*/
void
policyman_item_collection_release(
  policyman_item_collection_t const *pCollection
  );



/*=============================================================================
  Registration for updates from policyman.
=============================================================================*/

/*-----------------------------------------------------------------------------
  To register for updated configurations from policyman, register with MSGR
  to receive POLICYMAN_CFG_UPDATE_IND (defined in policyman_msg.h).

  Each indication received will contain reference-counted pointers
  (as attachments to the message) to all configuration items that were updated
  by policyman as a result of a policy change.  Your code may or may not want
  all the items - just ignore any item whose ID (obtained via a call to
  policyman_item_get_id) does not interest you.
-----------------------------------------------------------------------------*/



/*=============================================================================
  APIs to access POLICYMAN_ITEM_DEVICE_CONFIGURATION data.
=============================================================================*/

/*-------- policyman_device_config_num_sims --------*/
/**
@brief  Get the number of SIMs in the device configuration.

@param[in]  pItem   Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[out] pnSims  Pointer to storage for the number of SIMs in the configuration.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pnSims is NULL.
  - POLICYMAN_STATUS_WRN_MODIFIED if the number of SIMs specified in the
    configuration file is not supported on the device.  In this case, the
    maximum number that are supported is returned.
*/
policyman_status_t
policyman_device_config_num_sims(
  policyman_item_t const  *pItem,
  size_t                  *pnSims
  );


/*-------- policyman_device_config_max_active_voice --------*/
/**
@brief  Get the maximum number of active subscriptions allowed in the
        device configuration.

@param[in]  pItem       Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[out] pnMaxActive Pointer to storage for the maximum number of active
                        subscriptions in the configuration.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pnMaxActive is NULL.
  - POLICYMAN_STATUS_WRN_MODIFIED if the number of active subscriptions from
    the configuration file is not supported on the device.  In this case, the
    maximum number supported will be returned.
*/
policyman_status_t
policyman_device_config_max_active_voice(
  policyman_item_t const  *pItem,
  size_t                  *pnMaxActive
  );


#define policyman_device_config_max_active policyman_device_config_max_active_voice

/*-------- policyman_device_config_max_active_data --------*/
/**
@brief  Get the maximum number of data active subscriptions allowed in the
        device configuration.

@param[in]  pItem       Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[out] pnMaxActiveData Pointer to storage for the maximum number of active
                        subscriptions in the configuration.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pnMaxActive is NULL.
  - POLICYMAN_STATUS_WRN_MODIFIED if the number of active subscriptions from
    the configuration file is not supported on the device.  In this case, the
    maximum number supported will be returned.
*/
policyman_status_t
policyman_device_config_max_active_data(
  policyman_item_t const  *pItem,
  size_t                  *pnMaxActiveData
  );



/*-------- policyman_device_config_overall_feature --------*/
/**
@brief  Get the configured high-level feature for the device.
        
The overall device-level feature specification is an enum that combines
all the information contained in subscription-level features of each
subscription into a single enum for components that are not designed to
handle subscription-level information.

Components should attempt to work against the subscription-level features
obtained from policyman_device_config_get_subs_feature, as it will make them
more immune to changes in the overall feature enum.

@param[in]  pItem       Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[out] pFeature    Pointer to storage for a sys_overall_feature_t value.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pFeature is NULL.
  - POLICYMAN_STATUS_WRN_MODIFIED if the feature in the configuration file is
    not supported on this device.  In this case, SYS_FEATURE_MODE_NORMAL will
    be returned.
*/
policyman_status_t
policyman_device_config_overall_feature(
  policyman_item_t const  *pItem,
  sys_overall_feature_t   *pFeature
  );


/*-------- policyman_device_config_num_cfgs --------*/
/**
@brief  Get the number of RAT configurations in the device configuration.

@param[in]  pItem   Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[out] pnCfgs  Pointer to storage for the number of RAT configurations
                    in the device configuration.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pnCfgs is NULL.
*/
policyman_status_t
policyman_device_config_num_cfgs(
  policyman_item_t const  *pItem,
  size_t                  *pnCfgs
  );


/*-------- policyman_device_config_get_config --------*/
/**
@brief  Get a particular RAT configuration item from the device configuration.


@param[in]  pItem   Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[in]  nCfg    Index of the configuration to access (must be less than the
                    number of configurations).
@param[in]  nSim    Index of the sim to access (must be less than the number of
                    SIMs).
@param[out] pRatCfg Pointer to storage for a bitmask of the supported RATs for
                    the given nCfg/nSim combination.  The bitmask is to be
                    interpreted as a mask of sys_sys_mode_mask_e_type bits
                    as defined in sys.h.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pRatCfg is NULL or nCfg or nSim is
    out of range.
*/
policyman_status_t
policyman_device_config_get_config(
  policyman_item_t const  *pItem,
  size_t                  nCfg,
  size_t                  nSim,
  uint32                  *pRatCfg
  );


/*-------- policyman_device_config_get_subs_feature --------*/
/**
@brief  Get the feature for a specific subscription.


@param[in]  pItem   Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[in]  nCfg    Index of the configuration to access (must be less than the
                    number of configurations).
@param[in]  nSim    Index of the sim to access (must be less than the number of
                    SIMs).
@param[out] pFeature Pointer to storage for a sys_subs_feature_t value.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pRatCfg is NULL or nCfg or nSim is
    out of range.
*/
policyman_status_t
policyman_device_config_get_subs_feature(
  policyman_item_t const  *pItem,
  size_t                  nCfg,
  size_t                  nSim,
  sys_subs_feature_t      *pFeature
  );


/*-------- policyman_device_config_get_specialization --------*/
/**
@brief  Get the "specialization" for this device configuration.  This indicates
        which special (perhaps spec-violating) code is to be run in modem.

@param
@param[in]  pItem     Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[out] pSpecial  Pointer to storage for a sys_specialization_t.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pSpecial is NULL.
*/
policyman_status_t
policyman_device_config_get_specialization(
  policyman_item_t const  *pItem,
  sys_specialization_t    *pSpecial
  );


/*-------- policyman_device_config_get_max_capability --------*/
/**
@brief  Get the maximum RAT capability for this device configuration supported by Hardware.

@param[in]  pItem      Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[in]  nSim       Index of the sim to access (must be less than the number of SIMs).
@param[out] pRatCfg  Pointer to storage for a bitmask of the supported RATs for
                               the given nSim.  The bitmask is to be
                               interpreted as a mask of sys_sys_mode_mask_e_type bits
                               as defined in sys.h.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pRatCfg is NULL.
*/
policyman_status_t policyman_device_config_get_max_capability(
  policyman_item_t const  *pItem,
  size_t                   nSim,
  uint32                  *pRatCfg
);

/*-------- policyman_device_config_get_current_subphone_capability --------*/
/**
@brief  Get the maximum RAT capability for current active device configuration.

@param[in]  pItem      Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[in]  nSim       Index of the sim to access (must be less than the number of SIMs).
@param[out] pRatCfg  Pointer to storage for a bitmask of the supported RATs for
                               the given nSim.  The bitmask is to be
                               interpreted as a mask of sys_sys_mode_mask_e_type bits
                               as defined in sys.h.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pRatCfg is NULL.
*/
policyman_status_t policyman_device_config_get_current_subphone_capability(
  policyman_item_t const  *pItem,
  size_t                   nSim,
  uint32                  *pRatCfg
);

/*-------- policyman_device_config_get_current_config_index --------*/
/**
@brief  Get the index of current active configuration.

@param[in]  pItem      Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[out]  pIndex     Container to get index.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pIndex is NULL.
*/
policyman_status_t policyman_device_config_get_current_config_index(
  policyman_item_t const  *pItem,
  size_t                  *pIndex
);


/*-------- policyman_device_config_max_active_subs --------*/
/**
@brief  Get the maximum number of subs that can ever be active.

@param[in]    pItem               Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[out]  pnMaxActive     Container to get maximum number of subs that can ever be active.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pIndex is NULL.
*/

policyman_status_t policyman_device_config_max_active_subs(
  policyman_item_t const  *pItem,
  size_t                  *pnMaxActive
);


/*-------- policyman_device_config_set_explicit_index --------*/
/**
@brief  Set the active subphone configuration explicitly. Index value of -1 means 
          modem controls setting of subphone configuration while for other values caller
          is reponsible for setting correct values. Index value must be less than the  
          configs returned in policyman_device_config_num_cfgs().

@param[in] explicitIndex    Index of the configuration that we want to set in configs Array.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if index is out of bounds.
*/
policyman_status_t policyman_device_config_set_explicit_index(
  int  explicitIndex
);


/*-------- policyman_device_config_get_explicit_index --------*/
/**
@brief  Get the explicit configuration index.  If this returns -1, modem is in control 
          of which configuration is being used. The actual configuration can be read 
          from policyman_device_config_get_config().  Any other value means that 
          the configuration has been explicitly set.

@param[out] pExplicitIndex    Container to get explicit index.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pExplicitIndex is NULL.
*/
policyman_status_t policyman_device_config_get_explicit_index(
  int *pExplicitIndex
);

/*=============================================================================
  APIs to access POLICYMAN_ITEM_RAT_CAPABILITY data.
=============================================================================*/

/*-------- policyman_get_rat_capability --------*/
/**
@brief  Return the RAT priority capability to be used.

@param[in]  pItem   Pointer to a POLICYMAN_ITEM_RAT_CAPABILITY item.
@param[out] pRatCfg Pointer to storage for the RAT capability to be used.
                    This is a bitmask of the sys_sys_mode_mask_e_type defined
                    in sys.h.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_RAT_CAPABILITY item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pRatCfg is NULL.
*/
policyman_status_t
policyman_get_rat_capability(
  policyman_item_t const  *pItem,
  uint32                  *pRatCfg
  );

/*=============================================================================
  APIs to access POLICYMAN_ITEM_RF_BANDS data.
=============================================================================*/

/*-------- policyman_get_rf_bands --------*/
/**
@brief  Return the RF band information to be used.

@param[in]  pItem     Pointer to a POLICYMAN_ITEM_RF_BANDS item.
@param[out] pGWbands  Pointer to storage for the GW band mask.  May be NULL.
@param[out] pLTEbands Pointer to storage for the LTE band mask. May be NULL.
@param[out] pTDSbands Pointer to storage for the TDS band mask. May be NULL.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_RF_BANDS item.
*/
policyman_status_t
policyman_get_rf_bands(
  policyman_item_t const  *pItem,
  sys_band_mask_type        *pGWbands,
  sys_lte_band_mask_e_type  *pLTEbands,
  sys_band_mask_type        *pTDSbands
  );


/*-------- policyman_get_rf_bands_force --------*/
/**
@brief  Return whether to force registration with the RF bands returned.

@param[in]  pItem   Pointer to a POLICYMAN_ITEM_RF_BANDS item.
@param[out] pForce  Pointer to storage for a boolean indicating whether to
                    force registration.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_RF_BANDS item.
*/
policyman_status_t
policyman_get_rf_bands_force(
  policyman_item_t const  *pItem,
  boolean                 *pForce
  );

/*=============================================================================
  APIS used to access POLICYMAN_ITEM_SVC_MODE data.
=============================================================================*/

typedef enum
{
  POLICYMAN_SVC_MODE_LIMITED,
  POLICYMAN_SVC_MODE_CAMP_ONLY,
  POLICYMAN_SVC_MODE_FULL,
  POLICYMAN_SVC_MODE_ALL,
} policyman_svc_mode_t;


/*-------- policyman_get_svc_mode --------*/
/**
@brief  Return the service mode to be used.

@param[in]  pItem     Pointer to a POLICYMAN_ITEM_SVC_MODE item.
@param[out] pSvcMode  Pointer to storage for the service mode as specified
                      by policyman_svc_mode_t_t.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_SVC_MODE item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pSvcMode is NULL.
*/
policyman_status_t
policyman_get_svc_mode(
  policyman_item_t const  *pItem,
  policyman_svc_mode_t    *pSvcMode
  );

/*=============================================================================
  APIs used to suspend/resume Policy Manager from running rules.
=============================================================================*/

/*-------- policyman_suspend --------*/
/**
@brief  Suspend policy manager from running rules.

@return
   None.
*/
void
policyman_suspend(
  void
  );

/*-------- policyman_resume--------*/
/**
@brief  Resume policy manager for running rules.

@return
   None.
*/
void
policyman_resume(
  void
  );


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  APIs below are not for general use, but are targeted to specific uses.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/*=============================================================================
  API to return hardware RAT/band capabilities.
=============================================================================*/


/*-------- policyman_get_hardware_rats_bands --------*/
/**
@brief  Return the RATs and bands supported by hardware, taking into account
        RF, MCS, and segment-loading aspects.  This is a synchronous call that
        can be made at any time.

@param[out] pRatMask  Pointer to storage for the RAT mask.
@param[out] pGwBands  Pointer to storage for the GW band mask.
@param[out] pLteBands Pointer to storage for the LTE band mask.
@param[out] pTdsBands Pointer to storage for the TDS bands.

@return
  None
*/
void policyman_get_hardware_rats_bands(
  uint32             *pRatMask,
  sys_band_mask_type *pGwBands,
  sys_lte_band_mask_e_type  *pLteBands,
  sys_band_mask_type *pTdsBands
  );


/*-------- policyman_set_periodic_plmn_search_allowed--------*/
/**
@brief  Set the boolean for allowing periodic PLMN search

@param[in] subsId         Subscription id for which PLMN search is allowed
@param[in]  isAllowed  Boolean value indicating if periodic PLMN search is allowed

@return
    None
*/
void
policyman_set_periodic_plmn_search_allowed(
   sys_modem_as_id_e_type  subsId,
   boolean                 isAllowed
);


/*=============================================================================
  API to set HLOS MCC.
=============================================================================*/

/*-------- policyman_set_hlos_mcc --------*/
/**
@brief  Notify Policy Manager about the MCC received from HLOS 
and its associated confidence level. 

@param[in] mcc              HLOS MCC
@param[in] confidenceLevel  Confidence level associated with that MCC
@param[in] status           Enumeration to indicate HLOS status
@return
    POLICYMAN_STATUS_SUCCESS if information is consumed, otherwise relevant
    error is returned.
*/
policyman_status_t policyman_set_hlos_mcc(
  sys_mcc_type  mcc,
  size_t        confidenceLevel,
  size_t        status
);

/*-------- policyman_get_e911_rat_mask --------*/
/**
@brief  Return the RATs allowed for e911 calls.
        This is a synchronous call that can be made at any time.

@return
  ratMask  The allowed RAT mask.
*/
uint32 policyman_get_e911_rat_mask(
  sys_modem_as_id_e_type  subsId
);

/*-------- policyman_get_call_mode--------*/
/**
@brief  Query Policy Manager for the current call_mode. 
        The call_mode indicates whether normal call
        handling should be done (SYS_CALL_MODE_NORMAL) or
        whether calls must be restricted to volte.

@param[in]  subsId         Modem subs to query call_mode
@param[in]  pCampedPLMN    Pointer to a sys_plmn_id_s_type containing the 
                           PLMN of the network to check against the whitelist.
                           Only the MCC of the PLMN is used.
@return
  - sys_call_mode_t member:
      SYS_CALL_MODE_NORMAL:
        Indicates normal call handling. Either volte is disabled, or
        the MCC is not in the whitelist.
      SYS_CALL_MODE_VOLTE_ONLY:
        Indicates that calls should only be made on volte.
*/
sys_call_mode_t policyman_get_call_mode(
  sys_modem_as_id_e_type  subsId,
  sys_plmn_id_s_type     *pCampedPLMN
);

/*-------- policyman_get_boolean_value--------*/
/**
@brief  Query a Policy Manager boolean.
         The caller needs to pass in correct name

@param[in]  pBooleanName  Pointer to the name of boolean to query
@param[in]  pValue        Pointer to a boolean to store the value
                          of the queried boolean.
@return
  - TRUE if the queried boolean is found.
  - FALSE otherwise
*/

boolean policyman_get_boolean_value(
  char const  *pBooleanName,
  boolean     *pValue
);


/*-------- policyman_get_items_since_sequence_number --------*/
/**
@brief  Return a collection of items that have changed since a particular
        sequence number.

@param[in]  seqno   The sequence number from which updates are desired.

@return
  NULL if seqno is equal to or greater than the current sequence number.
  A pointer to a reference-counted policyman_item_collection_t that contains
  all items that have changed since seqno.
  In the case of a non-NULL return, the client should call
  policyman_item_collection_release() in order to release this collection.
*/
policyman_item_collection_t *policyman_get_items_since_sequence_number(
  size_t  seqno
);

#endif /* _POLICYMAN_H_ */
