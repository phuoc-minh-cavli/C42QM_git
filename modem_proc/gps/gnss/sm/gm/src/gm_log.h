/*===========================================================================

                  GM_LOG Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM Log.

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/13/15   ss     Geofencing Stats support
12/10/14   ssu    DM log for GM Add, Delete, Purge and breach operations.
10/04/13   ss   Initial version
===========================================================================*/
#ifndef GM_LOG_H
#define GM_LOG_H

#include "gm_core.h"

#if defined(T_WINNT)
#error code not present
#endif


#define GM_MSG_HIGH(str,a,b,c)  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,str,a,b,c);
#define GM_MSG_MED(str,a,b,c)   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,str,a,b,c);
#define GM_MSG_LOW(str,a,b,c)   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,str,a,b,c);
#define GM_MSG_ERROR(str,a,b,c) MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,str,a,b,c);
#define GM_MSG_FATAL(str,a,b,c) MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_FATAL,str,a,b,c);


#define GM_MSG_9(level,str,a,b,c,d,e,f,g,h,i) \
MSG_9(MSG_SSID_DFLT,level,str,a,b,c,d,e,f,g,h,i);
#define GM_MSG_8(level,str,a,b,c,d,e,f,g,h)  \
MSG_8(MSG_SSID_DFLT,level,str,a,b,c,d,e,f,g,h);

#define GM_MSG_7(level,str,a,b,c,d,e,f,g)  \
MSG_7(MSG_SSID_DFLT,level,str,a,b,c,d,e,f,g);

#define GM_MSG_6(level,str,a,b,c,d,e,f)  \
MSG_6(MSG_SSID_DFLT,level,str,a,b,c,d,e,f);
#define GM_MSG_5(level,str,a,b,c,d,e)  \
MSG_5(MSG_SSID_DFLT,level,str,a,b,c,d,e);

#define GM_MSG_4(level,str,a,b,c,d)  \
MSG_4(MSG_SSID_DFLT,level,str,a,b,c,d);

/****************** Data Structures*********************************************/
typedef enum
{
  GEOFENCE_MOTION_DETECTOR_NONE = 0,
  GEOFENCE_MOTION_DETECTOR_AMD_RMD_PED = 0x1,
  GEOFENCE_MOTION_DETECTOR_CMC = 0x2,
  GEOFENCE_MOTION_DETECTOR_WIFI = 0x4,
  GEOFENCE_MOTION_DETECTOR_WWAN = 0x8,
  GEOFENCE_MOTION_DETECTOR_SMD = 0x10,
} geofence_motion_detector_type;

typedef PACKED struct PACKED_POST
{
  uint8 detectors_used; /*Motion detector used*/
  uint32 distance_to_check; /*Distance for motion check*/
} geofence_motion_detection_start_rpt;

typedef PACKED struct PACKED_POST
{
  uint8 motion_detector; /*Motion detector*/
} geofence_motion_detection_stop_rpt;

typedef PACKED struct PACKED_POST
{
  uint8 motion_detector; /*Motion detector*/
} geofence_motion_detection_distance_met_rpt;

typedef PACKED struct PACKED_POST
{
  uint8 motion_detector; /*Motion detector*/
} geofence_motion_detection_error_rpt;

typedef PACKED struct PACKED_POST
{
  uint8 motion_detector; /*Motion detector*/
  uint32 distance_accumulated;   /*Distance accumulated thus far*/
} geofence_motion_detection_distance_accum_rpt;

typedef enum
{
  GEOFENCE_MOTION_DETECTION_START_RPT = 0x1,
  GEOFENCE_MOTION_DETECTION_STOP_RPT,
  GEOFENCE_MOTION_DETECTION_ERROR_RPT,
  GEOFENCE_MOTION_DETECTION_DISTANCE_MET_RPT,
  GEOFENCE_MOTION_DETECTION_DIST_ACCUM_RPT
} geofence_motion_report_type;

