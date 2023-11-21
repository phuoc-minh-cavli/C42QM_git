/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.

==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/src/jslave/fd/fd_storage_scsi.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================










************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include <jos_int.h>
#include <jusb_common_int.h>
#include <jusb_core_fd_int.h>
#include "fd_storage.h"
#include <uw_device_mass.h>
#include "fd_storage_statistics.h"
/* HSU addition: SPS */
#if defined (FEATURE_HS_USB_MS_BAM) || defined (FEATURE_HS_USB_USE_MS_LA)
#include "hsu_al_ms_int.h"
#endif /* FEATURE_HS_USB_MS_BAM || FEATURE_HS_USB_USE_MS_LA */
/* End of HSU addition */

#if defined (FEATURE_HS_USB_USE_MS_LA) && (HS_USB_SCSI_BUFFER_SIZE)!=(0x10000)
#error "SCSI_BUFFER_SIZE must be 64KB when \
  FEATURE_HS_USB_USE_MS_LA is defined"
#endif /* FEATURE_HS_USB_USE_MS_LA && HSU_SCSI_BUFFER_SIZE!=64KB*/

#define PAGE_CACHE_SIZE PAGE_SIZE

/* SCSI commands that we recognize */
#define SC_FORMAT_UNIT                  0x04
#define SC_INQUIRY                      0x12
#define SC_MODE_SELECT_6                0x15
#define SC_MODE_SELECT_10               0x55
#define SC_MODE_SENSE_6                 0x1a
#define SC_MODE_SENSE_10                0x5a
#define SC_PREVENT_ALLOW_MEDIUM_REMOVAL 0x1e
#define SC_READ_6                       0x08
#define SC_READ_10                      0x28
#define SC_READ_12                      0xa8
#define SC_READ_CAPACITY                0x25
#define SC_READ_FORMAT_CAPACITIES       0x23
#define SC_RELEASE                      0x17
#define SC_REQUEST_SENSE                0x03
#define SC_RESERVE                      0x16
#define SC_SEND_DIAGNOSTIC              0x1d
#define SC_START_STOP_UNIT              0x1b
#define SC_SYNCHRONIZE_CACHE            0x35
#define SC_TEST_UNIT_READY              0x00
#define SC_VERIFY                       0x2f
#define SC_WRITE_6                      0x0a
#define SC_WRITE_10                     0x2a
#define SC_WRITE_12                     0xaa
#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
#define SC_TRANSPARENT                  0xff
#endif

/* SCSI Sense Key/Additional Sense Code/ASC Qualifier values */
#define SS_NO_SENSE                             0
#define SS_COMMUNICATION_FAILURE                0x040800
#define SS_INVALID_COMMAND                      0x052000
#define SS_INVALID_FIELD_IN_CDB                 0x052400
#define SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE   0x052100
#define SS_LOGICAL_UNIT_NOT_SUPPORTED           0x052500
#define SS_MEDIUM_NOT_PRESENT                   0x023a00
#define SS_MEDIUM_REMOVAL_PREVENTED             0x055302
#define SS_NOT_READY_TO_READY_TRANSITION        0x062800
#define SS_RESET_OCCURRED                       0x062900
#define SS_SAVING_PARAMETERS_NOT_SUPPORTED      0x053900
#define SS_UNRECOVERED_READ_ERROR               0x031100
#define SS_WRITE_ERROR                          0x030c02
#define SS_WRITE_PROTECTED                      0x072700

/* Define names for the different SCSI commands (only in JDEBUG mode)*/
#ifdef JDEBUG
#define SC_TEST_UNIT_READY_NAME                "TEST UNIT READY"
#define SC_REQUEST_SENSE_NAME                  "REQUEST SENSE"
#define SC_READ_6_NAME                         "READ(6)"
#define SC_READ_10_NAME                        "READ(10)"
#define SC_READ_12_NAME                        "READ(12)"
#define SC_WRITE_6_NAME                        "WRITE(6)"
#define SC_WRITE_10_NAME                       "WRITE(10)"
#define SC_WRITE_12_NAME                       "WRITE(12)"
#define SC_INQUIRY_NAME                        "INQUIRY"
#define SC_MODE_SELECT_6_NAME                  "MODE SELECT(6)"
#define SC_MODE_SELECT_10_NAME                 "MODE SELECT(10)"
#define SC_MODE_SENSE_6_NAME                   "MODE SENSE(6)"
#define SC_MODE_SENSE_10_NAME                  "MODE SENSE(10)"
#define SC_START_STOP_UNIT_NAME                "START-STOP UNIT"
#define SC_PREVENT_ALLOW_MEDIUM_REMOVAL_NAME   "PREVENT-ALLOW MEDIUM REMOVAL"
#define SC_READ_FORMAT_CAPACITIES_NAME         "READ FORMAT CAPACITIES"
#define SC_READ_CAPACITY_NAME                  "READ CAPACITY"
#define SC_VERIFY_NAME                         "VERIFY"
#define SC_SYNCHRONIZE_CACHE_NAME              "SYNCHRONIZE CACHE"
#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
#define SC_TRANSPARENT_NAME                    "TRANSPARENT COMMAND"
#endif
#ifdef CONFIG_FD_MASS_CDROM_SIM
#define SC_CDROM_NAME                          "CDROM"
#endif
#else
#define SC_TEST_UNIT_READY_NAME                ""
#define SC_REQUEST_SENSE_NAME                  ""
#define SC_READ_6_NAME                         ""
#define SC_READ_10_NAME                        ""
#define SC_READ_12_NAME                        ""
#define SC_WRITE_6_NAME                        ""
#define SC_WRITE_10_NAME                       ""
#define SC_WRITE_12_NAME                       ""
#define SC_INQUIRY_NAME                        ""
#define SC_MODE_SELECT_6_NAME                  ""
#define SC_MODE_SELECT_10_NAME                 ""
#define SC_MODE_SENSE_6_NAME                   ""
#define SC_MODE_SENSE_10_NAME                  ""
#define SC_START_STOP_UNIT_NAME                ""
#define SC_PREVENT_ALLOW_MEDIUM_REMOVAL_NAME   ""
#define SC_READ_FORMAT_CAPACITIES_NAME         ""
#define SC_READ_CAPACITY_NAME                  ""
#define SC_VERIFY_NAME                         ""
#define SC_SYNCHRONIZE_CACHE_NAME              ""
#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
#define SC_TRANSPARENT_NAME                    ""
#endif
#ifdef CONFIG_FD_MASS_CDROM_SIM
#define SC_CDROM_NAME                          ""
#endif
#endif

/* SCSI-2 device types mask */
#define SCSI_DEVICE_UNKNOWN     0x1F

#define SK(x)           ((juint8_t) ((x) >> 16)) /* Sense Key byte, etc. */
#define ASC(x)          ((juint8_t) ((x) >> 8))
#define ASCQ(x)         ((juint8_t) (x))

typedef struct scsi_dev scsi_dev_t;

typedef enum {
    DATA_DIR_UNKNOWN = 0,
    DATA_DIR_FROM_HOST,
    DATA_DIR_TO_HOST,
    DATA_DIR_NONE
} data_direction_t;

typedef struct lun {
    juint_t ro                           : 1;
    juint_t removable                    : 1;
    juint_t prevent_medium_removal       : 1;
    juint_t enabled                      : 1;
        
    juint32_t    sense_data;
    juint32_t    sense_data_info;
    juint32_t    num_blocks;
    juint32_t    block_size;
    /* HSU addition: SPS */
#ifdef FEATURE_HS_USB_MS_BAM
    jbool_t      sps_mode;
    juint8_t     in_sps_pipe_id;
    juint8_t     out_sps_pipe_id;
#endif /* FEATURE_HS_USB_MS_BAM*/
    /* End of HSU addition */
} lun_t;

typedef struct {
    juint8_t  lun;
    juint32_t blocks;
    juint32_t lba;
    jbool_t is_read;
} job_t;

typedef enum {
    BUF_DONE,
    BUF_READING,
    BUF_DONE_READ,
    BUF_WRITING,
    BUF_ERROR
} buffer_state_t;

typedef struct job_buf_s {
    juint8_t id;
    job_t job;
    buffer_state_t state;
    scsi_dev_t *scsi;
    juint32_t buf_size;
    jresult_t error;
    request_t *io_request;
#ifdef FEATURE_HS_USB_USE_MS_LA
    jbool_t is_la_buf;
#endif /* FEATURE_HS_USB_USE_MS_LA */
    TAILQ_ENTRY(job_buf_s) next;
} job_buf_t;

typedef struct {
    juint32_t total;
    juint32_t pending;
    juint32_t done;
    juint32_t lba;
    jbool_t is_read;
    jbool_t aborting;
    job_buf_t *usb_job;
    job_buf_t *disk_job;

    TAILQ_HEAD(work_queue_list, job_buf_s) work_queue;
    /* HSU addition: Error handling */
    jbool_t finished;
    /******* End HSU addition **************************/
    /* HSU addition: CR 175102 fix */
    /* 
      This flag is used to disable transfer when the USB cable 
      is disconnected or the USB reset is happened (scsi_disable 
      is called) before transfer is finished.
      The values are:
      0 - transfer is enabled.
      1 - transfer is disabled because USB transfer or DISK transfer is not finished.
      2 - transfer is disabled because USB transfer and DISK transfer are not finished.
    */
    juint32_t disabled;
    /******* End HSU addition **************************/
} transfer_t;

struct scsi_dev {
    jbool_t      phase_error;
    juint8_t     cmnd_size;
    juint8_t     cmnd[MAX_COMMAND_SIZE];
    juint8_t     command_status;
    /*HSU Addition*/
    jbool_t      is_cmd_handled_by_app;
    /*End of HSU addition*/
    juint32_t    data_size;
    juint32_t    data_size_from_cmnd;
    juint32_t    tag;
    juint32_t    residue;
    juint8_t     lun;
    data_direction_t data_dir;

    juint8_t nluns;
    lun_t *luns;
    lun_t *curlun;

    /* Function Driver using this SCSI device */
    fd_storage_t *fd;

    msfd_appctx_t appctx;
    msfd_ops_t msfd_ops;

    job_buf_t buffers[SCSI_TRANSFER_BUFFERS];
    transfer_t transfer;

#ifdef FEATURE_HS_USB_USE_MS_LA
    /* We use one 64K buffer for a transfer: la_buffer[0] is dedicated to next_transfer[0]
    and la_buffer[1] is dedicated to next_transfer[1] */
    job_buf_t la_buffer[SCSI_LOOKAHEAD_BUFFERS];
    transfer_t next_transfer[SCSI_LOOKAHEAD_BUFFERS];
    jbool_t is_la_transfer_pending;
    /* The current lookahead buffer array index 
    that is being used */
    juint8_t la_transfer_idx;
#endif /* FEATURE_HS_USB_USE_MS_LA */
}; 

typedef jresult_t (*scsi_command_handler)(scsi_dev_t *, juint8_t *);
typedef juint32_t (*calc_cmd_data_size)(lun_t *curlun, juint8_t *cmd, 
    juint8_t cmd_size);

typedef struct {
    char                        *name;
    juint8_t                     cmd_size;
    juint32_t                    valid_fields;
    short                       needs_medium;
    data_direction_t            data_dir;
    scsi_command_handler        handler;
    calc_cmd_data_size          get_size_from_header;

    /* Used if data size from command is constant and does
     * not depend on the SCSI command header */
    juint32_t                    data_from_cmd; 
} scsi_command_data;

static boolean is_statistics_update_supported = FALSE;
static io_disk_statistics_type hsu_ms_disk_statistics;
static io_disk_statistics_type hsu_ms_usb_statistics;

#ifdef FEATURE_HS_USB_USE_MS_LA
/*statistics*/
typedef struct {
  juint32_t all_read_req;
  juint32_t found_right_away;
  juint32_t found_and_not_ready;
  juint32_t didnt_find_at_all;
} ms_la_statistics_type;

typedef enum {
  BUF_WAS_NOT_FOUND = -1,
  BUF_NOT_REAY= -2
} la_search_res;

#define MAX_NO_OF_LA_BUFFERS 2

static ms_la_statistics_type la_statistics;
static jint_t find_current_request(scsi_dev_t *scsi);
#endif /* FEATURE_HS_USB_USE_MS_LA */

