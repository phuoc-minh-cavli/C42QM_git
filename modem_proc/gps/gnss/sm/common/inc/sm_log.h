#ifndef _SM_LOG_H
#define _SM_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   LOCATION SERVICES MANAGER LOGGING MODULE

GENERAL DESCRIPTION
  This file contains function prototypes for logging

  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/inc/sm_log.h#2 $ 
  $DateTime: 2020/07/28 13:22:35 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/17/15   jv      Added QZSS Support
08/08/14   jv      Added Galileo Support
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos)   
04/21/12   rk      Added new RRC GANSS protocol version id to distinguish in 0x1386/87 log.
04/23/12   ssu     Adding sm_log_get_best_avail_pos_info, sm_log_report_ext_status_best_avail_pos_info  
03/12/12   rh      Added version 2 for CPI log
4/02/10    gk      Added a event callback to inject position
01/29/10   pn      Updated DmLog structs with PACKED_POST.
09/21/09   rb      added a log for Eph download
04/02/09   ns      support for Qwip logs.
01/16/09   atien   XTRA-T support
07/25/08   gk      Log was not properly coded
06/09/08   gk      On-Demand Changes to add some logs and events
2/15/07    gk      Changed notification flags to  uint16
2/06/07    gk      Fixed notification parameters logs, lint errors, warnings 
1/11/07    gk      Added field for delete parms log
12/04/06   gk      Updated position report log to reflect the spec, minor mods
11/8/06    gk      Initial checkin for converged GPS

===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */

#include "pdapiclient_lite.h"

#include "log.h"
#include "log_codes.h"
#include "sm_api.h"
#include "gnss_common.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif


/* link types */

#define POSITION_SOURCE_GPS         0x0001
#define POSITION_SOURCE_SFT         0x0004
#define POSITION_SOURCE_OLDNEW      0x0008
#define POSITION_SOURCE_DEFAULT     0x0010
#define POSITION_SOURCE_KF_WLS      0x0040
#define POSITION_SOURCE_GLO         0x0080
/* TODO above flag in log packet is only 8 bit! how to add BDS? RH */
/* TODO add GAL*/

#define SM_LOG_TIME_MASK_SOURCE_GPS                   (0x1<<0)
#define SM_LOG_TIME_SOURCE_MASK_GLO                   (0x1<<1)
#define SM_LOG_TIME_MASK_GLO_GPS_TIMEOFFSET_VALID     (0x1<<2)
#define SM_LOG_TIME_SOURCE_MASK_BDS                   (0x1<<3)
#define SM_LOG_TIME_SOURCE_MASK_GAL                   (0x1<<4)

#ifdef FEATURE_CIRC_BUF_F3
#define SM_LOG_CIRC_BUF_F3_MAX                   30
#endif /*#ifdef FEATURE_CIRC_BUF_F3*/

#ifdef FEATURE_CIRC_BUF_IPC
#define SM_LOG_CIRC_BUF_TIME_STAMP_LEN           12
#define SM_LOG_CIRC_BUF_IPC_MAX                  30
#endif /*#ifdef FEATURE_CIRC_BUF_IPC*/


/* extract specified byte from given data
Parameters:
byte_nume: zero based byte number to extract
data: data from which byte needs to be extracted

NOTE: data is converted to uint32, so meaningful range
for byte_num is [0,3] inclusive. 

E.g. usage:

SM_LOG_EXTRACT_BYTE (0x1, 0xDeadBabe)

returns 0xBA.
*/

#define SM_LOG_EXTRACT_BYTE(byte_num, data) ((uint8)\
   (( ((uint32)(data)) >> ((byte_num)<<3) ) & 0xff) \
)

