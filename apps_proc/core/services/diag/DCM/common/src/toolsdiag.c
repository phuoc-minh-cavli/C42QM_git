/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Routines
                  WCDMA Targets Common Routines

General Description
  Core diagnostic packet processing routines that are common to all targets.

Copyright (c) 2001 - 2014,2016 by QUALCOMM Technologies.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagpkt_tools.c_v   1.6   02 Apr 2002 11:36:40   amcintos  $
$Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/toolsdiag.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/20/16   nk      Changes to support ThreadX
05/27/16   ph      Fixed registration of version 2 extended Mobile ID command.
06/02/15   sa      Added support for Version 2 Extended Mobile ID Command.
12/17/14   ph      Removed diag_strlcpy() and replaced with strlcpy(). 
12/06/13   is      Replace dload_entry() with tms_dload_reset()
10/01/13   xy      Removed code under FEATURE_WINCE 
01/25/13   sr      Fixed compiler warnings  
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
10/31/12   ph      To enable support for version number and extended build id
                   on APPS
09/20/11   hm      Register for only the DLOAD command on Sparrow 
08/24/11   vk      Featurized customer.h inclusion 
04/27/11   is      Resolve compiler warnings
03/30/11   sg      Fixed compilation error 
03/01/11   hm      Replaced deprecated systemdrivers API (hw_partnum_version) 
03/01/11   is      Migrate to diag_strlcpy()
01/21/11   is      Featurize DLoad feature
10/25/10   is      Diag feature flag cleanup
09/17/10   is      Migrate from deprecated clk driver APIs
06/10/10   is      Core image feature flag cleanup
06/02/10   vs      Added feature FEATURE_TOOLSDIAG_ESN
04/29/10   is      Reduce some feature flag uses
03/11/09   vs      fix compiler warnings
07/10/09   as      Updated usage of accessor function instead of CDMA sturct
10/03/08   vg      Updated code to use PACK() vs. PACKED
03/27/08   as      Added support to 32 bit HW model numbers.
07/12/06   as      Featurized for AppsProc,to prevent access to modem code.
10/31/05   as      Fixed lint errors.
04/25/05   as      Added extended build id response to include MSM 
                   hardware revision register under HW_REV_TYPE_V2.
12/17/04   as      Added check for FEATURE_HW_FULL_VERSION to get hw version
12/02/03   yll     Changed the mob_cai_rev to reflect the NV setting.
01/08/03   ma      featurization for GSM PLT
04/01/02   ATM     Added check for NULL after call to diagpkt_alloc
01/10/02   ATM     Corrected the ESN response length
09/21/01   abp     Using correct mobile model number for WCDMA
09/18/01   jal     Removed DIAG_STAT_F support
08/23/01   jal     Removed redundant DIAG_TS_F packet processing
05/08/01   cpe     renamed this file. Previously diagpkt_wcdma.c
05/07/01   lad     Cleaned up file.
02/23/01   lad     Created file.

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#if !defined (FEATURE_DIAG_QNX)
#include "customer.h"
#endif

#ifdef DIAG_TOOLSDIAG_INIT

#if !defined (FEATURE_QNX)
#include "mobile.h"    /* for ver_date, ver_time, rel_date, rel_time */
#include "diagbuf.h"
#if defined (FEATURE_DIAG_NV)
#include "diagnv.h"
#include "nv.h"
#endif
#endif

#if !defined(DIAG_MP_MASTER)
#include "comdef.h"
#include "diag.h"
#include "diagi.h"
#include "string1.h"     

#ifdef DIAG_IS2000
#include "mccdma.h"
#endif

#endif /* !defined(DIAG_MP_MASTER) */

#include "diagcmd.h"   /* for command code DIAG_EXT_BUILD_ID_F */
#include "toolsdiag.h" /* for toolsdiag_init() and structs */

#ifdef DIAG_TOOLSDIAG_DLOAD
  #include "DALSys.h"
  #include "diagbuf_v.h"
  #include "busywait.h"
#endif

#if !defined (FEATURE_QNX)
#include "err.h"
#else
  #include "err_qnx.h"
#endif
#include "diagtune.h"
#include <string.h>    /* for memcpy */

/* for getting ChipFamily/ChipId */
#include <DDIChipInfo.h>
#include "DALDeviceId.h"

#if defined(DIAG_MP_MASTER)

#if defined(FEATURE_DIAG_QNX)
#include "diagpkti.h"  /* for diagpkt_alloc_dcm() */

