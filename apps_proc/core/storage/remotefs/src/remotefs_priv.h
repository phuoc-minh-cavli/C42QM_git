#ifndef REMOTEFS_PRIV_H
#define REMOTEFS_PRIV_H
/*===================================================================================*//**

                   Remote FS Server Private Header File

DESCRIPTION
   This is the private header file for the remote FS service 

Copyright (c) 2009-2013 by Qualcomm Technologies Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary

=========================================================================================*/


/*=======================================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/remotefs/src/remotefs_priv.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $ 


YYYY-MM-DD       who     what, where, why
--------------   ---     ----------------------------------------------------------
2011-03-25       vj      Changed the shared_ram_addr member to 64Bit for QMI
2011-02-18       vj      Added QMI support
2011-02-21       vj      Support for multiple remotefs users
2010-03-10      yog      Added define for RMFS_MAX_NAME_LEN.
2010-01-27      yog      Implemented the callback server on Modem.
2009-06-09       rh      Add open by friendly name feature
2009-03-30       rh      Initial creation
=========================================================================================*/

/*=======================================================================================
                  Include Files
=========================================================================================*/
#ifdef FEATURE_REMOTEFS_USES_QMI

//#include "qmi.h"

#include "remotefs_cli_api.h"

#ifndef FEATURE_REMOTEFS_OFF_TARGET
#include "rex.h"
#include "task.h"
#endif
#else
#include "fs_hotplug.h"
#include "remote_storage.h"
#endif /* FEATURE_REMOTEFS_USES_QMI */

/*=======================================================================================
 
                  Data structure and definitions

=========================================================================================*/
#define RMFS_CHECK_WORD             0x4C4F5645           /* Check for inuse structure    */
#define RMFS_MAX_CLIENT_NUM         10                   /* Number of client available   */
#define RMFS_TEST_PATTERN_SIZE      16                   /* Size of the test pattern     */
#define RMFS_MAX_IOVEC_XFR_CNT      16                   /* Maximum number of IOVEC xfer */
#define RMFS_MAX_NAME_LEN           50                   /* Maximum length of file path  */

/* Statistics gathering */
#define NUM_BUCKETS 8
#define NUM_WINDOWS 3

#define WINDOW_10_MINS  (10*60*1000) //number of milliseconds
#define WINDOW_1_HOUR   (WINDOW_10_MINS * 6)
#define WINDOW_1_WEEK   (WINDOW_1_HOUR * 24 * 7)

typedef struct
{
   /* cumulative totals */
   uint32 total_num_of_reads;
   uint32 total_num_of_writes;
   uint32 total_sectors_written;
   uint32 total_sectors_read;
   
   /* short and long duration averages */
   uint32 write_counter[NUM_WINDOWS][NUM_BUCKETS];
   uint32 read_counter[NUM_WINDOWS][NUM_BUCKETS];
   /* 
   This is a circular list that tracks how often a certain function was called in the recent past.
   Everytime the function is called, we read system time, and if duration "time window" has not yet
   elapsed, the current bucket is incremented. If it has elapsed, the next bucket is incremented.
   When the last bucket is reached, we start again at the first bucket.
   e.g At time = 0
   [0][0][0]...[0]   // Consecutive entries in a given row are adjacent 10 minute windows
   [0][0][0]...[0]   // The next row holds stats for the next larger time window
   [0][0][0]...[0]   

   e.g. If 3 syncs happen betwen time = 0 and time = 10 minutes. Another sync happens at t = 11 minutes
   [3][1][0]...[0]
   [4][0][0]...[0]
   [4][0][0]...[0]

   8 days and many syncs later,
   [8] [1] [7]... [6]    // The 1 here indicates that the current 5 min window is still filling up.
   [50][40][45]...[4]
   [200][4][0]....[0]
   */
   uint32 current_write_bucket_index[NUM_WINDOWS];
   uint32 current_read_bucket_index[NUM_WINDOWS];
   uint64 read_start_time[NUM_WINDOWS];
   uint64 write_start_time[NUM_WINDOWS];
}rmt_stats_t;

#if defined(FEATURE_REMOTEFS_USES_QMI)

#define REMOTEFS_MAX_ENTRIES        15

/**
 * \struct remotefs_client_type
 * \brief A internal structure used to record all the information required for the
 *        client
 */
struct remotefs_client_type
{
   boolean                       client_init_done;
                                                /**< Client Init done flag   */
   qmi_idl_service_object_type   service_object;
                                                /**< Object of the server to connect */
   qmi_cci_os_signal_type        os_params;     /**< Signal on which to wait on */
   qmi_client_type               notifier;      /**< client notifier */
   uint32                        num_services;  /**< Number of Services */
   uint32                        num_entries;   /**< Number of entries in service info */
   qmi_service_info              service_info[REMOTEFS_MAX_ENTRIES]; 
                                                /**< service info */
   boolean                       crit_sec_initialized;
                                                /**< Indicates critical section initialized or not */
   rex_crit_sect_type            crit_sect; 
                                                /**< Critical section to protect the data structures */
};

