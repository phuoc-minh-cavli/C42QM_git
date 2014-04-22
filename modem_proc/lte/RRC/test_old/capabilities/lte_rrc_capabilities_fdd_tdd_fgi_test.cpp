/*!
  @file lte_rrc_capabilities_fdd_tdd_fgi_test.cpp

  @brief
  Inject Capability Enquiry DLM and get response with diff FGI for FDD and TDD controlled via EFS

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/13   vv     Added assitional unit test cases and modified existing test cases per current src code
05/01/12   ap     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_osys_asn1util.h>
#include <__lte_rrc_capabilities.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_capabilities.h>
#include <lte_rrc_capabilitiesi.h>
#include <fs_public.h>
}

#include <tf_stub.h>
#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_capabilities_test_common.h"
#include <qtf_efs.h>
#include <fs_lib.h>
extern "C"
{
  TF_PROTO_STUB(lte_3gpp_release_ver_e,lte_rrc_get_3gpp_release_version, void);
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
  TF_PROTO_STUB(uint64, lte_rrc_cap_get_lte_ca_band_mask, sys_sband_lte_e_type);
  extern STATIC lte_rrc_cap_s lte_rrc_cap_data;
}

//message structures
static lte_rrc_mh_dlm_s cap_enq_dlm;
static lte_rrc_ul_msg_cnfi_s cap_ulm_cnfi;
/******************CREATING LOOKUP  TABLE**********************/

 /*
FDD-NI6.0-1-32	:: 7FDFFFE0
FDD-DI1.0-1-32	:: 7F5FFFE0
FDD-DI2.0-1-32	:: 7FDFFFE0  
FDD-BO1.0-1-32  :: 7FDFFFE2   
TDD-NI6.0-1-32	:: 7F0FFDE0
TDD-DI1.0-1-32	:: 5E0DD880
TDD-DI2.0-1-32	:: 7FCFFFC0  
TDD-BO1.0-1-32	:: 7FCFFFC2 
*/

unsigned int mask2 = 0;

static unsigned int mode_rel8_pl_fgi[2][4] = {{                                             0x7FDFFFE2,		/*FDD-NI6.0-1-32*/
											   0x7F5FFFE2,		/*FDD-DI1.0-1-32*/
											   0x7FDFFFE2,		/*FDD-DI2.0-1-32*/
                                                                                           0x7FDFFFE2},         /*FDD-BO1.0-1-32*/  
											  {0x5F0FFDE2 ,		/*TDD-NI6.0-1-32*/
											   0x5E0DD882,		/*TDD-DI1.0-1-32*/
											   0x7F4FFFC2,   	/*TDD-DI2.0-1-32*/
                                                                                           0x7FCFFFC2}};        /*TDD-BO1.0-1-32*/

 /*
[0][0][0] : FDD-NI6.0-1-32      	::  7FDFFFFE
[0][0][1] : FDD-NI6.0-33-64     	::  F0000000
[0][1][0] : FDD-DIM1.0-1-32    	::  7F5FFFFA  
[0][1][1] : FDD-DIM1.0-33-64  	::  F0000000  
[0][2][0] : FDD-DIM2.0-1-32    	::  7FDFFFFE
[0][2][1] : FDD-DIM2.0-33-64   	::  F0000000  
[0][3][0] : FDD-BO1.0-1-32    	::  7FDFFFFE
[0][3][1] : FDD-BO1.0-33-64   	::  F0000000  
[1][0][0] : TDD-NI6.0-1-32      	::  7F0FFDE6
[1][0][1] : TDD-NI6.0-33-64	::  80000000
[1][1][0] : TDD-DIM1.0-1-32	::  7E0DD882
[1][1][1] : TDD-DIM1.0-33-64	::  0
[1][2][0] : TDD-DIM2.0-1-32	::  7FCFFFCE
[1][2][1] : TDD-DIM2.0-33-64	::  C0000000
[1][3][0] : TDD-BO1.0-1-32	::  7FCFFFDE
[1][3][1] : TDD-BO1.0-33-64	::  C0000000  
 */ 
static unsigned int mode_rel9_pl_fgi[2][4][2] = {{{0x7FDFFFFE,		/*FDD-NI6.0-1-32*/
												   0xF0000000},		/*FDD-NI6.0-33-64*/
												  {0x7F5FFFFA,		/*FDD-DI1.0-1-32*/
                                                   0xF0000000},		/*FDD-DI1.0-33-64*/
                                                  {0x7FDFFFFE,		/*FDD-DI2.0-1-32*/
                                                   0xF0000000},	/*FDD-DI2.0-33-64*/
                                                { 0x7FDFFFFE,  /*FDD-BO1.0-1-32*/
                                                  0xD0000000}}, /*FDD-BO1.0-33-64*/    
                                                 {{0x7F0FFDE6,		/*TDD-NI6.0-1-32*/
                                                   0x80000000},		/*TDD-NI6.0-33-64*/
                                                  {0x7E0DD882,		/*TDD-DI1.0-1-32*/
                                                   0x00000000},		/*TDD-DI1.0-33-64*/
                                                  {0x7F4FFFDE,		/*TDD-DI2.0-1-32*/
                                                   0xC0000000},	/*TDD-DI2.0-33-64*/
                                                  {0x7FCFFFDE, /*TDD-BO1.0-1-32*/
                                                   0xC0000000}}};    /*TDD-BO1.0-33-64*/
                                     

