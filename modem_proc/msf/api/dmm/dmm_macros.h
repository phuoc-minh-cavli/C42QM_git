/**
  @file dmm_macro.h

  @brief  
*/
/*
    Copyright (c) 2017 Qualcomm Technologies, Inc.
              All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

All ideas, data and information contained in or disclosed by this
document are confidential and proprietary information of
QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
By accepting this material the recipient agrees that this material
and the information contained therein are held in confidence and in
trust and will not be used, copied, reproduced in whole or in part,
nor its contents revealed in any manner to others without the express
written permission of QUALCOMM Technologies Incorporated.

$Header: //components/rel/msf.mpss/1.0/api/dmm/dmm_macros.h#1 $
$DateTime: 2018/06/27 11:21:46 $
$Author: pwbldsvc $
*/

#pragma once

#define _DMMFUNC1(_dmm_vf) _dmm_vf()
#define _DMMFUNC2(_dmm_vf,a) _dmm_vf(a)
#define _DMMFUNC3(_dmm_vf,a,b) _dmm_vf(a,b)
#define _DMMFUNC4(_dmm_vf,a,b,c) _dmm_vf(a,b,c)
#define _DMMFUNC5(_dmm_vf,a,b,c,d) _dmm_vf(a,b,c,d)
#define _DMMFUNC6(_dmm_vf,a,b,c,d,e) _dmm_vf(a,b,c,d,e)
#define _DMMFUNC7(_dmm_vf,a,b,c,d,e,f) _dmm_vf(a,b,c,d,e,f)
#define _DMMFUNC8(_dmm_vf,a,b,c,d,e,f,g) _dmm_vf(a,b,c,d,e,f,g)
#define _DMMFUNC9(_dmm_vf,a,b,c,d,e,f,g,h) _dmm_vf(a,b,c,d,e,f,g,h)
#define _DMMFUNC10(_dmm_vf,a,b,c,d,e,f,g,h,i) _dmm_vf(a,b,c,d,e,f,g,h,i)
#define _DMMFUNC11(_dmm_vf,a,b,c,d,e,f,g,h,i,j) _dmm_vf(a,b,c,d,e,f,g,h,i,j)
#define _DMMFUNC12(_dmm_vf,a,b,c,d,e,f,g,h,i,j,k) _dmm_vf(a,b,c,d,e,f,g,h,i,j,k)

#define _DMMPASTE(a,b) _DMMPASTE_(a,b)
#define _DMMPASTE_(a,b) a##b
#define _DMMCNT(...) _DMMCNT_(__VA_ARGS__,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define _DMMCNT_(a,b,c,d,e,f,g,h,i,j,k,l,cnt,...) cnt

