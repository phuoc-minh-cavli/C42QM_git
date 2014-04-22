
/*
WARNING: This file is auto-generated.

Generated at:    Mon Sep 19 12:07:45 2016
Generated using: nb_settings_autogen.pl v1.0
Generated from:  v1.0 of NB_Settings.xlsx.
*/

/*=============================================================================

        N B	   S E T T I N G S	 A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the NB region map settings 
  provided by the System team.

Copyright (c) 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

$DateTime: 2020/04/16 12:27:55 $
$Header: //components/rel/rfmodem_jolokia.mpss/1.10/api/fw/rflm_lte_rx_nb_region_settings.h#1 $

=============================================================================*/

/*=============================================================================
                           REVISION HISTORY
Version 		Author		Date		Comments		Other POC (Comma Delimited)		
v1.0		Shashi		2016-04-07		NB Region Mappings for Jolokia				
v2.0		Shashi		2016-09-14		NB region mapping changes for 15MHz				
v2.1		Shashi		2016-09-16		NB region mappching change for 15MHz				

=============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rflm_api_lte.h"
#include "rxlm_intf.h"
/*! CATM NB Region and Max Index*/
#define RFLM_LTE_MAX_NUM_NB_REGION	 5
#define RFLM_LTE_MAX_NUM_NB_INDEX 	 16
typedef struct
{
	rflm_lte_bw_type_s	sys_bw;
	int16	nb_region_offset[RFLM_LTE_MAX_NUM_NB_REGION];
	int16	nb_to_nb_region_map[RFLM_LTE_MAX_NUM_NB_INDEX];
} rflm_lte_get_nb_region_offests_input_s;

lm_status_type rflm_lte_get_rx_nb_region_offsets ( rflm_lte_get_nb_region_offests_input_s *nb_region_info );