/*
[0][0][0] : FDD-DIM1.0-1-32         :: 7F5FFFFA
[0][0][1] : FDD-DIM1.0-33-64       :: F0000000
[0][0][2] : FDD-DIM1.0-101-132    :: 240000
[0][1][0] : FDD-DIM2.0-1-32         :: 7FDFFFFE
[0][1][1] : FDD-DIM2.0-33-64       :: F0000000
[0][1][2] : FDD-DIM2.0-101-132    :: 3407872 
[0][2][0] : FDD-BO1.0-1-32         :: 7FDFFFFE
[0][2][1] : FDD-BO1.0-33-64       :: F0000000
[0][2][2] : FDD-BO1.0-101-132    :: 2A240000 
[1][0][0] : TDD-DIM1.0-1-32         :: 7E0D9882
[1][0][1] : TTD-DIM1.0-33-64        :: 0
[1][0][2] : TDD-DIM1.0-101-132    :: 0
[1][1][0] : TDD-DIM2.0-1-32         :: 7FCFBFDE
[1][1][1] : TDD-DIM2.0-33-64       :: 0  
[1][1][2] : TDD-DIM2.0-101-132    :: 00040000 
[1][2][0] : TDD-BO1.0-1-32         :: 7FCFFFDE
[1][2][1] : TDD-BO1.0-33-64       ::  C0000000  
[1][2][2] : TDD-BO1.0-101-132    :: 7CE60000 
*/
static unsigned int mode_rel10_pl_fgi[2][3][3] = {{{0x7F5FFFFA, 	/*FDD-DIM1.0-1-32*/
													0xF0000000,		/*FDD-DIM1.0-33-64*/
													0x00240000},	/*FDD-DIM1.0-101-132*/
												   {0x7FDFFFFE,		/*FDD-DIM2.0-1-32*/
												    0xD0000000,		/*FDD-DIM2.0-33-64*/
												    0x68240000},	/*FDD-DIM2.0-101-132*/
                                                                                                   {0x7FDFFFFE,   /*FDD-BO1.0-1-32*/
                                                                                                    0xD0000000,    /*FDD-BO1.0-33-64*/
                                                                                                    0x68260000}},  /*FDD-BO1.0-101-132*/
												  {{0x7E0D9882,		/*TDD-DIM1.0-1-32*/
												    0x0000000,		/*TDD-DIM1.0-33-64*/
												    0x0000000},		/*TDD-DIM1.0-101-132*/
												   {0x7F4FFFDE,		/*TDD-DIM2.0-1-32*/
												    0xC0000000,		/*TDD-DIM2.0-33-64*/
												    0x28040000},	/*TDD-DIM2.0-101-132*/
                                                                                                   {0x7FCFFFDE,       /*TDD-BO1.0-1-32*/
                                                                                                   0xC0000000,       /*TDD-BO1.0-33-64*/
                                                                                                   0x7CE60000}}};     /*TDD-BO1.0-101-132*/
                                                                                                 
//MU ID to be returned from UL message send
#define LTE_RRC_CAPABILITIES_TEST_MU_ID 7

/*===========================================================================

                                UNIT TEST

===========================================================================*/
/*===========================================================================
	
  FUNCTION:  lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9

===========================================================================*/
/*!
	@brief
	This function return ref_fgi after looking in the lookuptable.
	
	mod 	::	FDD(0) TDD(1)
	pl		::	NI-6.X(0) DI-1.X(1) DI-2.X(2) BO-1.X (3):: for release 8 &9
				DI-1.X(0) DI-2.X(1) BO-1.X(2):: for release 10
	fgi_rel 	::	1-32(0) 33-64(1) 101-132(2)
	gpp_rel 	::	8(0), 9(1), 10(2)

	@return
	ref_fgi
	
*/
/*=========================================================================*/
#define PL 		3
#define FDD 		0
#define TDD 		1
#define FGI_REL_1_32 	0
#define FGI_REL_33_64 	1
#define FGI_REL_101_132 2


static unsigned int ref_fgi(int mod, int pl, int fgi_rel, int gpp_rel)
{
   switch (gpp_rel)
   {
   	case 0:
   	{
	   return mode_rel8_pl_fgi[mod][pl];
   	}
   	break;
   	case 1:
   	{
	   return mode_rel9_pl_fgi[mod][pl][fgi_rel];
   	}
   	break;
   	case 2:
   	{
	   return mode_rel10_pl_fgi[mod][pl][fgi_rel];
   	}
   	break;
   	default:
	{
	   TF_MSG("Invalid release");
	   return -1;
	}
   };
					   
}


/*===========================================================================

  FUNCTION:  lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API that returns the release version of LTE RRC.

    @return
    lte_rrc_release_e, denoting the LTE release version.

*/
/*=========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API that returns the release version of LTE RRC.

    @return
    lte_rrc_release_e, denoting the LTE release version.

*/
/*=========================================================================*/
static lte_3gpp_release_ver_e lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel8
(
  void
)
{
  return LTE_3GPP_REL8_JUN09 ;
} /* lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel8 */


static lte_3gpp_release_ver_e lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9
(
  void
)
{
  return LTE_3GPP_REL9_SEP10 ;
} /* lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9 */

static lte_3gpp_release_ver_e lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10
(
  void
)
{
  return LTE_3GPP_REL10 ;
} /* lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10 */

static lte_3gpp_release_ver_e lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel11
(
  void
)
{
  return LTE_3GPP_REL11 ;
} /* lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel11 */

/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd

===========================================================================*/
/*!
    @brief
    This function simulates the function that returns the last camped mode
    
    @return
    lte_rrc_div_duplex_e, LTE_RRC_FREQ_DIV_DUPLEX means last camped mode is FDD

*/
/*=========================================================================*/
static lte_rrc_div_duplex_e lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd
(
  void
)
{
  return LTE_RRC_FREQ_DIV_DUPLEX;
} /* lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd */

/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd

===========================================================================*/
/*!
    @brief
    This function simulates the function that returns the last camped mode
    
    @return
    lte_rrc_div_duplex_e, LTE_RRC_TIME_DIV_DUPLEX means last camped mode is TDD

*/
/*=========================================================================*/
static lte_rrc_div_duplex_e lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd
(
  void
)
{
  return LTE_RRC_TIME_DIV_DUPLEX;
} /* lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd */

/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_efs_diffxddfgi_init