#include <sys/slog.h>  /* for slogf */
#include "osal.h"      /* for _SLOGC_DIAGDCM */


#define NOTICE_TYPE       const char

  /* ==========================================================================
  ** Version Information
  **   Version Date and Time - when compiled
  **   Version Directory     - QCT CRM four letter build id
  **   Version Number        - QCT CRM four number build version
  **   Release Date          - Same as version
  **   Release Time          - Same as version
  **   Version String        - QCT CRM fully qualified version
  ** ========================================================================== */

   NOTICE_TYPE ver_date[] = __DATE__;
   NOTICE_TYPE ver_time[] = __TIME__;

   NOTICE_TYPE rel_date[] = __DATE__;
   NOTICE_TYPE rel_time[] = __TIME__;
   
#endif /* #if defined (FEATURE_DIAG_QNX) */

typedef enum chipinfo_qpst
{
	MDOEL_INVALLID = 0,

	MODEL_MSM8660 = 4061,                /* no AO defined for it, returning the only value defined by QPST */
	MODEL_AO8960 = 4064,

	/* Data cards */
	MODEL_SURF9615 = 4070,

	MODEL_APQ8064 = 4072,

	/* 8x30 series 8930, 8630, 8230, 8030*/
	MODEL_SURF_APQ8030 = 4079,

	/* 8x27 series */
	MODEL_SURF_MSM8627 = 4080,
	MODEL_SURF_MSM8227 = 4081,

	/* 8974 */
	MODEL_SURF_MSM8974 = 4083,

    /*9625*/
    MODEL_MDM9x25 = 4087,

}chipinfo_qpst;

#endif /* #if defined(DIAG_MP_MASTER) */

extern void tms_dload_reset( void );

/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not 
   used in some of the packets, as this is dictated by the awesome design */
/* lint -esym(715,pkt_len,req_ptr) */

/*=========================================================================== 
 
FUNCTION DIAG_HW_PARTNUM_VERSION 
 
DESCRIPTION 
  This procedure replaces the depreciated system driver's API hw_partnum_version
  Uses Chipinfo DAL APIs
 
===========================================================================*/
void diag_hw_partnum_version(diag_hw_rev_type *pmRev)
{
  uint32 nPartNum;
  uint32 nVersion;
  DalDeviceHandle *phChipInfo;

  /*-----------------------------------------------------------------------*/
  /* Connect to the DAL.                                                   */
  /*-----------------------------------------------------------------------*/
  if (DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo) != DAL_SUCCESS)
 {
   return;
 }

  DalChipInfo_GetRawChipId(phChipInfo, &nPartNum);
  DalChipInfo_GetRawChipVersion(phChipInfo, &nVersion);

  pmRev->version = (uint8)nVersion;
  pmRev->partnum = (uint16)nPartNum;
  pmRev->hw_register =
    (pmRev->version << 28) | (pmRev->partnum << 12);

} /* diag_hw_partnum_version */

/*===========================================================================

FUNCTION TOOLSDIAG_VERNO

DESCRIPTION
  This procedure processes a version number request.  It returns the version
  number, and the date and time of compilation.

===========================================================================*/
PACK(void *) toolsdiag_verno (PACK(void *) req_pkt, uint16 pkt_len)
{
  int i;
  word ver;
  DIAG_VERNO_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_VERNO_F_rsp_type);
  int8 sci_p = 0;
  byte scm = 0;
  byte mob_cai_rev = 0;

  diag_hw_rev_type new_ver = {0,0,0}; /* Buffer for the new MSM version information */

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
#if defined (FEATURE_DIAG_QNX)
	rsp_ptr = (DIAG_VERNO_F_rsp_type *)diagpkt_alloc_dcm(DIAG_VERNO_F, rsp_len);
#else
  rsp_ptr = (DIAG_VERNO_F_rsp_type *)diagpkt_alloc(DIAG_VERNO_F, rsp_len);
