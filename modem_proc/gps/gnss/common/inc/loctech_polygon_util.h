/*=============================================================================

                             LOCTECH Polygon Util Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant to loctech polygon support.
  
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2018 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/loctech_polygon_util.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

  when        who      what, where, why
  ----------  ---  -----------------------------------------------------------
  

=============================================================================*/

#ifndef LOCTECH_POLYGON_UTIL_H
#define LOCTECH_POLYGON_UTIL_H

#include "comdef.h"

/*Coordinate Type*/
typedef struct
{
	double   d_Latitude;  /* Lat.: in degrees -90.0 to 90.0, positive for northern */
	double   d_Longitude; /* Long.: in degrees -180.0 to 180.0, positive for eastern */
}loctech_coordinate_s_type;

/*Polygon Type*/
typedef struct
{
	uint32  u_NumVertices; /*Number of Vertices of a polygon*/
	loctech_coordinate_s_type *pz_Coordinates; /*Pointer to array of vertices of a polygon*/
	/*Each ith vertex shall be connected to the i+1th vertex, except the last one
	which shall be connected to the first vertex*/
}loctech_polygon_s_type;

typedef uint32 loctech_pos_lite_opt_flags;
#define LOCTECH_POS_LITE_GPS_WEEK_MSEC_VALID           0x0001

typedef struct
{	
	loctech_pos_lite_opt_flags  q_PosOptFlags; /*Flags to indicate optional elements*/
	loctech_coordinate_s_type   z_Coordinate;

	/*Vaid only if LOCTECH_POS_LITE_GPS_WEEK_MSEC_VALID*/
	uint16   w_GpsWeek;   /* GPS week number of fix */	
	/*Vaid only if LOCTECH_POS_LITE_GPS_WEEK_MSEC_VALID*/
	uint32   q_GpsTimeMs; /* GPS time in week (milliseconds) of fix */	
	/*Client can fill above value from QMI position report QMI_LOC_EVENT_POSITION_REPORT_IND. 
	 Element: gpsWeek and gpsTimeOfWeekMs*/

	double    d_HorUncCirc;   /* horizontal location unc.: meters */
	/*Client can fill in above value from QMI position report QMI_LOC_EVENT_POSITION_REPORT_IND. 
	 Element: horUncCircular*/
}loctech_position_lite_s_type;

/*=============================================================================

FUNCTION 
  loctech_polygon_breach

DESCRIPTION
  This function detects whether the position is inside\outside\unknown
  a polygon. It also returns the minimum distance to the boundary of polygon in meters.

  Note:  This function shall use 99% confidence of uncertainity for its calculations.

DEPENDENCIES
  None.

PARAMETERS
  MANDATORY: pz_Polygon - pointer to Polygon. 
  MANDATORY: pz_Position - pointer to Position
  OPTIONAL:  pq_PercentagePosInsidePolygon - Pointer to percentage of position which is inside
  					  					    polygon. 
  					  					    If a NULL pointer is passed, then this parameter 
  					  					    is optional.
   										    The pointer shall be allocated by the caller
											and contents filled in by this function. 
  OPTIONAL: pq_MinDistanceToPolygon: Pointer to minimum distance from the edge of a polygon.  
   									 If a NULL pointer is passed, then this parameter 
   									 is optional.

RETURN VALUE
  boolean: Pass\Fail

SIDE EFFECTS
  None.

=============================================================================*/
boolean loctech_polygon_breach(const loctech_polygon_s_type* const pz_Polygon,
										const loctech_position_lite_s_type* const pz_Position,
										uint8* pq_PercentagePosInsidePolygon,										
										uint32* pq_MinDistanceToPolygon);



#endif /*LOCTECH_POLYGON_UTIL_H*/