===========================================================================*/
/*!
    @brief
    This function initializes the diff_fdd_tdd_fgi_enable efs with the value given
    
    @return
    None

*/
/*=========================================================================*/
static void lte_rrc_capabilities_test_efs_diffxddfgi_init
(
  uint8 diff_fdd_tdd_fgi_enable
)
{
  int oflag, mode, efs_ret;
  uint8 test;
  
  /* Init EFS */
  qtf_efs_initialize("diff_fdd_tdd_fgi_enable");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/diff_fdd_tdd_fgi_enable", 
					&diff_fdd_tdd_fgi_enable, sizeof(diff_fdd_tdd_fgi_enable), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/cap/diff_fdd_tdd_fgi_enable", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}


/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_efs_fgi_rel9_fdd_init

===========================================================================*/
/*!
    @brief
    This function initializes the fgi efs with the value given
    
    @return
    None

*/
/*=========================================================================*/
static void lte_rrc_capabilities_test_efs_fgi_rel9_fdd_init
(
  uint32 tdd_fgi_enable
)
{
  int oflag, mode, efs_ret;
  uint32 test;
  
  /* Init EFS */
  qtf_efs_initialize("tdd_fgi_enable");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/fgi_rel9", 
					&tdd_fgi_enable, sizeof(uint32), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/cap/fgi_rel9", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}

static void lte_rrc_capabilities_test_efs_fgi_rel8_tdd_init
(
  uint32 tdd_fgi_enable
)
{
  int oflag, mode, efs_ret;
  uint32 test;
  
  /* Init EFS */
  qtf_efs_initialize("tdd_fgi_enable");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/fgi_tdd", 
					&tdd_fgi_enable, sizeof(uint32), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/cap/fgi_tdd", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}

static void lte_rrc_capabilities_test_efs_fgi_rel9_tdd_init
(
  uint32 tdd_fgi_enable
)
{
  int oflag, mode, efs_ret;
  uint32 test;
  
  /* Init EFS */
  qtf_efs_initialize("tdd_fgi_enable");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel9", 
					&tdd_fgi_enable, sizeof(uint32), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel9", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}

static void lte_rrc_capabilities_test_efs_fgi_rel10_fdd_init
(
  uint32 tdd_fgi_enable
)
{
  int oflag, mode, efs_ret;
  uint32 test;
  
  /* Init EFS */
  qtf_efs_initialize("tdd_fgi_enable");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/fgi_r10", 
					&tdd_fgi_enable, sizeof(uint32), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/cap/fgi_r10", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}

static void lte_rrc_capabilities_test_efs_fgi_rel10_tdd_init
(
  uint32 tdd_fgi_enable
)
{
  int oflag, mode, efs_ret;
  uint32 test;
  
  /* Init EFS */
  qtf_efs_initialize("tdd_fgi_enable");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel10", 
					&tdd_fgi_enable, sizeof(uint32), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel10", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}

static void lte_rrc_capabilities_test_efs_fgi_rel8_fdd_init
(
  uint32 tdd_fgi_enable
)
{
  int oflag, mode, efs_ret;
  uint32 test;
  
  /* Init EFS */
  qtf_efs_initialize("tdd_fgi_enable");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/fgi", 
					&tdd_fgi_enable, sizeof(uint32), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/cap/fgi", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}


/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_neigh_cell_si_acq_efs_init

===========================================================================*/
/*!
    @brief
    This function initializes the neigh cell si acq for hoi efs with the value given
    
    @return
    None

*/
/*=========================================================================*/
static void lte_rrc_capabilities_neigh_cell_si_acq_efs_init
(
  uint32 neigh_cell_si_acq_enable
)
{
  int oflag, mode, efs_ret;
  uint32 test;
  
  /* Init EFS */
  qtf_efs_initialize("neigh_cell_si_acq_efs_enable");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/neigh_cell_si_acq_feature", 
					&neigh_cell_si_acq_enable, 3, oflag, mode) >= 0);

  /* Read the first byte */
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/cap/neigh_cell_si_acq_feature", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}


//-----------------------------------------------------------------------------
/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_efs_diffxddfgi_init

===========================================================================*/
/*!
    @brief
    This function initializes the all supported RAT
    
    @return
    None

*/
/*=========================================================================*/

static void lte_rrc_enable_all_rat()
{

	sys_band_mask_type cdma_band_preference;
	sys_band_mask_type gsm_band_preference;
	sys_band_mask_type wcdma_band_preference;

		
	lte_rrc_capabilities_test_set_cdma_1x_is_supported(TRUE);
	lte_rrc_capabilities_test_set_gsm_is_supported(TRUE);
	lte_rrc_capabilities_test_set_cdma_do_is_supported(TRUE);
	lte_rrc_capabilities_test_set_w_is_supported(TRUE);
	
	
	 //specify three CDMA bands
	  cdma_band_preference = 0;
	  cdma_band_preference |= SYS_BAND_MASK_BC8; 
	  cdma_band_preference |= SYS_BAND_MASK_BC10; 
	  cdma_band_preference |= SYS_BAND_MASK_BC6;
	
	  lte_rrc_capabilities_test_stub_csp_set_do_band_preference(cdma_band_preference);
    lte_rrc_capabilities_test_stub_csp_set_1x_band_preference(
    cdma_band_preference);	 
	  /*Set the GSM stuff*/
	  gsm_band_preference = 0;
	  gsm_band_preference |= SYS_BAND_MASK_GSM_750 ;
	  lte_rrc_capabilities_test_stub_csp_set_gsm_band_preference(gsm_band_preference);
	
	//specify three CDMA bands
	   cdma_band_preference = 0;
	   cdma_band_preference |= SYS_BAND_MASK_BC8; 
	   cdma_band_preference |= SYS_BAND_MASK_BC10; 
	   cdma_band_preference |= SYS_BAND_MASK_BC6;
	   lte_rrc_capabilities_test_stub_csp_set_do_band_preference(
		cdma_band_preference);
	 lte_rrc_capabilities_test_stub_csp_set_1x_band_preference(
		cdma_band_preference);
	//specify three CDMA bands
	   wcdma_band_preference = 0;
	   wcdma_band_preference |=  SYS_BAND_MASK_WCDMA_I_IMT_2000;
	   lte_rrc_capabilities_test_stub_csp_set_wcdma_band_preference(
		wcdma_band_preference);

}

//-----------------------------------------------------------------------------

#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel8);

void LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel8::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel8;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel8::Test()
{
#if 1
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  uint32 rcv_fgi = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND2);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND4);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND8);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 == ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);
  
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.featureGroupIndicatorsPresent); 	
  	
  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
 

  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL, FGI_REL_1_32, 0),rcv_fgi);

  TF_ASSERT(ref_fgi(FDD, PL, FGI_REL_1_32, 0) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == LTE_RRC_CAPABILITIES_SM__INITIAL);
#endif
  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel8::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel8);

void LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel8::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel8;

  lte_rrc_capabilities_test_efs_fgi_rel8_fdd_init(0xFFFFFFFF);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel8::Test()
{
#if 1
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  uint32 rcv_fgi = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND2);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND4);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND8);	

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 == ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);
  
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.featureGroupIndicatorsPresent); 	
  	
  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
 

  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",(ref_fgi(FDD, PL, FGI_REL_1_32, 0) | (1 << 31) | (1 << 21)),rcv_fgi);

  TF_ASSERT((ref_fgi(FDD, PL, FGI_REL_1_32, 0) | (1 << 31) | (1 << 21)) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == LTE_RRC_CAPABILITIES_SM__INITIAL);