//--------------------------------------------------------------------------------------
// Delete Params that are included from pdsm_delete_parms_type.
// If pdsm_delete_parms_type is changed, DM log structures shall be updated accordingly. 
// Note: pdsm_delete_parms_type contains more information than what is written in the 
// DM log. Fields that are marked with "-" are excluded from the log. This should be taken
// into account while updating SM_LOG_LENGTH_PA_EVENT_DELETE_PARAMS below.
// Fields that are marked with "+" are included in payload but set to zero in this version.
//--------------------------------------------------------------------------------------
// uint64 pdsm_delete_parms_flags;  /* 64 bits now with BDS support */
// uint32 time_unc; /* time unc in msec */
// uint32 pos_unc;  /* pos unc in meters */
// uint32 time_offset; /* time offset in msec*/
// uint32 pos_offset; /* pos offset in meters. currently not supported */
// uint32 eph_sv_mask; /* delete targeted GPS*/
// uint32 alm_sv_mask; /* delete targeted GPS*/
// uint32 glo_eph_sv_mask; /* delete targeted GLO eph */
// uint32 glo_alm_sv_mask; /* delete targeted GLO ALM */
// uint32 sbas_eph_sv_mask; /* delete targeted SBAS EPH */
// uint32 sbas_alm_sv_mask; /* delete targeted SBAS ALM */
// uint64 bds_eph_sv_mask; /* delete targeted BDS EPH */
// uint64 bds_alm_sv_mask; /* delete targeted BDS ALM */
// uint64 gal_eph_sv_mask; /* delete targeted GAL EPH */
// uint64 gal_alm_sv_mask; /* delete targeted GAL ALM */
// uint32 qzss_eph_sv_mask; /* delete targeted QZSS eph */
// uint32 qzss_alm_sv_mask; /* delete targeted QZSS alm*/
// uint32 gps_gnss_sv_blacklist_mask;  /* delete targeted GPS SV in GNSS SV blacklist */
// uint32 glo_gnss_sv_blacklist_mask;  /* delete targeted GLO SV in GNSS SV blacklist */
// uint64 bds_gnss_sv_blacklist_mask;  /* delete targeted BDS SV in GNSS SV blacklist */
// uint64 gal_gnss_sv_blacklist_mask;  /* delete targeted GAL SV in GNSS SV blacklist */
// uint32 qzss_gnss_sv_blacklist_mask; /* delete targeted QZSS SV in GNSS SV blacklist */
// uint32 delete_celldb_mask; /* delete cell db */
// uint32 delete_clock_info_mask; /* must set PDSM_PA_DELETE_CLOCK_INFO.
//                                see PDSM_PA_DELETE_CLOCK_INFOxxx for 
//                                 permitted flags */
//- float f_time_unc; /* time unc in msec, can accept sub-ms values in the decimal point*/
//- float f_time_offset; /* time offset in msec can accept submeter value in the decimal point */
//- float f_pos_unc;  /* pos unc in meters. can accept submeter values in the decimal point */

//--------------------------------------------------------------------------------------
//set according to above. DM Log does not contain all fields from pdsm_delete_parms_type.
#define SM_LOG_LENGTH_PA_EVENT_DELETE_PARAMS \
        ( sizeof(pdsm_lite_delete_parms_type) - sizeof(float) * 3 \
         - sizeof(uint32) + sizeof(uint64) * 3 ) /* bytes */

#define SM_LOG_LENGTH_MAX_SM_ERROR_MESSAGE    (255)

#define SM_LOG_WRITE_ERROR_MSG(err_msg, severity) sm_log_error_message((err_msg), (severity), __FILENAME__, __LINE__)

#ifdef FEATURE_CIRC_BUF_F3
#define SM_LOG_CIRC_BUF_F3_WRITE(fine_name, line_number, num_arg, arg1, arg2, arg3, arg4) \
    sm_log_write_F3_to_circ_buf((fine_name), (line_number), (num_arg), (arg1), \
                      (arg2), (arg3), (arg4))

#define MSG_CIRC_BUF_1(module ,level,format,arg1)\
  do { \
    /*lint -e506 -e774*/ \
    SM_LOG_CIRC_BUF_F3_WRITE(__FILE__, __LINE__, (SM_LOG_NUM_ARGS_1), (arg1), (0), (0), (0));\
  } while (0)
  