typedef PACKED struct PACKED_POST
{
  uint8 report_type;
  union
  {
   geofence_motion_detection_start_rpt start_rpt;   /* Motion Start Report */
   geofence_motion_detection_stop_rpt stop_rpt;   /* Motion Stop Report */
   geofence_motion_detection_error_rpt error_rpt; /* Motion Error Report */
   geofence_motion_detection_distance_met_rpt distance_met_rpt; /* Distance Met Report */
   geofence_motion_detection_distance_accum_rpt dist_accum_rpt; /* Distance Accumulated Report */
  } motion_rpt_type;
} geofence_motion_detection_rpt;

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;
  geofence_motion_detection_rpt motion_detection_rpt;
}geofence_motion_detection_log_rpt;


/* Wifi is used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_WIFI_MASK 0x0001
/* Sensors are used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_SENSORS_MASK 0x0002
/* Satellite is used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_SATELLITE_MASK 0x0004
/* Cell Id is used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_CELLID_MASK 0x0008
/* Coarse position is used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_COARSE_POS_MASK 0x0010


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;         /* Version number of DM log */
  uint32      q_Fcount;          /* Local millisecond counter */
  uint32      q_count;

  uint8             pos_fix_src;        /*Position Fix src*/
  double            latitude;          /*Latitude in Degrees*/
  double            longitude;         /*Longitude in Degrees*/
  double            altitude;          /*Altitude*/
  double            pos_unc_2d;        /*2D position uncertainity*/
  uint8             pos_unc_2d_conf;   /*2D position uncertainity confidence*/
  float             heading_radians;   /*Heading in Radians*/
  uint16            w_GpsWeek;         /* GPS week number of fix */
  uint32            q_GpsTimeMs;       /* GPS time in week (milliseconds) of fix */
  uint64            t_source_used_for_fix_gen_mask; /* The sources used for fix generation */
}geofence_position_log_rpt;


#define GM_WWAN_MOT_CLASS_LOG_MAX_CELLS  50

typedef PACKED struct PACKED_POST
{
  uint8  is_active_set_info_incl;
  uint8  is_active_set;
  uint32 cell_freq;
  uint32 cell_psc;
  uint32 cell_rsp;
  uint8  is_cell_ec_no_incl;
  uint32 cell_ec_no;
  uint8  is_delta_rscp_incl;
  uint32 delta_rscp;
  uint8 is_cell_rsrq_incl;
  uint32 cell_rsrq;
} gm_meas_cell_info_log_s;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  /* Client handle */
  sm_gm_client_handle   z_client_handle;

  /*  Transaction ID */
  uint32 q_tid;
    
  /* Breach Event Mask */
  uint8 q_breach_mask;

  /* Circular geofence dimensions */
  sm_gm_circular_target_area z_circular_target;

  /* Flag to include the position */
  uint8 b_include_position;

  /* The responsiveness value of the geofence */
  uint8 geofence_responsiveness;

  /* The confidence value of the geofence */
  uint8 geofence_confidence;

  /*start time of geofence in secs*/
  uint32 q_start_time; 

  /*stop time relative to start time of geofence in secs*/
  uint32 q_stop_time; 

  /*If persistency needed for the geofence*/
  boolean b_persistency_needed; 

  /*Minimum time interval between breaches*/
  uint32 q_min_interval_time; 

  /*Maximum number of breach reports*/  
  uint32 q_max_num_reports; 

  /*When dynamic responsiveness is enabled, higher user experience is provided
    while adhering to responsiveness setting where ever possible*/
  boolean b_enable_dyn_resp; 

  /* The geofence id allocated by the engine */
  uint32 q_geofenceId;

}geofence_add_info_log_rpt;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  /* Client handle */
  sm_gm_client_handle   z_client_handle;

  /* The geofence to be deleted */
  uint32 q_geofenceId;

  /*  Transaction ID */
  uint32 q_tid;
}geofence_del_info_log_rpt;

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  /* Client handle */
  sm_gm_client_handle  z_client_handle;

  /*  Transaction ID */
  uint32 q_tid;
}geofence_purge_info_log_rpt;

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  /*Unique Transaction Id provided by the client*/
  uint32 q_transaction_id; 

  /*Unique Geofence id indentifying the geofence*/
  uint32  geofence_id;

  /*Geofence Origin Type MO/NI/etc*/
  uint8 geofence_type;

  /*Breach event: Entering/Leaving/Inside/Outside/etc*/
  uint8 breach_type;

  /*Breach confidence assosicated with the fix*/
  uint8 breach_confidence;

  /*Whether fix is included with the report*/
  boolean fix_included;
}geofence_breach_info_log_rpt;