#endif
  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel8::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel8);

void LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel8::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
 
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel8;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
  lte_rrc_enable_all_rat();  
}

void LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel8::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  
  //lte_irat_capabilities_s irat_capabilities;
  uint32 rcv_fgi = 0;
  uint32 fgi_mask = 0;
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);
  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND34);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND35);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND36);	

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.featureGroupIndicatorsPresent); 		
  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  
  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL, FGI_REL_1_32, 0),rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL, FGI_REL_1_32, 0) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel8::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel8);

void LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel8::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel8;
  lte_rrc_capabilities_test_efs_fgi_rel8_tdd_init(0xFFFFFFFF);
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
  lte_rrc_enable_all_rat();  
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel8::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  
  //lte_irat_capabilities_s irat_capabilities;
  uint32 rcv_fgi = 0;
  uint32 fgi_mask = 0;
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);


  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
    SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND36);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND37);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND38);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND39);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.featureGroupIndicatorsPresent); 		
  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  
  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",(ref_fgi(TDD, PL, FGI_REL_1_32, 0) | (1 << 31) ),rcv_fgi);
  TF_ASSERT((ref_fgi(TDD, PL, FGI_REL_1_32, 0) | (1 << 31) ) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel8::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif


#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiDisableWithLastTddBandRel8);

void LteRrcCapabilitiesFddTddFgiDisableWithLastTddBandRel8::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(0);
  
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel8;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiDisableWithLastTddBandRel8::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  uint32 rcv_fgi = 0;

  TF_MSG("Start of test");
  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);


  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND43);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND42);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.featureGroupIndicatorsPresent); 		
  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(0 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			

  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL, FGI_REL_1_32, 0),rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL, FGI_REL_1_32, 0) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiDisableWithLastTddBandRel8::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif


#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiDisableWithLastFddBandRel8);

void LteRrcCapabilitiesFddTddFgiDisableWithLastFddBandRel8::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(0);
  
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel8;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiDisableWithLastFddBandRel8::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  uint32 rcv_fgi = 0;
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

   //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND10);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND12);
  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.featureGroupIndicatorsPresent); 		
  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(0 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			

  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL, FGI_REL_1_32, 0),rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL, FGI_REL_1_32, 0) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiDisableWithLastFddBandRel8::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiDisableAllWithLastFddBandRel8);

void LteRrcCapabilitiesFddTddFgiDisableAllWithLastFddBandRel8::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(0);
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel8;

  lte_rrc_capabilities_test_efs_fgi_rel8_fdd_init(0xFFFFFFFF);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiDisableAllWithLastFddBandRel8::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  uint32 rcv_fgi = 0;
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);
  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND10);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND12);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.featureGroupIndicatorsPresent); 		
  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(0 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			

  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",(ref_fgi(FDD, PL, FGI_REL_1_32, 0) | (1 << 31) | (1 << 21)),rcv_fgi);
  TF_ASSERT((ref_fgi(FDD, PL, FGI_REL_1_32, 0) | (1 << 31) | (1 << 21)) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiDisableAllWithLastFddBandRel8::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (FDD only supported) */

#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel9);

void LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel9::Setup()
{
  lte_rrc_capabilities_neigh_cell_si_acq_efs_init(0x030303);
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  lte_rrc_cap_data.sd_ptr->diff_fdd_tdd_fgi_enable = 1;
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel9::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  sys_lte_band_mask_e_type lte_band_preference;
  uint32 rcv_fgi = 0;
  uint32 refe_fgi = 0;
  
  //specify bands
   SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	 SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	 SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND20);
	 SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND8);	 

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(lte_band_preference);

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == LTE_RRC_CAPABILITIES_SM__INITIAL);

    //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(1  == decoded_eutra_cap_ptr->m.featureGroupIndicatorsPresent); 		
  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1  == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL, FGI_REL_33_64, 1) , rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL, FGI_REL_33_64, 1) == rcv_fgi);

  
  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  refe_fgi = ref_fgi(FDD, PL, FGI_REL_1_32, 1);
  refe_fgi &= (~(1 << 2));
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n", refe_fgi, rcv_fgi);
  TF_ASSERT(refe_fgi == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel9::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif


#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel9);

void LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel9::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9;
  
  lte_rrc_capabilities_test_efs_fgi_rel9_fdd_init(0xFFFFFFFF);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel9::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  sys_lte_band_mask_e_type lte_band_preference;
  uint32 rcv_fgi = 0;
  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
    SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND8);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND20);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND39);
  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(lte_band_preference);

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == LTE_RRC_CAPABILITIES_SM__INITIAL);

    //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(1  == decoded_eutra_cap_ptr->m.featureGroupIndicatorsPresent); 		
  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1  == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL, FGI_REL_33_64, 1), rcv_fgi);
  TF_ASSERT((ref_fgi(FDD, PL, FGI_REL_33_64, 1) | (1 << (LTE_RRC_CAP_FGI_BIT_64 - LTE_RRC_CAP_FGI_BIT_35)))== rcv_fgi);

  
  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n", ref_fgi(FDD, PL, FGI_REL_1_32, 1) , rcv_fgi);
  TF_ASSERT( ref_fgi(FDD, PL, FGI_REL_1_32, 1) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel9::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

#if 1 
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (TDD only supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel9);

void LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel9::Setup()
{
  lte_rrc_capabilities_neigh_cell_si_acq_efs_init(0x000200);
  
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  lte_rrc_cap_data.sd_ptr->diff_fdd_tdd_fgi_enable = 1;
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel9::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  uint32 rcv_fgi = 0;
  uint32 refe_fgi = 0;
  
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);	

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1  == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL, FGI_REL_33_64, 1), rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL, FGI_REL_33_64, 1) == rcv_fgi);

  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  refe_fgi = ref_fgi(TDD, PL, FGI_REL_1_32, 1);
  refe_fgi &= (~(1 << 2));
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n", refe_fgi, rcv_fgi);
  TF_ASSERT(refe_fgi == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel9::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

#if 1 
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (TDD only supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel9);

void LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel9::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  lte_rrc_capabilities_test_efs_fgi_rel9_tdd_init(0xFFFFFFFF);
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel9::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  uint32 rcv_fgi = 0;
  
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND36);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND41);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1  == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL, FGI_REL_33_64, 1), rcv_fgi);
  TF_ASSERT((ref_fgi(TDD, PL, FGI_REL_33_64, 1) | (1 << (LTE_RRC_CAP_FGI_BIT_64 - LTE_RRC_CAP_FGI_BIT_35)))== rcv_fgi);

  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",(ref_fgi(TDD, PL, FGI_REL_1_32, 1) & 0xFFFFFFFB /*
  /*Inter Frequency HO between FDD and TDD*/) , rcv_fgi);
  TF_ASSERT(((ref_fgi(TDD, PL, FGI_REL_1_32, 1))& 0xFFFFFFFB) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel9::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif


#if 1
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (FDD and TDD supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableWithFddTddBandRel9);

void LteRrcCapabilitiesFddTddFgiEnableWithFddTddBandRel9::Setup()
{
  lte_rrc_capabilities_neigh_cell_si_acq_efs_init(0x000200);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  lte_rrc_cap_data.sd_ptr->diff_fdd_tdd_fgi_enable = 1;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =  	lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddTddBandRel9::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  uint32 rcv_fgi = 0;
  uint32 fgi_mask = 0;
  unsigned char *encoded_msg;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);
  //specify two bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);
  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.\
  		    m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    m.lateNonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndicators_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9.numbits);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndicators_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9.numbits);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.m.neighCellSI_AcquisitionParameters_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.m.neighCellSI_AcquisitionParameters_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);
  

  /* Verify FDD FGI */ /*individual FDD FGI for rel 9*/
  /*FDD FGI = FGI defaults(1-32) 000*/
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9);  
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL, FGI_REL_1_32, 1), rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL, FGI_REL_1_32, 1) == rcv_fgi);  
  /* Verify TDD FGI */ 

  /*TDD FGI = FDD FGI & TDD mask(1-32)  000 & 100 & some masked bits in TDD*/
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL, FGI_REL_1_32, 1), rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL, FGI_REL_1_32, 1) == rcv_fgi); 

  /* Verify Common FGI */ /* FDD + TDD rel 8 */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  /*common FGI = FDD FGI & TDD FGI(1-32) 000 & 100*/
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL, FGI_REL_1_32, 1),rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL, FGI_REL_1_32, 1) == rcv_fgi);

  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
  /*common FGI = FDD FGI & TDD FGI(33-64) 001 & 101*/

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD,PL,FGI_REL_33_64,1) , rcv_fgi);
  TF_ASSERT(ref_fgi(TDD,PL,FGI_REL_33_64,1) == rcv_fgi);


  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9.numbits);

  /*FDD FGI = FGI defaults(33-64) 001*/

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD,PL,FGI_REL_33_64,1) , rcv_fgi);
  TF_ASSERT(ref_fgi(FDD,PL,FGI_REL_33_64,1) == rcv_fgi);


  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9.numbits);
  /*TDD FGI = FDD FGI & TDD mask(33-64) 001 & 101 & some masked bits in TDD*/

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD,PL,FGI_REL_33_64,1),rcv_fgi);
  TF_ASSERT(ref_fgi(TDD,PL,FGI_REL_33_64,1) == rcv_fgi); 