#endif

  if (rsp_ptr != NULL)
  {   /* Format response packet with version, date and time. */
      for ( i = 0; i < VERNO_DATE_STRLEN; i++ ) {
			rsp_ptr->comp_date[i] = ver_date[i];
			rsp_ptr->rel_date[i]  = rel_date[i];
      }

      for ( i=0; i<VERNO_TIME_STRLEN; i++ ) {
			rsp_ptr->comp_time[i] = ver_time[i];
			rsp_ptr->rel_time[i]  = rel_time[i];
      }

#if defined (FEATURE_DIAG_QNX)
		memset (rsp_ptr->ver_dir, 0, VERNO_DIR_STRLEN);
#else
      for ( i=0; i<VERNO_DIR_STRLEN; i++ ) {
			rsp_ptr->ver_dir[i] = ver_dir[i];
      }
#endif

#if defined (DIAG_IS2000) 
      mccdma_get_cur_cdma_info(&scm,&mob_cai_rev,&sci_p);
#endif /* DIAG_IS2000 */

    rsp_ptr->mob_model = 0xff;   /* Invalid */

#if defined(DIAG_MP_MASTER)
		rsp_ptr->mob_firm_rev = 0;   /* pass zero for APPS*/
#else
    rsp_ptr->mob_firm_rev = mob_firm_rev;
#endif

    rsp_ptr->slot_cycle_index = sci_p;
    rsp_ptr->scm = scm;
    rsp_ptr->mob_cai_rev = mob_cai_rev;

    /* Supply the hardware version */
    /* NOTE: This should be in 0xF0nn format where F0 is hw_maj_ver */
    diag_hw_partnum_version(&new_ver);
    ver = new_ver.partnum;

    rsp_ptr->hw_maj_ver = ver >> 8;     /* Upper nibble */
    rsp_ptr->hw_min_ver = (byte)(ver & 0x00FF); /* Lower nibble */
  }

  return (rsp_ptr);

} /* toolsdiag_verno */

/*===========================================================================

FUNCTION TOOLSDIAG_ESN

DESCRIPTION
  This procedure processes a received Mobile Station ESN request.

===========================================================================*/

#ifdef DIAG_TOOLSDIAG_ESN
PACK(void *) toolsdiag_esn (PACK(void *)req_pkt, uint16 pkt_len)
{
  DIAG_ESN_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_ESN_F_rsp_type);

  if (pkt_len != sizeof (DIAG_ESN_F_req_type))
  {
#if defined (FEATURE_DIAG_QNX)
    return (diagpkt_err_rsp_dcm (DIAG_BAD_LEN_F, req_pkt, pkt_len));
#else
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
#endif	
  }

#if defined (FEATURE_DIAG_QNX)
  rsp_ptr = (DIAG_ESN_F_rsp_type *)diagpkt_alloc_dcm(DIAG_ESN_F, rsp_len);
#else
  rsp_ptr = (DIAG_ESN_F_rsp_type *)diagpkt_alloc(DIAG_ESN_F, rsp_len);
#endif  
  
  if (rsp_ptr != NULL)
  {
    /* Format response packet ESN. */
    rsp_ptr->esn = diagnv_get_esn ();
  }
 
  return(rsp_ptr);

} /* toolsdiag_esn */
#endif /*DIAG_TOOLSDIAG_ESN */

#if defined(DIAG_MP_MASTER)
/*===========================================================================

  FUNCTION GETCHIPINFO_QPST

  DESCRIPTION
    This procedure maps DAL chip family with QPST chip family
  RETURN
    chipinfo_qpst that is QPST understandable chip family number

    NOTE:- Agreement is to return chip family number if AO/APQ specific
           number is not defined by QPST

 ===========================================================================*/

chipinfo_qpst getchipinfo_qpst(void)
{
	chipinfo_qpst chipinfo_qpst_value = MDOEL_INVALLID;
	DalDeviceHandle *phChipInfo;
	DalChipInfoFamilyType eFamily;

	if (DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo) != DAL_SUCCESS)
	{
		return chipinfo_qpst_value;
	}

	DalDevice_Open(phChipInfo, DAL_OPEN_SHARED);
	DalChipInfo_GetChipFamily(phChipInfo, &eFamily);

	switch(eFamily)
	{
	case DALCHIPINFO_FAMILY_MSM8960:
		chipinfo_qpst_value = MODEL_AO8960;
		break;

	// Since QNX is on 8x30
	case DALCHIPINFO_FAMILY_MSM8930: //8630, 8230, 8030
		chipinfo_qpst_value = MODEL_SURF_APQ8030; // For all 8x30 series we want
		                                          // to return this
		break;

	// Since QNX is on 8227
	case DALCHIPINFO_FAMILY_MSM8227:
		chipinfo_qpst_value = MODEL_SURF_MSM8227;
		break;

	case DALCHIPINFO_FAMILY_MSM8974:
		chipinfo_qpst_value = MODEL_SURF_MSM8974;
		break;

	case DALCHIPINFO_FAMILY_APQ8064:
		chipinfo_qpst_value = MODEL_APQ8064;
		break;

	case DALCHIPINFO_FAMILY_MDM9X15:
		chipinfo_qpst_value = MODEL_SURF9615; // no AO defined by QPST
		break;

	// Since QNX was on 8660
	case DALCHIPINFO_FAMILY_MSM8660:
		chipinfo_qpst_value = MODEL_MSM8660;
		break;

   case DALCHIPINFO_FAMILY_MDM9x25:
        chipinfo_qpst_value = MODEL_MDM9x25;

	default:
		chipinfo_qpst_value = MDOEL_INVALLID;
		break;
	}
	return chipinfo_qpst_value;
} /* getchipinfo_qpst */