#define MSG_CIRC_BUF_2(module ,level,format,arg1,arg2)\
  do { \
    SM_LOG_CIRC_BUF_F3_WRITE(__FILE__, __LINE__, (SM_LOG_NUM_ARGS_2), (arg1), (arg2), (0), (0));\
  } while (0) 
  
#define MSG_CIRC_BUF_3(module ,level,format,arg1,arg2,arg3)\
  do { \
    /*lint -e506 -e774*/ \
     SM_LOG_CIRC_BUF_F3_WRITE(__FILE__, __LINE__, (SM_LOG_NUM_ARGS_3), (arg1), (arg2), (arg3), (0));\
   } while (0)
#endif /*#ifdef FEATURE_CIRC_BUF_F3*/

#ifdef FEATURE_CIRC_BUF_IPC 
#define SM_LOG_CIRC_BUF_IPC_WRITE(arg) \
      sm_log_write_IPC_to_circ_buf(arg)

#define MSG_CIRC_BUF_IPC(arg)\
do { \
  SM_LOG_CIRC_BUF_IPC_WRITE((arg));\
} while (0)
#endif /*#ifdef FEATURE_CIRC_BUF_IPC */

/* Structure definitions */
typedef struct
{
  uint8   u_EventId;
  uint8   u_EventData;
  uint8   u_PacketVersion;
  uint8   u_Reserved2;
} sm_report_evt_type;
typedef PACKED struct PACKED_POST
{
  uint32 q_IPAddr;
  uint32 q_PortId;
} sm_log_pdsm_ipv4_addr;

typedef PACKED struct PACKED_POST
{
  uint16 w_IPAddr[8];
  uint32 q_PortId;
} sm_log_pdsm_ipv6_addr;

typedef PACKED union PACKED_POST
{
  uint32 q_Value;
  sm_log_pdsm_ipv4_addr z_IPV4Addr;
  sm_log_pdsm_ipv6_addr z_IPV6Addr;
  char   u_Url[255];
  uint8  delete_params[28];
} sm_log_pdsm_set_parameters_value;

typedef PACKED struct PACKED_POST
{
  uint16 w_SvId;
  uint16 w_CNo;
  uint32 q_Latency;
  uint32 q_Azimuth;
  uint32 q_Elevation;
  uint8 u_Status;
}  sm_ext_stat_meas_report_data ;

#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(push, 1)
#endif /* #ifdef FEATURE_CGPS_PLAYBACK */

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8  u_PktVersion;
  uint32 q_ClientId;
  uint32 q_opt_field_mask; 
  uint32 q_time_info_type;
  uint32 q_timestamp_sec; 
  uint32 q_alt_def; 
  uint32 q_source;
  DBL    d_latitude;  /* Lat.: in degrees -90.0 to 90.0, positive for northern */
  DBL    d_longitude; /* Long.: in degrees -180.0 to 180.0, positive for eastern */ 
  FLT    f_loc_unc_hor;   /* horizontal location unc.: meters */
  FLT    f_altitude; /* alt: meters, positive height, negative depth  */ 
  FLT    f_loc_unc_vert;   /* loc unc vertical: meters */
  uint8  u_confidence_hor;  /* horizontal confidence */
  uint8  u_confidence_vert; /* vertical confidence */
  uint32 q_Reserved[4];
} LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C_type;

typedef PACKED struct PACKED_POST
{
   uint16 w_Sv;                     /* SV PRN */
   uint16 w_Iode;                   /* Issue of data Ephemeris tag */

} sm_PackedSvEphIodeStructType;

typedef PACKED struct PACKED_POST
{
   uint8  u_TestDecision; // 0 = req, 1 = reject
   uint32 q_EphHaveList;
   uint32 q_VisibleSVList;
   uint32 q_SvNoExistMask;
   uint8 u_min_eph_have_threshold;
   uint8 u_max_eph_need_threshold;
   uint8 u_num_eph_have_known_in_view;
   uint8 u_num_eph_need_known_in_view;
   uint8 u_sv_cnt;
   uint16 w_OldestToe;
   uint16 w_GpsWeek;
   sm_PackedSvEphIodeStructType z_SvEphIode[32];
} sm_log_eph_download_data;