//free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddTddBandRel9::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif
#if 1
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD after disabling splitFGI feature (supporting both FDD & TDD band):: last camped mode TDD */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiDisableWithTddBandRel9);

void LteRrcCapabilitiesFddTddFgiDisableWithTddBandRel9::Setup()
{
  lte_rrc_capabilities_neigh_cell_si_acq_efs_init(0x030303);
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
lte_rrc_cap_data.sd_ptr->diff_fdd_tdd_fgi_enable = 0;
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9;  
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =  	lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiDisableWithTddBandRel9::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  lte_irat_capabilities_s irat_capabilities;
  uint32 rcv_fgi = 0;
  
  TF_MSG("Start of test");
  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);
  //specify two bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);		
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);			
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.\
  		    m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL, FGI_REL_33_64, 1),rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL, FGI_REL_33_64, 1) == rcv_fgi);

  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL, FGI_REL_1_32, 1),rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL, FGI_REL_1_32, 1) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiDisableWithTddBandRel9::Teardown()
{
  lte_rrc_utf_teardown();
}

#endif

#if 1
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD after disabling splitFGI feature (supporting both FDD & TDD band):: last camped mode FDD */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiDisableWithFddBandRel9);

void LteRrcCapabilitiesFddTddFgiDisableWithFddBandRel9::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(0);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel9;  
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiDisableWithFddBandRel9::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  
  lte_irat_capabilities_s irat_capabilities;

  uint32 rcv_fgi = 0;
  uint32 fgi_mask = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == LTE_RRC_CAPABILITIES_SM__INITIAL);
	  //specify two bands
	  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
		SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
		SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.\
  		    m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL, FGI_REL_33_64, 1),rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL, FGI_REL_33_64, 1) == rcv_fgi);

  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL, FGI_REL_1_32, 1),rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL, FGI_REL_1_32, 1) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiDisableWithFddBandRel9::Teardown()
{
  lte_rrc_utf_teardown();
}

#endif
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (FDD only supported) */

static uint64 lte_rrc_test_cap_get_lte_ca_band_mask1(sys_sband_lte_e_type lte_band)
{
   TF_MSG("Stub 'lte_rrc_test_cap_get_lte_ca_band_mask1' invoked");
   switch (lte_band) {
   case SYS_SBAND_LTE_EUTRAN_BAND1:
      return 0x160011;
      break;
   case SYS_SBAND_LTE_EUTRAN_BAND2:
      return 0x10010000;
      break;
      case SYS_SBAND_LTE_EUTRAN_BAND3:
      return 0x800D0;
      break;

   case SYS_SBAND_LTE_EUTRAN_BAND4:
   return 0x10010011858;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND5:
   return 0x1080D;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND6:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND7:
   return 0x8004C;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND8:
   return 0x80004;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND9:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND10:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND11:
   return 0x20000;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND12:
   return 0x18;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND13:
   return 0x8;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND14:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND17:
   return 0x1A;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND18:
   return 0x401;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND19:
   return 0x1;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND20:
   return 0xC4;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND21:
   return 0x1;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND23:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND24:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND25:
   return 0x1000000;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND26:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND27:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND28:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND29:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND30:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND31:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND33:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND34:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND35:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND36:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND37:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND38:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND39:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND40:
   return 0;
   break;

   case SYS_SBAND_LTE_EUTRAN_BAND41:
   return 0x10000000000;
   break;

   default:
      TF_ASSERT(0);
}

     
}