static jresult_t scsi_read(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_write(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_synchronize_cache(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_verify(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_inquiry(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_request_sense(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_read_capacity(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_mode_sense(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_start_stop(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_test_unit_ready(scsi_dev_t *scsi, juint8_t *buffer);
static jresult_t scsi_prevent_allow(scsi_dev_t *scsi, juint8_t *buffer);
#ifndef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
static jresult_t scsi_read_format_capacities(scsi_dev_t *scsi, juint8_t *buffer);
#endif
static jresult_t scsi_mode_select(scsi_dev_t *scsi, juint8_t *buffer);
#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
static jresult_t scsi_transparent(scsi_dev_t *scsi, juint8_t *buffer);
#endif
#ifdef CONFIG_FD_MASS_CDROM_SIM
static jresult_t scsi_cdrom_command(scsi_dev_t *scsi, juint8_t *buffer);
#endif
static void send_error_reply(scsi_dev_t *scsi);

/**
 * Function name:  calc_expected_data_size
 * Description:    Extracts the expected data size from the SCSI command
 *                 header.
 * Parameters: 
 *     @curlun    (IN) Current LUN structure
 *     @cmd:      (IN) SCSI command buffer
 *     @cmd_size: (IN) Command size, in bytes
 *
 * Return value: The expected data size, in bytes
 * Scope:        Local
 **/
static juint32_t calc_expected_data_size(lun_t *curlun, juint8_t *cmd,
    juint8_t cmd_size)
{
    switch (cmd_size)
    {
    case 6:
        return cmd[4];
    
    case 10:
        return be16toh_t(&cmd[7]);
    
    case 12:
        return be32toh_t(&cmd[6]);
    };

    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MS_SCSI, invalid_cmd_size");    
    return 0;
}

/**
 * Function name:  calc_expected_data_size_with_offset
 * Description:    Extracts the expected data size with offset from the SCSI
 *                 command header.
 * Parameters: 
 *     @curlun    (IN) Current LUN structure
 *     @cmd:      (IN) SCSI command buffer
 *     @cmd_size: (IN) Command size, in bytes
 *
 * Return value: The expected data size, in bytes
 * Scope:        Local
 **/
static juint32_t calc_expected_data_size_with_offset(lun_t *curlun, juint8_t *cmd,
    juint8_t cmd_size)
{
    switch (cmd_size)
    {
    case 6:
        return ((cmd[4] == 0 ? 256 : cmd[4]) * curlun->block_size);
    
    case 10:
        return (be16toh_t(&cmd[7]) * curlun->block_size);
    
    case 12:
        return (be32toh_t(&cmd[6]) * curlun->block_size);
    }   

    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MS_SCSI, invalid_cmd_size");    
    return 0;
}

static scsi_command_data cmd_data_test_unit_ready = { 
    SC_TEST_UNIT_READY_NAME,
    6,
    0,
    0,
    DATA_DIR_NONE,
    scsi_test_unit_ready,
    0,
    0
};

static scsi_command_data cmd_data_request_sense = {
    SC_REQUEST_SENSE_NAME,
    6,
    (1<<4),
    0,
    DATA_DIR_TO_HOST,
    scsi_request_sense,
    calc_expected_data_size
};

#ifndef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
static scsi_command_data cmd_data_read_6 = {
    SC_READ_6_NAME,
    6,
    (7<<1) | (1<<4),
    1,
    DATA_DIR_TO_HOST,
    scsi_read,
    calc_expected_data_size_with_offset
};
#endif

static scsi_command_data cmd_data_read_10 = {
    SC_READ_10_NAME,
    10,
    (1<<1) | (0xf<<2) | (3<<7),
    1,
    DATA_DIR_TO_HOST,
    scsi_read,
    calc_expected_data_size_with_offset
};

#ifndef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
static scsi_command_data cmd_data_read_12 = {
    SC_READ_12_NAME,
    12,
    (1<<1) | (0xf<<2) | (0xf<<6),
    1,
    DATA_DIR_TO_HOST,
    scsi_read,
    calc_expected_data_size_with_offset
};
#endif

#ifndef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
static scsi_command_data cmd_data_write_6 = {
    SC_WRITE_6_NAME,
    6,
    (7<<1) | (1<<4),
    1,
    DATA_DIR_FROM_HOST,
    scsi_write,
    calc_expected_data_size_with_offset
};
#endif

static scsi_command_data cmd_data_write_10 = {
    SC_WRITE_10_NAME,
    10,
    (1<<1) | (0xf<<2) | (3<<7),
    1,
    DATA_DIR_FROM_HOST,
    scsi_write,
    calc_expected_data_size_with_offset
};

#ifndef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
static scsi_command_data cmd_data_write_12 = {
    SC_WRITE_12_NAME,
    12,
    (1<<1) | (0xf<<2) | (0xf<<6),
    1,
    DATA_DIR_FROM_HOST,
    scsi_write,
    calc_expected_data_size_with_offset
};
#endif

#define INQUIRY_RESPONSE_LEN 36
static scsi_command_data cmd_data_inquiry = { 
    SC_INQUIRY_NAME,
    6,
    (1<<4),
    0,
    DATA_DIR_TO_HOST,
    scsi_inquiry,
    calc_expected_data_size
};
    
static scsi_command_data cmd_data_mode_select_6 = {
    SC_MODE_SELECT_6_NAME,
    6,
    (1<<1) | (1<<4),
    0,
    DATA_DIR_FROM_HOST,
    scsi_mode_select,
    calc_expected_data_size
};

#ifndef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
static scsi_command_data cmd_data_mode_select_10 = {
    SC_MODE_SELECT_10_NAME,
    10,
    (1<<1) | (3<<7),
    0,
    DATA_DIR_FROM_HOST,
    scsi_mode_select,
    calc_expected_data_size
};
#endif

static scsi_command_data cmd_data_mode_sense_6 = {
    SC_MODE_SENSE_6_NAME,
    6,
    (1<<1) | (1<<2) | (1<<4),
    0,
    DATA_DIR_TO_HOST,
    scsi_mode_sense,
    calc_expected_data_size
};

#ifndef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
static scsi_command_data cmd_data_mode_sense_10 = {
    SC_MODE_SENSE_10_NAME,
    10,
    (1<<1) | (1<<2) | (3<<7),
    0,
    DATA_DIR_TO_HOST,
    scsi_mode_sense,
    calc_expected_data_size
};
#endif

static scsi_command_data cmd_data_start_stop = {
    SC_START_STOP_UNIT_NAME,
    6,
    (1<<1) | (1<<4),
    0,
    DATA_DIR_NONE,
    scsi_start_stop,
    0,
    0
};

static scsi_command_data cmd_data_prevent_allow = {
    SC_PREVENT_ALLOW_MEDIUM_REMOVAL_NAME,
    6,
    (1<<4),
    0,
    DATA_DIR_NONE,
    scsi_prevent_allow,
    0,
    0
};

#ifndef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
static scsi_command_data cmd_data_read_format_capacities = {
    SC_READ_FORMAT_CAPACITIES_NAME,
    10,
    (3<<7),
    1,
    DATA_DIR_TO_HOST,
    scsi_read_format_capacities,
    calc_expected_data_size
};
#endif

static scsi_command_data cmd_data_read_capacity = {
    SC_READ_CAPACITY_NAME,
    10,
    (0xf<<2) | (1<<8), 
    1,
    DATA_DIR_TO_HOST,
    scsi_read_capacity,
    0,
    8
};

static scsi_command_data cmd_data_verify = {
    SC_VERIFY_NAME,
    10,
    (1<<1) | (0xf<<2) | (3<<7),
    1,
    DATA_DIR_NONE,
    scsi_verify,
    0,
    0
};

static scsi_command_data cmd_data_sync_cache = {
    SC_SYNCHRONIZE_CACHE_NAME,
    10,
    (0xf<<2) | (3<<7),
    1,
    DATA_DIR_NONE,
    scsi_synchronize_cache,
    0,
    0
};

#ifdef CONFIG_FD_MASS_CDROM_SIM
static scsi_command_data cmd_cdrom = {
    SC_CDROM_NAME,
    10,
    0xffff,
    0,
    DATA_DIR_TO_HOST,
    scsi_cdrom_command,
    calc_expected_data_size,
    0
};
#define CMD_CDROM &cmd_cdrom
#else
#define CMD_CDROM 0
#endif

#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
static scsi_command_data cmd_data_transparent = {
    SC_TRANSPARENT_NAME,
    10,
    (0xf<<2) | (3<<7),
    1,
    DATA_DIR_NONE,
    scsi_transparent,
    0,
    0
};
#endif

/* This structure lists all the command-specific data for the
 * SCSI commands that we support */
#ifdef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
static scsi_command_data *cmd_data_vector[256] = 
{
/* 0x00 - 0x0f */ &cmd_data_test_unit_ready, 0, 0, &cmd_data_request_sense,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x10 - 0x1f */ 0, 0, &cmd_data_inquiry, 0, 
                  0, &cmd_data_mode_select_6, 0, 0,
                  0, 0, &cmd_data_mode_sense_6, &cmd_data_start_stop, 
                  0, 0, &cmd_data_prevent_allow, 0,
/* 0x20 - 0x2f */ 0, 0, 0, 0, 0, &cmd_data_read_capacity, 0, 0,
                  &cmd_data_read_10, 0, &cmd_data_write_10, 0, 
                  0, 0, 0, &cmd_data_verify,
/* 0x30 - 0x3f */ 0, 0, 0, 0, 0, &cmd_data_sync_cache, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0,
/* 0x40 - 0x4f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0x50 - 0x5f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0x60 - 0x6f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0x70 - 0x7f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0x80 - 0x8f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0x90 - 0x9f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xa0 - 0xaf */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xb0 - 0xbf */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xc0 - 0xcf */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xd0 - 0xdf */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xe0 - 0xef */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xf0 - 0xff */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 
#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
&cmd_data_transparent
#else
0
#endif
};
#else
static scsi_command_data *cmd_data_vector[256] = 
{
/* 0x00 - 0x0f */ &cmd_data_test_unit_ready, 0, 0, &cmd_data_request_sense,
                  0, 0, 0, 0,
                  &cmd_data_read_6, 0, &cmd_data_write_6, 0, 0, 0, 0, 0,
/* 0x10 - 0x1f */ 0, 0, &cmd_data_inquiry, 0, 
                  0, &cmd_data_mode_select_6, 0, 0,
                  0, 0, &cmd_data_mode_sense_6, &cmd_data_start_stop, 
                  0, 0, &cmd_data_prevent_allow, 0,
/* 0x20 - 0x2f */ 0, 0, 0, &cmd_data_read_format_capacities, 
                  0, &cmd_data_read_capacity, 0, 0,
                  &cmd_data_read_10, 0, &cmd_data_write_10, 0, 
                  0, 0, 0, &cmd_data_verify,
/* 0x30 - 0x3f */ 0, 0, 0, 0, 0, &cmd_data_sync_cache, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0,
/* 0x40 - 0x4f */ 0, 0, CMD_CDROM, CMD_CDROM, 0, 0, CMD_CDROM, 0,  
                  0, 0, CMD_CDROM, 0, 0, 0, 0, 0,
/* 0x50 - 0x5f */ 0, CMD_CDROM, 0, 0, 0, &cmd_data_mode_select_10, 0, 0,
                  0, 0, &cmd_data_mode_sense_10, 0, 0, 0, 0, 0,
/* 0x60 - 0x6f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0x70 - 0x7f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0x80 - 0x8f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0x90 - 0x9f */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xa0 - 0xaf */ 0, 0, 0, 0, 0, 0, 0, 0,
                  &cmd_data_read_12, 0, &cmd_data_write_12, 0,
                  0, 0, 0, 0,
/* 0xb0 - 0xbf */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xc0 - 0xcf */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xd0 - 0xdf */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xe0 - 0xef */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 0,
/* 0xf0 - 0xff */ 0, 0, 0, 0, 0, 0, 0, 0,       0, 0, 0, 0, 0, 0, 0, 
#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
&cmd_data_transparent
#else
0
#endif
};
#endif

/**
 * Function name:  scsi_write_data
 * Description:    Writes data to the host.
 * Parameters: 
 *     @scsi:     (IN)  SCSI device context
 *     @amount:   (IN)  Number of sectors
 *     @buf_id:   (IN)  Buffer index (zero based) for handling double buffering
 *     @callback: (IN)  Completion callback
 *     @status:   (IN)  Command status
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static __INLINE__ jresult_t scsi_write_data(scsi_dev_t *scsi, 
    juint32_t amount, juint8_t buf_id, callback_t callback, juint8_t status)
{
    juint32_t len;

    scsi->command_status = status;
    
    if (!command_in_progress(scsi->fd))
        return JEPERM;

    len = MIN(amount, scsi->data_size);
    len = MIN(len, scsi->data_size_from_cmnd);

    if (amount != len)
    {
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Reply truncated %d to [%d:%d]\n",
            amount, scsi->data_size, scsi->data_size_from_cmnd));
    }

    if (len > SCSI_MAX_REQUEST_SIZE)
    {
        HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "MS_SCSI, write_req_larger_then_buffer %u > %u",
          len, SCSI_MAX_REQUEST_SIZE);        
        return JEINVAL;
    }

    /* HSU addition: Error handling */
    if (len == 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MS_SCSI, zero_data_length");
        scsi->curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }
    /******* End HSU addition **************************/

    DBG_X(DSLAVE_MS_SCSI, ("SCSI: Write data %d len %d lun %d\n",
        amount, len, scsi->lun));
    
    return post_write(scsi->fd, scsi->fd->cmd_request, len, 
        callback, scsi);
}

/**
 * Function name:  scsi_read_data
 * Description:    Reads data from the host.
 * Parameters: 
 *     @scsi:     (IN)  SCSI device context 
 *     @amount:   (IN)  Number of sectors
 *     @buf_id:   (IN)  Buffer index (zero based) for handling double buffering
 *     @callback: (IN)  Completion callback
 *     @status:   (IN)  Command status
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static __INLINE__ jresult_t scsi_read_data(scsi_dev_t *scsi, 
    juint32_t amount, juint8_t buf_id, callback_t callback, juint8_t status)
{
    juint32_t len;

    if (!command_in_progress(scsi->fd))
        return JEPERM;

    scsi->command_status = status;

    len = MIN(amount, scsi->data_size);
    len = MIN(len, scsi->data_size_from_cmnd);
    
    if (len > SCSI_MAX_REQUEST_SIZE)
    {
        HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "MS_SCSI, read_req_larger_then_buffer %u > %u",
          len, SCSI_MAX_REQUEST_SIZE);         
        return JEINVAL;
    }

    /* HSU addition: Error handling */
    if (len == 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MS_SCSI, zero_data_length");        
        scsi->curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }
    /******* End HSU addition **************************/

    return post_read(scsi->fd, scsi->fd->cmd_request, len, 
        callback, scsi);
}

/**
 * Function name:  scsi_standard_callback
 * Description:    Completion callback for a read/write SCSI command
 *                 (scsi_read_data/scsi_write_data).
 * Parameters: 
 *     @request: (IN) Core request
 *
 * Return value: None
 * Scope:        Local
 **/
static void scsi_standard_callback(request_t *request)
{
    scsi_dev_t *scsi = (scsi_dev_t *)request->context;
    request_status_t status = request->status;
    
    DBG_V(DSLAVE_MS_SCSI, ("SCSI: Completed SCSI command. "
        "Status: 0x%x [%d out of %d sent] scsi %p\n", 
        status, request->bytes_transferred, request->transfer_size, scsi));

    /* Request is ready for next transfer */
    request->status = REQUEST_READY;

    if (!command_in_progress(scsi->fd))
        return;

    if (status != REQUEST_COMPLETED || 
        request->bytes_transferred != request->transfer_size)
    {
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Error during send %d [%d out of %d]\n", 
            status,  request->bytes_transferred, request->transfer_size));
        fatal_processing_error(scsi->fd);
    }
    else
    {
        scsi->residue -= request->bytes_transferred;

        if (scsi->residue)
        {
            DBG_V(DSLAVE_MS_SCSI, ("SCSI: Command finished with "
                "residue %x [%d]\n", scsi->cmnd[0], scsi->residue));
        }

        send_csw(scsi->fd, scsi->tag, scsi->residue, 
            (scsi->phase_error ? 
            USB_STATUS_PHASE_ERROR : scsi->command_status));
    }
}

/**
 * Function name:  dump_cdb
 * Description:    Dump command data block.
 * Parameters: 
 *     @scsi: (IN) SCSI device context 
 *
 * Return value: None
 * Scope:        Local
 **/
static __INLINE__ void dump_cdb(scsi_dev_t *scsi)
{
    jint_t i;
    char cmdbuf[3 * MAX_COMMAND_SIZE + 1];

    for (i = 0; i < scsi->cmnd_size; ++i)
    {
        j_snprintf(cmdbuf + i * 3, sizeof(cmdbuf) - i*3 - 1, " %02x", 
            scsi->cmnd[i]);
    }
    DBG_X(DSLAVE_MS_SCSI, ("SCSI: CDB dump: [%s].\n", cmdbuf));
}

static jresult_t schedule_advance(scsi_dev_t *scsi);
static void finish_job(scsi_dev_t *scsi, juint16_t size, job_buf_t *buffer);

/* Read/Write transfer handling:
 * - Each SCSI device has one "transfer" object which holds the information for
 * the currently performed transfer.
 * - There are two buffer object that transfer data.
 *   The buffer object request "jobs" from transfer object.
 * 
 * The buffer handling is done in two phases:
 * 1) Read (read data from usb/disk depending on direction of transfer)
 * 2) Write (write data to usb/disk)
 * 
 * On completion a schedule advance function is called to continue.
 */

static void io_done(job_buf_t *buffer, jresult_t status)
{
    transfer_t *transfer = &(buffer->scsi->transfer);
#ifdef FEATURE_HS_USB_USE_MS_LA    
    if(buffer->is_la_buf)
    {
      transfer = &(buffer->scsi->next_transfer[buffer->id]);
    }
#endif /* FEATURE_HS_USB_USE_MS_LA */
    DBG_I(DSLAVE_MS_TRANS, ("[%d] IO_DONE: status %d state %d [%x]\n", 
        buffer->id, status, buffer->state, buffer));

    if (transfer->aborting)
    {
        buffer->state = BUF_ERROR;
        goto Exit;
    }

    if (buffer->state == BUF_ERROR)
        goto Exit;

    if (status)
    {
        buffer->state = BUF_ERROR;
        buffer->error = status;
        /* HSU addition: Error handling */
        transfer->aborting = 1;
        /******* End HSU addition **************************/
        goto Exit;
    }

    if (buffer->state == BUF_READING)
        buffer->state = BUF_DONE_READ;
    else
    {
        buffer->state = BUF_DONE;

        /* HSU addition: Error handling */
#ifndef CONFIG_FAST_CSW
        finish_job(buffer->scsi, buffer->job.blocks, buffer);
#endif
        /******* End HSU addition **************************/
    }

Exit:
    schedule_advance(buffer->scsi);
}

static void usb_done(struct request_t *request)
{
    job_buf_t *buffer = (job_buf_t *)request->context;
    jresult_t rc = request->status == REQUEST_COMPLETED ? 0 : JEIO;
    transfer_t *transfer = &(buffer->scsi->transfer);

#ifdef FEATURE_HS_USB_USE_MS_LA    
    if(buffer->is_la_buf)
    {
      transfer = &(buffer->scsi->next_transfer[buffer->id]);
    }
#endif /* FEATURE_HS_USB_USE_MS_LA */    
    request->status = REQUEST_READY;
    /* HSU addition: CR 175102 fix */
    transfer->usb_job = NULL;

    if (transfer->disabled)
    {
        HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "MS_TRANS, %u USB_DONE, transfer_is_disabled, rc %u",
          buffer->id, rc);        
        transfer->disabled--;
        return;
    }
    if (is_statistics_update_supported)
    {
      if (transfer->is_read)
      { 
        update_statistics_for_usb_done(&hsu_ms_usb_statistics.read, 
          buffer->job.blocks);
      }
      else
      { 
        update_statistics_for_usb_done(&hsu_ms_usb_statistics.write,
          buffer->job.blocks);
      }
    }
    /******* End HSU addition **************************/

    DBG_W(DSLAVE_MS_TRANS, ("[%d] USB_DONE: Status %d\n", 
        buffer->id, request->status));
#ifdef FEATURE_HS_USB_USE_MS_LA  
    if(!buffer->is_la_buf)
#endif /* FEATURE_HS_USB_USE_MS_LA   */
      TAILQ_INSERT_TAIL(&(transfer->work_queue), buffer, next);

    /* HSU deletion: CR 175102 fix, moved up
    buffer->scsi->transfer.usb_job = NULL;

    ** End of HSU deletion */
    /* HSU addition: Error handling */
    buffer->scsi->residue -= request->bytes_transferred;
#ifdef CONFIG_FAST_CSW
    finish_job(buffer->scsi, buffer->job.blocks, buffer);
#endif
    /******* End HSU addition **************************/

    io_done(buffer, rc);
}

static void disk_done(void *context, jbool_t status)
{
    scsi_dev_t *scsi = (scsi_dev_t *)context;
    job_buf_t *buffer = scsi->transfer.disk_job;
#ifdef FEATURE_HS_USB_USE_MS_LA
    job_buf_t *la_disk_job = NULL;
#endif /* FEATURE_HS_USB_USE_MS_LA */
    transfer_t *transfer = &(scsi->transfer);

#ifdef FEATURE_HS_USB_USE_MS_LA
{
/* checking if the disk_done cb was called due to la buffer */   
    juint_t i;
    for(i=0; i<SCSI_LOOKAHEAD_BUFFERS; i++)
    {
      la_disk_job = scsi->next_transfer[i].disk_job;
      if (la_disk_job!=NULL)
      {
        break;
      }
    }
/* If two read requests were sent, the disk_done that was received
   is for a la_buffer */        
    if ((buffer && la_disk_job) || (buffer == NULL))
    {
      buffer = la_disk_job;
      transfer = &(scsi->next_transfer[buffer->id]);
    }
}
#endif /* FEATURE_HS_USB_USE_MS_LA */
    if (is_statistics_update_supported)
    {
      if (transfer->is_read)
      { 
      update_statistics_for_disk_done(&hsu_ms_disk_statistics.read,
          buffer->job.blocks);
      }
      else
      { 
      update_statistics_for_disk_done(&hsu_ms_disk_statistics.write,
          buffer->job.blocks);
      }
    }
    DBG_W(DSLAVE_MS_TRANS, ("[%d] DISK_DONE: [%x]\n", buffer->id, buffer));
    transfer->disk_job = NULL;
    /* HSU addition: CR 175102 fix */
    if (transfer->disabled)
    {
        HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "MS_TRANS, %u DISK_DONE, transfer_is_disabled, status %u",
          buffer->id, status);              
        transfer->disabled--;
        return;
    }
    /******* End HSU addition **************************/

#ifdef FEATURE_HS_USB_USE_MS_LA
 if(!buffer->is_la_buf)
#endif /* FEATURE_HS_USB_USE_MS_LA */
    TAILQ_INSERT_TAIL(&(transfer->work_queue), buffer, next);

    io_done(buffer, status ? 0 : JEIO);
}

#define DO_READ  1
#define DO_WRITE 0
#define DO_DISK  1
#define DO_USB   0

static jresult_t start_io(job_buf_t *buffer, juint32_t size, juint32_t lba,
    jbool_t is_read, jbool_t is_disk)
{
    scsi_dev_t *scsi = buffer->scsi;
    jresult_t rc = JEINVAL;
    transfer_t *transfer = &(scsi->transfer);

#ifdef FEATURE_HS_USB_USE_MS_LA
    if(buffer->is_la_buf)
    {
      transfer =&(scsi->next_transfer[buffer->id]);
      DBG_W(DSLAVE_MS_TRANS, ("start_io(): with LA buffer"));
    }
#endif /* FEATURE_HS_USB_USE_MS_LA */

#ifdef FEATURE_HS_USB_USE_MS_LA
    if(!buffer->is_la_buf)
#endif /* FEATURE_HS_USB_USE_MS_LA */
      TAILQ_REMOVE(&(transfer->work_queue), buffer, next);

    if (is_disk)
        transfer->disk_job = buffer;
    else
        transfer->usb_job = buffer;
    
    if (is_disk)
    {
        if (is_read)
        {
            DBG_W(DSLAVE_MS_TRANS, ("[%d] DISK_READ: Size %d from %d [%x]\n",
                 buffer->id, size, lba, buffer));
            if (is_statistics_update_supported)
            {
              update_statistics_for_disk_job(&hsu_ms_disk_statistics.read, 
                 buffer->job.blocks);
            }
            rc = scsi->msfd_ops.vread(scsi->appctx, buffer->job.lun,
                (juint8_t *)(buffer->io_request->buffer.vaddr), size, lba);
        }
        else
        {
            DBG_W(DSLAVE_MS_TRANS, ("[%d] DISK_WRITE: Size %d from %d [%x]\n",
              buffer->id, size, lba, buffer));
            if (is_statistics_update_supported)
            {
              update_statistics_for_disk_job(&hsu_ms_disk_statistics.write,
                 buffer->job.blocks);
            }
            rc = scsi->msfd_ops.vwrite(scsi->appctx, buffer->job.lun,
                (juint8_t *) (buffer->io_request->buffer.vaddr), size, lba);
        }            
    }
    else
    {
        if (is_read)
        {
            DBG_W(DSLAVE_MS_TRANS, ("[%d] USB_READ: Size %d from %d [%x]\n",
                buffer->id, size, lba, buffer));
            if (is_statistics_update_supported)
            {
              update_statistics_for_usb_job(&hsu_ms_usb_statistics.write, 
                buffer->job.blocks);
            }
            rc = post_read(scsi->fd, buffer->io_request, 
                size * scsi->curlun->block_size, usb_done, buffer);
        }
        else
        {
            DBG_W(DSLAVE_MS_TRANS, ("[%d] USB_WRITE: Size %d from %d [%x]\n",
                buffer->id, size, lba, buffer));
            if (is_statistics_update_supported)
            {
              update_statistics_for_usb_job(&hsu_ms_usb_statistics.read,
                buffer->job.blocks);
            }
            rc = post_write(scsi->fd, buffer->io_request, 
                size * scsi->curlun->block_size, usb_done, buffer);
        }
    }
    
    if (rc)
        goto Error;

    return 0;

Error:

    if (is_disk)
        transfer->disk_job = NULL;
    else
        transfer->usb_job = NULL;
#ifdef FEATURE_HS_USB_USE_MS_LA
    if(!buffer->is_la_buf)
#endif /* FEATURE_HS_USB_USE_MS_LA */
      TAILQ_INSERT_TAIL(&(transfer->work_queue), buffer, next);
 
    return rc;
}

static void get_job(scsi_dev_t *scsi, job_t *job, juint32_t buf_size, job_buf_t *buffer)
{
    juint16_t max_size;
    transfer_t *transfer = &(scsi->transfer);
    
#ifdef FEATURE_HS_USB_USE_MS_LA
    if (buffer->is_la_buf)    
    {
      transfer = &(scsi->next_transfer[buffer->id]);
    }
#endif /* FEATURE_HS_USB_USE_MS_LA */
    /* Are there any jobs to do ? */
    if (transfer->total == transfer->pending)
    {
        job->blocks = 0;
        DBG_W(DSLAVE_MS_TRANS, ("job blocks is 0"));
        return;
    }

    max_size = buf_size / scsi->curlun->block_size;

    DBG_I(DSLAVE_MS_TRANS, ("GET_JOB: Total %d Pending %d "
        "Done %d (max size %d)\n",
        transfer->total, transfer->pending, transfer->done, max_size));

    job->blocks = MIN(max_size, transfer->total - transfer->pending);
    job->lba = transfer->lba + transfer->pending;
    job->is_read = transfer->is_read;
    job->lun = scsi->lun;

    transfer->pending += job->blocks;
}

static void finish_job(scsi_dev_t *scsi, juint16_t size, job_buf_t *buffer)
{
    transfer_t *transfer = &scsi->transfer;

    if (transfer->aborting)
        return;

    DBG_I(DSLAVE_MS_TRANS, ("FINISH_JOB: Total %d Pending %d "
        "Done %d (done now %d : residue %d)\n",
        transfer->total, transfer->pending, transfer->done, 
        transfer->done + size, 
        scsi->residue / scsi->curlun->block_size));

    transfer->done += size;

    if (transfer->done != transfer->total)
        return;

    /* HSU addition: for unhandled SCSI OUT the data size is in bytes according 
      to the SBW and not in blocks*/
    /* HSU deletion: moved to usb_done 
    if((scsi->is_cmd_handled_by_app) && (scsi->data_dir ==  DATA_DIR_FROM_HOST))
    {
      scsi->residue -= transfer->done ;
    }
    else
    {
      scsi->residue -= scsi->curlun->block_size * transfer->done;
    }
    */
    /*End of HSU addition*/

    /* In case the host expects to send/receive more data, need to stop
     * the data stage and send an error */
    if (scsi->residue)
    {
        DBG_W(DSLAVE_MS_TRANS, ("FINISH_JOB: With resudie %d\n",
            scsi->residue));

        send_error_reply(scsi);
    }
    else
    {
        /* HSU addition */
        /* MSC CV Cases 7 and 13 support */
        if (scsi->phase_error)
            DBG_W(DSLAVE_MS_TRANS, ("FINISH_JOB: MSC CV Cases 7 and 13 are not supported\n"));
        else
        /******* End HSU addition **************************/
            DBG_I(DSLAVE_MS_TRANS, ("FINISH_JOB: Success, send CSW\n"));
        send_csw(scsi->fd, scsi->tag, scsi->residue, USB_STATUS_PASS);
    }

    /* HSU addition: Error handling */
    transfer->finished = 1;
    /******* End HSU addition **************************/
}

static jresult_t start_new_job(job_buf_t *buffer)
{
    job_t *job = &(buffer->job);
    jresult_t rc=0;
    
    DBG_I(DSLAVE_MS_TRANS, ("[%d] START_NEW_JOB: [%x]\n", buffer->id, buffer));

    get_job(buffer->scsi, job, buffer->buf_size, buffer);

    if (job->blocks == 0)
        return 0;

    buffer->state = BUF_READING;

    rc = start_io(buffer, job->blocks, job->lba, DO_READ, 
        job->is_read ? DO_DISK : DO_USB);
   
    if (rc)
    {
        DBG_I(DSLAVE_MS_TRANS, ("[%d] START_NEW_JOB: Error %d [%x]\n",
            buffer->id, rc, buffer));

        buffer->state = BUF_ERROR;
        buffer->error = rc;

        return rc;
    }
    
    return 0;
}

#ifdef FEATURE_HS_USB_USE_MS_LA
static jbool_t la_advance_job(job_buf_t *buffer)
{
  uint8 i;
  job_t *job = &(buffer->job);
  job_buf_t *next_la_buffer;
  transfer_t *next_transfer;
  transfer_t *transfer= &(buffer->scsi->transfer);
  scsi_dev_t *scsi = buffer->scsi;

  if(buffer->is_la_buf)
  {
    transfer = &(buffer->scsi->next_transfer[buffer->id]);
  }

  /* check other transfers */
  if (buffer->is_la_buf)
  {
    /*For the case of write req. with disk_write and than new read req. found in lookahead buffer*/
    if (job->is_read && buffer->scsi->transfer.disk_job)
    {
      return FALSE;
    } 
    for (i=0; i<SCSI_LOOKAHEAD_BUFFERS; i++)
    {
      if (job->is_read && i!=buffer->id && buffer->scsi->next_transfer[i].usb_job)
        return FALSE;  
    }
  }

  else
  {
    /*check that there isn't a lookahead usb transfer going on*/
    for (i=0; i<SCSI_LOOKAHEAD_BUFFERS; i++)
    { 
      if (job->is_read && buffer->scsi->next_transfer[i].usb_job)
      {
        return FALSE;
      } 
      /*for the case of la_buff in the middle of disk_read and
        we have new write request after usb_done */
      if (!job->is_read && buffer->scsi->next_transfer[i].disk_job)
      {
        return FALSE;
      } 
    }  
  }

  if (job->is_read)
  {
    if (buffer->is_la_buf)
    {
      /* this is a l_a buffer, and we need to prepare next l_a buffer*/
      scsi->la_transfer_idx = (buffer->id+1)%MAX_NO_OF_LA_BUFFERS;
    }

    next_transfer = &(buffer->scsi->next_transfer[scsi->la_transfer_idx]);
    next_la_buffer = &(buffer->scsi->la_buffer[scsi->la_transfer_idx]);

    /* Prepare next transfer */
    if (next_la_buffer->state!=BUF_READING && next_la_buffer->state!=BUF_WRITING)
    {
      /* check that we are not out of range with the new transfer parameters */
      if (job->lba + (transfer->total)*2 > buffer->scsi->curlun->num_blocks) 
      {
        DBG_W(DSLAVE_MS_SCSI, ("la_advance_job: lba out of range for next transfer\n"));
      }
      else /* Updating next transfer params */
      {
        next_transfer->total    = MIN (buffer->scsi->transfer.total, buffer->buf_size / scsi->curlun->block_size ); 
        /*max number of blocks for read/write operation = buffer->buf_size / scsi->curlun->block_size*/
        next_transfer->lba      = buffer->scsi->transfer.lba + buffer->scsi->transfer.total;
        next_transfer->is_read  = job->is_read;
        next_transfer->pending  = 0;
        next_transfer->done     = 0;
        next_transfer->finished = 0;
        DBG_W(DSLAVE_MS_TRANS, ("next_transfer param: Size %d from %d \n",
          next_transfer->total, next_transfer->lba));

        /* start a new read operation */
        if (buffer->scsi->next_transfer->total)
        {
          DBG_W(DSLAVE_MS_TRANS, ("[%x]la_advance_job: sending next read req: Size %d from %d \n",
            next_la_buffer, next_transfer->total, next_transfer->lba));
          start_new_job (next_la_buffer);

          if (!buffer->is_la_buf)
          {
            scsi->la_transfer_idx++;
            scsi->la_transfer_idx = (scsi->la_transfer_idx)%SCSI_LOOKAHEAD_BUFFERS;
          }
        }
      }
    }
  }
  return TRUE;
}
#endif /*FEATURE_HS_USB_USE_MS_LA*/

static jresult_t advance_job(job_buf_t *buffer)
{
    job_t *job = &(buffer->job);
    jresult_t rc;
    transfer_t *transfer= &(buffer->scsi->transfer);

    DBG_I(DSLAVE_MS_TRANS, ("[%d] ADVANCE_JOB [%x]\n", buffer->id, buffer));

    /*Return if host is busy */   
    if (job->is_read && transfer->usb_job)
      return 0;

    /*Return if disk is busy */
    if (!job->is_read && transfer->disk_job)
      return 0;

#ifdef FEATURE_HS_USB_USE_MS_LA
    {
      jbool_t res = TRUE;
      if (buffer->scsi->transfer.total - buffer->scsi->transfer.done <= 
        HSU_SCSI_BUFFER_SIZE / buffer->scsi->curlun->block_size)
      { 
        DBG_W(DSLAVE_MS_TRANS, ("[%d] entering la_advance_job [%x]\n", buffer->id, buffer));
        res = la_advance_job(buffer);
        if (!res)
        {
          return res;
        } 
      }
      /*we didn't return on any of the condition above, so we send the buffer*/
      if (buffer->is_la_buf)
      {
        buffer->scsi->is_la_transfer_pending = FALSE;
	transfer->pending += job->blocks; /*so we won't sent a normal buffer after the la buffer with the same data 
                             for the case we get a new req. while the la didn't return*/
      }
    }  
#endif /*FEATURE_HS_USB_USE_MS_LA*/

    /* send the original buffer*/
    buffer->state = BUF_WRITING;
    rc = start_io(buffer, job->blocks, job->lba, DO_WRITE, 
        job->is_read ? DO_USB : DO_DISK);

    if (rc)
        goto Error;

    return 0;

Error:
    buffer->state = BUF_ERROR;
    buffer->error = rc;
    return rc;
}

static jresult_t advance_buffer(job_buf_t *buffer)
{
    jresult_t rc = 0;
    transfer_t *transfer = &buffer->scsi->transfer;

#ifdef FEATURE_HS_USB_USE_MS_LA
    juint8_t i; 
    if (buffer->is_la_buf)    
    {
      transfer = &(buffer->scsi->next_transfer[buffer->id]);
    }
#endif /* FEATURE_HS_USB_USE_MS_LA */

    DBG_I(DSLAVE_MS_TRANS, ("[%d] ADVANCE_BUFFER: State %d [%x]\n", 
        buffer->id, buffer->state, buffer));
    
    switch (buffer->state)
    {
    case BUF_READING:
    case BUF_WRITING:
        break;
        
    case BUF_DONE:
        /* Check if DISK is busy or not */
        if (transfer->is_read && transfer->disk_job)
            break;

        /* Check if USB is busy or not */
        if (!transfer->is_read && transfer->usb_job)
            break;

#ifdef FEATURE_HS_USB_USE_MS_LA
        if (buffer->is_la_buf)
        {
          /*check that there is  normal usb transfer going on*/
          if ((transfer->is_read && buffer->scsi->transfer.disk_job) || (!transfer->is_read && buffer->scsi->transfer.usb_job))
          {
            return 0;
          } 
          for (i=0; i<SCSI_LOOKAHEAD_BUFFERS; i++)
          {
            if (transfer->is_read && i!=buffer->id && buffer->scsi->next_transfer[i].disk_job)
              return 0;  
          }
        }

        else
        {
          /*check that there isn't a lookahead usb transfer going on*/
          for (i=0; i<SCSI_LOOKAHEAD_BUFFERS; i++)
          { 
            if (transfer->is_read && buffer->scsi->next_transfer[i].disk_job)
            {
              return 0;
            } 
            /* if we are in the middle of la disk_read, and we have new write operation, return. */
            if (!transfer->is_read && buffer->scsi->next_transfer[i].disk_job)
            {
              return 0;
            } 
          }  
        }

#endif /* FEATURE_HS_USB_USE_MS_LA */

        /* Get a new job for this buffer to perform */
        rc = start_new_job(buffer);
        break;

    case BUF_DONE_READ:
        rc = advance_job(buffer);
        break;
          
    case BUF_ERROR:
        rc = buffer->error;
    }
    
    DBG_I(DSLAVE_MS_TRANS, ("[%d] ADVANCE_BUFFER_DONE: State %d Rc %d [%x]\n", 
        buffer->id, buffer->state, rc, buffer));

    return rc;
}

static jresult_t schedule_advance(scsi_dev_t *scsi)
{
    jresult_t rc = 0;
    jint_t i;
    
    job_buf_t *curr, *next;
   
    DBG_I(DSLAVE_MS_TRANS, ("SCHED_ADVANCE\n"));
#ifdef FEATURE_HS_USB_USE_MS_LA
   if (scsi->is_la_transfer_pending)
   {
      jint_t transfer_idx = BUF_WAS_NOT_FOUND;
      DBG_W(DSLAVE_MS_TRANS, ("schedule_advance: current transfer is pending\n"));
      transfer_idx = find_current_request(scsi);

    if (transfer_idx>=0)      /* buffer was found and ready*/
    {

      DBG_W(DSLAVE_MS_TRANS, ("schedule_advance: current transfer is ready\n"));
      advance_job(&(scsi->la_buffer[transfer_idx]));
      return 0;
    }
    else /* Current read request still in progress */
    {
       DBG_W(DSLAVE_MS_TRANS, ("schedule_advance:Current transfer request didn't \
          return from disk\n"));
       return 0;
    }
   }
#endif /* FEATURE_HS_USB_USE_MS_LA */

    /* HSU addition: Error handling */
    if (!scsi->transfer.aborting) 
    {
    /******* End HSU addition **************************/
        /* Check if there are any buffers that are not submitted to IO */
        curr = TAILQ_FIRST(&(scsi->transfer.work_queue));
        if (!curr)
            return 0;

        /* SAFE-ly traverse the buffer list,
            advance_buffer() might remove the processed buffer from the list */
        while (curr)
        {
            next = TAILQ_NEXT(curr, next);
    
            rc = advance_buffer(curr);
            if (rc)
                break;

            curr = next;
        }

        /* If there is no error, we are done */
        if (!rc)
            return rc;
    /* HSU addition: Error handling */
    }
    /******* End HSU addition **************************/

    /* Check if all buffers were aborted */ 
    /* HSU addition: Error handling */
    for (i = 0, curr = TAILQ_FIRST(&(scsi->transfer.work_queue));
        i < SCSI_TRANSFER_BUFFERS && curr; 
        i++, curr = TAILQ_NEXT(curr, next))
    /******* End HSU addition **************************/
        ;

    /* If not all buffers are aborted */
    if (i != SCSI_TRANSFER_BUFFERS)
    {
        /* If we already started abort, just quit */
        if (scsi->transfer.aborting)
            return 0;

        scsi->transfer.aborting = 1;

        /* Abort pending usb jobs */
        if (scsi->transfer.usb_job)
        {
            if (scsi->transfer.is_read)
                core_abort_pipe(scsi->fd->core_ctx, scsi->fd->in_pipe);
            else
                core_abort_pipe(scsi->fd->core_ctx, scsi->fd->out_pipe);
        }

        /* Abort pending disk jobs */
        if (scsi->transfer.disk_job)
            scsi->msfd_ops.vcancel(scsi->appctx, scsi->lun); 

        return 0;
    }

    /* HSU addition: Error handling */
    if (scsi->transfer.finished)
    {
      return 0;
    }
    /******* End HSU addition **************************/

    /* All buffers have aborted */

    /* Re-Init buffer */
    for (i = 0; i < SCSI_TRANSFER_BUFFERS; i++)
    {
        scsi->buffers[i].state = BUF_DONE;
        scsi->buffers[i].error = 0;
    }

    scsi->transfer.aborting = 0;

    /* Raise error condition */
    /* HSU addition: Error handling */
    scsi->curlun->sense_data = scsi->transfer.is_read ? 
        SS_UNRECOVERED_READ_ERROR : SS_WRITE_ERROR;
    send_error_reply(scsi);

    scsi->transfer.finished = 1;

#ifdef  FEATURE_HS_USB_USE_MS_LA 
    /* Re-Init lookahead buffer */
    for (i = 0; i < SCSI_LOOKAHEAD_BUFFERS; i++)
    {
      scsi->la_buffer[i].state = BUF_DONE;
      scsi->la_buffer[i].error = 0;
    }
    scsi->next_transfer[i].aborting = 0;
    scsi->next_transfer[i].finished = 1;

#endif /* FEATURE_HS_USB_USE_MS_LA */
    return 0;
    /******* End HSU addition **************************/
}
/* HSU addition: SPS */

#ifdef FEATURE_HS_USB_MS_BAM
static void finish_data_transfer(scsi_dev_t *scsi)
{
  if (scsi->residue)
  {
    if (scsi->data_dir == DATA_DIR_TO_HOST)
    {
      /* If the host expects data, the STALL on the IN pipe will
      * tell it there is nothing interesting coming */
      DBG_W(DSLAVE_MS_SCSI, ("Stalling IN pipe, residue %d",
        scsi->residue));
      stall_in_pipe(scsi->fd);
    }
    else
    {
      /* If the host wants to send more data, the STALL on the OUT pipe
      * will tell it to move on to the status stage */
      DBG_W(DSLAVE_MS_SCSI, ("Stalling OUT pipe, residue %d",
        scsi->residue));
      stall_out_pipe(scsi->fd);
    }
  }
  send_csw(scsi->fd, scsi->tag, scsi->residue, 
    (scsi->phase_error ? USB_STATUS_PHASE_ERROR : USB_STATUS_PASS));
}

static void usb_done_sps(struct request_t *request)
{
  scsi_dev_t *scsi = ((job_buf_t *)request->context)->scsi;
  transfer_t *transfer = &scsi->transfer;
  request_status_t status = request->status;

  /* Request is ready for next transfer */
  request->status = REQUEST_READY;
  request->sps_mode = 0;
  scsi->residue -= request->bytes_transferred;

  if (status != REQUEST_COMPLETED || 
    request->bytes_transferred != request->transfer_size)
  {
    if (status != REQUEST_CANCELLED)
    {
      HSU_ULOG_3(ERROR_MSG, DBG_E_LOG, "MS_TRANS, sending_error, transferring %u [%u out of %u]",
        status,  request->bytes_transferred, request->transfer_size);       
      /* Abort pending disk job */
      if (transfer->disk_job)
      {
        transfer->disabled++;

        scsi->msfd_ops.vcancel(scsi->appctx, scsi->lun); 
      }
      scsi->curlun->sense_data = transfer->is_read ? 
        SS_UNRECOVERED_READ_ERROR : SS_WRITE_ERROR;
      send_error_reply(scsi);
    }
    else
    {
      DBG_W(DSLAVE_MS_TRANS, ("Cancel during transferring %d [%d out of %d].", 
        status,  request->bytes_transferred, request->transfer_size));
    }
  }
  else
  {
    if (!transfer->disk_job)
    {
      finish_data_transfer(scsi);
    }
  }
}

/**
* Function name:  schedule_advance_sps
* Description:    Handles a SCSI READ/WRITE command in SPS mode. 
* Parameters: 
*     @scsi:   (IN) SCSI device context 
*
* Return value: 0 on success, otherwise an error code 
* Scope:        Local
**/
static jresult_t schedule_advance_sps(scsi_dev_t *scsi)
{
  transfer_t *transfer = &scsi->transfer;
  job_buf_t *buffer = &scsi->buffers[0];
  request_t *request = buffer->io_request;
  jresult_t rc;

  request->sps_mode = 1;
  request->tbe = 1;
  if (transfer->is_read)
  {
    request->sps_pipe_id = (juint8_t)scsi->curlun->in_sps_pipe_id;
    rc = post_write(scsi->fd, request, 
      scsi->data_size_from_cmnd, usb_done_sps, buffer);
    if (!rc)
    {
      rc = scsi->msfd_ops.vread(scsi->appctx, scsi->lun,
        (juint8_t *)NULL, scsi->data_size_from_cmnd / scsi->curlun->block_size, transfer->lba);
      if (!rc)
      {
        transfer->disk_job = buffer;
      }
      else
      {
        core_abort_pipe(scsi->fd->core_ctx, scsi->fd->in_pipe);
        scsi->curlun->sense_data = SS_UNRECOVERED_READ_ERROR;
      }
    }
    else
    {
      request->sps_mode = 0;
      scsi->curlun->sense_data = SS_UNRECOVERED_READ_ERROR;
    }
  }
  else
  {
    request->sps_pipe_id = (juint8_t)scsi->curlun->out_sps_pipe_id;
    rc = post_read(scsi->fd, request, 
      scsi->data_size_from_cmnd, usb_done_sps, buffer);
    if (!rc)
    {
      rc = scsi->msfd_ops.vwrite(scsi->appctx, scsi->lun,
        (juint8_t *) NULL, scsi->data_size_from_cmnd / scsi->curlun->block_size, transfer->lba);
      if (!rc)
      {
        transfer->disk_job = buffer;
      }
      else
      {
        core_abort_pipe(scsi->fd->core_ctx, scsi->fd->out_pipe);
        scsi->curlun->sense_data = SS_WRITE_ERROR;
      }
    }
    else
    {
      request->sps_mode = 0;
      scsi->curlun->sense_data = SS_WRITE_ERROR;
    }
  }
  return rc;
}

static void disk_done_sps(void *context, jbool_t status)
{
  scsi_dev_t *scsi = (scsi_dev_t *)context;
  transfer_t *transfer = &scsi->transfer;
  request_t *request = transfer->disk_job->io_request;

  transfer->disk_job = NULL;

  if (transfer->disabled)
  {
    DBG_W(DSLAVE_MS_TRANS, ("Cancel during media transfer %d.", 
      status));
    transfer->disabled--;
  }
  else if (status == 0)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MS_TRANS, media_transfer_error");           
    /* Abort pending usb job */
    if (transfer->is_read)
    {
      core_abort_pipe(scsi->fd->core_ctx, scsi->fd->in_pipe);
      scsi->curlun->sense_data = SS_UNRECOVERED_READ_ERROR;
    }
    else
    {
      core_abort_pipe(scsi->fd->core_ctx, scsi->fd->out_pipe);
      scsi->curlun->sense_data = SS_WRITE_ERROR;
    }
    send_error_reply(scsi);
  }
  else
  {
    if (request->status == REQUEST_READY)
    {
      finish_data_transfer(scsi);
    }
  }
}
#endif /* FEATURE_HS_USB_MS_BAM */
/* End of HSU addition */

#ifdef FEATURE_HS_USB_USE_MS_LA
/**
 * Function name:  discard_la_on_write
 * Description:    Discard the LA buffer is there was a write operation over
                   its content.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @new_lba: LBA of the write operation 
 *     @num_of_blocks: num_of_blocks of the write operation
 *
 * Return value: none
 * Scope:        Local
 **/
static void discard_la_on_write(scsi_dev_t *scsi, juint32_t new_lba ,juint32_t num_of_blocks)
{
  juint8_t i;
  job_buf_t *curr_buf;
  /*TODO: invalidate our cache on
    1.remove the SD card and insert different one.
    2.when disconnecting the USB cable (scsi_disable)*/
  for(i=0; i<SCSI_LOOKAHEAD_BUFFERS; i++)
  {
    curr_buf = &(scsi->la_buffer[i]);
    if (scsi->lun != curr_buf->job.lun || new_lba+num_of_blocks < curr_buf->job.lba || new_lba > curr_buf->job.lba+curr_buf->job.blocks)
    {
      continue;
    }
    else /* discard la buffer */
    {
      DBG_W(DSLAVE_MS_TRANS, ("discard_la_on_write() - buffer no %d.", i));
      curr_buf->job.blocks = 0; 
    }
  }
}
#endif /* FEATURE_HS_USB_USE_MS_LA */

/**
 * Function name:  scsi_read_write
 * Description:    Handles a SCSI read/write command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *     @is_read: (IN) Direction of transfer
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static jresult_t scsi_read_write(scsi_dev_t *scsi, juint8_t *buffer, 
    jbool_t is_read)
{
    struct lun *curlun = scsi->curlun;
    transfer_t *transfer = &scsi->transfer;
    juint32_t   lba;

    /* HSU addition: CR 175102 fix */
    if (transfer->disabled)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MS_SCSI, transfer_is_disabled");                   
        curlun->sense_data = is_read ? SS_UNRECOVERED_READ_ERROR : SS_WRITE_ERROR;
        return JEIO;
    }
    /******* End HSU addition **************************/

    if (!is_read)
    {
        if (curlun->ro)
        {
            curlun->sense_data = SS_WRITE_PROTECTED;
            return JEINVAL;
        }
        scsi->msfd_ops.vallow_cache(scsi->appctx, scsi->lun, ALLOW_CACHE);
    }

    /* Get the starting Logical Block Address and check that it's
     * not too big */
    if (scsi->cmnd[0] == SC_READ_6 || scsi->cmnd[0] == SC_WRITE_6 /* HSU fix */)
    {
        lba = (scsi->cmnd[1] << 16) | be16toh_t(&scsi->cmnd[2]);
    }
    else 
    {
        lba = be32toh_t(&scsi->cmnd[2]);

        /* We allow DPO (Disable Page Out = don't save data in the
         * cache) and FUA (Force Unit Access = don't read from the
         * cache), but we don't implement them */
        if ((scsi->cmnd[1] & ~0x18) != 0) 
        {
            curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
            return JEINVAL;
        }

        if (!is_read && scsi->cmnd[1] & 0x08)
        {
            scsi->msfd_ops.vallow_cache(scsi->appctx, scsi->lun,
                DISALLOW_CACHE);
        }
    }

    /* HSU addition: Error handling */
    if (scsi->data_size_from_cmnd == 0)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MS_SCSI, zero_data_length");
        curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }
    /******* End HSU addition **************************/

    transfer->total   = scsi->data_size_from_cmnd / scsi->curlun->block_size;
    transfer->lba     = lba;
    transfer->is_read = is_read;
    transfer->pending = 0;
    transfer->done    = 0;
    /* HSU addition: Error handling */
    transfer->finished = 0;
    /******* End HSU addition **************************/
    
    if (lba >= curlun->num_blocks ||
        transfer->lba + transfer->total > curlun->num_blocks) 
    {
        curlun->sense_data = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
        return JEINVAL;
    }

  #ifdef FEATURE_HS_USB_USE_MS_LA
   if (is_read)
  {
     jint_t transfer_index = BUF_WAS_NOT_FOUND;
     DBG_W(DSLAVE_MS_TRANS, ("NEW READ REQ: %d from %d.\n", transfer->total, transfer->lba));
       la_statistics.all_read_req++;

     DBG_W(DSLAVE_MS_TRANS, ("LA statistics: all_read_req: %d, found_right_away: %d,\
        found_and_not_ready: %d, didnt_find_at_all: %d \n", la_statistics.all_read_req, la_statistics.found_right_away,
         la_statistics.found_and_not_ready, la_statistics.didnt_find_at_all));
     
     /* Check if the current request exists in a la buffer */
     transfer_index = find_current_request(scsi);
     if (transfer_index >= 0)
     {
       jresult_t rc = 0;
         la_statistics.found_right_away++;
       rc = advance_job(&(scsi->la_buffer[transfer_index]));
       return rc;
     }
     else if (transfer_index == BUF_NOT_REAY)
     {
         la_statistics.found_and_not_ready++;
       return 0;
     }
     else /*BUF_WAS_NOT_FOUND*/
     {
       la_statistics.didnt_find_at_all++;   
     }

     if (scsi->is_la_transfer_pending == FALSE)
     {
        return schedule_advance(scsi);
     }/* if is_transfer_pending== true, meaning that we will get to schedule_advance() from disk_done() */
    }
    else 
    {
       DBG_W(DSLAVE_MS_TRANS, ("NEW WRITE REQ: %d from %d.\n", transfer->total, transfer->lba));
       /*On write operation discard la buffer info if it is in range */
       discard_la_on_write(scsi , transfer->lba , transfer->total);
    }