#endif /* #if defined(DIAG_MP_MASTER) */

/*===========================================================================

FUNCTION TOOLSDIAG_EXT_BUILD_ID

DESCRIPTION
    This procedure processes a extended build id command i.e. 0x7C/124.
    This is called when no polling command is registered by any peripherals
    and APPS diag is responding to COREBSP polling command.

    or

    The code is running on modem where extended build id command is registered.

  RETURN
    pointer to toolsdiag_ext_build_id_rsp_type struct

===========================================================================*/
PACK(void *) toolsdiag_ext_build_id (PACK(void *) req_pkt, uint16 pkt_len)
{
  toolsdiag_ext_build_id_rsp_type *build_id = NULL;
  diag_hw_rev_type new_ver = {0,0,0}; /* Buffer for the new MSM version information */
  uint32 rsp_len = 0;

#if !defined(DIAG_MP_MASTER)
  char *str = NULL;
  uint32 build_id_strlen = 0, model_strlen = 0; /* Includes NULL term char */
#endif

#if !defined(DIAG_MP_MASTER)
  rsp_len = FPOS (toolsdiag_ext_build_id_rsp_type, ver_strings);
#else
  rsp_len = sizeof(toolsdiag_ext_build_id_rsp_type);
#endif

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if !defined(DIAG_MP_MASTER)
  /* calc length for the "build_id" field */
  build_id_strlen = strlen (mob_sw_rev) + 1; /* NULL char */
  rsp_len += build_id_strlen;

  //XXX not supported
  model_strlen = 1; /* NULL char */
  rsp_len += model_strlen;
#endif

#if defined (FEATURE_DIAG_QNX)
  build_id = (toolsdiag_ext_build_id_rsp_type *) 
    diagpkt_alloc_dcm (DIAG_EXT_BUILD_ID_F, rsp_len);
#else
  build_id = (toolsdiag_ext_build_id_rsp_type *) 
    diagpkt_alloc (DIAG_EXT_BUILD_ID_F, rsp_len);
#endif	

  if (build_id != NULL)
  {   
    build_id->msm_hw_version_format = 0;
    build_id->reserved[0] = 0;
    build_id->reserved[1] = 0;

#if defined(DIAG_MP_MASTER)
 build_id->mobile_model_id = getchipinfo_qpst();
 #else
#if !defined(CUST_MOB_MODEL_EXT)
     build_id->mobile_model_id = mob_model;
#else
     build_id->mobile_model_id = mob_model_ext;
#endif
#endif /* #if defined(DIAG_MP_MASTER) */

    diag_hw_partnum_version(&new_ver);

#if defined HW_REV_TYPE_V2
    build_id->msm_hw_version = new_ver.hw_register;
    build_id->msm_hw_version_format = 2;
#else
    build_id->msm_hw_version =  new_ver.partnum << 4;
    build_id->msm_hw_version |= new_ver.version;
    build_id->msm_hw_version_format = 1;
#endif

#if !defined(DIAG_MP_MASTER)
    str = build_id->ver_strings;
    
    memset (str, 0, build_id_strlen + model_strlen);

    strlcpy(str, mob_sw_rev, build_id_strlen);
#else
	build_id->ver_strings[0] = '\0';
	build_id->ver_strings[1] = '\0';
#endif
  }

  return (build_id);

} /* toolsdiag_ext_build_id */

