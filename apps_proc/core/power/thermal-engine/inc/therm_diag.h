#ifndef THERM_DIAG_H
#define THERM_DIAG_H
/*============================================================================
  @file therm_diag.h

  This header provides type and enum definitions for thermal diag packets.
  -- all intended for to be called within CoreBSP only.

  Copyright (c) 2013 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.tx/6.0/power/thermal-engine/inc/therm_diag.h#1 $
$DateTime: 2020/01/30 22:49:35 $
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
   PA_THERM_SS,      /* Power Amplifier Sub-System */

   MSM_THERM_SS,     /* On-Chip Sensor Sub-System */

   BATT_THERM_SS,    /* Battery Sub-System */

   PMIC_THERM_SS,    /* PMIC Sensor Sub-System */

   XO_THERM_SS,      /* XO Sensor Sub-System */

   GPS_THERM_SS,     /* GPS Sub-System */

} therm_ss_id;


#endif /* THERM_DIAG_H */
