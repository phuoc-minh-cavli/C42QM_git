/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Time Services Diagnostics Packet Processing
        T I M E   U N I T   T E S T   F R A M E W O R K 

General Description
  Diagnostic packet processing routines for Time Service operations.

Copyright (c) 2011 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/core.tx/6.0/services/time/src/time_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
01/07/09   pc      Created.
===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include <stringl/stringl.h>
#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "memheap.h"
#include "time_jul.h"
#include "time_svc.h"
#include "time_diag.h"
#include "msg.h"
#include "time_fn_map.h"

#include "time_genoff.h"

  /* test purpose */
#include "qw.h"

/****************************************************************************\
Example for Command compilation :-
===============================

 - Below is the diag packet req/resp structure for reference :-
   ---------------------------------------------------------
typedef struct
{
  diagpkt_subsys_header_type header;        - Sub System header
  uint16 command;                           - Which command
  uint16 command_data[DIAG_TIME_DATA_SIZE]; - Command parameters
                                              -to be used by function
  uint16 return_stat;                       - Return Status

}TIME_SERVICES_TEST_F_(req_type/resp_type);
  ----------------------------------------------------------

 - Sample Command building:-
   -----------------------
diagpkt_subsys_header_type header (4 bytes)
------------------------------------------------
Command Class is default   - 1 byte  : 0x4B
Subsystem id for Time_Diag - 1 byte  : 0x49(73 for Time Services)
??                         - 2 bytes : 0x00 0x00
------------------------------------------------

uint16 command (2 bytes)
-----------------------------------------------
Command ID                 - 2 bytes: 0x00 0x00 (Eg for TIME_SET)
-----------------------------------------------

uint16 command_data[DIAG_TIME_DATA_SIZE] (14 bytes max)
-----------------------------------------------
0xD9 0x07 0x02 0x00 0x05 0x00 0x0A 0x00 0x14 0x00 0x1E 0x00 0x03 00 - 14 bytes
 # Eg for time_set_julian()
   0xD9 0x07 -> 0x07D9 = 2009 (Year)
   0x02 0x00 -> 0x0002 =    2 (Month)
   0x05 0x00 -> 0x0005 =    5 (Day)
   0x0A 0x00 -> 0x000A =   10 (Hour)
   0x14 0x00 -> 0x0014 =   20 (Mins)
   0x1E 0x00 -> 0x001E =   30 (Secs)
   0x03 0x00 -> 0x0003 =    3 (day of week)
-----------------------------------------------

uint16 return_stat (2 bytes)
----------------------------------------------------------------
Return Status used in response packets "0x00 0x00", if succesful
----------------------------------------------------------------
If
Tx: 0x4B 0x49 0x00 0x00 0x00 0x00 0xD9 0x07 0x02 0x00 0x05 0x00 0x0A 0x00
                                  0x14 0x00 0x1E 0x00 0x01 0x00
then expected
Rx: 0x4B 0x49 0x00 0x00 0x00 0x00 0xD9 0x07 0x02 0x00 0x05 0x00 0x0A 0x00
                                  0x14 0x00 0x1E 0x00 0x01 0x00 0x00 0x00

NOTE : For Commands that donot require any payload, 14 byte command data
       need not be sent
\****************************************************************************/

/*=============================================================================

                         DATA DEFINITIONS
=============================================================================*/

#define TIME_SET                  0
#define TIME_GET                  1
#define USER_TIME_GENOFF_GET_MS   2
#define USER_TIME_GENOFF_SET_SECS 3
#define USER_TIME_GENOFF_GET_SECS 4
#define USER_TIME_GENOFF_SET_JULI 5
#define USER_TIME_GENOFF_GET_JULI 6
#define TOD_TIME_GENOFF_GET_MS    7
#define TOD_TIME_GENOFF_SET_SECS  8
#define TOD_TIME_GENOFF_GET_SECS  9
#define TOD_TIME_GENOFF_SET_JULI  10
#define TOD_TIME_GENOFF_GET_JULI  11
#define TIME_SET_REMOTE_JULIAN 14
#define TIME_REGISTER_WITH_SERVER_MODEM 15

#define TIME_ALARM_REGISTER   16
#define TIME_ALARM_SET        17
#define TIME_1X_SET           18
#define TIME_GPS_SET          19
#define TIME_SERVER_TEST      20