#endif /* FEATURE_HS_USB_USE_MS_LA */

    /* HSU addition: SPS */
#ifdef FEATURE_HS_USB_MS_BAM
    /* Check whether LUN operates in the SPS mode */
    if (curlun->sps_mode)
      return schedule_advance_sps(scsi);
#endif /* FEATURE_HS_USB_MS_BAM */
    /* End of HSU addition */
    return schedule_advance(scsi);
}

#ifdef FEATURE_HS_USB_USE_MS_LA
/**
* Function name:  find_current_request
* Description:    Search for the current transfer in the LA buffers
* Parameters: 
*     @request: (IN) SCSI device context 
*
* Return value: LA buffer index if request was found and ready
                -2  if request was found but not ready
                -1  if request wasn't found
* Scope:        Local
**/
static jint_t find_current_request(scsi_dev_t *scsi)
{
  transfer_t *transfer = &scsi->transfer;
  job_buf_t *curr_buf;
  jint_t i;

  for(i=0; i<SCSI_LOOKAHEAD_BUFFERS; i++)
  {
    curr_buf = &(scsi->la_buffer[i]);
    /* for transfers that are less or equal to the max number of blocks */
    if (transfer->total <= HSU_SCSI_BUFFER_SIZE / scsi->curlun->block_size) 
    {
      if (transfer->total != curr_buf->job.blocks)
      {
        continue;
      }
    }
    else
    { 
      if (HSU_SCSI_BUFFER_SIZE / scsi->curlun->block_size != curr_buf->job.blocks)
      {
       continue;
      }
    }
    if (curr_buf->state != BUF_ERROR &&
      transfer->lba == curr_buf->job.lba && 
      scsi->lun == curr_buf->job.lun) 
    {
      if(curr_buf->state == BUF_DONE_READ)
      {
        DBG_W(DSLAVE_MS_TRANS, ("find_current_request(): found [%x] and ready\n", curr_buf)); 
        scsi->is_la_transfer_pending = TRUE;
        return i;
      }
      else if(curr_buf->state == BUF_READING)
      {
        DBG_W(DSLAVE_MS_TRANS, ("find_current_request(): found [%d] and not ready : Size %d from %d [%x]\n",
        curr_buf->id, curr_buf->job.blocks, curr_buf->job.lba, curr_buf));
        scsi->is_la_transfer_pending = TRUE;
        return BUF_NOT_REAY;
      }
    } 
    else if (curr_buf->state == BUF_ERROR)
    { /* re-init buffer and discard data */
      curr_buf->state = BUF_DONE;
      curr_buf->error = 0;
      curr_buf->job.blocks = 0;
      scsi->next_transfer[curr_buf->id].aborting = 0;
      scsi->next_transfer[curr_buf->id].finished = 1;

    }
  }

  return BUF_WAS_NOT_FOUND; 
} 
#endif /* FEATURE_HS_USB_USE_MS_LA */