#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel10);

void LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel10::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  TF_STUB(lte_rrc_cap_get_lte_ca_band_mask) = lte_rrc_test_cap_get_lte_ca_band_mask1;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel10::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  sys_lte_band_mask_e_type lte_band_preference;
  uint32 rcv_fgi = 0;
  uint32 refe_fgi = 0;
 
 //specify bands
 SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
   SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
   SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND5);
   SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND13);
   SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND19);  
 
  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
	lte_band_preference);
  //lte_irat_capabilities_s irat_capabilities;
  
  
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

    //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.lateNonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.nonCriticalExtensionPresent);
  
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);

  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.featureGroupIndRel10_r10Present);
    
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10.numbits);
  

  
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL-1, FGI_REL_101_132, 2), rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL-1, FGI_REL_101_132, 2) == rcv_fgi);
  
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL-1, FGI_REL_33_64, 2), rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL-1, FGI_REL_33_64, 2) == rcv_fgi);

  
  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
    refe_fgi = ref_fgi(FDD, PL-1, FGI_REL_1_32, 2);
  refe_fgi &= (~(1 << 2));
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n", refe_fgi, rcv_fgi);
  TF_ASSERT(refe_fgi == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddBandRel10::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif


#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel10);

void LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel10::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10;

  lte_rrc_capabilities_test_efs_fgi_rel10_fdd_init(0xFFFFFFFF);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  lte_rrc_enable_all_rat();
  TF_STUB(lte_rrc_cap_get_lte_ca_band_mask) = lte_rrc_test_cap_get_lte_ca_band_mask1;
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel10::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  sys_lte_band_mask_e_type lte_band_preference;
  uint32 rcv_fgi = 0;
  
  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND5);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND13);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND19);	
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND40);		

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
	lte_band_preference);
  //lte_irat_capabilities_s irat_capabilities;
  
  
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

    //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.lateNonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.nonCriticalExtensionPresent);
  
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);

  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.featureGroupIndRel10_r10Present);
    
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10.numbits);
  

  
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL-1, FGI_REL_101_132, 2) | (1 << 25), rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL-1, FGI_REL_101_132, 2) | (1 << 25) == rcv_fgi);
  
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL-1, FGI_REL_33_64, 2), rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL-1, FGI_REL_33_64, 2) == rcv_fgi);

  
  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",(ref_fgi(FDD, PL-1, FGI_REL_1_32, 2)), rcv_fgi);
  TF_ASSERT((ref_fgi(FDD, PL-1, FGI_REL_1_32, 2)) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel10::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

#if 1 
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesR11Cap);

void LteRrcCapabilitiesR11Cap::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel11;
  qtf_efs_initialize("LteRrcCapabilitiesFddTddFgiEnableAllWithFddBandRel11 with Rel-11 EFS test case");

  lte_rrc_capabilities_test_efs_fgi_rel10_fdd_init(0xFFFFFFFF);
  uint32 tdd_cap_r11 = 0xFFFFFFFF; /*Enable all R11 Capabilties*/
  int result;
  result = efs_put("/nv/item_files/modem/lte/rrc/cap/tdd_cap_r11",
                    &tdd_cap_r11,
                    4, O_RDWR|O_AUTODIR|O_TRUNC,
                    0777);
   TF_ASSERT(0 == result);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  

  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
  lte_rrc_enable_all_rat();
  TF_STUB(lte_rrc_cap_get_lte_ca_band_mask) = lte_rrc_test_cap_get_lte_ca_band_mask1;
}

void LteRrcCapabilitiesR11Cap::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  sys_lte_band_mask_e_type lte_band_preference;
  uint32 rcv_fgi = 0;
  
  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND5);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND13);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND19);	
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND40);		

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
	lte_band_preference);
  //lte_irat_capabilities_s irat_capabilities;
  
  
  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

    //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);
  TF_ASSERT(lte_rrc_cap_get_r11_cap_bit(LTE_RRC_CAP_R11_BIT_8) == TRUE);
  TF_ASSERT(lte_rrc_cap_get_r11_cap_bit(LTE_RRC_CAP_R11_BIT_3) == TRUE); 
  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.lateNonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.pdcp_Parameters_v1130.m.supportRohcContextContinue_r11Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.phyLayerParameters_v1130Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.phyLayerParameters_v1130.m.tdd_SpecialSubframe_r11Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.ue_BasedNetwPerfMeasParameters_r10Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.ue_BasedNetwPerfMeasParameters_r10.m.loggedMeasurementsIdle_r10Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.ue_BasedNetwPerfMeasParameters_r10.m.standaloneGNSS_Location_r10Present);
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);

  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.featureGroupIndRel10_r10Present);
    
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10.numbits);
  

  

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesR11Cap::Teardown()
{
  lte_rrc_utf_teardown();
} 
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (TDD only supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel10);

void LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel10::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  lte_rrc_capabilities_test_efs_fgi_rel10_fdd_init(0xFFFFFFFF);
  //lte_rrc_capabilities_test_efs_fgi_rel9_fdd_init(0xFFFFFFFF);
  //lte_rrc_capabilities_test_efs_fgi_rel8_fdd_init(0xFFFFFFFF);

  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
 TF_STUB(lte_rrc_cap_get_lte_ca_band_mask) = lte_rrc_test_cap_get_lte_ca_band_mask1;
  lte_rrc_enable_all_rat();
}

void LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel10::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  lte_irat_capabilities_s irat_capabilities;
  uint32 rcv_fgi = 0;
  uint32 refe_fgi = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify two bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);



  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.lateNonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.nonCriticalExtensionPresent);
  
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);

  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.featureGroupIndRel10_r10Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10.numbits)



  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_101_132, 2)| (1 << 25), rcv_fgi);
  TF_ASSERT((ref_fgi(TDD, PL-1, FGI_REL_101_132, 2) & 0xFDFFFFFF)| (1 << 25)== rcv_fgi);
  
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_33_64, 2), rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL-1, FGI_REL_33_64, 2) == rcv_fgi);
  
  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  refe_fgi = ref_fgi(TDD, PL-1, FGI_REL_1_32, 2);
  refe_fgi &= (~(1 << 2));
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n", refe_fgi, rcv_fgi);
  TF_ASSERT(refe_fgi == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableWithTddBandRel10::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

#if 1 
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (TDD only supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel10);

void LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel10::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  lte_rrc_capabilities_test_efs_fgi_rel10_tdd_init(0xFFFFFFFF);
  lte_rrc_capabilities_test_efs_fgi_rel9_tdd_init(0xFFFFFFFF);
  lte_rrc_capabilities_test_efs_fgi_rel8_tdd_init(0xFFFFFFFF);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
  lte_rrc_enable_all_rat();
  TF_STUB(lte_rrc_cap_get_lte_ca_band_mask) = lte_rrc_test_cap_get_lte_ca_band_mask1;
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel10::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  lte_irat_capabilities_s irat_capabilities;
  uint32 rcv_fgi = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

 
  //specify two bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);



  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.lateNonCriticalExtensionPresent);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.nonCriticalExtensionPresent);
  
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);

  TF_ASSERT(1 ==  decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.featureGroupIndRel10_r10Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10.numbits)



  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_101_132, 2) | (1 << 25), rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL-1, FGI_REL_101_132, 2) | (1 << 25) == rcv_fgi);
  
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_33_64, 2), rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL-1, FGI_REL_33_64, 2) == rcv_fgi);
  
  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",(ref_fgi(TDD, PL-1, FGI_REL_1_32, 2) | (1 << 31) ) , rcv_fgi);
  TF_ASSERT((ref_fgi(TDD, PL-1, FGI_REL_1_32, 2) | (1 << 31) ) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableAllWithTddBandRel10::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

#if 1
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (FDD and TDD supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiEnableWithFddTddBandRel10);

void LteRrcCapabilitiesFddTddFgiEnableWithFddTddBandRel10::Setup()
{
  lte_rrc_capabilities_neigh_cell_si_acq_efs_init(0x000100);
#if 0
  lte_rrc_capabilities_test_efs_fgi_rel10_fdd_init(0xFFFFFFFF);

  lte_rrc_capabilities_test_efs_fgi_rel9_tdd_init(0xFFFFFFFF); 
  lte_rrc_capabilities_test_efs_fgi_rel8_tdd_init(0xFFFFFFFF); 
  lte_rrc_capabilities_test_efs_fgi_rel10_fdd_init(0xFFFFFFFF); 
  lte_rrc_capabilities_test_efs_fgi_rel9_fdd_init(0xFFFFFFFF); 
  lte_rrc_capabilities_test_efs_fgi_rel8_fdd_init(0xFFFFFFFF); 
#endif    

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  lte_rrc_cap_data.sd_ptr->diff_fdd_tdd_fgi_enable = 1;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10;
  lte_rrc_enable_all_rat();
  TF_STUB(lte_rrc_cap_get_lte_ca_band_mask) = lte_rrc_test_cap_get_lte_ca_band_mask1;
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddTddBandRel10::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  lte_irat_capabilities_s irat_capabilities;
  uint32 rcv_fgi = 0;

  lte_rrc_cell_id_s cell_id;
  lte_rb_id_t rb_id;
  lte_sfn_s sfn;
  int pdu_num;
  uint16 encoded_msg_len;
  unsigned char *encoded_msg;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify two bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);	

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.m.\
  	lateNonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndicators_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9.numbits);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndicators_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9.numbits);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.neighCellSI_AcquisitionParameters_r9.m.intraFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.neighCellSI_AcquisitionParameters_r9.m.interFreqSI_AcquisitionForHO_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.neighCellSI_AcquisitionParameters_r9.m.utran_SI_AcquisitionForHO_r9Present);


  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.featureGroupIndRel10_r10Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10.numbits);
  /*Common FDD + TDD FGI: REL-10 Feature: 101-132*/
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.\
  	nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10);  
  TF_MSG("\n1. ref_fgi [%x] rcv_fgi [%x]\n",(ref_fgi(FDD, PL-1, FGI_REL_101_132, 2) & ref_fgi(TDD, PL-1, FGI_REL_101_132, 2)) , rcv_fgi); /*****************************FGI :: 1*************************/
  TF_ASSERT((ref_fgi(FDD, PL-1, FGI_REL_101_132, 2) & ref_fgi(TDD, PL-1, FGI_REL_101_132, 2) & 0xFDEFFFFF ) == rcv_fgi); 
  
  
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.\
  	m.fdd_Add_UE_EUTRA_Capabilities_v1060Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.\
  	fdd_Add_UE_EUTRA_Capabilities_v1060.m.featureGroupIndRel10_v1060Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.\
  	fdd_Add_UE_EUTRA_Capabilities_v1060.featureGroupIndRel10_v1060.numbits);
  /*FDD FGI: REL-10 Feature: 101-132*/

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	nonCriticalExtension.nonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_v1060.featureGroupIndRel10_v1060);  
  TF_MSG("\n2. ref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL-1, FGI_REL_101_132, 2) , rcv_fgi); /*****************************FGI :: 2*************************/
  TF_ASSERT((ref_fgi(FDD, PL-1, FGI_REL_101_132, 2) & 0xFDEFFFFF )== rcv_fgi); 
  
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.\
  	m.tdd_Add_UE_EUTRA_Capabilities_v1060Present);
  TF_ASSERT(1  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.\
  	tdd_Add_UE_EUTRA_Capabilities_v1060.m.featureGroupIndRel10_v1060Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.\
  	tdd_Add_UE_EUTRA_Capabilities_v1060.featureGroupIndRel10_v1060.numbits);
  /*TDD FGI: REL-10 Feature: 101-132*/

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  	nonCriticalExtension.nonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_v1060.featureGroupIndRel10_v1060);  
  TF_MSG("\n3. ref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_101_132, 2) , rcv_fgi); /*****************************FGI :: 3*************************/
  TF_ASSERT((ref_fgi(TDD, PL-1, FGI_REL_101_132, 2) & 0xFDEFFFFF) == rcv_fgi); 



  /* Verify FDD FGI */ /*individual FDD FGI for rel 9*/
  /*FDD FGI = FGI defaults(1-32) 000*/
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9);  
  TF_MSG("\n4. ref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL-1, FGI_REL_1_32, 2), rcv_fgi); /*****************************FGI :: 4*************************/
  TF_ASSERT((ref_fgi(FDD, PL-1, FGI_REL_1_32, 2)) == rcv_fgi); 


  TF_ASSERT(ref_fgi(FDD, PL, FGI_REL_1_32, 1) == rcv_fgi);  
  /* Verify TDD FGI */ 
  /*TDD FGI = FDD FGI & TDD mask(1-32)  000 & 100 & some masked bits in TDD*/
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9);
  TF_MSG("\n5. ref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_1_32, 2), rcv_fgi); /*****************************FGI :: 5*************************/
  TF_ASSERT(ref_fgi(TDD, PL-1, FGI_REL_1_32, 2)== rcv_fgi); 
  

  /* Verify Common FGI */ /* FDD + TDD rel 8 */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  /*common FGI = FDD FGI & TDD FGI(1-32) 000 & 100*/
  TF_MSG("\n6. ref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_1_32, 2),rcv_fgi); /*****************************FGI :: 6*************************/
  TF_ASSERT(ref_fgi(TDD, PL-1, FGI_REL_1_32, 2) == rcv_fgi); 

  //TF_ASSERT(ref_fgi(TDD, 0, FGI_REL_1_32, 1)== rcv_fgi);

  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
  /*common FGI = FDD FGI & TDD FGI(33-64) 001 & 101*/

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\n7. ref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD,PL-1,FGI_REL_33_64,2) , rcv_fgi); /*****************************FGI :: 7*************************/
  TF_ASSERT(ref_fgi(TDD,PL-1,FGI_REL_33_64,2) == rcv_fgi); 

  TF_ASSERT(ref_fgi(TDD,PL,FGI_REL_33_64,1) == rcv_fgi);


  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9.numbits);

  /*FDD FGI = FGI defaults(33-64) 001*/

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9);
  TF_MSG("\n8. ref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD,PL-1,FGI_REL_33_64,2) , rcv_fgi); /*****************************FGI :: 8*************************/
  TF_ASSERT(ref_fgi(FDD,PL-1,FGI_REL_33_64,2) == rcv_fgi); 


  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9.numbits);
  /*TDD FGI = FDD FGI & TDD mask(33-64) 001 & 101 & some masked bits in TDD*/

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9);
  TF_MSG("\n9. ref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD,PL-1,FGI_REL_33_64,2),rcv_fgi); /*****************************FGI :: 9*************************/
  TF_ASSERT(ref_fgi(TDD,PL-1,FGI_REL_33_64,2) == rcv_fgi); 


  TF_ASSERT(ref_fgi(TDD,PL,FGI_REL_33_64,1)== rcv_fgi); 

//free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiEnableWithFddTddBandRel10::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif
#if 1
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD after disabling splitFGI feature (supporting both FDD & TDD band):: last camped mode TDD */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiDisableWithTddBandRel10);

