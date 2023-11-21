
#ifndef RFLM_NLIC_AG_H
#define RFLM_NLIC_AG_H


#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: This file is auto-generated.

Generated at:    Tue Aug 12 11:37:46 2014
Generated using: nlic_autogen.pl
Generated from:  v0.0.21 of Bolt_NLICLM_settings.xlsx
*/

/*=============================================================================

              A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the modem register settings 
  configured by FW, provided by the rflm_nlic.

Copyright (c) 2009, 2010, 2011, 2012, 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

$DateTime: 2020/04/16 12:27:55 $
$Header: //components/rel/rfmodem_jolokia.mpss/1.10/nlic/inc/rflm_nlic_ag.h#1 $

=============================================================================*/

/*=============================================================================
                           REVISION HISTORY
Version   Author   Date   
         Comments   
0.0.21   shwang   8/12/2014   
         B4+B2, B3+B3, and B12+B4 (20MHz) addition reverted.   
0.0.20   shwang   8/11/2014   
         clock mode forced to NOM always for NLIC   
0.0.19   shwang   7/30/2014   
         B4+B2, B3+B3, and B12+B4 (20MHz) added   
0.0.18   shwang   7/25/2014   
         clock_mode type changed to an enum. Also names are shortened. added nb clock mode    
0.0.17   shwang   7/10/2014   
         clock mode added. 20MHz BW for B3B5 added. B12B4 is added   
0.0.16   shwang   6/30/2014   
         BW combinations 5+5 and 5+10 added   
0.0.15   shwang   6/25/2014   
         delay data type: uint8 -> int16   
0.0.14   shwang   6/24/2014   
         5+10 and 5+5 are not supported for MAV 6/30 CS   
0.0.13   shwang   6/23/2014   
         delay data type uint16 -> uint8   
0.0.12   shwang   6/19/2014   
         removing 5+10, 5+5 from B3B5   
0.0.11   shwang   6/19/2014   
         adding clock_mode to API. adding 5+10, 5+5 to B17B4 and B3B5   
0.0.10   shwang   6/7/2014   
         adding time delay to API. adding 10+5 to B17B4 and B3B5   
0.0.9   shwang   6/6/2014   
         removed all band combinations other than B17B4 and B3B5: Unsupported use cases.   
0.0.8   shwang   4/11/2014   
         Diversity antenna also enabled for B17-B4, B12-B4, B8-B3   
0.0.7   shwang   4/1/2014   
         empty rows are filled for b3_b19 and later. nlic_mode added for wb+nb mode change   
0.0.6   shwang   3/31/2014   
         freature added: 1-reducing num_kernels. 2-selecting one antenna   
0.0.5   shwang   3/20/2014   
         FTL corrections. New equations to support PCC cancellation.   
0.0.4_B3B5   shwang   2/11/2014   
         B3-B3 FTL correction, NLIC enable always on for B3-B3, victim_carrier_id = 1, aggr_carrier_id = 0, f_offset_nom = 0   
0.0.4   shwang   12/9/2013   
         algorithm bug fix: kernel_term_extra_rxy_dly_acq = 2 for B8_B3 (was 1)   
0.0.3   shwang   11/25/2013   
         fix the algorithm: kernel_terms_cancel_mode_l0 = (1-k7_en)*kl0_on_off   
0.0.2   shwang   10/1/2013   
         added k7_en as static param   
0.0.1   shwang   9/27/2013   
         Initial Revision   

=============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "comdef.h" 
#include "rflm_nlic_typedef_ag.h" 



boolean rflm_nlic_get_static_settings (uint8 tx_band, uint8 rx_band, uint16 tx_chan, uint8 ul_bw, uint8 dl_bw, rflm_nlic_data_type * nlic_data);

#ifdef __cplusplus
}
#endif



#endif