/* HSU addition */
static jresult_t scsi_get_data_from_usb(scsi_dev_t *scsi);

/**
* Function name:  scsi_get_data_from_usb_done
* Description:    Completion callback for user-defined SCSI OUT command
*                 (scsi_get_data_from_usb).
* Parameters: 
*     @request: (IN) Core request
*
* Return value: None
* Scope:        Local
**/
static void scsi_get_data_from_usb_done(struct request_t *request)
{
    scsi_dev_t *scsi = (scsi_dev_t *)request->context;
    request_status_t status = request->status;
    
    /* Request is ready for next transfer */
    request->status = REQUEST_READY;
   
    if (status != REQUEST_COMPLETED || 
        request->bytes_transferred != request->transfer_size)
    {
        if (status != REQUEST_CANCELLED)
        {
            HSU_ULOG_3(ERROR_MSG, DBG_E_LOG, "MS_SCSI, receive_error, transferring %u [%u out of %u]",
                status,  request->bytes_transferred, request->transfer_size);            
            send_error_reply(scsi);
        }
        else
        {
            DBG_W(DSLAVE_MS_SCSI, ("SCSI: Cancel during receive %d [%d out of %d]. Stalling OUT pipe", 
                status,  request->bytes_transferred, request->transfer_size));
            stall_out_pipe(scsi->fd);
        }
    }
    else
    {
        jresult_t rc = 0;

        scsi->residue -= request->bytes_transferred;

        scsi->msfd_ops.scsi_out_data_stage_handler(scsi->appctx, scsi->lun,
            (juint8_t *)(request->buffer.vaddr), request->bytes_transferred, &rc);
        if (rc)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "MS_SCSI, scsi_out_data_stage_handler,"
              " Sending_error, rc %u", rc);
            send_error_reply(scsi);
            return;
        }

        scsi->data_size -= request->bytes_transferred;
        rc = scsi_get_data_from_usb(scsi);
        if (rc)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "MS_SCSI, Sending_error, rc %u", rc);            
            send_error_reply(scsi);
            return;
        }
    }
}