void LteRrcCapabilitiesFddTddFgiDisableWithTddBandRel10::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(0);
 
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10;  
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
  lte_rrc_enable_all_rat();
  TF_STUB(lte_rrc_cap_get_lte_ca_band_mask) = lte_rrc_test_cap_get_lte_ca_band_mask1;
}

void LteRrcCapabilitiesFddTddFgiDisableWithTddBandRel10::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  
  lte_irat_capabilities_s irat_capabilities;

  uint32 rcv_fgi = 0;
  uint32 fgi_mask = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify two bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);	

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.\
  		    m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);


  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.featureGroupIndRel10_r10Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10.numbits);
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.\
  	nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10);  
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_101_132, 2) , rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL-1, FGI_REL_101_132, 2) == rcv_fgi); 
  
  
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.\
  	m.fdd_Add_UE_EUTRA_Capabilities_v1060Present);
  
  TF_ASSERT(0  == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.\
  	m.tdd_Add_UE_EUTRA_Capabilities_v1060Present);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_33_64, 2),rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL-1, FGI_REL_33_64, 2) == rcv_fgi); 

 

  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(TDD, PL-1, FGI_REL_1_32, 2),rcv_fgi);
  TF_ASSERT(ref_fgi(TDD, PL-1, FGI_REL_1_32, 2) == rcv_fgi); 

  TF_ASSERT(ref_fgi(TDD, PL, FGI_REL_1_32, 1)  == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiDisableWithTddBandRel10::Teardown()
{
  lte_rrc_utf_teardown();
}

#endif

#if 1
//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD after disabling splitFGI feature (supporting both FDD & TDD band):: last camped mode TDD */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesFddTddFgiDisableWithFddBandRel10);

void LteRrcCapabilitiesFddTddFgiDisableWithFddBandRel10::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(0);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) =   lte_rrc_cap_test_stub_get_3gpp_release_version_for_rel10;  
  TF_STUB(lte_rrc_csp_get_last_camped_mode) =   lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
  lte_rrc_enable_all_rat();
  TF_STUB(lte_rrc_cap_get_lte_ca_band_mask) = lte_rrc_test_cap_get_lte_ca_band_mask1;

}

void LteRrcCapabilitiesFddTddFgiDisableWithFddBandRel10::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  lte_irat_capabilities_s irat_capabilities;
  uint32 rcv_fgi = 0;


  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify two bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);	

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.\
  		    m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);



  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.m.featureGroupIndRel10_r10Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10.numbits);
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.\
  	nonCriticalExtension.nonCriticalExtension.featureGroupIndRel10_r10);  
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL-1, FGI_REL_101_132, 2) , rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL-1, FGI_REL_101_132, 2) == rcv_fgi);

  

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL-1, FGI_REL_33_64, 2),rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL-1, FGI_REL_33_64, 2) == rcv_fgi);  


//  

  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_MSG("\nref_fgi [%x] rcv_fgi [%x]\n",ref_fgi(FDD, PL-1, FGI_REL_1_32, 2),rcv_fgi);
  TF_ASSERT(ref_fgi(FDD, PL-1, FGI_REL_1_32, 2) == rcv_fgi);

  TF_ASSERT(ref_fgi(FDD, PL, FGI_REL_1_32, 1) == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesFddTddFgiDisableWithFddBandRel10::Teardown()
{
  lte_rrc_utf_teardown();
}

#endif


