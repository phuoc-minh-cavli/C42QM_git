#ifndef TMS_UTILS_MSG_H
#define TMS_UTILS_MSG_H
/**
@file tms_utils_msg.h
@brief This file contains the Macro definitions for TMS utils
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
All rights reserved.
QUALCOMM Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.tx/6.0/debugtools/api/tms_utils_msg.h#1 $
$DateTime: 2020/01/30 22:49:35 $
$Change: 22400605 $
$Author: pwbldsvc $
===============================================================================*/

#include "msg.h"
#include "ULog.h"
#include "ULogFront.h"

/**
 * Log enable/disable macro
 */
#define TMS_MSG_DIAG_F3	// Enable this for Diag F3
// #define TMS_MSG_ULOG    // Enable this for ULOG

#define TMS_PERIPHERAL "APSS:"

/* F3 Flag */
extern unsigned int tms_diag_f3_flag;

/* ULOG handles */
extern ULogHandle tms_low_ulog_handle;
extern ULogHandle tms_med_ulog_handle;
extern ULogHandle tms_high_ulog_handle;
extern ULogHandle tms_err_ulog_handle;

/*------ LOW ------*/
#define TMS_MSG_LOW( str )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 2, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__);  \
    }  \
  } while(0)
	  
#define TMS_MSG_LOW_1( str, a )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_1( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str, a );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 3, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a);  \
    }  \
  } while(0)

#define TMS_MSG_LOW_2( str, a, b )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_2( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str, a, b );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 4, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b);  \
    }  \
  } while(0)

#define TMS_MSG_LOW_3( str, a, b, c )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_3( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str, a, b, c );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 5, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c);  \
    }  \
  } while(0)

/*------ MED ------*/
#define TMS_MSG_MED( str )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 2, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__);  \
    }  \
  } while(0)

#define TMS_MSG_MED_1( str, a )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_1( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str, a );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 3, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a);  \
    }  \
  } while(0)
  
#define TMS_MSG_MED_2( str, a, b )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_2( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str, a, b );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 4, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b);  \
    }  \
  } while(0)
  
#define TMS_MSG_MED_3( str, a, b, c )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_3( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str, a, b, c );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 5, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c);  \
    }  \
  } while(0)

/*------ HIGH ------*/
#define TMS_MSG_HIGH( str )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str);  \
    }  \
	if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 2, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__);  \
    }  \
  } while(0)

#define TMS_MSG_HIGH_1( str, a )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_1( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str, a );  \
    }  \
    if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 3, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a);  \
    }  \
  } while(0)
  
#define TMS_MSG_HIGH_2( str, a, b )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_2( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str, a, b );  \
    }  \
    if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 4, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b);  \
    }  \
  } while(0)
  
#define TMS_MSG_HIGH_3( str, a, b, c )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_3( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str, a, b, c );  \
    }  \
    if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 5, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c);  \
    }  \
  } while(0)    
  
/*------ ERROR ------*/
#define TMS_MSG_ERROR( str )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 2, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__);  \
    } \
  } while(0)

#define TMS_MSG_ERROR_1( str, a )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_1( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str, a );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 3, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a);  \
    }  \
  } while(0)  

#define TMS_MSG_ERROR_2( str, a, b )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_2( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str, a, b );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 4, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b);  \
    }  \
  } while(0)  
  
#define TMS_MSG_ERROR_3( str, a, b, c )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_3( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str, a, b, c );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 5, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c);  \
    }  \
  } while(0)

/*------ FATAL ------*/
#define TMS_MSG_FATAL( str )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG( MSG_SSID_TMS, MSG_LEGACY_FATAL, TMS_PERIPHERAL str );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 2, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__);  \
    }  \
  } while(0)

#define TMS_MSG_FATAL_1( str, a )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_1( MSG_SSID_TMS, MSG_LEGACY_FATAL, TMS_PERIPHERAL str, a );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 3, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a);  \
    }  \
  } while(0)

#define TMS_MSG_FATAL_2( str, a, b )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_2( MSG_SSID_TMS, MSG_LEGACY_FATAL, TMS_PERIPHERAL str, a, b );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 4, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b);  \
    }  \
  } while(0)
  