/**
* Function name:  scsi_get_data_from_usb
* Description:    Handles a SCSI OUT proprietary command. 
*                 Initiates read of data from USB
* Parameters: 
*     @scsi:   (IN) SCSI device context 
*
* Return value: 0 on success, otherwise an error code 
* Scope:        Local
**/
static jresult_t scsi_get_data_from_usb(scsi_dev_t *scsi)
{
    /* Check whether a DATA stage is finished */
    if (scsi->data_size == 0)
    {
        /* Check to see if there is any data pending and read/write it */
        if (scsi->residue)
        {
            /* If the host wants to send more data, the STALL on the OUT pipe
            * will tell it to move on to the status stage */
            DBG_W(DSLAVE_MS_SCSI, ("SCSI: Stalling OUT pipe, residue %d\n",
                scsi->residue));
            stall_out_pipe(scsi->fd);
        }
        send_csw(scsi->fd, scsi->tag, scsi->residue, USB_STATUS_PASS);
        return 0;
    }

    scsi->data_size_from_cmnd = 
        scsi->data_size < scsi->fd->scsi_buffer_size ? 
        scsi->data_size : scsi->fd->scsi_buffer_size;

    return post_read(scsi->fd, scsi->fd->io_request[0], scsi->data_size_from_cmnd, 
        scsi_get_data_from_usb_done, scsi);
}
/* End of HSU addition */

/**
 * Function name:  scsi_write
 * Description:    Handles a SCSI write command.
 *                 Starts reading data from the host to write to the disk.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope:        Local
 **/
static jresult_t scsi_write(scsi_dev_t *scsi, juint8_t *buffer)
{
    return scsi_read_write(scsi, buffer, 0);
}

/**
 * Function name:  scsi_read
 * Description:    Handles a SCSI read command.
 *                 Starts reading data from the disk to write to the host.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope:        Local
 **/
static jresult_t scsi_read(scsi_dev_t *scsi, juint8_t *buffer)
{
    return scsi_read_write(scsi, buffer, 1);
}

/**
 * Function name:  scsi_synchronize_cache
 * Description:    Handles a SCSI synchronize cache command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope:        Local
 **/
static jresult_t scsi_synchronize_cache(scsi_dev_t *scsi, juint8_t *buffer)
{
    jresult_t rc = 0;

    rc = scsi->msfd_ops.vflush(scsi->appctx, scsi->lun);
    if (rc)
        return rc;

    send_csw(scsi->fd, scsi->tag, scsi->residue, USB_STATUS_PASS);
    return 0;
}

#ifdef CONFIG_FD_MASS_CDROM_SIM
static jresult_t scsi_cdrom_command(scsi_dev_t *scsi, juint8_t *buffer)
{
    DBG_V(DSLAVE_MS_SCSI, ("scsi_cdrom_command: Entered\n"));

    /* Failed all cdrom commands */
    return -1;
}
#endif