#if defined(FEATURE_CIRC_BUF_IPC) || defined(FEATURE_CIRC_BUF_F3)
typedef struct
{
  uint64 t_time_hr;
  uint64 t_time_min;
  uint64 t_time_sec;
  uint64 t_time_msec;
  uint64 t_time_days;
}sm_log_circ_buf_entry_timestamp;
#endif /*#if defined(FEATURE_CIRC_BUF_IPC) || defined(FEATURE_CIRC_BUF_F3)*/


#ifdef FEATURE_CIRC_BUF_F3
typedef struct
{
  void*     p_msg_link[SM_LOG_CIRC_BUF_F3_MAX];  /* array of char pointer */
  uint32_t  q_curr_buf_index; /*index of array where current msg will be pushed*/
  uint16_t  w_heap_tracker; /*to track heap size*/
}sm_log_circ_buf_f3;
#endif /*#ifdef FEATURE_CIRC_BUF_F3*/

#ifdef FEATURE_CIRC_BUF_IPC
typedef struct
{
  uint8_t  u_timestamp[SM_LOG_CIRC_BUF_TIME_STAMP_LEN];  /*timestamp in format hr:min:sec:msec */
  uint32_t q_ipc; /*enum value */
}sm_log_ipc;

typedef struct
{
  sm_log_ipc  z_ipc_info[SM_LOG_CIRC_BUF_IPC_MAX]; /*array of ipc msg*/
  uint32_t    q_curr_buf_index; /*index of array where current msg will be pushed*/
}sm_log_circ_buf_ipc;
#endif /*#ifdef FEATURE_CIRC_BUF_IPC*/

#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(pop)
#endif /* #ifdef FEATURE_CGPS_PLAYBACK */

/************************LOG events*********************************************/
typedef enum
{
  SM_LOG_EVENT_ID_GPS_COMPLETE = 7,   /*  Receiver Result Event */
  SM_LOG_EVENT_ID_POST_ASSISTANCE_DATA_COMM = 8    /* Post Assistance Data communication */
} sm_log_event_id;

typedef enum
{
  SM_LOG_EVENT_DATA_POST_ASSISTANCE_DATA_COMM_START = 0,  
  SM_LOG_EVENT_DATA_POST_ASSISTANCE_DATA_COMM_END   = 1
} sm_log_event_data_e_type;

typedef enum  {
  PD_EVENT_END,
  PD_EVENT_BEGIN,
  PD_EVENT_UNUSED,
  PD_EVENT_DONE,
  PD_EVENT_GPS_BEGIN,
  PD_EVENT_GPS_END,
  PD_EVENT_UPDATE_FAILURE,
  PD_EVENT_DLOAD_BEGIN,
  PD_EVENT_DLOAD,
  PD_EVENT_DLOAD_DONE,
  PD_DLOAD_EVENT_END,
  PD_DLOAD_INVALID,
  PD_EVENT_COARSE_POS_INJ_DONE,   
  PD_EVENT_COARSE_POS_INJ_FAILED,
  PD_EVENT_COARSE_POS_INJ_ERROR,
  PD_EVENT_NW_CONNECTING, // Communication events for NW connection status
  PD_EVENT_NW_CONNECTED,
  PD_EVENT_SERVER_CONNECTING, // Communication events for server connection status
  PD_EVENT_SERVER_CONNECTED,
  PD_EVENT_UNDEF_FOR_LOG
}pd_event_e_type;

#ifdef FEATURE_CIRC_BUF_F3
typedef enum 
{
  SM_LOG_CIRC_BUF_ARGS_NUM_1 = 1,
  SM_LOG_CIRC_BUF_ARGS_NUM_2,
  SM_LOG_CIRC_BUF_ARGS_NUM_3,
  SM_LOG_CIRC_BUF_ARGS_NUM_4
} sm_log_args;
#endif /*#ifdef FEATURE_CIRC_BUF_F3*/

LOG_RECORD_DEFINE(LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C)
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint8 u_EventType; 
  uint8 u_EventData;
  uint8 u_Reserved[7];
LOG_RECORD_END