#define TMS_MSG_FATAL_3( str, a, b, c )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_3( MSG_SSID_TMS, MSG_LEGACY_FATAL, TMS_PERIPHERAL str, a, b, c );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 5, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c);  \
    }  \
  } while(0)

/*------ SPRINTF_LOW ------*/
#define TMS_MSG_SPRINTF_LOW( str )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 2, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_LOW_1( str, a )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_1( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str, a );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 3, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_LOW_2( str, a, b )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_2( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str, a, b );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 4, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b);  \
    }  \
  } while(0)
  
#define TMS_MSG_SPRINTF_LOW_3( str, a, b, c )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_3( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str, a, b, c );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 5, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c);  \
    }  \
  } while(0)
  
#define TMS_MSG_SPRINTF_LOW_4( str, a, b, c, d )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_4( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str, a, b, c, d );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 6, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c, d);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_LOW_5( str, a, b, c, d, e )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_5( MSG_SSID_TMS, MSG_LEGACY_LOW, TMS_PERIPHERAL str, a, b, c, d, e );  \
    }  \
    if( tms_low_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_low_ulog_handle, 7, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c, d, e);  \
    }  \
  } while(0)

/*------ SPRINTF_MED ------*/
#define TMS_MSG_SPRINTF_MED( str )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 2, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_MED_1( str, a )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_1( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str, a );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 3, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a); \
    }  \
  } while(0)
  
#define TMS_MSG_SPRINTF_MED_2( str, a, b )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_2( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str, a, b );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 4, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_MED_3( str, a, b, c )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_3( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str, a, b, c );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 5, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_MED_4( str, a, b, c, d )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_4( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str, a, b, c, d );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 6, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c, d);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_MED_5( str, a, b, c, d, e )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_5( MSG_SSID_TMS, MSG_LEGACY_MED, TMS_PERIPHERAL str, a, b, c, d, e );  \
    }  \
    if( tms_med_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_med_ulog_handle, 7, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c, d, e); \
    }  \
  } while(0)

/*------ SPRINTF_HIGH ------*/
#define TMS_MSG_SPRINTF_HIGH( str )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str );  \
    }  \
    if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 2, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_HIGH_1( str, a )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_1( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str, a );  \
    }  \
    if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 3, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_HIGH_2( str, a, b )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_2( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str, a, b );  \
    }  \
    if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 4, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_HIGH_3( str, a, b, c )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_3( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str, a, b, c );  \
    }  \
    if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 5, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_HIGH_4( str, a, b, c, d )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_4( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str, a, b, c, d );  \
    }  \
    if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 6, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c, d);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_HIGH_5( str, a, b, c, d, e )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_5( MSG_SSID_TMS, MSG_LEGACY_HIGH, TMS_PERIPHERAL str, a, b, c, d, e );  \
    }  \
    if( tms_high_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_high_ulog_handle, 7, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c, d, e);  \
    } \
  } while(0)

/*------ SPRINTF_ERROR ------*/
#define TMS_MSG_SPRINTF_ERROR( str )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 2, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_ERROR_1( str, a )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_1( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str, a );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 3, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_ERROR_2( str, a, b )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_2( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str, a, b );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 4, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b);  \
    }  \
  } while(0)

#define TMS_MSG_SPRINTF_ERROR_3( str, a, b, c )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_3( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str, a, b, c );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 5, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c);  \
    }  \
  } while(0)
  
#define TMS_MSG_SPRINTF_ERROR_4( str, a, b, c, d )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_4( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str, a, b, c, d );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 6, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c, d);  \
    }  \
  } while(0)
  
#define TMS_MSG_SPRINTF_ERROR_5( str, a, b, c, d, e )  \
  do {  \
    if( tms_diag_f3_flag )  \
    {  \
      MSG_SPRINTF_5( MSG_SSID_TMS, MSG_LEGACY_ERROR, TMS_PERIPHERAL str, a, b, c, d, e );  \
    }  \
    if( tms_err_ulog_handle )  \
    {  \
      ULogFront_RealTimePrintf(tms_err_ulog_handle, 7, TMS_PERIPHERAL"%s:%d:" str, __FILENAME__, __LINE__, a, b, c, d, e);  \
    }  \
  } while(0)


#endif /* TMS_UTILS_MSG_H */