/**
 * Function name:  scsi_verify
 * Description:    Handles a SCSI verify data command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t scsi_verify(scsi_dev_t *scsi, juint8_t *buffer)
{
    struct lun          *curlun = scsi->curlun;
    juint32_t            lba;
    juint32_t            amount;
    jint32_t             nread;
    jresult_t            rc = 0;

    /* Get the starting Logical Block Address and check that it's
     * not too big */
    lba = be32toh_t(&scsi->cmnd[2]);
    if (lba >= curlun->num_blocks) 
    {
        curlun->sense_data = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
        return JEINVAL;
    }

    /* We allow DPO (Disable Page Out = don't save data in the cache)
     * but we don't implement it */
    if ((scsi->cmnd[1] & ~0x10) != 0) 
    {
        curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }

    amount = be16toh_t(&scsi->cmnd[7]);
    if (amount == 0)
        return JEIO;             /* No default reply */
    
    /* Write out all the dirty buffers before invalidating them */
    rc = scsi->msfd_ops.vflush(scsi->appctx, scsi->lun);
    if (rc)
        return JEIO;

    if (amount > (curlun->num_blocks - lba))    
    {
        curlun->sense_data = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
        curlun->sense_data_info = lba;
        return JEIO;
    }

    /* Perform the read */
    nread = scsi->msfd_ops.vverify(scsi->appctx, scsi->lun, amount, 
        lba);

    DBG_V(DSLAVE_MS_SCSI, ("SCSI: File read (verify): %u @ %u -> %ld.\n",
        amount, lba, nread));

    if (nread < 0) 
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "MS_SCSI, error_varify, nread %1d", nread);        
        return JEIO;
    }
    else if ((juint32_t)nread < amount) 
    {
        DBG_V(DSLAVE_MS_SCSI, ("SCSI: Partial verify: %d/%u.\n", nread, 
            amount));
        curlun->sense_data = SS_UNRECOVERED_READ_ERROR;
        curlun->sense_data_info = lba;
        return JEIO;
    }
   
    send_csw(scsi->fd, scsi->tag, scsi->residue, USB_STATUS_PASS);

    return 0;
}

/**
 * Function name:  scsi_inquiry
 * Description:    Handles a SCSI inquiry command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer: (IN) Buffer to be used for USB data transfers
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t scsi_inquiry(scsi_dev_t *scsi, juint8_t *buffer)
{
    char *vendor = "Unknown";
    char *product = "Unknown";
    char *revision = "Unknown";
    juint32_t dev_type = 0;
    juint8_t version = 0; /* HSU temp fix B56842 */

    if (!scsi->curlun) 
    {
        /* Unsupported LUNs are okay */
        j_memset(buffer, 0, INQUIRY_RESPONSE_LEN);
        buffer[0] = 0x7f;               /* Unsupported, no device-type */
    }
    else
    {
        /* Get device description from the application */
        scsi->msfd_ops.vget_desc(scsi->appctx, scsi->lun, &dev_type ,
            /* HSU temp fix B56842 */ &version,
            &vendor, &product, &revision);
        
        j_memset(buffer, 0, 8);

        buffer[0] = (juint8_t)(dev_type & SCSI_DEVICE_UNKNOWN);

        if (scsi->curlun->removable)
            buffer[1] = 0x80;

        /* HSU temp fix B56842 */
        buffer[2] = version;
        buffer[3] = 2;
        buffer[4] = 31;

        j_snprintf((char *)(buffer + 8), INQUIRY_RESPONSE_LEN - 7, 
            "%-8s%-16s%-4s", vendor, product, revision);
        DBG_I(DSLAVE_MS_SCSI, ("SCSI: Inquiry reply: [%x %x: %s].\n",
            (juint32_t)buffer[0],
            (juint32_t)buffer[4],
            buffer + 8));
    }

    return scsi_write_data(scsi, 36, 0, scsi_standard_callback,
        USB_STATUS_PASS);
}

/**
 * Function name:  scsi_request_sense
 * Description:    Handles a SCSI request sense command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer: (IN) Buffer to be used for USB data transfers
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t scsi_request_sense(scsi_dev_t *scsi, juint8_t *buffer)
{
    struct lun  *curlun = scsi->curlun;
    juint32_t    sd, sdinfo;

    /* Unsupported LUNs are okay */
    if (!curlun)
    {
        sd = SS_LOGICAL_UNIT_NOT_SUPPORTED;
        sdinfo = 0;
    }
    else 
    {
        sd = curlun->sense_data;
        sdinfo = curlun->sense_data_info;
        curlun->sense_data = SS_NO_SENSE;
        curlun->sense_data_info = 0;
    }

    j_memset(buffer, 0, 18);
    buffer[0]   = 0x80 | 0x70;  
    buffer[2]   = SK(sd);
    htobe32_t(&buffer[3], sdinfo);
    buffer[7]   = 18 - 8;       
    buffer[12]  = ASC(sd);
    buffer[13]  = ASCQ(sd);

    DBG_I(DSLAVE_MS_SCSI, ("SCSI: Sense: %x:%x:%x (%x).\n",
        SK(sd), ASC(sd), ASCQ(sd), (juint32_t)buffer[3]));
    
    return scsi_write_data(scsi, 18, 0, scsi_standard_callback,
        USB_STATUS_PASS);
}

/**
 * Function name:  scsi_read_capacity
 * Description:    Handles a SCSI read capacity command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer: (IN) Buffer to be used for USB data transfers
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static jresult_t scsi_read_capacity(scsi_dev_t *scsi, juint8_t *buffer)
{
    struct lun  *curlun = scsi->curlun;
    juint32_t    lba = be32toh_t(&scsi->cmnd[2]);
    jint_t       pmi = scsi->cmnd[8];

    if (!curlun)
        return JEINVAL;

    /* Check the PMI and LBA fields */
    if (pmi > 1 || (pmi == 0 && lba != 0)) 
    {
        DBG_W(DSLAVE_MS_SCSI, ("Read capacity: Bad PMI (%d) / LBA (%d).\n",
            pmi, lba));
        curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }

    htobe32_t(&buffer[0], curlun->num_blocks - 1);      
    htobe32_t(&buffer[4], curlun->block_size);

    return scsi_write_data(scsi, 8, 0, scsi_standard_callback,
        USB_STATUS_PASS);
}

/**
 * Function name:  scsi_mode_sense
 * Description:    Handles a SCSI mode sense command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer: (IN) Buffer to be used for USB data transfers
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static jresult_t scsi_mode_sense(scsi_dev_t *scsi, juint8_t *buffer)
{
    struct lun  *curlun = scsi->curlun;
    juint8_t     mscmnd = scsi->cmnd[0];
    juint8_t     *buffer0 = buffer;
    juint8_t     pc, page_code;
    jbool_t      changeable_values, all_pages;
    jbool_t      valid_page = 0;
    juint32_t    len, limit;

    if (!curlun)
        return JEINVAL;

    if ((scsi->cmnd[1] & ~0x08) != 0) 
    {   /* Mask away DBD */
        curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }
    
    pc = scsi->cmnd[2] >> 6;
    page_code = scsi->cmnd[2] & 0x3f;
    if (pc == 3) 
    {
        curlun->sense_data = SS_SAVING_PARAMETERS_NOT_SUPPORTED;
        return JEINVAL;
    }
    changeable_values = (pc == 1);
    all_pages = (page_code == 0x3f);

    /* Write the mode parameter header. Fixed values are: default medium type,
     * no cache control (DPOFUA), and no block descriptors.
     * The only variable value is the WriteProtect bit.
     * We will fill in the mode data length later. */
    j_memset(buffer, 0, 8);
    if (mscmnd == SC_MODE_SENSE_6) 
    {
        buffer[2] = (curlun->ro ? 0x80 : 0x00);
        buffer += 4;
        limit = 255;
    } 
    else
    {   /* SC_MODE_SENSE_10 */
        buffer[3] = (curlun->ro ? 0x80 : 0x00);
        buffer += 8;
        limit = 65535;  
    }

    /* No block descriptors */

    /* The mode pages: */
    
    /* Caching page */
    if (page_code == 0x08 || all_pages) 
    {
        valid_page = 1;
        buffer[0] = 0x08;       /* Page code */
        buffer[1] = 10;         /* Page length */
        j_memset(buffer+2, 0, 10);      /* None of the fields are changeable */

        if (!changeable_values) 
        {
            buffer[2] = 0x04;   /* Write cache enable, */
            htobe16_t(&buffer[4], 0xffff);  /* Don't disable prefetch */
            htobe16_t(&buffer[8], 0xffff);  /* Maximum prefetch */
            htobe16_t(&buffer[10], 0xffff); /* Maximum prefetch ceiling */
        }
        buffer += 12;
    }

    /* Check that a valid page was requested and that the mode data length
     * isn't too long */
    len = buffer - buffer0;
    if (!valid_page || len > limit) 
    {
        curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }

    /* Check that we do not return more then needed */
    len = MIN(len, scsi->data_size_from_cmnd);

    /*  Store the mode data length */
    if (mscmnd == SC_MODE_SENSE_6)
        buffer0[0] = (juint8_t)(len - 1);
    else
        htobe16_t(buffer0, len - 2);
   
    DBG_V(DSLAVE_MS_SCSI, ("SCSI: Mode SENSE sent %d [Page: %x : %d - %d].\n",
        len, page_code, all_pages, scsi->data_size_from_cmnd));

    return scsi_write_data(scsi, len, 0, scsi_standard_callback, 
        USB_STATUS_PASS);
}

/**
 * Function name:  scsi_start_stop
 * Description:    Handles a SCSI start/stop command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static jresult_t scsi_start_stop(scsi_dev_t *scsi, juint8_t *buffer)
{
    struct lun  *curlun = scsi->curlun;
    juint8_t     loej, start;
    jresult_t    rc;

    if (!curlun)
        return JEINVAL;

    if (!scsi->curlun->removable) 
    {
        curlun->sense_data = SS_INVALID_COMMAND;
        return JEINVAL;
    }

    /* int immed = scsi->cmnd[1] & 0x01; */
    loej = scsi->cmnd[4] & 0x02;
    start = scsi->cmnd[4] & 0x01;

    if ((scsi->cmnd[1] & ~0x01) != 0 || /* Mask away Immed */
        (scsi->cmnd[4] & ~0x03) != 0)
    {   
        /* Mask LoEj, Start */
        curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }

    if (!start) 
    {
        /* Are we allowed to unload the media? */
        if (curlun->prevent_medium_removal) 
        {
            DBG_W(DSLAVE_MS_SCSI, ("SCSI: Unload attempt prevented.\n"));
            curlun->sense_data = SS_MEDIUM_REMOVAL_PREVENTED;
            return JEINVAL;
        }

        /* Eject the medium */
        if (loej) 
        {
            rc = scsi->msfd_ops.vunmount(scsi->appctx, scsi->lun);
            if (rc)
            /* HSU addition: Error handling */
            {
                curlun->sense_data = SS_INVALID_COMMAND;
                return rc;
            }
            /******* End HSU addition **************************/
        }
    } 
    else
    {
        if (loej)
        {
            DBG_I(DSLAVE_MS_SCSI, ("SCSI: Mount request %d.\n", scsi->lun));
            rc = scsi->msfd_ops.vmount(scsi->appctx, scsi->lun);
            if (rc)
            /* HSU addition: Error handling */
            {
                curlun->sense_data = SS_MEDIUM_NOT_PRESENT;
                return rc;
            }
            /******* End HSU addition **************************/
        }
        else if (!(scsi->msfd_ops.vready(scsi->appctx, scsi->lun))) 
        {
            DBG_V(DSLAVE_MS_SCSI, ("SCSI: Medium not present "
                "- reported to host.\n"));
            curlun->sense_data = SS_MEDIUM_NOT_PRESENT;
            return JEINVAL;
        }
    }

    send_csw(scsi->fd, scsi->tag, scsi->residue, USB_STATUS_PASS);

    return 0;
}

/**
 * Function name:  scsi_test_unit_ready
 * Description:    Handles a SCSI test unit ready command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static jresult_t scsi_test_unit_ready(scsi_dev_t *scsi, juint8_t *buffer)
{
    struct lun  *curlun = scsi->curlun;
    juint8_t status = USB_STATUS_PASS;

    if (!scsi->msfd_ops.vready(scsi->appctx, scsi->lun))
    {
        DBG_I(DSLAVE_MS_SCSI, ("SCSI: Medium not present, reported to "
            "host.\n"));
        curlun->sense_data = SS_MEDIUM_NOT_PRESENT;
        status = USB_STATUS_FAIL;
    }

    send_csw(scsi->fd, scsi->tag, scsi->residue, status);
    return 0;
}

/**
 * Function name:  scsi_prevent_allow
 * Description:    Handles a SCSI prevent/allow remove media command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *
 * Return value: 
 * Scope:        Local
 **/
static jresult_t scsi_prevent_allow(scsi_dev_t *scsi, juint8_t *buffer)
{
    struct lun  *curlun = scsi->curlun;
    jbool_t      prevent;
    jresult_t    rc = 0;

    if (!curlun)
        return JEINVAL;

    if (!scsi->curlun->removable) 
    {
        curlun->sense_data = SS_INVALID_COMMAND;
        return JEINVAL;
    }

    prevent = scsi->cmnd[4] & 0x01;
    if ((scsi->cmnd[4] & ~0x01) != 0) 
    {   /* Mask away Prevent */
        curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }

    if (curlun->prevent_medium_removal && !prevent)
    {
        rc = scsi->msfd_ops.vflush(scsi->appctx, scsi->lun);
        if (rc)
            return rc;
    }
    curlun->prevent_medium_removal = prevent;
    
    send_csw(scsi->fd, scsi->tag, scsi->residue, USB_STATUS_PASS);
    return 0;
}

#ifndef CONFIG_SCSI_REDUCED_BLOCK_COMMAND
/**
 * Function name:  scsi_read_format_capacities
 * Description:    Handles a SCSI read format capacities command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer: (IN) Buffer to be used for USB data transfers
 
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t scsi_read_format_capacities(scsi_dev_t *scsi, 
    juint8_t *buffer)
{
    struct lun  *curlun = scsi->curlun;

    if (!curlun)
        return JEINVAL;

    buffer[0] = buffer[1] = buffer[2] = 0;
    buffer[3] = 8;          /* Only the Current/Maximum Capacity Descriptor */
    buffer += 4;

    htobe32_t(&buffer[0], curlun->num_blocks);          /* Number of blocks */

    htobe32_t(&buffer[4], curlun->block_size);          /* Block length */
    buffer[4] = 0x02;                                   /* Current capacity */
    
    return scsi_write_data(scsi, 12, 0, scsi_standard_callback,
        USB_STATUS_PASS);
}
#endif