extern void sm_log_sm_event( sm_report_evt_type *p_EvtInfo );

extern void sm_log_event_report( uint8 u_EventId, uint8 u_EventData );

extern void sm_log_get_position_request(pdsm_lite_cmd_s_type*  cmd_ptr);

extern void sm_log_set_parameters_request( pdsm_lite_cmd_s_type*  cmd_ptr);
extern void sm_log_end_session(pdsm_lite_client_id_type          client_id);


/*
 ******************************************************************************
 * sm_log_report_position
 *
 * Function description:
 *
 * This function generates a DM log message when position report is recvd by the
 * PDAPI
 *
 * Parameters: 
 * pointer to sm_pd_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
extern void sm_log_report_position(pdsm_client_id_type client_id,
                               sm_FixInfoStructType * input_lr_buf_ptr, 
                               boolean sft, boolean kf);


/*
 ******************************************************************************
 * sm_log_pd_event_callback
 *
 * Function description:
 *
 * This function generates a DM log message when a PD_Event_Callback other than
 * a position report is recvd PDAPI
 *
 * Parameters: 
 * pdsm_pd_event_type
 * pointer to pdsm_pd_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

extern void sm_log_pd_event_callback(pdsm_pd_event_type pd_event, 
                            const pdsm_lite_pd_info_s_type *pd_info_ptr);



/*
 ******************************************************************************
 * sm_log_pa_event_callback
 *
 * Function description:
 *
 * This function generates a DM log message when a PA_Event_Callback 
 * is recvd PDAPI
 *
 * Parameters: 
 * sm_pa_event_type
 * pointer to sm_pa_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
extern void sm_log_pa_event_callback(pdsm_lite_pa_event_type pa_event, 
                            const pdsm_lite_pa_info_s_type *pa_info_ptr);
                           
/*
 ******************************************************************************
 * sm_log_pd_cmd_err
 *
 * Function description:
 *
 * This function generates a DM log message when a pd_cmd_err
 * is recvd by PDAPI
 *
 * Parameters: 
 * client_id
 * command_type
 * err_value
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_pd_cmd_err(uint32 client_id, 
                        pdsm_lite_pd_cmd_e_type cmd_type, 
                        pdsm_lite_pd_cmd_err_e_type cmd_err_type);
/*
 ******************************************************************************
 * sm_log_pa_cmd_err
 *
 * Function description:
 *
 * This function generates a DM log message when a pa_cmd_err
 * is recvd by PDAPI
 *
 * Parameters: 
 * client_id
 * command_type
 * err_value
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_pa_cmd_err(uint32 client_id, 
                        pdsm_lite_pa_cmd_e_type cmd_type, 
                        pdsm_lite_pa_cmd_err_e_type cmd_err_type);

enum {
  POS_FROM_MGP,
  POS_FROM_OEM
};
/*
 ******************************************************************************
 * sm_log_diag_position
 *
 * Function description:
 *
 * This function generates a DM log message when diagnostic position report is recvd by the
 * PDAPI
 *
 * Parameters: 
 * client_id - client_id 
 * pd_info_ptr - pointer to position
 * pos_from_oem - 1 if the position is received from the OEM, 0 if the position is from position engine. 
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_diag_position(uint32 client_id,
                           sm_FixInfoStructType *lr_data,
                           uint8 pos_from_oem,
                           boolean sft, boolean kf);
 /*
 ******************************************************************************
 * sm_log_ext_nmea_position_report
 *
 * Function description:
 *
 * This function generates a DM log message when nmea report is sent to the
 * PDAPI
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
extern void sm_log_nmea_position_report(uint32 l_ClientId,
                           const pdsm_lite_pd_nmea_s_type *p_ExtNMEAType);

/*
 ******************************************************************************
 * sm_log_extern_coarse_pos_inj_start
 *
 * Function description:
 *
 * This function generates a DM log message when external coarse position 
 * injection is started
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_extern_coarse_pos_inj_start(pdsm_lite_cmd_s_type*  p_cmd_type);

/*
 ******************************************************************************
 * sm_log_get_gps_state_info
 *
 * Function description:
 *
 * This function generates a DM log message when the receiver state info is
 * reported to PDAPI
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_get_gps_state_info( uint32 q_ClientId);


/*
 ******************************************************************************
 * sm_log_get_best_avail_pos_info
 *
 * Function description:
 *
 * This function generates a DM log message when the best available position info
 * is reported to PDAPI from the measurement engine.
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_get_best_avail_pos_info( uint32 q_ClientId);

/**
* writes out a given uint32 as little endian to given byte array
* @param pOutputByteArray
   array where data will be written out to
* @param data 
   uint32 data to be written out
* @returns boolean
   false if pOutputByteArray == NULL. True otherwise.
  @notes Caller must ensure that pOutputByteArray has enough space
*/
boolean sm_log_write_uint32_little_endian(uint8 *pOutputByteArray, uint32 data);