/*===========================================================================
FUNCTION TOOLSDIAG_DLOAD_JUMP

DESCRIPTION
  Processes a request for the DMSS to jump to the download protocol.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  A download will occur as soon as this packet has been acknowledged. Normal
  processing will cease.

===========================================================================*/
#ifdef DIAG_TOOLSDIAG_DLOAD
PACK(void *) toolsdiag_dload_jump (PACK(void *) req_pkt, uint16 pkt_len)
{
  toolsdiag_dload_rsp_type *rsp;
  const int rsp_len = sizeof (toolsdiag_dload_rsp_type);
  int j;

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

#if defined (FEATURE_DIAG_QNX)
  rsp = (toolsdiag_dload_rsp_type *) diagpkt_alloc_dcm (DIAG_DLOAD_F, rsp_len);
#else
  rsp = (toolsdiag_dload_rsp_type *) diagpkt_alloc (DIAG_DLOAD_F, rsp_len);
#endif  

  if (rsp)
  {
#if defined (FEATURE_DIAG_QNX)
    diagpkt_commit_dcm (rsp);
#else
    diagpkt_commit (rsp);
#endif	
    rsp = NULL;
  }

  /* This call disables all streamed data services and flushes the buffers. */
  diagbuf_flush_timeout(DIAG_SIO_TIMEOUT_TIMER_LEN);

  /* pause briefly to allow the response to reach QPST */
  for(j=0; j<1000; j++)
  {
	  busywait(1000);
  }

  tms_dload_reset ();
  /*lint -unreachable*/

  return (rsp);

} /* toolsdiag_dload_jump */
#endif /* DIAG_TOOLSDIAG_DLOAD */

#if (defined(DIAG_MP_MASTER)) || (!defined(DIAG_MP_MASTER) && !defined(DIAG_MP))
/*===========================================================================
FUNCTION TOOLSDIAG_EXT_MOBILE_ID_HANDLER

DESCRIPTION
    Processes the request of mobile ID command. It returns the chip family information
    in the response packet.

DEPENDENCIES
  DAL driver to be available.

RETURN VALUE
  Pointer to the response packet.

SIDE EFFECTS
  None

===========================================================================*/
PACK(void *) toolsdiag_ext_mobile_id_handler (PACK(void *) req_pkt, uint16 pkt_len)
{
    toolsdiag_cmd_ext_mobile_rsp_type *rsp =NULL;

    if (pkt_len != sizeof (diagpkt_subsys_header_type))
    {
        return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
    }

    /* Allocate for rsp packet */
    rsp = (toolsdiag_cmd_ext_mobile_rsp_type *) diagpkt_subsys_alloc (DIAG_SUBSYS_PARAMS, TOOLSDIAG_EXT_MOBILE_ID, sizeof(toolsdiag_cmd_ext_mobile_rsp_type));

    if(rsp)
    {
        rsp->version=2;
        rsp->pad[0]=0;
        rsp->pad[1]=0;
        rsp->pad[2]=0;
        rsp->family=DalChipInfo_ChipFamily();
        rsp->chip_id = DalChipInfo_ChipId();
    }

    return (void *)rsp;
}
#endif

/*===========================================================================
  TOOLS DIAG TABLE registration
    To register handlers for verno and extended build id commands
  ===========================================================================*/
static const diagpkt_user_table_entry_type toolsdiag_tbl[] =
{
  {DIAG_VERNO_F, DIAG_VERNO_F, toolsdiag_verno},
  {DIAG_EXT_BUILD_ID_F, DIAG_EXT_BUILD_ID_F, toolsdiag_ext_build_id},
#ifdef DIAG_TOOLSDIAG_ESN
  {DIAG_ESN_F, DIAG_ESN_F, toolsdiag_esn},
#endif /* end of #ifdef DIAG_TOOLSDIAG_ESN */
#ifdef DIAG_TOOLSDIAG_DLOAD
  {DIAG_DLOAD_F, DIAG_DLOAD_F, toolsdiag_dload_jump}
#endif
};

#if (defined(DIAG_MP_MASTER)) || (!defined(DIAG_MP_MASTER) && !defined(DIAG_MP))
static const diagpkt_user_table_entry_type toolsdiag_ext_mobile_id_tbl[] =
{
    {TOOLSDIAG_EXT_MOBILE_ID, TOOLSDIAG_EXT_MOBILE_ID, toolsdiag_ext_mobile_id_handler}
};
#endif

/*===========================================================================

    FUNCTION TOOLSDIAG_INIT

  DESCRIPTION
    Registers toolsdiag_tbl dispatch table with diag

  RETURN
    None

  ===========================================================================*/
#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, toolsdiag_tbl);
#else
  void toolsdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, toolsdiag_tbl);

#if (defined(DIAG_MP_MASTER)) || (!defined(DIAG_MP_MASTER) && !defined(DIAG_MP))
    DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_PARAMS, toolsdiag_ext_mobile_id_tbl);
#endif
  }
#endif
#endif /* DIAG_TOOLSDIAG_INIT */

/*lint +esym(715,pkt_len,req_ptr) */

#ifdef __cplusplus
  }
#endif