/**
 * Function name:  scsi_mode_select
 * Description:    Handles a SCSI mode select command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t scsi_mode_select(scsi_dev_t *scsi, juint8_t *buffer)
{
    struct lun  *curlun = scsi->curlun;

    if (!curlun)
        return JEINVAL;

    /* We don't support MODE SELECT */
    curlun->sense_data = SS_INVALID_COMMAND;

    /* Since the host is sending data to us,
    * we need to get residue from the OUT pipe */
   return scsi_read_data(scsi, scsi->residue, 0, scsi_standard_callback,
       USB_STATUS_FAIL);
}

#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
/**
 * Function name:  scsi_transparent
 * Description:    Handles a SCSI transport command.
 * Parameters: 
 *     @scsi:   (IN) SCSI device context 
 *     @buffer:      N/A
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t scsi_transparent(scsi_dev_t *scsi, juint8_t *buffer)
{    
    jresult_t rc;
    struct lun *curlun = scsi->curlun;

    if (!curlun)
        return JEINVAL;
    
    rc = scsi->msfd_ops.vtransparent(scsi->appctx, scsi->lun,scsi->cmnd);
    if (rc) 
    {
        curlun->sense_data = rc;
        return rc;
    }

    send_csw(scsi->fd, scsi->tag, scsi->residue, USB_STATUS_PASS);
    return 0;
}
#endif

/**
 * Function name:  scsi_get_lun
 * Description:    Updates the current LUN to the one requested in the command.
 * Parameters: 
 *     @scsi: (IN) SCSI device context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t scsi_get_lun(scsi_dev_t *scsi)
{
    juint8_t lun = scsi->cmnd[1] >> 5;

    /* Does the CBW and SCSI lun values agree ? */
    if (scsi->lun != lun)
    {
        DBG_V(DSLAVE_MS_SCSI, ("SCSI: Using LUN %d from CBW, "
            "not LUN %d from CDB.\n", scsi->lun, lun));
    }

    /* Check the LUN */
    if (scsi->lun < scsi->nluns && scsi->luns[scsi->lun].enabled) 
    {
        scsi->curlun = &scsi->luns[scsi->lun];
        if (scsi->cmnd[0] != SC_REQUEST_SENSE) 
        {
            scsi->curlun->sense_data = SS_NO_SENSE;
            scsi->curlun->sense_data_info = 0;
        }
    }
    else 
    {
        scsi->curlun = NULL;

        /* INQUIRY and REQUEST SENSE commands are explicitly allowed
         * to use unsupported LUNs; all others may not. */
        if (scsi->cmnd[0] != SC_INQUIRY &&
            scsi->cmnd[0] != SC_REQUEST_SENSE) 
        {
            DBG_W(DSLAVE_MS_SCSI, ("SCSI: Unsupported LUN %d\n", scsi->lun));
            return JEINVAL;
        }
    }

    return 0;
}

/**
 * Function name:  verify_scsi_cmd
 * Description:    Verifies that the SCSI command is legal, and that the device
 *                 is able to handle the specified command.
 * Parameters: 
 *     @scsi:     (IN) SCSI device context
 *     @cmd_data: (IN) SCSI command data
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static jresult_t verify_scsi_cmd(scsi_dev_t *scsi, 
    scsi_command_data *cmd_data)
{
    struct lun *curlun = &scsi->luns[scsi->lun];
    jint_t i;

    if (!curlun)
        return JEINVAL;

    /* Verify the length of the command itself */
    if (cmd_data->cmd_size != scsi->cmnd_size) 
    {
        DBG_V(DSLAVE_MS_SCSI, ("SCSI: Possible phase error encountered. "
            "Command sizes conflict. Should be %d but is %d.\n",
            cmd_data->cmd_size, scsi->cmnd_size));

        /* There is a chance that command padding was done,
           try to handle the command anyway */
        scsi->cmnd_size = cmd_data->cmd_size;
    }

    /* Calculate the expected data size */
    scsi->data_size_from_cmnd = 
        (cmd_data->get_size_from_header ?
        cmd_data->get_size_from_header(curlun, scsi->cmnd, scsi->cmnd_size) : 
        cmd_data->data_from_cmd);

    DBG_I(DSLAVE_MS_SCSI, ("SCSI: Command: %s; Lun %d Dc = %d, D%d = %u\n",
        cmd_data->name, scsi->lun, cmd_data->cmd_size, cmd_data->data_dir,
        scsi->data_size_from_cmnd));

    if (scsi->data_size_from_cmnd == 0 && cmd_data->data_dir != DATA_DIR_NONE)
    {
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Host expects no data but the command "
            "is an I/O command. \n"));
    }

    /* HSU addition */
    /* MSC CV Case 4 and 9 support */
    if (scsi->data_size_from_cmnd == 0 && scsi->data_size > 0)
    {
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Host expects data but the command "
            "has no data to transmit.\n"));
        if (scsi->curlun)
            scsi->curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }

    /* MSC CV Case 2 and 3 support */
    if (scsi->data_size == 0 && scsi->data_size_from_cmnd > 0)
    {
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Host expects no data but the command "
            "has data to transmit.\n"));
        scsi->phase_error = 1;
        if (scsi->curlun)
            scsi->curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
        return JEINVAL;
    }
    /******* End HSU addition **************************/

    if (scsi->data_size < scsi->data_size_from_cmnd) 
    {
        /* Host data size < Device data size ->
         * Carry out the command, but only transfer as much
         * as we are allowed. */
        scsi->data_size_from_cmnd = scsi->data_size;
        /* HSU addition */
        /* MSC CV Cases 7 and 13 support */
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: MSC CV Cases 7 and 13 are not supported\n"));
        /******* End HSU addition **************************/
    }

    /* Conflicting data directions is a phase error */
    if (scsi->data_dir != cmd_data->data_dir && scsi->data_size_from_cmnd > 0)
    {
        scsi->phase_error = 1;
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Phase error encountered. Data "
            "directions conflict.\n"));
        return JEINVAL;
    }

    /* Verify the length of the command itself */
    if (cmd_data->cmd_size != scsi->cmnd_size) 
    {
        scsi->phase_error = 1;
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Phase error encountered. Command sizes "
            "conflict. Should be %d but is %d.\n", cmd_data->cmd_size,
            scsi->cmnd_size));
        return JEINVAL;
    }

    /* If a unit attention condition exists, only INQUIRY and
     * REQUEST SENSE commands are allowed; anything else must fail. */
    if (curlun && curlun->sense_data != SS_NO_SENSE &&
        scsi->cmnd[0] != SC_INQUIRY &&
        scsi->cmnd[0] != SC_REQUEST_SENSE) 
    {
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Command not allowed. Only Inquiry and "
            "Sense commands are allowed.\n"));
        return JEINVAL;
    }

    /* Check that only command bytes listed in the mask are non-zero */
    scsi->cmnd[1] &= 0x1f;                      /* Mask away the LUN */
    for (i = 1; i < cmd_data->cmd_size; ++i) 
    {
        if (scsi->cmnd[i] && !(cmd_data->valid_fields & (1 << i))) 
        {
            if (curlun)
                curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
            DBG_W(DSLAVE_MS_SCSI, ("SCSI: Invalid field in CDB\n"));
            return JEINVAL;
        }
    }

    /* If media changed, set sense and fail the command */
    if (curlun && (scsi->cmnd[0] != SC_REQUEST_SENSE))
    {
        /* Inner IF to make sure vchanged is not called when the command
         * is request_sense */
        if (scsi->msfd_ops.vchanged(scsi->appctx, scsi->lun))
        {   
            /* HSU fix */
            /* Is the media read only? Currently we just assume it's not read only. */
            juint32_t ro = 0;
            /******* End HSU fix **************************/
            DBG_I(DSLAVE_MS_SCSI, ("SCSI: Medium changed, reported to "
                "host.\n"));
            scsi->msfd_ops.vget_media_details(scsi->appctx, scsi->lun, 
                &(curlun->block_size), &(curlun->num_blocks), &ro);
            curlun->ro = ro;
            curlun->sense_data = SS_NOT_READY_TO_READY_TRANSITION;
            /******** HSU Temp Fix  - CR B42897 ******************************/
            /* This prevents the failure in USBCV MSC Test 
            ** PowerUpTest_DeviceConfigured.
            */
            if (scsi->cmnd[0] != SC_INQUIRY)
            {
              return JEINVAL;
            }
            else
            {
              return 0;
            }
            /******* End HSU Temp Fix **************************/
        }
    }

    /* If the medium isn't mounted and the command needs to access it,
     * return an error. */
    if (curlun && !(scsi->msfd_ops.vready(scsi->appctx, scsi->lun)) &&
        cmd_data->needs_medium) 
    {
      DBG_I(DSLAVE_MS_SCSI, ("SCSI: Medium not present, reported to "
          "host.\n"));
      curlun->sense_data = SS_MEDIUM_NOT_PRESENT;
      return JEINVAL;
    }

    return 0;
}

/**
 * Function name:  send_error_reply
 * Description:    Sends an error reply, including cleanup of relevant
 *                 transfers.
 * Parameters: 
 *     @scsi: (IN) SCSI device context
 *
 * Return value: None
 * Scope: static
 **/
static void send_error_reply(scsi_dev_t *scsi)
{
    DBG_W(DSLAVE_MS_SCSI, ("---- ERROR encountered during processing of SCSI "
        "command %x.\n", scsi->cmnd[0]));

    /* Check to see if there is any data pending and read/write it */
    if (scsi->residue)
    {
        switch (scsi->data_dir)
        {
        case DATA_DIR_FROM_HOST:
            DBG_W(DSLAVE_MS_SCSI, ("SCSI: Flushing OUT pipe.\n"));
            /* If controller got any data from the host tell it to drop it,
             * we don't care */
            abort_out_pipe(scsi->fd);

            /* If the host wants to send more data, the STALL on the OUT pipe
             * will tell it to move on to the status stage */
            stall_out_pipe(scsi->fd);
            break;

        case DATA_DIR_TO_HOST:
            DBG_W(DSLAVE_MS_SCSI, ("SCSI: Clearing IN pipe residue %d\n",
                scsi->residue));
            /* If the host expects data, the STALL on the IN pipe will
             * tell it there is nothing interesting coming */
            stall_in_pipe(scsi->fd);
            break;

        default:
            /* Should not happen!
             * This means device expects a data transfer
             * but we don't know where to send the data. */
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MS_SCSI, err_in_cmd, residue_left_but_direction_unknown");             
            fatal_processing_error(scsi->fd);
            return;
        }
    }
    
    /* Send CSW with error */
    send_csw(scsi->fd, scsi->tag, scsi->residue, 
        (scsi->phase_error ? USB_STATUS_PHASE_ERROR : USB_STATUS_FAIL));
}

/**
 * Function name:  do_scsi_command
 * Description:    Handles a SCSI command received from the host.
 * Parameters: 
 *     @scsi_dev:  (IN) SCSI device context 
 *     @command:   (IN) SCSI command buffer
 *     @size:      (IN) Command size, in bytes
 *     @data_size: (IN) Size of data (in bytes) for the command's data stage
 *     @tag:       (IN) Tag for matching the CSW to the original CBW tag
 *     @lun:       (IN) LUN number
 *     @direction  (IN) Transfer direction: 0 = OUT / 1 = IN
 *     @buffer:    (IN) Buffer to be used for USB data transfers
 *
 * Return value: None
 * Scope:        Global
 **/
void do_scsi_command(void *scsi_dev, juint8_t *command, juint32_t size,
    juint32_t data_size, juint32_t tag, juint8_t lun,
    jbool_t direction, juint8_t *buffer)
{
    scsi_dev_t *scsi = (scsi_dev_t *)scsi_dev;
    jresult_t    rc = JEINVAL;
    juint32_t reply_len = 0; /* HSU addition: SCSI commands hook */

    scsi_command_data *cmd_data = cmd_data_vector[command[0]];

    j_memcpy(scsi->cmnd, command, size);
    scsi->cmnd_size     = (juint8_t)size;
    scsi->tag           = tag;
    scsi->phase_error   = 0;
    scsi->lun           = lun; 
    scsi->data_dir      = (direction ? DATA_DIR_TO_HOST : DATA_DIR_FROM_HOST);
    scsi->data_size     = data_size;
    scsi->residue       = scsi->data_size;

    DBG_V(DSLAVE_MS_SCSI, ("SCSI: Received command: %x, cmd size: %d, "
        "Data size: %d. LUN %d\n", scsi->cmnd[0], size, data_size, lun));

    dump_cdb(scsi);
    
    /* Retrieve the LUN */
    rc = scsi_get_lun(scsi);
    if (rc)
    {
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Invalid LUN.\n"));
        goto Exit;
    }

    /* HSU addition:*/
    /* SCSI IN command handler hook*/
    if ((scsi->msfd_ops.scsi_in_handler) &&
        ((scsi->data_dir == DATA_DIR_TO_HOST) || (scsi->data_size == 0)) &&
        (scsi->msfd_ops.scsi_in_handler(scsi->appctx, lun, command, size,
        data_size, buffer, SCSI_MAX_REQUEST_SIZE, &reply_len, &rc, !(cmd_data == NULL))))
    {
      if (rc)
      {
        goto Exit;
      }

      if (reply_len > 0)
      {
        /* we are not parsing the command so we don't know how to
             calculate data_size_from_cmnd, so assign data_size to
             it so the reply will not get truncated */
        scsi->data_size_from_cmnd = scsi->data_size;
        rc = scsi_write_data(scsi, reply_len, 0, scsi_standard_callback, USB_STATUS_PASS);
      }
      else
      {
        send_csw(scsi->fd, scsi->tag, scsi->residue, USB_STATUS_PASS);
      }
      goto Exit;
    }

    /*SCSI OUT command handler hook*/
    if ((scsi->msfd_ops.scsi_out_handler) &&
      (scsi->data_dir == DATA_DIR_FROM_HOST))
    {
      rc = JEINVAL;
      DBG_I(DSLAVE_MS_SCSI, ("SCSI: calling scsi_out_handler"));
      scsi->is_cmd_handled_by_app = 
        scsi->msfd_ops.scsi_out_handler(scsi->appctx, lun, command, size,
                                        data_size, &rc,!(cmd_data == NULL));
      if (scsi->is_cmd_handled_by_app)
      {
        if (!scsi->msfd_ops.scsi_out_data_stage_handler)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MS_SCSI, scsi_out_data_stage_handler, handler_is_null");              
          rc = JEINVAL;
        }
        if (!rc)
        {
          /* we are not parsing the command so we don't know how to
          *  calculate data_size_from_cmnd, so assign data_size to
          *  it so the reply will not get truncated */
          rc = scsi_get_data_from_usb(scsi);
        }
        goto Exit;
      }
    }
    /* End of HSU addition */

    /* Check that the command is legal */
    if (!cmd_data)
    {
        /* HSU addition: SCSI commands hook */
        if ((scsi->data_dir == DATA_DIR_TO_HOST) &&
            scsi->msfd_ops.unhandled_cmd_in)
        {
          /* we are not parsing the command so we don't know how to
             calculate data_size_from_cmnd, so assign data_size to
             it so the reply will not get truncated */
          scsi->data_size_from_cmnd = scsi->data_size;
          rc = scsi->msfd_ops.unhandled_cmd_in(scsi->appctx, lun, command, size,
            data_size, buffer, SCSI_MAX_REQUEST_SIZE, &reply_len);

          if (!rc)
          {
            rc = scsi_write_data(scsi, reply_len, 0, scsi_standard_callback, USB_STATUS_PASS);
            goto Exit;  
          }
        }
        /* HSU addition end: SCSI commands hook */

        /* Unknown command */
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Unknown SCSI command received.\n"));
        scsi->curlun->sense_data = SS_INVALID_COMMAND;
        rc = JEINVAL;
        goto Exit;
    }
    
    /* REQUEST SENSE in windows sets command size to 12, but it's 6 */
    if (command[0] == SC_REQUEST_SENSE)
        scsi->cmnd_size = 6;

    /* Verify command is legal */
    rc = verify_scsi_cmd(scsi, cmd_data);
    if (rc)
    {
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Command verification failed\n"));
        goto Exit;
    }

    /* Execute the correct SCSI handler */
    rc = cmd_data->handler(scsi, scsi->fd->cmd_request->buffer.vaddr);

    /* HSU addition: SCSI commands hook */
    if (scsi->msfd_ops.handled_cmd_ind)
    {
      scsi->msfd_ops.handled_cmd_ind(scsi->appctx, lun, command, size);
    }
    /* HSU addition end: SCSI commands hook */

