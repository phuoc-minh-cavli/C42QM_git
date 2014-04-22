#ifndef DCVS_UTILS_H
#define DCVS_UTILS_H
/**********************************************************************
 * dcvs_algorithm.h
 *
 * Copyright (C) 2012 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/

/*=======================================================================
                        Edit History
$Header: //components/rel/core.tx/6.0/power/dcvs/inc/dcvs_utils.h#1 $ 
$DateTime: 2020/01/30 22:49:35 $

when       who     what, where, why
--------   ----    ---------------------------------------------------
09/18/12   nj      Initial Check-in
========================================================================*/

// Macros for splitting up a 64 bit value

#define DCVS_ULOG64_LOWWORD(x) (uint32){x}
#define DCVS_ULOG64_HIGHWORD(x) (uint32)((uint64){x}>>32)
#define DCVS_ULOG64_DATA(x) DCVS_ULOG64_LOWWORD(x), DCVS_ULOG64_HIGHWORD(x)


#endif /* DCVS_UTILS_H */
