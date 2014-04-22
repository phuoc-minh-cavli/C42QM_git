/* Copyright (c) 2011-2012, 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation, nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "loc_log.h"
#include "msg_q.h"
#include "log_util.h"

#include "qapi_location.h"

#define  BUFFER_SIZE  120

// Logging Improvements
const char *loc_logger_boolStr[]={"False","True"};

/* Logging Mechanism */
loc_logger_s_type loc_logger;

/* Get names from value */
const char* loc_get_name_from_mask(const loc_name_val_s_type table[], size_t table_size, long mask)
{
   size_t i;
   for (i = 0; i < table_size; i++)
   {
      if (table[i].val & (long) mask)
      {
         return table[i].name;
      }
   }
   return UNKNOWN_STR;
}

/* Get names from value */
const char* loc_get_name_from_val(const loc_name_val_s_type table[], size_t table_size, long value)
{
   size_t i;
   for (i = 0; i < table_size; i++)
   {
      if (table[i].val == (long) value)
      {
         return table[i].name;
      }
   }
   return UNKNOWN_STR;
}

static const loc_name_val_s_type loc_msg_q_status[] =
{
    NAME_VAL( eMSG_Q_SUCCESS ),
    NAME_VAL( eMSG_Q_FAILURE_GENERAL ),
    NAME_VAL( eMSG_Q_INVALID_PARAMETER ),
    NAME_VAL( eMSG_Q_INVALID_HANDLE ),
    NAME_VAL( eMSG_Q_UNAVAILABLE_RESOURCE ),
    NAME_VAL( eMSG_Q_INSUFFICIENT_BUFFER )
};
static const size_t loc_msg_q_status_num = LOC_TABLE_SIZE(loc_msg_q_status);

/* Find msg_q status name */
const char* loc_get_msg_q_status(int status)
{
   return loc_get_name_from_val(loc_msg_q_status, loc_msg_q_status_num, (long) status);
}

const char* log_succ_fail_string(int is_succ)
{
   return is_succ? "successful" : "failed";
}

//Target names
static const loc_name_val_s_type target_name[] =
{
    NAME_VAL(GNSS_NONE),
    NAME_VAL(GNSS_MSM),
    NAME_VAL(GNSS_GSS),
    NAME_VAL(GNSS_MDM),
    NAME_VAL(GNSS_QCA1530),
    NAME_VAL(GNSS_AUTO),
    NAME_VAL(GNSS_UNKNOWN)
};

static const size_t target_name_num = LOC_TABLE_SIZE(target_name);

//Target names
static const loc_name_val_s_type engine_status[] =
{
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_UNKNOWN),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_SUCCESS),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_FAILED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_START),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_SUCCESS),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_FAILED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_NTP_DL_START),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_NTP_DL_SUCCESS),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_NTP_DL_FAILED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_NTP_DL_THROTTLED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_START),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_SUCCESS),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_FAILED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_START),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_SUCCESS),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_FAILED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_UNLOADED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_FIX_FAILED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_POS_ACC_CHECK_FAILED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_START),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_SUCCESS),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_FAILED),
    NAME_VAL(QAPI_LOCATION_ENGINE_STATUS_WEAK_GNSS_SIGNALS),
};

static const size_t engine_status_num = LOC_TABLE_SIZE(engine_status);

/*===========================================================================

FUNCTION loc_get_target_name

DESCRIPTION
   Returns pointer to a string that contains name of the target

   XX:XX:XX.000\0

RETURN VALUE
   The target name string

===========================================================================*/
const char *loc_get_target_name(unsigned int target)
{
    int index = 0;
    static char ret[BUFFER_SIZE];

    index =  getTargetGnssType(target);
    if( index < 0 || (unsigned)index >= target_name_num )
        index = target_name_num - 1;

    if( (target & HAS_SSC) == HAS_SSC ) {
        snprintf(ret, sizeof(ret), " %s with SSC",
           loc_get_name_from_val(target_name, target_name_num, (long)index) );
    }
    else {
       snprintf(ret, sizeof(ret), " %s  without SSC",
           loc_get_name_from_val(target_name, target_name_num, (long)index) );
    }
    return ret;
}

const char* loc_log_get_engine_status_name(long status)
{
    return loc_get_name_from_val(engine_status, engine_status_num, status);
}

/*===========================================================================
FUNCTION loc_logger_init

DESCRIPTION
   Initializes the state of DEBUG_LEVEL and TIMESTAMP

DEPENDENCIES
   N/A

RETURN VALUE
   None

SIDE EFFECTS
   N/A
===========================================================================*/
void loc_logger_init(unsigned long debug, unsigned long timestamp)
{
   loc_logger.DEBUG_LEVEL = debug;
   loc_logger.TIMESTAMP   = timestamp;
}