Exit:
    if (rc)
        send_error_reply(scsi);
}

/**
 * Function name:  scsi_lun_enable
 * Description:    Enables the specified LUN.
 * Parameters: 
 *     @scsi: (IN) SCSI device context
 *     @lun:  (IN) LUN number
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t scsi_lun_enable(scsi_dev_t *scsi, juint8_t lun)
{
    struct lun *curlun = &scsi->luns[lun];
    jresult_t rc = 0;
    juint8_t removable;
    /* HSU fix */
    /* Is the media read only? Currently we just assume it's not read only. */
    juint32_t ro = 0;
    /******* End HSU fix **************************/

    if (scsi->luns[lun].enabled)
    {
        DBG_W(DSLAVE_MS_SCSI, ("SCSI: Enabling a LUN that is "
            "already enabled %d.\n", lun));
        return 0;
    }

    DBG_I(DSLAVE_MS_SCSI, ("SCSI: Enabling LUN %d.\n", lun));

    rc = scsi->msfd_ops.vopen(scsi->appctx, lun, &removable, disk_done, 
      (void *)scsi);
    if (rc)
    {
      return rc;
    }

    curlun->removable = removable ? 1 : 0;
    curlun->prevent_medium_removal = 0;

    /* HSU addition: CR 189027 */
    rc = scsi->msfd_ops.vget_media_details(scsi->appctx, lun, 
        &(curlun->block_size), &(curlun->num_blocks), &ro);
    if (rc)
    {
      DBG_I(DSLAVE_MS_SCSI, ("SCSI: Medium is enabled but not present.\n"));
    }
    /******* End HSU addition **************************/
    
    curlun->ro                  = ro ? 1 : 0;
    curlun->enabled             = 1;
    
    return 0;
}

/**
 * Function name:  scsi_lun_disable
 * Description:    Disables the specified LUN.
 * Parameters: 
 *     @scsi: (IN) SCSI device context
 *     @lun:  (IN) LUN number
 *
 * Return value: None
 * Scope:        Local
 **/
static void scsi_lun_disable(scsi_dev_t *scsi, juint8_t lun)
{
    jresult_t rc;

    DBG_I(DSLAVE_MS_SCSI, ("SCSI: Disabling LUN %d.\n", lun));
    
    if (!scsi->luns[lun].enabled)
        return;
    
    scsi->luns[lun].enabled = 0;
    rc = scsi->msfd_ops.vclose(scsi->appctx, lun);
    if (rc)
    {
        DBG_W(DSLAVE_MS_SCSI, ("scsi_lun_disable: Failed to close LUN %d\n",
            lun));
    }
}

/**
 * Function name:  scsi_enable
 * Description:    Enables all LUNs on the specified SCSI device.
 * Parameters: 
 *     @scsi_dev: (IN) SCSI device context 
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 **/
jresult_t scsi_enable(void *scsi_dev)
{
    jint_t i;
    jresult_t rc = 0;
    scsi_dev_t *scsi = (scsi_dev_t *)scsi_dev;

    DBG_I(DSLAVE_MS_SCSI, ("SCSI: Enabling\n"));

    scsi->curlun = NULL;
    
    for (i = 0; i < scsi->nluns; i++)
    {
        rc = scsi_lun_enable(scsi, (juint8_t)i);
        if (rc)
            break;
    }

    /* In case of error, disable all LUNs */
    /* HSU deletion: scsi_lun_disable is called in scsi_disable only 
    if (rc)
        scsi_disable(scsi);
    */

    TAILQ_INIT(&scsi->transfer.work_queue);
    
    DBG_I(DSLAVE_MS_TRANS, ("SCSI: Initialize %d transfer buffers\n",
        SCSI_TRANSFER_BUFFERS));
    
    for (i = 0; i < SCSI_TRANSFER_BUFFERS; i++)
    {
        job_buf_t *buffers = scsi->buffers;

        buffers[i].id = i;
        buffers[i].state = BUF_DONE;
        buffers[i].scsi = scsi;
        buffers[i].error = 0;
        buffers[i].buf_size =  scsi->fd->scsi_buffer_size; 
        buffers[i].io_request = scsi->fd->io_request[i];
#ifdef FEATURE_HS_USB_USE_MS_LA
        buffers[i].is_la_buf = FALSE;
#endif /* FEATURE_HS_USB_USE_MS_LA */
        DBG_I(DSLAVE_MS_TRANS, ("SCSI: Init buffer %d: Id %d State %d "
            "scsi %x error %d size %d req %x [%x]\n", i, buffers[i].id, 
            buffers[i].state, buffers[i].scsi, buffers[i].error, 
            buffers[i].buf_size, buffers[i].io_request, &(buffers[i])));
    
        TAILQ_INSERT_TAIL(&(scsi->transfer.work_queue), 
            &(buffers[i]), next);
    }

#ifdef FEATURE_HS_USB_USE_MS_LA
    DBG_W(DSLAVE_MS_TRANS, ("SCSI: Initialize %d lookahead buffers\n",
      SCSI_LOOKAHEAD_BUFFERS)); 

    for(i = 0; i<SCSI_LOOKAHEAD_BUFFERS; i++)
    {
      job_buf_t *buffer = scsi->la_buffer;

      buffer[i].id = i;
      buffer[i].state = BUF_DONE;
      buffer[i].scsi = scsi;
      buffer[i].error = 0;
      buffer[i].is_la_buf = TRUE;
      buffer[i].buf_size =  scsi->fd->scsi_buffer_size; 
      buffer[i].io_request = scsi->fd->la_io_request[i];
    }

    /* initialize lookahead statistics */
    la_statistics.all_read_req = 0;
    la_statistics.found_right_away = 0;
    la_statistics.found_and_not_ready = 0;
    la_statistics.didnt_find_at_all = 0;

    scsi->is_la_transfer_pending = FALSE;
#endif /* FEATURE_HS_USB_USE_MS_LA */
    if (is_statistics_update_supported)
    {
      init_statistics(&hsu_ms_disk_statistics.read);
      init_statistics(&hsu_ms_disk_statistics.write);
      init_statistics(&hsu_ms_usb_statistics.read);
      init_statistics(&hsu_ms_usb_statistics.write);
    }
    return rc;
}

/**
 * Function name:  scsi_disable
 * Description:    Disables all LUNs on the specified SCSI device.
 * Parameters: 
 *     @scsi_dev:  (IN) SCSI device context
 *
 * Return value: None
 * Scope:        Global
 **/
void scsi_disable(void *scsi_dev)
{
    jint_t i;
    scsi_dev_t *scsi = (scsi_dev_t *)scsi_dev;
    
    DBG_I(DSLAVE_MS_SCSI, ("SCSI: Disabling.\n"));

    /* HSU addition: CR 175102 fix */
    if (scsi->transfer.usb_job)
        scsi->transfer.disabled++;
    if (scsi->transfer.disk_job)
        scsi->transfer.disabled++;

#ifdef FEATURE_HS_USB_USE_MS_LA
    for (i=0; i<SCSI_LOOKAHEAD_BUFFERS; i++)
    { 
      /*check that there is not lookahead usb transfer going on*/
      if (scsi->next_transfer[i].usb_job)
      {
        scsi->next_transfer[i].disabled++;
      } 
      if (scsi->next_transfer[i].disk_job)
      {
        scsi->next_transfer[i].disabled++;
      } 
    }  
#endif /* FEATURE_HS_USB_USE_MS_LA */

    /******* End HSU addition **************************/
    scsi->curlun = NULL;

    for (i = 0; i < scsi->nluns; i++)
        scsi_lun_disable(scsi, (juint8_t)i);

#ifndef FEATURE_HS_USB_CORE_RESET_ON_DISC
	if (scsi->msfd_ops.msfd_app_uninit)
		scsi->msfd_ops.msfd_app_uninit(scsi->appctx);
#endif

}

/* Check that all required APIs are defined */

/**
 * Function name:  check_msfd_ops
 * Description:    Verifies that all necessary application callbacks are valid.
 * Parameters: 
 *     @ops: (IN)  Mass storage application callbacks structure
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static jresult_t check_msfd_ops(msfd_ops_t *ops)
{
    /* The following APIs are required */
    if (!ops->vlun_count         ||
        !ops->vget_desc          ||
        !ops->vready             ||
        !ops->vchanged           ||
        !ops->vmount             ||
        !ops->vunmount           ||
        !ops->vget_media_details ||
        !ops->vopen              ||
        !ops->vclose             ||
        !ops->vread              ||
        !ops->vwrite             ||
        !ops->vverify            ||
        !ops->vcancel            ||
        !ops->vflush             ||
        !ops->vallow_cache)
        return 1;
    return 0;
}

/**
 * Function name:  scsi_init
 * Description:    Initializes the SCSI layer.
 * Parameters: 
 *     @fd:      (IN)  FD context
 *     @luns:    (OUT) Number of LUNs
 *     @context: (OUT) SCSI device context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 **/
jresult_t scsi_init(fd_storage_t *fd, juint8_t *luns, void **context)
{
    scsi_dev_t *scsi;
    jresult_t rc;
    juint8_t lun_count = 0;
    msfd_init_info_t info;

    scsi = jmalloc(sizeof(scsi_dev_t), M_ZERO);
    if (!scsi)
        return JENOMEM;
    
    scsi->fd = fd;

    info.interface_number = UWD_UNDEFINED_INTERFACE;
    info.scsi_buffer_size = 0;
    /* HSU addition: Interface string */
    info.interface_string = NULL;
    /* End of HSU addition */
    rc = msfd_app_init(&(scsi->msfd_ops), &(scsi->appctx), &info);
    if (rc || check_msfd_ops(&(scsi->msfd_ops)))
        return JEINVAL;

    fd->fd_desc->interfaces[0].number = info.interface_number;
    if (info.scsi_buffer_size)
        fd->scsi_buffer_size = info.scsi_buffer_size;
    else
        fd->scsi_buffer_size = SCSI_BUFFER_SIZE;
    /* HSU addition: Interface string */
    fd->fd_desc->interfaces[0].if_string = core_add_string_descriptor(fd->core_ctx,
      info.interface_string, HSU_STR_IDX_NOT_SET);
    /* HSU addition end */

    rc = scsi->msfd_ops.vlun_count(scsi->appctx, &lun_count);
    if (rc)
        return JEINVAL;
   
    *luns = lun_count;
    DBG_I(DSLAVE_MS_SCSI, ("SCSI: Initializing with %d LUNs.\n", *luns));

    scsi->nluns = *luns;
    scsi->luns  = jmalloc(sizeof(struct lun) * (*luns), M_ZERO);
    if (!scsi->luns)
        return JENOMEM;

#ifdef FEATURE_HS_USB_USE_MS_LA
    scsi->is_la_transfer_pending = FALSE;
#endif /* FEATURE_HS_USB_USE_MS_LA */

    *context = scsi;
    
    return 0;
}

/**
 * Function name:  scsi_uninit
 * Description:    Un-initializes the SCSI layer.
 * Parameters: 
 *     @scsi_dev: (IN) SCSI device context
 *
 * Return value: None
 * Scope:        Global
 **/
void scsi_uninit(void *scsi_dev)
{
    scsi_dev_t *scsi = (scsi_dev_t *)scsi_dev;
    
    DBG_I(DSLAVE_MS_SCSI, ("SCSI: Uninitializing.\n"));

    if (!scsi)
        return;

    /* HSU addition: Interface string */
    if (scsi->fd->fd_desc->interfaces[0].if_string)
    {
      core_release_string_descriptor(scsi->fd->core_ctx,
        scsi->fd->fd_desc->interfaces[0].if_string);
    }
    /* HSU addition end */
    if (scsi->msfd_ops.msfd_app_uninit)
        scsi->msfd_ops.msfd_app_uninit(scsi->appctx);

    if (scsi->luns)
    {
        jfree(scsi->luns);
        scsi->luns = NULL;
    }

    jfree(scsi);
}
#ifdef FEATURE_HS_USB_MS_BAM
 /* HSU addition: SPS */
/**
 * Function name:  update_sps_info
 * Description:    Updating luns data structures in SPS info. The
 *								 function is called from hsu_al_ms_get_sps_info_for_inserted_device()
 *                 whether we succeeded in enabling SPS or not.
 * Parameters: 
 *     @handler:        (IN) SCSI device context
 *     @lun:						(IN) lun number
 *		 @sps_mode:				(IN) SPS mode TRUE/FALSE
 *		 @in_sps_pipe_id: (IN) in SPS pipe
 *		 @out_sps_pipe_id:(IN) out SPS pipe
 *
 * Return value: None
 * Scope:        Global
 **/
void update_sps_info(void *handler, juint8_t lun_idx, jbool_t sps_mode,
  juint8_t in_sps_pipe_id, juint8_t out_sps_pipe_id, completion_callback* lun_completion_cb)
{
	scsi_dev_t *scsi;
	struct lun *curlun;
	
	if (handler)
	{
		scsi = (scsi_dev_t *)handler;
		curlun = &scsi->luns[lun_idx];
		if (curlun)
		{
			curlun->sps_mode        = sps_mode;
			curlun->in_sps_pipe_id  = in_sps_pipe_id;
			curlun->out_sps_pipe_id = out_sps_pipe_id;
			if (sps_mode)
			{
				*lun_completion_cb = disk_done_sps;
			}
			else
			{
				*lun_completion_cb = disk_done;
			}
		}
	}
}
#endif /* FEATURE_HS_USB_MS_BAM */
/* End of HSU addition: SPS */