#define GEOFENCE_LOG_RESP_MAX_BUCKETS 9
#define GEOFENCE_LOG_RADIUS_MAX_BUCKETS 6

typedef struct
{
  uint32 q_min;
  uint32 q_max;
} geofence_log_bucket_info_type;

#define GM_POS_OP_MODE_STANDALONE   0
#define GM_POS_OP_MODE_MS_BASED     1
#define GM_POS_OP_MODE_MS_ASSISTED  2


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;
  uint8       u_pos_fix_src;
  uint8       client_id;
  uint32      q_hor_acc_req;
  uint8       u_num_fixes;
  uint32      q_tbf;
  uint8       u_pos_op_mode;
  uint8       u_power_prof;
  uint8       u_hp_proc_wakeup_prof;
  uint8       u_nw_usage_prof;
  uint32      q_meas_age;
}geofence_pos_req_log_rpt;

#define IOD_SENSOR_STATE_ON  (1)
#define IOD_SENSOR_STATE_OFF (0)

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8        u_Version;                /* Version number of DM log */
  uint8        u_sensor_state;
}geofence_als_iod_sensor_state_log_rpt;

typedef PACKED struct PACKED_POST
{
  uint8       u_backoff_algorithm; /*Backoff algorithm Used*/
  uint32      q_backoff_secs; /*Backoff seconds used*/
  uint32      q_gf_id_for_backoff; /*Geofence Id used for backoff*/
  boolean     u_dpo_can_be_tried; /*Dpo can be tried*/
  boolean     u_need_apptrack; /*Whether apptrack is needed*/
  
}geofence_ebee_log_info;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;
  geofence_ebee_log_info z_ebee_log_info;
}geofence_ebee_log_rpt;

#if defined(T_WINNT)
#error code not present
#endif

/****************** Function Declarations**************************************/
/*===========================================================================
FUNCTION gm_log_iod_state

DESCRIPTION
  This function logs Geofencing IOD sensors state

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_iod_state(uint8 u_sensor_state);

/*===========================================================================
FUNCTION gm_log_motion_sensing

DESCRIPTION
  This functionlogs Geofencing motion sensing events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_motion_sensing(geofence_motion_detection_rpt motion_detection_report);

/*===========================================================================
FUNCTION gm_log_position

DESCRIPTION
  This functionlogs logs positions received by Geofencing

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_position(sm_gm_position_notification_type *gm_position);


/*===========================================================================
FUNCTION gm_log_geofence_add

DESCRIPTION
  This function logs the information related to geofence add operation.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_geofence_add(geofence_add_info_log_rpt *p_geofence_add_info);

/*===========================================================================
FUNCTION gm_log_geofence_del

DESCRIPTION
  This function logs the information related to geofence delete operation.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_geofence_del(geofence_del_info_log_rpt *p_geofence_del_info);

/*===========================================================================
FUNCTION gm_log_geofence_purge

DESCRIPTION
  This function logs the information related to geofence purge operation.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_geofence_purge(geofence_purge_info_log_rpt *p_geofence_purge_info);

/*===========================================================================
FUNCTION gm_log_geofence_breach

DESCRIPTION
  This function logs the information related to geofence breach operation.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_geofence_breach(geofence_breach_info_log_rpt *p_geofence_breach_info);


/*===========================================================================
FUNCTION gm_log_pos_request

DESCRIPTION
  This function logs the position request from Geofencing engine

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_pos_request(sm_gm_client_pos_req_type *pz_pos_req_type,
                        sm_gm_client_id  client_id);

/*===========================================================================
FUNCTION gm_log_ebee_info

DESCRIPTION
  This function logs Geofencing EbEE Info

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_ebee_info(geofence_ebee_log_rpt *p_ebee_info);

#endif /*GM_LOG_H*/