#define TIME_GET_MEMHEAP_DEBUG_INFO 21
#define TIME_TEST_EXACT_TIME 22
/*=============================================================================

                         FUNCTION DEFINITIONS
=============================================================================*/

/*extern functions to be called*/
#ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT
extern void time_remote_mm_set_julian(time_julian_type *jul_time);
extern void time_start_mm_registration(void);
#endif

static uint32 alarm_in_sec = 10;
extern int32 time_alarm_set_secs(uint32 expiration);

static uint32 time_diag_fps_initialized = 0;
static time_fn_map_fp_type *time_diag_fps;

static uint32 time_diag_tt_valid = 0;
static DalDeviceHandle *hTimerDiagHandle = NULL;
/*===========================================================================

 FUNCTION TIME_SERVICES_TEST

DESCRIPTION
 This function would test the Time services.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
PACK(void *) time_services_test 
(
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  time_julian_type julian;
  TIME_SERVICES_TEST_F_req_type *req =(TIME_SERVICES_TEST_F_req_type *) req_pkt;
   uint16 base;
   uint64 offset;
   uint64 curr_timetick_val = 0;
  // uint8 shift=0;
   uint16* offset_ptr = (uint16*)&offset;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   if ( time_diag_tt_valid == 0 )
   {
     DalTimetick_Attach("SystemTimer", &hTimerDiagHandle);
     DalTimetick_Enable(hTimerDiagHandle,1);
     DalTimetick_InitTimetick64(hTimerDiagHandle);
     time_diag_tt_valid++;
   }

  /* TIME_SERVICES_TEST_F_rsp_type *rsp; */
  const int rsp_len = sizeof( TIME_SERVICES_TEST_F_rsp_type );

  PACK(void *) rsp_ptr = NULL;

  /* Allocate the same length as the request. */
  rsp_ptr = (TIME_SERVICES_TEST_F_rsp_type *)diagpkt_subsys_alloc
                                         ( DIAG_SUBSYS_TIME,
                                           TIME_SERVICES_TEST_F,
                                           rsp_len );

  if (rsp_ptr != NULL)
  {
    memscpy ((void *) rsp_ptr, pkt_len, (void *) req_pkt, pkt_len);
  }

  switch ( req->command )
  {
     case TIME_SET :
     /* This case would be exercising the TOD setting operation */
       julian.year        = req->command_data[0];
       julian.month       = req->command_data[1];
       julian.day         = req->command_data[2];
       julian.hour        = req->command_data[3];
       julian.minute      = req->command_data[4];
       julian.second      = req->command_data[5];
       julian.day_of_week = req->command_data[6];

       time_set_julian (&julian);

       MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: Year=%d : Month=%d : Day=%d",julian.year,
                                            julian.month,julian.day);
       MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: Hour=%d : Minute=%d : Second=%d",julian.hour,
                                            julian.minute,julian.second);
       MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: Day_of_week=%d",julian.day_of_week);

       break;

       case TIME_GET :
       
       time_get_julian (&julian);

       MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Get: Year=%d : Month=%d : Day=%d",julian.year,
                                            julian.month,julian.day);
       MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Get: Hour=%d : Minute=%d : Second=%d",julian.hour,
                                            julian.minute,julian.second);
       MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Get: Day_of_week=%d",julian.day_of_week);

       break;
    
     case USER_TIME_GENOFF_GET_SECS:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;

         /* Pointer to Time_genoff instance in consideration */
     time_type                       ts_val_secs;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_USER; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = ts_val_secs;
         temp_genoff_args.unit        = TIME_SECS;
         temp_genoff_args.operation   = T_GET;
         time_genoff_opr( &temp_genoff_args );
     
         MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: ts_val[0]=%d : ts_val[1]=%d",ts_val_secs[0],
                                            ts_val_secs[1]);
       }
       break;

     case USER_TIME_GENOFF_SET_SECS:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;

         /* Pointer to Time_genoff instance in consideration */
     time_type                       ts_val_secs = {0x20000, 0};

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_USER; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = ts_val_secs;
         temp_genoff_args.unit        = TIME_SECS;
         temp_genoff_args.operation   = T_SET;
         time_genoff_opr( &temp_genoff_args );
     
       }
       break;

     case USER_TIME_GENOFF_SET_JULI:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;
     
         /* Time julian Type */
     time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         jul_val.year        = req->command_data[0];
         jul_val.month       = req->command_data[1];
         jul_val.day         = req->command_data[2];
         jul_val.hour        = req->command_data[3];
         jul_val.minute      = req->command_data[4];
         jul_val.second      = req->command_data[5];
         jul_val.day_of_week = req->command_data[6];

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_SET;
         time_genoff_opr( &temp_genoff_args );
         MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: jul_val.yr=%d",jul_val.year);  
       }
       break;

	   /*set 1x time*/
	   case TIME_1X_SET:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;
     
         /* Time julian Type */
     time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         jul_val.year        = req->command_data[0];
         jul_val.month       = req->command_data[1];
         jul_val.day         = req->command_data[2];
         jul_val.hour        = req->command_data[3];
         jul_val.minute      = req->command_data[4];
         jul_val.second      = req->command_data[5];
         jul_val.day_of_week = req->command_data[6];

         temp_genoff_args.base        = ATS_1X; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_SET;
         time_genoff_opr( &temp_genoff_args );
         MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: jul_val.yr=%d",jul_val.year);  
       }
       break;

       /*TIME_GPS_SET*/
	    /*set 1x time*/
	   case TIME_GPS_SET:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;
     
         /* Time julian Type */
     time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         jul_val.year        = req->command_data[0];
         jul_val.month       = req->command_data[1];
         jul_val.day         = req->command_data[2];
         jul_val.hour        = req->command_data[3];
         jul_val.minute      = req->command_data[4];
         jul_val.second      = req->command_data[5];
         jul_val.day_of_week = req->command_data[6];

         temp_genoff_args.base        = ATS_GPS; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_SET;
         time_genoff_opr( &temp_genoff_args );
         MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: jul_val.yr=%d",jul_val.year);
       }
       break;


	  
     case USER_TIME_GENOFF_GET_JULI:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args ;
     
     /* Time julian Type */
     time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_GET;
         time_genoff_opr( &temp_genoff_args );

         MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Get: Year=%d : Month=%d : Day=%d",jul_val.year,
                                            jul_val.month,jul_val.day);
         MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Get: Hour=%d : Minute=%d : Second=%d",jul_val.hour,
                                            jul_val.minute,jul_val.second);
         MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Get: Day_of_week=%d",jul_val.day_of_week);
     
         MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Get: jul_val.yr=%d",jul_val.year);
       }
       break;

     case TOD_TIME_GENOFF_GET_SECS:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;

         /* Pointer to Time_genoff instance in consideration */
     time_type                       ts_val_secs;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = ts_val_secs;
         temp_genoff_args.unit        = TIME_SECS;
         temp_genoff_args.operation   = T_GET;
         time_genoff_opr( &temp_genoff_args );
     
         MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: ts_val[0]=%d : ts_val[1]=%d",ts_val_secs[0],
                                            ts_val_secs[1]);
       }
       break;

     case TOD_TIME_GENOFF_SET_SECS:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;

         /* Pointer to Time_genoff instance in consideration */
     time_type                       ts_val_secs = {0x20000, 0};

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = ts_val_secs;
         temp_genoff_args.unit        = TIME_SECS;
         temp_genoff_args.operation   = T_SET;
         time_genoff_opr( &temp_genoff_args );
     
       }
       break;

     case TOD_TIME_GENOFF_SET_JULI:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;
     
         /* Time julian Type */
     time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         jul_val.year        = req->command_data[0];
         jul_val.month       = req->command_data[1];
         jul_val.day         = req->command_data[2];
         jul_val.hour        = req->command_data[3];
         jul_val.minute      = req->command_data[4];
         jul_val.second      = req->command_data[5];
         jul_val.day_of_week = req->command_data[6];

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_SET;
         time_genoff_opr( &temp_genoff_args );
     
       }
       break;

     case TOD_TIME_GENOFF_GET_JULI:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args ;
     
     /* Time julian Type */
     time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_GET;
         time_genoff_opr( &temp_genoff_args );

         MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: Year=%d : Month=%d : Day=%d",jul_val.year,
                                            jul_val.month,jul_val.day);
         MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: Hour=%d : Minute=%d : Second=%d",jul_val.hour,
                                            jul_val.minute,jul_val.second);
         MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Set: Day_of_week=%d",jul_val.day_of_week);
     
       }
       break;
     
    case TIME_SET_REMOTE_JULIAN:
      julian.year        = 2030;
      julian.month       = 9;
      julian.day         = 9;
      julian.hour        = 18;
      julian.minute      = 55;
      julian.second      = 0;
      julian.day_of_week = 3; 
      #ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT
      time_remote_mm_set_julian(&julian);
      MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_fusion_debug:7K:Set julian value on Q6, jul_yr=%d",julian.year);
      #endif
    break;

    case TIME_REGISTER_WITH_SERVER_MODEM:
      #ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT
      time_start_mm_registration();
      #endif
    break;

    case TIME_ALARM_REGISTER:
    break;

    case TIME_ALARM_SET:
      time_alarm_set_secs(alarm_in_sec);
    break;

    case TIME_SERVER_TEST:
      #ifdef FEATURE_QMI_TIME_REMOTE
      /*first 16 bits can be treated as a base*/
      base =  req->command_data[0];
      offset = 0;
      *offset_ptr++ = req->command_data[1];
      *offset_ptr++ = req->command_data[2];
      *offset_ptr++ = req->command_data[3];
      *offset_ptr++ = req->command_data[4];

      MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_server_test:indication message to be sent base=%d,offset[0]=%d, offset[1]=%d",base,offset&0xFFFF,offset&0xFFFF0000);
      MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_server_test:offset[2]=%d,offset[3]=%d",offset&0xFFFF00000000,offset&0xFFFF000000000000);
      time_server_call_ind(base, offset);
      /* Initialize Function Pointers to eliminate dependencies */
      if ( !time_diag_fps_initialized )
      {
        time_diag_fps = time_fn_map_get_fp();
        time_diag_fps_initialized++;
      }
      
      /* Send QMI notification if QMI is available */
      if ( time_diag_fps->time_server_call_ind_fp != NULL )
      {
        MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_server_test:indication message to be sent base=%d,offset[0]=%d, offset[1]=%d",base,offset&0xFFFF,offset&0xFFFF0000);
        MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_server_test:offset[2]=%d,offset[3]=%d",offset&0xFFFF00000000,offset&0xFFFF000000000000);
        time_diag_fps->time_server_call_ind_fp(base, offset);
      }
      #else /*FEATURE_QMI_TIME_REMOTE*/
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"FEATURE_QMI_TIME_REMOTE is not defined");
      #endif /*FEATURE_QMI_TIME_REMOTE*/
    break;
    case TIME_GET_MEMHEAP_DEBUG_INFO:
       #ifdef FEATURE_ENABLE_HEAP_PROFILING
         print_memheap_profile_table();
         break;
       #endif
     case TIME_TEST_EXACT_TIME:
     {

      time_genoff_args_struct_type    temp_genoff_args;

      julian.year        = 1988;
      julian.month       = 4;
      julian.day         = 10;
      julian.hour        = 7;
      julian.minute      = 0;
      julian.second      = 0;
      julian.day_of_week = 0;

      DalTimetick_GetTimetick64(hTimerDiagHandle, &curr_timetick_val);

      /* Subtract 30 seconds from the timetick value */
      if ( curr_timetick_val < 19200000*60 )
      {
        curr_timetick_val = 0;
      }
      else
      {
        curr_timetick_val -= 19200000*60;
      }

      temp_genoff_args.base        = ATS_TOD; 
      temp_genoff_args.base_source = TIME_SCLK;
      temp_genoff_args.ts_val      = (void *)(&julian);
      temp_genoff_args.unit        = TIME_JULIAN;
      temp_genoff_args.operation   = T_SET;
      temp_genoff_args.dal_timetick_val = curr_timetick_val;
      temp_genoff_args.dal_timetick_val_valid = TIME_GENOFF_TIMETICK_VALID;
      time_genoff_opr( &temp_genoff_args );
      break;
      }
          default :
     /* for un-implemented and wrong commands */
       MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_services_test : invalid command - %d",req->command);
       break;
  }

  (void)pkt_len;

  return (rsp_ptr);

} /* time_services_test */


/*===========================================================================

              FUNCTION DISPATCH TABLE FOR DIAG COMMANDS

DESCPRITIION  
  TIME_SERVICES_TEST_F needs to be defined to a value in diagcmd.h if
no subsys ID is used to register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static const diagpkt_user_table_entry_type time_services_test_tbl[] =
{
  {TIME_SERVICES_TEST_F,TIME_SERVICES_TEST_F,time_services_test}
};


/*===========================================================================

FUNCTION TIME_SERVICES_TEST_INIT

DESCRIPTION
  Registers the diag packet function dispatch table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void time_services_test_init (void)
{
 /* Could use the following registration without specifying the processor */
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_TIME, time_services_test_tbl);

}