/** logs an error message generated within SM.
 * 
 * @param p_error_msg
 * pointer to error message string to be outputted.
 * 
 * @param u_severity
 *  user defined numeric value denoting severity of error (maybe
 *  meaningful only to SM)
 * 
 * @param p_filename
 * pointer to filename where error message occured
 * 
 * @param q_linenum
 * line number of error
 */
 
/*
 ******************************************************************************
 * sm_log_report_ext_status_gps_state_info
 *
 * Function description:
 *
 * This function generates a DM log message when the GPS state info is
 * reported to PDAPI by SM.
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_report_ext_status_gps_state_info(uint32 q_ClientId, 
                           const pdsm_lite_pd_gps_state_s_type *p_ExtGpsStateType);

/*
 ******************************************************************************
 * sm_log_report_ext_status_best_avail_pos_info
 *
 * Function description:
 *
 * This function generates a DM log message when the best available position info is
 * reported to PDAPI by SM.
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_report_ext_status_best_avail_pos_info(uint32 q_ClientId, 
                           const qmiLocGetBestAvailablePositionIndMsgT_v02 *p_ExtBestAvailPosType);
void sm_log_error_message(const char* p_error_msg, 
                          uint8 u_severity, 
                          const char* p_filename, 
                          uint32 q_linenum);

#if defined(FEATURE_CIRC_BUF_F3) || defined(FEATURE_CIRC_BUF_F3)

/*===========================================================================

FUNCTION sm_log_circ_buf_get_timestamp

DESCRIPTION
  This function is to fetch time from 6 Jan 1980 00:00:00 in format days,hr,sec,msec

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_circ_buf_get_timestamp(sm_log_circ_buf_entry_timestamp* pz_timestamp);
#endif /*#if defined(FEATURE_BUFFER_IPC) || defined(FEATURE_BUFFER_F3)*/

#ifdef FEATURE_CIRC_BUF_F3
/*===========================================================================

FUNCTION sm_log_circ_buf_get_file_name

DESCRIPTION
  This function will extract filename from the full pathname and encodes the filename

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_circ_buf_get_file_name(char* p_path_name, char* p_file_name, uint32 q_size);

/*===========================================================================

FUNCTION sm_log_circ_buf_push

DESCRIPTION
 This function will push log into the buffer to f3 buffer or ipc buffer

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_circ_buf_push(char* pc_buffer);

/*===========================================================================

FUNCTION sm_log_write_F3_to_circ_buf

DESCRIPTION
  This function will push a log into a buffer

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_write_F3_to_circ_buf(char* p_path_name, uint32 q_line, sm_log_args num_arg, int32 l_arg1, int32 l_arg2, int32 l_arg3, int32 l_arg4);
#endif /*#ifdef FEATURE_BUFFER_F3*/


#ifdef FEATURE_CIRC_BUF_IPC
/*===========================================================================

FUNCTION sm_log_write_IPC_to_circ_buf

DESCRIPTION
  This function will push a log into a ipc buffer

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_write_IPC_to_circ_buf (uint32 q_arg);
#endif /*#ifdef FEATURE_BUFFER_IPC*/

#ifdef __cplusplus
}
#endif
#endif /* _LSM_LOG_H */
