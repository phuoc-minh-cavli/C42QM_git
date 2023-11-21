#ifndef THERM_DIAG_H
#define THERM_DIAG_H
/*============================================================================
  @file therm_diag.h

  This header provides type and enum definitions for thermal diag packets.
  -- all intended for to be called within CoreBSP only.

  Copyright (c) 2011-2014, 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.10/power/thermal/inc/therm_diag.h#2 $
$DateTime: 2019/01/21 21:54:53 $
$Author: pwbldsvc $
========================================================================*/

typedef struct
{
   unsigned int subsystem_id; /* Subsystem id of sensor */

   unsigned int sensor_id;    /* Sensor id of sensor */

   int temp;                  /* Temperature Reading from sensor */

} therm_sample;


typedef struct
{
   unsigned int version;      /* Packet version number for Diag */

   unsigned int num_samples;  /* Number of samples  in Diag Packet */

   therm_sample samples[8];

} diag_therm_packet;

typedef enum
{
   PA_THERM_SS,        /* Power Amplifier Sub-System */

   MSM_THERM_SS,       /* On-Chip Sensor Sub-System */

   BATT_THERM_SS,      /* Battery Sub-System */

   PMIC_THERM_SS,      /* PMIC Sensor Sub-System */

   XO_THERM_SS,        /* XO Sensor Sub-System */

   GPS_THERM_SS,       /* GPS Sub-System */

   LOGICAL_THERM_SS,   /* Logical/Virtual Sub-System */

   WTR_THERM_SS,       /* WTR Sub-System */

} therm_ss_id;


#endif /* THERM_DIAG_H */
