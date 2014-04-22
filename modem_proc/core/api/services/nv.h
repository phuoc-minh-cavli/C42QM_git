#ifndef NV_H
#define NV_H

/**
@file nv.h
@brief
This header file contains all the definitions necessary for any task
to interface with the Non Volatile Memory task, in order to access
the NV items in EFS to read, write, and start.

*/
/*==========================================================================

         N V    T A S K    P U B L I C    H E A D E R    F I L E

DESCRIPTION

  This header file contains all the definitions necessary for any task
  to interface with the Non Volatile Memory task, in order to access
  the NV items in EFS to read, write, and start.

Copyright (c) 2005-2009, 2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.10/api/services/nv.h#1 $ $DateTime: 2018/06/26 03:18:17 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/18/13   rks     CR525590:removed modem API depedency and feature FEATURE_NV_MEID_ENABLE
03/05/13   ph      Provide nv_ruim_register() a cb registration routine for RUIM
12/10/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
11/08/10   EBR     Doxygenated File.
08/14/09   sri     Cleanup the feature : FEATURE_UIM_RUN_TIME_ENABLE as the 
                   functions nv_rtre_control, nv_rtre_polling_control and 
                   nv_rtre_polling_for_nam are mainlined for UMTS targets also
08/11/09   sri     Removed REX dependencies from the NV on WM side 
07/15/09   sri     Remoted nv_rtre_control, nv_rtre_polling_control and 
                   nv_rtre_polling_for_nam
07/08/09   sri     Moved inline function implementations to nvim.c
11/15/08   pc      Split header into public and private versions for CMI.
06/05/05   pc      Created NV-2.
===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "coreapi_variation.h"
#include "comdef.h"
#ifndef FEATURE_WINCE
#include "rex.h"
#endif /* FEATURE_WINCE */
#include "queue.h"
#include "qw.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "nv_items.h"     // This header file is generated by NVC

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** @addtogroup services_nv
@{ */

/** R-UIM access synchronization
*/
#define  NV_RUIM_ACCESS_SIG   0x0010      

/*===========================================================================
                                                                         
                              MACROS                                     
                                                                         
===========================================================================*/

/** Watchdog report signal
*/
#define  NV_RPT_TIMER_SIG     0x0001

/** Command codes used when a command is issued to the NV task.
*/
  typedef enum {
    NV_READ_F,                    /**< Read an item. */
    NV_WRITE_F,                   /**< Write an item. */
    NV_PEEK_F,                    /**< Peek at a location. */
    NV_POKE_F,                    /**< Poke into a location. */
    NV_FREE_F,                    /**< Free an NV item's memory allocation. */
    NV_CHKPNT_DIS_F,              /**< Disable cache checkpointing for glitch
								       recovery. */
    NV_CHKPNT_ENA_F,              /**< Enable cache checkpointing for glitch
								       recovery. */
    NV_OTASP_COMMIT_F,            /**< Commit (write) Over-The-Air Service
								       Provisioning (OTASP) parameters to the
									   NV. */
    NV_REPLACE_F,                 /**< Replace (overwrite) a dynamic pool item. */
    NV_INCREMENT_F,               /**< Increment the rental timer item. */
    NV_RTRE_OP_CONFIG_F,          /**< Set the operational configuration of
								       Runtime R-UIM Enable (RTRE). */
    NV_FUNC_ENUM_PAD = 0x7FFF,    /**< Pad to 16 bits on the ARM<sup>&reg;</sup>
								       processor. */
    NV_FUNC_ENUM_MAX = 0x7fffffff /**< Maximum valid value in the enumeration.
								       Pad to 32 bits. */

  } nv_func_enum_type;


/** Returned status codes for requested operation.
*/
  typedef enum {
    NV_DONE_S,                    /**< Request was completed. */
    NV_BUSY_S,                    /**< Request is queued. */
    NV_BADCMD_S,                  /**< Unrecognizable command field. */
    NV_FULL_S,                    /**< NVM is full. */
    NV_FAIL_S,                    /**< Command failed for a reason other than
								       NVM full. */
    NV_NOTACTIVE_S,               /**< Variable was not active. */
    NV_BADPARM_S,                 /**< Bad parameter in the command block. */
    NV_READONLY_S,                /**< Parameter is write-protected and thus
								       read-only. */
    NV_BADTG_S,                   /**< Item is not valid for this target. */
    NV_NOMEM_S,                   /**< Free memory has been exhausted. */
    NV_NOTALLOC_S,                /**< Address is not a valid allocation. */
    NV_RUIM_NOT_SUPPORTED_S,      /**< NV item is not supported in RUIM */
    NV_STAT_ENUM_PAD = 0x7FFF,    /**< Pad to 16 bits on the ARM processor. */
    NV_STAT_ENUM_MAX = 0x7FFFFFFF /**< Maximum valid value in the enumeration.
								       Pad to 32 bits. */
  } nv_stat_enum_type;

/** commands to use in regsitered 1X-CP API for geting the apropriate action done.
*/
typedef enum nv_1x_cp_cmd_type_tag{
   NV_MC_ESN_PROG_ALLOWED_WITH_MEID_SUPPORT_F,/**< Command .to check if 
                                esn programming is allowed with meid support */
   NV_MC_COMPUTE_PESN_F,     /**< Command to get pesn computed using meid. */
   NV_MC_INVALID             /**< Invlaid command.*/
} nv_1x_cp_cmd_type;

/** Function pointer type to be registered by 1X-CP.
*/
typedef boolean nv_1x_cp_cb_type (uint32 cmd_id, void * in_data_ptr, void * out_data_ptr);

/** Valid Electronic Serial Number (ESN) checksum.
*/
#define     NV_VALID_ESN_CHKSUM     0xFFFFFFFFL


/** @brief Command in the Nonvolatile Memory (NVM) queue.
*/
typedef struct {
  q_link_type           link;          /**< Queue field. */
  nv_items_enum_type    item;          /**< Item to access. */
  nv_func_enum_type     cmd;           /**< Command to use: READ, WRITE, PEEK,
										    POKE, etc. */
  nv_item_type          *data_ptr;     /**< Pointer to the read or write data. */
#ifndef FEATURE_WINCE
  rex_tcb_type          *tcb_ptr;      /**< Task to signal; valid only if
									        FEATURE_WINCE is not defined. */
  rex_sigs_type         sigs;          /**< REX signals for the task; valid only
									        if FEATURE_WINCE is not defined. */
#else /* FEATURE_WINCE */
  #error code not present
#endif /* FEATURE_WINCE */
  q_type                *done_q_ptr;   /**< Where to place the done buffer. */
  nv_stat_enum_type     status;        /**< Status of the request. */
} nv_cmd_type;


/** @brief Command in the NVM queue with support for accessing items on
    multiple Subscriber Identification Modules (SIMs) via context.
*/
typedef struct {
  q_link_type       link;   /**< Queue field. */
  nv_cmd_type		*nvcmd; /**< Command to use: READ, WRITE, PEEK, POKE, etc. */
  uint16		context;	/**< Context for identifying the SIM. */
}nv_cmd_ext_type;

/** @brief IOCTLs exposed by NV for RUIM to perform appropriate operations
           on RUIM.
*/
typedef enum 
{
NV_RUIM_CMD_VERIFY_IOCTL,                   
/**< This IOCTL is used to verify whether an NV item is card bound or not.
Usage : nv_ruim_callback(NV_RUIM_CMD_VERIFY_IOCTL, (void *) cmd_ptr,  NULL) ;
@param[in] : Pointer pbuf_in of type nv_cmd_type
@param[out] : None
@return:
NV_DONE_S  if the NV item is supported in RUIM.
NV_RUIM_ITEM_NOT_SUPPORTED  if the item is not supported in RUIM.
 */

NV_RUIM_CMD_ACCESS_IOCTL,                   
/**< process the card bound NV item command
Usage : nv_ruim_callback(NV_RUIM_CMD_ACCESS_IOCTL, (void *) cmd_ptr,  NULL) ;
@param[in] : Pointer pbuf_in of type nv_cmd_type
@param[out] : None
@return:
NV_DONE_S  if the NV item is supported and processed in RUIM.
NV_RUIM_ITEM_NOT_SUPPORTED  if the item is not supported in RUIM.
*/

NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL,        
/**< to verify whether an NV item is supported in RUIM and also process the 
command if supported.
Usage : nv_ruim_callback(NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL, 
                                    (void *) cmd_ptr, NULL);
@param[in] : Pointer pbuf_in of type nv_cmd_type
@param[out] : None
@return:
NV_DONE_S  if the NV item is supported and processed in RUIM.
NV_RUIM_ITEM_NOT_SUPPORTED  if the item is not supported in RUIM.
 */

NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL,            
/**< For nv_rtre_control_value 
Usage :nv_ruim_callback(NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL, NULL, 
                                (void *) & nv_cmd_rtre_control_value);
@param[in] : None
@param[out]: Pointer to a nv_rtre_control_type that has nv_rtre_control_value.
@return:
NV_DONE_S if RUIM processes the request.
NV_FAIL_S if RUIM is unable to process.
 */

NV_RUIM_CMD_RTRE_NAM_IOCTL,                
/**< For nv_rtre_use_ruim_for_nam 
Usage : nv_ruim_callback(NV_RUIM_CMD_RTRE_NAM_IOCTL , (void *) &nam_value,
                                                    (void *) &status);
@param[in] : Pointer to a byte which has nam_value.
@param[out]: Pointer to a boolean value in status. 
@return: 
NV_DONE_S if RUIM processes the request.
NV_FAIL_S if RUIM is unable to process.
 */

NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL,  
/**< For nv_rtre_polling_control
Usage: nv_ruim_callback(NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL,  NULL,  
                            (void *) & nv_cmd_rtre_polling_control_value ) ;
@param[in] : None
@param[out]: Pointer to a nv_rtre_polling_control_type that has rtre polling 
             control value.
@return:
NV_DONE_S if RUIM processes the request. 
NV_FAIL_S if RUIM is unable to process.
*/

NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL,
/**< For nv_rtre_polling_for_nam 
Usage: nv_ruim_callback(NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL, 
                        (void *) &nam_value,  (void *) &status ) ;
@param[in] :  Pointer to a byte which has nam_value.
@param[out]:  Pointer to a boolean value in status. 
@return:
NV_DONE_S if RUIM processes the request.
NV_FAIL_S if RUIM is unable to process.
*/

NV_RUIM_CMD_ROAMING_LIST_MAX_SIZE_IOCTL  ,            
/**< For nv_max_size_of_roaming_list  
Usage: nv_ruim_callback(NV_RUIM_CMD_ROAMING_LIST_MAX_SIZE_IOCTL, NULL, (void *) &size) ;
@param[in] : NULL
@param[out]: Pointer to a word with its value in size. 
@return:
NV_DONE_S if RUIM processes the request.
NV_FAIL_S if RUIM is unable to process.
*/

NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL       
/**< Process RTRE configuration command
Usage: nv_ruim_callback(NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL, 
                                    (void *) cmd_ptr,  NULL);
@param[in] : cmd_ptr
@param[out]: None
@return:
NV_DONE_S if RUIM processes the request.
NV_FAIL_S if RUIM is unable to process.
*/

}nv_ruim_enum_type;

/** @brief callback function pointer exposed to RUIM for registering its 
           routine that takes care of NV-RUIM access operations.
*/
typedef nv_stat_enum_type (*nv_ruim_register_cb_type) 
            ( nv_ruim_enum_type type, void * pbuf_in, void * pbuf_out );


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*These are HTORPC Meta comments. Do not delete this*/
/*~ FIELD nv_cmd_type.data_ptr POINTER DISC nv_cmd_type.item */
/*~ FIELD nv_cmd_ext_type.data_ptr POINTER DISC nv_cmd_ext_type.item */

/**
  Interfaces to the NVM task.

  This function places an already filled out nv_cmd_type in the NV command queue.
  The function then returns to the caller, which signifies that the command has
  been queued for execution, not that it has been executed. When nv_cmd is done
  processing the request, it returns the buffer to the specified queue if a
  return queue adress is provided. Otherwise, it does not move the buffer.

  An example NV_READ request for MIN1 is as follows:\n

  - static nv_cmd_type cmd_buf               -- Defines the command buffer.
  - static nv_item_type data_buf             -- Defines the data buffer.
  - data_buf.min1.nam  = current_nam         -- Specifies which NAM to use.
  - cmd_buf.cmd        = NV_READ_F           -- Specifies the read operation.
  - cmd_buf.tcb_ptr    = rex_self()          -- Pointer to the current Task
                                                Control Block (TCB).
  - cmd_buf.sigs       = XX_NV_CMD_SIG       -- Signal to set when done.
  - cmd_buf.done_q_ptr = NULL                -- No buffer return required.
  - cmd_buf.item       = NV_MIN1_I           -- Specifies the item to read.
  - cmd_buf.data_ptr   = &data_buf           -- Where to place read data.
  - rex_clr_sigs (rex_self(), XX_NV_CMD_SIG) -- Clears the signal.
  - nv_cmd (&cmd_buf)                        -- Issues the command.@latexonly \\[28pt] @endlatexonly

@note
  Nothing is returned directly. The requested data is returned in the buffer
  pointed to inside the nv_cmd_type. The status variable of the nv_cmd_type
  will be updated to reflect the current status of this command as it is
  processed. In the above nv_cmd buffer, nv_cmd.status is set to:\n

  - NV_DONE_S      -- Request was completed.
  - NV_BUSY_S      -- Request is queued.
  - NV_BADCMD_S    -- Unrecognizable command field.
  - NV_FULL_S      -- NVM is full.
  - NV_FAIL_S      -- Command failed for a reason other than NVM full.
  - NV_NOTACTIVE_S -- Variable was not active.
  - NV_BADPARM_S   -- Bad parameter in the command block.
  - NV_READONLY_S  -- Parameter is write-protected and thus read-only.
  - NV_BADTG_S     -- Item is not valid for this target.
  - NV_NOMEM_S     -- Free memory has been exhausted.
  - NV_NOTALLOC_S  -- Address is not a valid allocation.@latexonly \\[24pt] @endlatexonly

@param[in,out] cmd_ptr Pointer to the NV command block.

@return
  None (see note above).

@dependencies
  The NV task must have been started already. All of the items in nv_cmd_type
  must already be set.

@sideeffects
  The nv_cmd_type is placed in the NV command queue. It must not be modified
  until the command has been processed.
*/
extern void 
nv_cmd (
  nv_cmd_type *cmd_ptr
);


/**
 Interfaces with the NVM task when there is a requirement to specify which
 SIM/subscription is being referenced.

 This function places an already filled out nv_cmd_ext_type in the NV command
 queue. The function then returns to the caller, which signifies that the
 command has been queued for execution, not that it has been executed. When
 nv_cmd is done processing the request, it returns the buffer to the specified
 queue if a return queue adress is provided. Otherwise, it does not move the
 buffer.@latexonly \\[20pt] @endlatexonly
 
@note
  Nothing is returned directly. The requested data is returned in the buffer
  pointed to in nv_cmd_ext_type. The status variable of nv_cmd_type will be
  updated to reflect the current status of this command as it is processed.
  In the nv_cmd buffer, nv_cmd.status is set to:\n

  - NV_DONE_S      -- Request was completed.
  - NV_BUSY_S      -- Request is queued.
  - NV_BADCMD_S    -- Unrecognizable command field.
  - NV_FULL_S      -- NVM is full.
  - NV_FAIL_S      -- Command failed for a reason other than NVM full.
  - NV_NOTACTIVE_S -- Variable was not active.
  - NV_BADPARM_S   -- Bad parameter in the command block.
  - NV_READONLY_S  -- Parameter is write-protected and thus read-only.
  - NV_BADTG_S     -- Item is not valid for this target.
  - NV_NOMEM_S     -- Free memory has been exhausted.
  - NV_NOTALLOC_S  -- Address is not a valid allocation.
  
@param[in,out] cmd_ext_ptr Pointer to the NV command block.

@return
  None (see note above).

@dependencies
  The NV task must have been started already. All of the items in nv_cmd_ext_type
  must already be set.

@sideeffects
  nv_cmd_ext_type is placed in the NV command queue. It must not be modified
  until the command has been processed.
*/
extern void 
nv_cmd_ext (
  nv_cmd_ext_type *cmd_ext_ptr
);


/**
 Initializes NV timers and the command queue.
 
 The main control task calls this function with rex_def_task to start the NV
 task. If the major revision number has changed, this function rebuilds the NV.

 Once initialization has completed, the function sets MC_ACK_SIG and waits for
 NV_START_SIG. When the start signal is received, the function reads and
 processes any request it finds in its queue until there are no more requests.
 Once the request queue is empty, the task starts normal operation by waiting
 for and processing new commands when they arrive in its command queue. 
 Throughout its operation, the NV task wakes up periodically to kick the
 watchdog task.

@param[in] param Unused parameter from REX.
 
@return
  None.

@dependencies
  This function must be called with rex_def_task _once_. It requires
  NV_START_SIG before proceeding past the initialization stage.

@sideeffects
  Modifies the NVM.
*/
extern void 
nv_task (
  dword param
);


/**
  Reads an error log from the NVM.

@param[out] nv_err_log_ptr Pointer to where to return the read data.

@return
  NV_DONE_S       -- Read was completed.\n
  NV_NOT_ACTIVE_S -- Item was not active.\n
  NV_FAIL_S       -- Item could not be read.

@dependencies
  - The function MUST NOT USE ANY TASK SERVICES, so that it can be called when
  tasks are not initialized.\n
  - The function MUST NOT USE ERROR SERVICES. It returns a status to the caller
  if there is a failure.
*/
nv_stat_enum_type 
nv_read_err_log (
  nv_err_log_type  *nv_err_log_ptr
);

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_read_err_log */
/*~ PARAM INOUT nv_err_log_ptr POINTER */

/**
  Writes the boot error log to the NVM.

@param[in] nv_err_log_ptr Pointer to where to get the write data.

@return
  NV_DONE_S -- Error log was written successfully.\n
  NV_FAIL_S -- Writing the error log failed.

@dependencies
 - The function MUST NOT USE ANY TASK SERVICES, so that it can be called when
 tasks are not initialized.\n
 - The function MUST NOT USE ERROR SERVICES. It returns a status to the caller
 if there is a failure.
*/
nv_stat_enum_type
nv_write_err_log (
  nv_err_log_type  *nv_err_log_ptr
);

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_write_err_log */
/*~ PARAM INOUT nv_err_log_ptr POINTER */

/**
  Returns the maximum number of bytes available for storage of a roaming
  list (per NAM). The value returned depends on the total amount of NVRAM
  installed in the phone and the total number of NAMs defined.
 
@return
  Returns the maximum number of bytes available for storage of a roaming list
  (per NAM).

@dependencies
  Cannot be called until the NV is initialized. A premature call raises the
  error code err_fatal.
*/
word  nv_max_size_of_roaming_list (void);

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_max_size_of_roaming_list */

/**
  Returns the ``valid'' indicator from the specified NAM's roaming list.

@param[in] nam NAM to check for validity.

@return
  TRUE  -- Roaming list valid field is TRUE.\n
  FALSE -- Roaming list valid field is FALSE, the specified NAM is invalid,
           or the roaming list is currently NOT_ACTIVE.
		 
@dependencies
  None.
*/
boolean  
nv_prl_is_valid (
  byte  nam
);

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_prl_is_valid */

/**
  Checks the major version number of the the NV task, as stored at edit time,
  against the major version number of the NV EEPROM, as read from the NVM. If
  the numbers are different, the function returns FALSE, which indicates that
  the NV is not built and it may not be accessed from an external task. If the
  numbers are the same, the function returns TRUE, which indicates that the NV
  is built and may be accessed. This function lets other units issue direct NV
  read and write commands if the status returned is TRUE, thus allowing NV read
  and write before the NV task has been started.
 
@return
  TRUE  -- NV has been built and direct read/write is allowed.\n
  FALSE -- NV has not been built and access is not allowed.

@dependencies
  This is a special use function, normally called by error services to allow
  early access to the NV before the NV task has been started. If NV_BUILT
  returns TRUE, the NV may be accessed, BUT ONLY USING DIRECT READS AND WRITES
  OF THE ERROR LOG (NV_READ_ERR_LOG and NV_WRITE_ERR_LOG). The NV task cannot
  be counted on to have been started yet, and normal NV requests must not be
  used. As with all direct read and write operations, the data item (the major
  version number in this case) must be contained on a single physical NV page.
*/
extern boolean nv_built (void);

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_built */

/**
  Returns the prl_version indicator from the specified NAM's roaming list.

@param[in] nam NAM roaming list for which to check the prl_version field.

@return
  prl_version            -- Roaming list for the specified NAM is currently
                            ACTIVE.\n
  NV_PRL_VERSION_INVALID -- Specified NAM is invalid or the roaming list
                            for the specified NAM is currently NOT_ACTIVE.

@dependencies
  None.
*/
word  
nv_prl_version (
  byte  nam
);

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_prl_version */

/**
  Determines if the current ESN has been programmed and if the checksum is
  is correct.

@return
  TRUE  -- Current ESN is nonzero and has the proper checksum.\n
  FALSE -- Current ESN is zero or the checksum is incorrect.

@dependencies
  None.
*/
boolean nv_determine_esn_validity( void);

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_determine_esn_validity */

/**
  Interfaces with the NVM task.

@param[in,out] cmd Command to be executed.
@param[in] item Item with which to interface.
@param[in,out] data_ptr Pointer to the data that is to be read/written.

@return
  NV_DONE_S      -- Request was completed.\n
  NV_BUSY_S      -- Request is queued.\n
  NV_BADCMD_S    -- Unrecognizable command field.\n
  NV_FULL_S      -- NVM is full.\n
  NV_FAIL_S      -- Command failed for a reason other than NVM full.\n
  NV_NOTACTIVE_S -- Variable was not active.\n
  NV_BADPARM_S   -- Bad parameter in the command block.\n
  NV_READONLY_S  -- Parameter is write-protected and thus read-only.\n
  NV_BADTG_S     -- Item is not valid for this target.\n
  NV_NOMEM_S     -- Free memory has been exhausted.\n
  NV_NOTALLOC_S  -- Address is not a valid allocation.

@dependencies
  The NV task must have been started already. All of the items in nv_cmd_type
  must already be set.

@sideeffects
  nv_cmd_type is placed in the NV command queue. It must not be modified until
  the command has been processed.
*/
nv_stat_enum_type nv_cmd_remote 
( 
  nv_func_enum_type cmd, 
  nv_items_enum_type item, 
  nv_item_type *data_ptr 
); 
/*~ FUNCTION nv_cmd_remote */ 
/*~ PARAM INOUT data_ptr POINTER DISC item */ 

/**
  Interfaces with the NVM task.

@param[in,out] cmd Command to be executed.
@param[in] item Item with which to interface.
@param[in,out] data_ptr Pointer to the data that is to be read/written.

@return
  NV_DONE_S      -- Request was completed.\n
  NV_BUSY_S      -- Request is queued.\n
  NV_BADCMD_S    -- Unrecognizable command field.\n
  NV_FULL_S      -- NVM is full.\n
  NV_FAIL_S      -- Command failed for a reason other than NVM full.\n
  NV_NOTACTIVE_S -- Variable was not active.\n
  NV_BADPARM_S   -- Bad parameter in the command block.\n
  NV_READONLY_S  -- Parameter is write-protected and thus read-only.\n
  NV_BADTG_S     -- Item is not valid for this target.\n
  NV_NOMEM_S     -- Free memory has been exhausted.\n
  NV_NOTALLOC_S  -- Address is not a valid allocation.

@dependencies
  The NV task must have been started already. All of the items in
  nv_cmd_ext_type must already be set.

@sideeffects
  nv_cmd_ext_type is placed in the NV command queue. It must not be modified
  until the command has been processed.
*/
nv_stat_enum_type nv_cmd_ext_remote
( 
  nv_func_enum_type cmd, 
  nv_items_enum_type item, 
  nv_item_type *data_ptr,
  uint16 context 
); 
/*~ FUNCTION nv_cmd_ext_remote */ 
/*~ PARAM INOUT data_ptr POINTER DISC item */ 

/**
  Provides the current RTRE control value. The control value determines whether
  to use the R-UIM in an operation.

@return
  The RTRE control value.

@dependencies
  The return value has a default setting and is updated when the NV receives a
  NV_RTRE_OP_CONFIG_F command.

@sideeffects
  The return value is based on the RTRE value at the instant the function is
  called and does not update if the RTRE control value changes.
*/
nv_rtre_control_type nv_rtre_control( void );

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_rtre_control */

/**
  Determines if the R-UIM is to be used (based on RTRE control) and if the NAM
  is the R-UIM NAM.

@param[in] nam NAM to be queried.

@return
  TRUE -- R-UIM is to be used and the NAM passed in matches the R-UIM NAM.\n
  FALSE -- Otherwise.

@dependencies
  The return value has a default setting and is updated when the NV receives a
  NV_RTRE_OP_CONFIG_F command.

@sideeffects
  The return value is based on the RTRE value at the instant the function is
  called and does not update if the RTRE control value changes.
*/
boolean nv_rtre_use_ruim_for_nam ( byte nam );

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_rtre_use_ruim_for_nam */

/**
  Provides the current RTRE polling control value. The polling control
  indicates whether to poll the UIM card when originating a call or while
  on the traffic channel.
 
@return
  The RTRE polling control value.

@dependencies
  The return value has a default setting and is updated when the NV receives a
  NV_RTRE_OP_CONFIG_F command.

@sideeffects
  The return value is based on the RTRE value at the instant the function is
  called and does not update if the RTRE control value changes.
*/
nv_rtre_polling_control_type nv_rtre_polling_control( void );

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_rtre_polling_control */

/**
  Provides the current RTRE polling control value when the NAM is the R_UIM
  NAM. The polling control value indicates whether to poll the UIM card when
  originating a call or while on the traffic channel.
 
@param[in] nam NAM to be queried.

@return
  TRUE -- The RTRE polling control indicates to poll and the NAM matches the
  R-UIM NAM.\n
  FALSE -- Otherwise.

@dependencies
  The return value has a default setting and is updated when the NV receives a
  NV_RTRE_OP_CONFIG_F command.

@sideeffects
  The return value is based on the RTRE value at the instant the function is
  called and does not update if the RTRE control value changes.
*/
boolean nv_rtre_polling_for_nam ( byte nam );

/*These are HTORPC Meta comments. Do not delete this*/
/*~ FUNCTION nv_rtre_polling_for_nam */

/**
  Allows RUIM to register its callback with NV.

@param[in] : function pointer to register with NV.

@return
  None

@dependencies
  None

@sideeffects
  None
===========================================================================*/
void nv_ruim_register(nv_ruim_register_cb_type register_fn);

/**
  Allows RUIM to update the prl version and other params.

@param[in] : nv_cmd_type pointer of the command

@return
  None

@dependencies
  None

@sideeffects
  None
===========================================================================*/
nv_stat_enum_type nv_update_prl_version_data(nv_cmd_type* cmd_ptr);

/**
  function to be used to register an API from 1X-CP.

@return
  TRUE  -- if registration is successfull.\n

@dependencies
  None.
*/
boolean nv_mc_init_fp(
   nv_1x_cp_cb_type * fp, 
   uint32 mcc_pesn_prefix_mask,
   uint32 mcc_pesn_prefix
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */ /* end_addtogroup services_nv */

#endif /* NV_H */