/**
 * \struct remotefs_handle_type
 * \brief A internal structure used to record all the information required for the
 *        caller of the remotefs
 */
struct remotefs_handle_type
{
   char                           path_name[RMFS_MAX_NAME_LEN];
                                                      /**< Path Name of the caller */
   uint32                         caller_id;          /**< Client Id from server */
   remotefs_event_func_ptr_type   write_callback;     /**< Caller registered Callback */
   uint32                         callback_data;      /**< Caller registered Callback data */
   uint64                         shared_ram_addr;    /**< Shared RAM address */
   uint32                         shared_ram_size;    /**< Shared RAM size */
   boolean                        alloc_done;         /**< Flag indicates alloc done */
   boolean                        handle_in_use;      /**< Flag indicates handle availability */
   boolean                        force_sync;         /**< Flag indicates force sync to be done */
   qmi_cci_os_signal_type         os_params;          /**< Signal on which to wait on */
   qmi_client_type                client;             /**< client object returned by server */
   rex_tcb_type                   *caller_tcb;        /**< TCB of calling task > */
   rex_sigs_type                  caller_sig;         /**< Signal to wait on > */
   qmi_client_type                temp_client;        /**< client object returned by server > */
   boolean                        temp_client_init_done; /**< flag inidcates temp client init done > */
   rmt_stats_t                    stats;
};
#else
/**
 * \struct remotefs_client
 * \brief A internal structure used to record all the information required for the
 *        transaction
 */
typedef struct remotefs_client
{
   uint32   check_word;                               /**< For checking unused element   */
   remotefs_iovec_desc_type xfer_desc[RMFS_MAX_IOVEC_XFR_CNT];                
                                                      /**< For single sector transfer    */
   uint32   iovec_xfer_cnt;                           /**< Number of transfer described  */
   struct   hotplug_device *hdev;                     /**< Device handle                 */
   boolean  is_transferring;                          /**< Flag to mark onging transfer  */
   remotefs_status_type last_op_stat;                 /**< Status of the last operation  */
   int      fs_dev_stat;                              /**< Status of the fs operation    */
   boolean  is_userdata;                              /**< Flag to check is user_data present*/
   uint32   cb_user_data;                             /**< User data for callback        */
   uint8    test_data[RMFS_TEST_PATTERN_SIZE];        /**< For internal testing only     */                       
} remotefs_client_type;

/**
 * Enumerated data type representing the EFS callers
 */ 

enum remotefs_caller_id
{
   REMOTEFS_FS1 = 1,  
   REMOTEFS_FS2,
   REMOTEFS_FSG,
   REMOTEFS_OTHER
};

/**
 * \struct remotefs_caller_handle
 * \brief A internal structure used to store all the caller information required to 
 *        perform a remotefs operation by the server
 */
struct remotefs_caller_handle
{
   boolean handle_in_use;                /** For checking unused handle                    */ 
   boolean rmt_alloc_done;               /** To indicate if remote shared RAM is allocated */ 
   uint32 shared_ram_size;               /** Size of the shared RAM allocated              */  
   enum remotefs_caller_id caller_id;    /** Identifies the caller of remote EFS           */ 
   void *handle;                         /** Stores the actual handle (from open) for the client*/ 
   byte *local_buf;                      /** To store the local buffer addr for EFS clients */
   byte *remote_buf;                     /** To store the remote buffer addr for EFS clients*/
   remotefs_event_func_ptr_type write_callback;  /** Points to Write-callback function      */   
   uint32 user_data;                      /** Userdata for callback functions               */ 
   rmt_stats_t stats;
};
#endif /* FEATURE_REMOTEFS_USES_QMI */

/*=======================================================================================
 
                  Function Prototypes

=========================================================================================*/

/*===================================================================================*//**
 *
 *    remotefs_test_start - Internal test script to test the remotefs server
 *
 * PARAMETERS
 *    None
 *
 * RETURN VALUE
 *    \return None
 *
 *//*===================================================================================*/
void remotefs_test_start(void);

/*===================================================================================*//**
 *
 *    remotefs_test_delay_start - Internal test script to test the remotefs server
 *    Use timer to delay starting time
 *
 * PARAMETERS
 *    None
 *
 * RETURN VALUE
 *    \return None
 *
 *//*===================================================================================*/
void remotefs_test_delay_start(void);


#endif /* REMOTEFS_PRIV_H */

