/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM Utils

General Description
  This file contains implementations for TM Utils
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2020 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_util.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/20   sai     Time validity check is included for final fix,special case for GTP fix reported as final fix with invalid time
06/26/20   sai     Added QZSS default enablement based on config NV as part of FR63097
06/18/19   sai     Set SpeedUnc_valid flag in Final fix QMI indication processing 
05/20/20   py      vertUnc in Qmi position should be reported with full precision 
11/20/19   ska     Added RFLoss to SNR if there is a valid CN0
08/21/19   ska     Added support for NMEA GSV
07/31/19    py     Fill SV Used list for intermediate fixes
04/26/19    nath   Qualified enablement related changes.
05/23/19    vg     Implement system unc specific session duration
03/28/19    sum    Populating GPS and UTC time
10/30/17    nath   Updated loc_unc_horizontal,alongAxisUnc and perpAxisUnc to float
09/14/17    Sum    Added function for conversion of PZ90 coordinate to Lat/Long/Alt
03/30/17    sum    Added support for horizontal velocity greater than 0x7fff in tm_util_pd_info_data_init()
11/28/16    mj     Set UTC mask for leap second retrieval
04/26/16    rn     Change directory paths for EFS Quota feature.
07/09/15    va     Time utilities branched into tm_time_util.c
07/03/15    jv     Added QZSS Support
05/21/15    nl     Refactor code
04/16/15    jv     Lowering priority of some messages from MSG_ERROR to MSG_MED
10/25/14    jv     Added Galio Support
09/24/14    rk     Added missing steering source type while injecting AcqAssist.
10/16/13    rk     Added generic Glonass AcqAsst processing functions.
12/27/12    rk     Update fix report translation functions - tm_util_pd_info_data_init & 
                   tm_util_convert_lr - to include sensor bias parameter struct.
12/04/12    rk     CR 408753 - Updated PD function to include extended SV parameters in the client notifiation.
02/16/12    gk     Altitude also reported in float now
05/18/11    rc     Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
01/03/11    gk     Eph throttling rework
01/14/10    rb     Set PDSM_PD_UTC_TIME_VALID flag when UTC time is available
12/02/09    gk     intermediate position fixes
11/11/09    gk     Set optional field masks for position
09/01/09    gk     Added a check to validate the directory for EFS operation
08/14/07    ank    Corrected units of velHor and velVert in ext_pos_data.
08/02/07    LT     Added a utility function for handling SVs non-existence case.
07/30/07    ank    Added support for ext_pos_data in Position Report.
05/25/07    lt     Made two functions accessible by external, modified tm_util_pd_info_data_init.
05/09/07    ank    Changed prototype of tm_util_pd_info_data_init
12/11/06    ank    Changes to report time_stamp_msec field.
10/25/06    ank    Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"

#include "msg.h"
#include "qw.h"
#include "gps_types.h"
#include "gps_consts.h"
#include "aries_os_api.h"
#include "tm_common.h"
#include "pdapi_lite.h" //#9x05
#include "math.h"
#include "tm_prtl_iface.h"
#include "tm_xtra.h"
#include "tm_data.h"
#include "secapi.h"
#include <math.h>
#include "sm_util.h"
#include "gts_loc_api.h"
#include "gnss_calendar_util.h"

#define INV_SQRT_2 0.7071067811865

#define MCC_ENABLED         (1<<31)
#define UNCERTAINTY_TABLE_SIZE   30
extern int const GeoSep[19][36];

static int const positionUncertaintyTable[UNCERTAINTY_TABLE_SIZE] =
   {  1,    1,    1,   2,    2,      3,    4,    6,
      8,   12,   16,   24,   32,     48,   64,   96,
    128,  192,  256,  384,  512,    768, 1024, 1536,
   2048, 3072, 4096, 6144, 8192, 12288};

extern float locPd_GetGeoidalSeparation
(
      double dbLatitude,
      double dbLongitude
);

									
/* (Sv line of sight vector calculation works for up to 75 km. The last
   but one entry in the table below is specified in IS-801 as unc. >
   12,288m. Our algorithm works for up to 75Km, so set that entry to
   70000.0 m.) */
const FLT f_PosUncArr[MAX_POS_UNC_IND] =
{ 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 6.0, 8.0, 12.0, 16.0, 24.0, 32.0, 48.0, 64.0, 96.0,
  128.0, 192.0, 256.0, 384.0, 512.0, 768.0, 1024.0, 1536.0, 2048.0, 3072.0, 4096.0,
  6144.0, 8192.0, 12288.0, 70000.0, 999999.0 };

#define TM_CM_INVALID_MCC 0xFFFFFFFF
#define MAX_MCC_ENTRIES 231

#define TM_GNSS_CONFIG_GLO_ENABLED                (1<<1)
/*
   The below masks have been set according to 
   bitmask provided by NV 70326. 
*/

typedef uint32 constellation_id_type;

typedef struct mcc_constelaltion_map
{
    uint16                w_MCC;   // 3-digit country code
    constellation_id_type gnns_contstellation;    
} mcc_constelaltion_map_entry;

/*Map of mcc of different countries and the gnss constellation 
 to be used for navigation in those countries*/
mcc_constelaltion_map_entry mcc_constelaltion_map[MAX_MCC_ENTRIES] =
{
//  MCC  Constellation)
  {0x440,  TM_GNSS_QZSS_CONSTELL },         //Asia_Japan_440
  {0x454,  TM_GNSS_BDS_CONSTELL},           //Asia_Hong Kong China_454
  {0x310,  TM_GNSS_GLO_CONSTELL},              //North America_United States of America_310
  {0x311,  TM_GNSS_GLO_CONSTELL},              //North America_United States of America_311
  {0x312,  TM_GNSS_GLO_CONSTELL},              //North America_United States of America_312
  {0x313,  TM_GNSS_GLO_CONSTELL},              //North America_United States of America_313
  {0x314,  TM_GNSS_GLO_CONSTELL},              //North America_United States of America_314
  {0x315,  TM_GNSS_GLO_CONSTELL},              //North America_United States of America_315
  {0x316,  TM_GNSS_GLO_CONSTELL},              //North America_United States of America_316
  {0x234,  TM_GNSS_GAL_CONSTELL},                 //Europe_United Kingdom_234
  {0x262,  TM_GNSS_GAL_CONSTELL},           //Europe_Germany_262
  {0x460,  TM_GNSS_BDS_CONSTELL},               //Asia_China_460
  {0x461,  TM_GNSS_BDS_CONSTELL},               //Asia_China_461
  {0x404,  TM_GNSS_GLO_CONSTELL},          //Asia_India_404
  {0x222,  TM_GNSS_GAL_CONSTELL},             //Europe_Italy_222
  {0x515,  TM_GNSS_GLO_CONSTELL },         //Asia_Philippines_515
  {0x520,  TM_GNSS_GLO_CONSTELL},          //Asia_Thailand_520
  {0x214,  TM_GNSS_GAL_CONSTELL},          //Europe_Spain_214
  {0x466,  TM_GNSS_BDS_CONSTELL},          //Asia_Taiwan_466
  {0x208,  TM_GNSS_GAL_CONSTELL},                  //Europe_France_208
  {0x505,  TM_GNSS_GLO_CONSTELL },              //Australia_Australia_505
  {0x525,  TM_GNSS_GLO_CONSTELL},            //Asia_Singapore_525
  {0x232,  TM_GNSS_GAL_CONSTELL},           //Europe_Austria_232
  {0x450,  TM_GNSS_GLO_CONSTELL},          //Asia_South Korea_450
  {0x455,  TM_GNSS_BDS_CONSTELL},           //Asia_Macao China_455
  {0x452,  TM_GNSS_GLO_CONSTELL},           //Asia_Viet Nam_452
  {0x204,  TM_GNSS_GAL_CONSTELL},                  //Europe_Netherlands_204
  {0x510,  TM_GNSS_GLO_CONSTELL},         //Asia_Indonesia_510
  {0x530,  TM_GNSS_GLO_CONSTELL},              //Australia_New Zealand_530
  {0x260,  TM_GNSS_GAL_CONSTELL },           //Europe_Poland_260
  {0x334,  TM_GNSS_GLO_CONSTELL},        //North America_Mexico_334
  {0x502,  TM_GNSS_GLO_CONSTELL},           //Asia_Malaysia_502
  {0x230,  TM_GNSS_GLO_CONSTELL},            //Europe_Czech Republic_230
  {0x206,  TM_GNSS_GAL_CONSTELL},            //Europe_Belgium_206
  {0x228,  TM_GNSS_GAL_CONSTELL},            //Europe_Switzerland_228
  {0x724,  TM_GNSS_GLO_CONSTELL},              //South America_Brazil_724
  {0x302,  TM_GNSS_GLO_CONSTELL},          //North America_Canada_302
  {0x240,  TM_GNSS_GLO_CONSTELL},                 //Europe_Sweden_240
  {0x286,  TM_GNSS_GLO_CONSTELL},          //Europe_Turkey_286
  {0x268,  TM_GNSS_GAL_CONSTELL },              //Europe_Portugal_268
  {0x272,  TM_GNSS_GAL_CONSTELL},           //Europe_Ireland_272
  {0x655,  TM_GNSS_GLO_CONSTELL },               //Africa_South Africa_655
  {0x250,  TM_GNSS_GLO_CONSTELL},           //Asia_Russian Federation_250
  {0x216,  TM_GNSS_GLO_CONSTELL},            //Europe_Hungary_216
  {0x425,  TM_GNSS_GLO_CONSTELL},           //Asia_Israel_425
  {0x244,  TM_GNSS_GLO_CONSTELL },                 //Europe_Finland_244
  {0x255,  TM_GNSS_GLO_CONSTELL},          //Europe_Ukraine_255
  {0x202,  TM_GNSS_GAL_CONSTELL },           //Europe_Greece_202
  {0x212,  TM_GNSS_GAL_CONSTELL},             //Europe_Monaco_212
  {0x213,  TM_GNSS_GAL_CONSTELL},              //Europe_Andorra_213
  {0x218,  TM_GNSS_GAL_CONSTELL},           //Europe_Bosnia and Herzegovina_218
  {0x219,  TM_GNSS_GAL_CONSTELL},           //Europe_Croatia_219
  {0x220,  TM_GNSS_GAL_CONSTELL},           //Europe_Serbia
  {0x225,  TM_GNSS_GAL_CONSTELL},             //Europe_Vatican City State_225
  {0x226,  TM_GNSS_GAL_CONSTELL },           //Europe_Romania_226
  {0x231,  TM_GNSS_GAL_CONSTELL},            //Europe_Slovakia_231
  {0x235,  TM_GNSS_GAL_CONSTELL},                 //Europe_United Kingdom_235
  {0x238,  TM_GNSS_GAL_CONSTELL },           //Europe_Denmark_238
  {0x242,  TM_GNSS_GLO_CONSTELL },               //Europe_Norway_242
  {0x246,  TM_GNSS_GLO_CONSTELL},            //Europe_Lithuania_246
  {0x247,  TM_GNSS_GLO_CONSTELL},            //Europe_Latvia_247
  {0x248,  TM_GNSS_GLO_CONSTELL},            //Europe_Estonia_248
  {0x257,  TM_GNSS_GLO_CONSTELL},           //Europe_Belarus_257
  {0x259,  TM_GNSS_GLO_CONSTELL},           //Europe_Moldova_259
  {0x266,  TM_GNSS_GAL_CONSTELL},            //Europe_Gibraltar_266
  {0x270,  TM_GNSS_GAL_CONSTELL },             //Europe_Luxembourg_270
  {0x274,  TM_GNSS_GAL_CONSTELL},          //Europe_Iceland_274
  {0x276,  TM_GNSS_GAL_CONSTELL},           //Europe_Albania_276
  {0x278,  TM_GNSS_GAL_CONSTELL},            //Europe_Malta_278
  {0x280,  TM_GNSS_GAL_CONSTELL},           //Europe_Cyprus_280
  {0x282,  TM_GNSS_GLO_CONSTELL},           //Europe_Georgia_282
  {0x283,  TM_GNSS_GLO_CONSTELL},           //Europe_Armenia_283
  {0x284,  TM_GNSS_GLO_CONSTELL},           //Europe_Bulgaria_284
  {0x288,  TM_GNSS_GLO_CONSTELL},                 //Europe_Faroe Islands_288
  {0x290,  TM_GNSS_GLO_CONSTELL},               //Island_Greenland_290
  {0x292,  TM_GNSS_GAL_CONSTELL},            //Europe_San Marino_292
  {0x293,  TM_GNSS_GAL_CONSTELL},           //Europe_Slovenia_293
  {0x294,  TM_GNSS_GAL_CONSTELL},           //Europe_Macedonia_294
  {0x295,  TM_GNSS_GAL_CONSTELL},             //Europe_Liechtenstein_295
  {0x297,  TM_GNSS_GAL_CONSTELL},             //Europe_Montenegro_297
  {0x308,  TM_GNSS_GLO_CONSTELL},           //North America_Saint Pierre and Miquelon_308
  {0x330,  TM_GNSS_GLO_CONSTELL},           //North America_Puerto Rico_330
  {0x332,  TM_GNSS_GLO_CONSTELL},           //North America_United States Virgin Islands_3
  {0x338,  TM_GNSS_GLO_CONSTELL},         //North America_Jamaica_338
  {0x340,  TM_GNSS_GLO_CONSTELL},          //North America_GuadeloupeAndMartinique_340
  {0x342,  TM_GNSS_GLO_CONSTELL},           //North America_Barbados_342
  {0x344, TM_GNSS_GLO_CONSTELL},              //North America_Antigua and Barbuda_344
  {0x346, TM_GNSS_GLO_CONSTELL},           //North America_Cayman Islands_346
  {0x348, TM_GNSS_GLO_CONSTELL},           //North America_British Virgin Islands_348
  {0x350, TM_GNSS_GLO_CONSTELL},           //North America_Bermuda_350
  {0x352, TM_GNSS_GLO_CONSTELL},           //North America_Grenada_352
  {0x354, TM_GNSS_GLO_CONSTELL},           //Oceania_Montserrat_354
  {0x356, TM_GNSS_GLO_CONSTELL},            //North America_Saint Kitts and Nevis_356
  {0x358, TM_GNSS_GLO_CONSTELL},                 //North America_Saint Lucia_358
  {0x360, TM_GNSS_GLO_CONSTELL},            //North America_Saint Vincent and the Grenadin
  {0x362, TM_GNSS_GLO_CONSTELL},           //North America_Netherlands Antilles_362
  {0x363, TM_GNSS_GLO_CONSTELL },           //North America_Aruba_363
  {0x364, TM_GNSS_GLO_CONSTELL },          //North America_Bahamas_364
  {0x365, TM_GNSS_GLO_CONSTELL},           //North America_Anguilla_365
  {0x366, TM_GNSS_GLO_CONSTELL},           //North America_Dominica_366
  {0x368, TM_GNSS_GLO_CONSTELL},          //North America_Cuba_368
  {0x370, TM_GNSS_GLO_CONSTELL},           //North America_Dominican Republic_370
  {0x372, TM_GNSS_GLO_CONSTELL},          //North America_Haiti_372
  {0x374, TM_GNSS_GLO_CONSTELL},           //South America_Trinidad and Tobago_374
  {0x376, TM_GNSS_GLO_CONSTELL},          //North America_Turks and Caicos Islands_376
  {0x400, TM_GNSS_GLO_CONSTELL},           //Europe_Azerbaijan_400
  {0x401, TM_GNSS_GLO_CONSTELL},           //Asia_Kazakstan_401
  {0x402, TM_GNSS_BDS_CONSTELL },           //Asia_Bhutan_402
  {0x410, TM_GNSS_BDS_CONSTELL},          //Asia_Pakistan_410
  {0x412, TM_GNSS_GLO_CONSTELL},           //Asia_Afghanistan_412
  {0x413, TM_GNSS_GLO_CONSTELL},            //Asia_Sri Lanka_413
  {0x414, TM_GNSS_BDS_CONSTELL },          //Asia_Myanmar_414
  {0x415, TM_GNSS_GLO_CONSTELL },           //Asia_Lebanon_415
  {0x416, TM_GNSS_GLO_CONSTELL},           //Asia_Jordan_416
  {0x417, TM_GNSS_GLO_CONSTELL},           //Asia_Syria_417
  {0x418, TM_GNSS_GLO_CONSTELL},           //Asia_Iraq_418
  {0x419, TM_GNSS_GLO_CONSTELL},           //Asia_Kuwait_419
  {0x420, TM_GNSS_GLO_CONSTELL},          //Asia_Saudi Arabia_420
  {0x421, TM_GNSS_GLO_CONSTELL},           //Asia_Yemen_421
  {0x422, TM_GNSS_GLO_CONSTELL},           //Asia_Oman_422
  {0x424, TM_GNSS_GLO_CONSTELL},           //Asia_United Arab Emirates_424
  {0x426, TM_GNSS_GLO_CONSTELL},            //Asia_Bahrain_426
  {0x427, TM_GNSS_GLO_CONSTELL },           //Asia_Qatar_427
  {0x428, TM_GNSS_BDS_CONSTELL },         //Asia_Mongolia_428
  {0x429, TM_GNSS_BDS_CONSTELL},           //Asia_Nepal_429
  {0x430, TM_GNSS_GLO_CONSTELL },            //Asia_United Arab Emirates (Abu Dhabi)_430
  {0x431, TM_GNSS_GLO_CONSTELL},            //Asia_United Arab Emirates (Dubai)_431
  {0x432, TM_GNSS_GLO_CONSTELL },          //Asia_Iran_432
  {0x434, TM_GNSS_GLO_CONSTELL},          //Asia_Uzbekistan_434
  {0x436, TM_GNSS_GLO_CONSTELL},           //Asia_Tajikistan_436
  {0x437, TM_GNSS_GLO_CONSTELL},            //Asia_Kyrgyzstan_437
  {0x438, TM_GNSS_GLO_CONSTELL },           //Asia_Turkmenistan_438
  {0x441, TM_GNSS_QZSS_CONSTELL},         //Asia_Japan_441
  {0x456, TM_GNSS_BDS_CONSTELL},          //Asia_Cambodia_456
  {0x457, TM_GNSS_BDS_CONSTELL },           //Asia_Laos_457
  {0x467, TM_GNSS_BDS_CONSTELL },           //Asia_North Korea_467
  {0x470, TM_GNSS_BDS_CONSTELL },            //Asia_Bangladesh_470
  {0x472, TM_GNSS_GLO_CONSTELL},               //Asia_Maldives_472
  {0x514, TM_GNSS_QZSS_CONSTELL},            //Oceania_East Timor_514
  {0x528, TM_GNSS_QZSS_CONSTELL},            //Asia_Brunei Darussalam_528
  {0x534, TM_GNSS_QZSS_CONSTELL},          //Oceania_Northern Mariana Islands_534
  {0x535, TM_GNSS_QZSS_CONSTELL},           //Oceania_Guam_535
  {0x536, TM_GNSS_QZSS_CONSTELL},           //Oceania_Nauru_536
  {0x537, TM_GNSS_QZSS_CONSTELL},         //Asia_Papua New Guinea_537
  {0x539, TM_GNSS_QZSS_CONSTELL},        //Oceania_Tonga_539
  {0x540, TM_GNSS_QZSS_CONSTELL},          //Oceania_Solomon Islands_540
  {0x541, TM_GNSS_QZSS_CONSTELL},         //Oceania_Vanuatu_541
  {0x542, TM_GNSS_QZSS_CONSTELL},               //Oceania_Fiji_542
  {0x543, TM_GNSS_QZSS_CONSTELL},         //Oceania_Wallis and Futuna_543
  {0x544, TM_GNSS_QZSS_CONSTELL},        //Oceania_American Samoa_544
  {0x545, TM_GNSS_QZSS_CONSTELL},           //Oceania_Kiribati_545
  {0x546, TM_GNSS_QZSS_CONSTELL},         //Oceania_New Caledonia_546
  {0x547, TM_GNSS_QZSS_CONSTELL},       //Oceania_French Polynesia_547
  {0x548, TM_GNSS_QZSS_CONSTELL},        //Oceania_Cook Islands_548
  {0x549, TM_GNSS_QZSS_CONSTELL},          //Oceania_Samoa_549
  {0x550, TM_GNSS_QZSS_CONSTELL},          //Oceania_Micronesia_550
  {0x551, TM_GNSS_QZSS_CONSTELL},           //Oceania_Marshall Islands_551
  {0x552, TM_GNSS_QZSS_CONSTELL},           //Oceania_Palau_552
  {0x602, TM_GNSS_GLO_CONSTELL},           //Africa_Egypt_602
  {0x603, TM_GNSS_GLO_CONSTELL},           //Africa_Algeria_603
  {0x604, TM_GNSS_GLO_CONSTELL},           //Africa_Morocco_604
  {0x605, TM_GNSS_GLO_CONSTELL},            //Africa_Tunisia_605
  {0x606, TM_GNSS_GLO_CONSTELL},          //Africa_Libya_606
  {0x607, TM_GNSS_GLO_CONSTELL},          //Africa_Gambia_607
  {0x608, TM_GNSS_GLO_CONSTELL},          //Africa_Senegal_608
  {0x609, TM_GNSS_GLO_CONSTELL},             //Africa_Mauritania_609
  {0x610, TM_GNSS_GLO_CONSTELL},          //Africa_Mali_610
  {0x611, TM_GNSS_GLO_CONSTELL},           //Africa_Guinea_611
  {0x612, TM_GNSS_GLO_CONSTELL},            //Africa_C?te d'Ivoire_612
  {0x613, TM_GNSS_GLO_CONSTELL},           //Africa_Burkina Faso_613
  {0x614, TM_GNSS_GLO_CONSTELL},           //Africa_Niger_614
  {0x615, TM_GNSS_GLO_CONSTELL},             //Africa_Togo_615
  {0x616, TM_GNSS_GLO_CONSTELL},             //Africa_Benin_616
  {0x617, TM_GNSS_GLO_CONSTELL},            //Africa_Mauritius_617
  {0x618, TM_GNSS_GLO_CONSTELL},            //Africa_Liberia_618
  {0x619, TM_GNSS_GLO_CONSTELL},           //Africa_Sierra Leone_619
  {0x620, TM_GNSS_GLO_CONSTELL},            //Africa_Ghana_620
  {0x621, TM_GNSS_GLO_CONSTELL},             //Africa_Nigeria_621
  {0x622, TM_GNSS_GLO_CONSTELL},          //Africa_Chad_622
  {0x623, TM_GNSS_GLO_CONSTELL},            //Africa_Central African Republic_623
  {0x624, TM_GNSS_GLO_CONSTELL},            //Africa_Cameroon_624
  {0x625, TM_GNSS_GLO_CONSTELL},          //Africa_Cape Verde_625
  {0x626, TM_GNSS_GLO_CONSTELL},             //Africa_S?o Tom? and Pr?ncipe_626
  {0x627, TM_GNSS_GLO_CONSTELL},             //Africa_Equatorial Guinea_627
  {0x628, TM_GNSS_GLO_CONSTELL},           //Africa_Gabon_628
  {0x629, TM_GNSS_GLO_CONSTELL},           //Africa_Congo_629
  {0x630, TM_GNSS_GLO_CONSTELL},          //Africa_Congo-The Democratic Republic of_630
  {0x631, TM_GNSS_GLO_CONSTELL},          //Africa_Angola_631
  {0x632, TM_GNSS_GLO_CONSTELL},          //Africa_Guinea-Bissau_632
  {0x633, TM_GNSS_GLO_CONSTELL},            //Africa_Seychelles_633
  {0x634, TM_GNSS_GLO_CONSTELL},           //Africa_Sudan_634
  {0x635, TM_GNSS_GLO_CONSTELL},           //Africa_Rwanda_635
  {0x636, TM_GNSS_GLO_CONSTELL},           //Africa_Ethiopia_636
  {0x637, TM_GNSS_GLO_CONSTELL},             //Africa_Somalia_637
  {0x638, TM_GNSS_GLO_CONSTELL},            //Africa_Djibouti_638
  {0x639, TM_GNSS_GLO_CONSTELL},             //Africa_Kenya_639
  {0x640, TM_GNSS_GLO_CONSTELL},           //Africa_Tanzania_640
  {0x641, TM_GNSS_GLO_CONSTELL},             //Africa_Uganda_641
  {0x642, TM_GNSS_GLO_CONSTELL},           //Africa_Burundi_642
  {0x643, TM_GNSS_GLO_CONSTELL},         //Africa_Mozambique_643
  {0x645, TM_GNSS_GLO_CONSTELL},          //Africa_Zambia_645
  {0x646, TM_GNSS_GLO_CONSTELL},          //Africa_Madagascar_646
  {0x647, TM_GNSS_GLO_CONSTELL},           //Africa_Reunion_647
  {0x648, TM_GNSS_GLO_CONSTELL},          //Africa_Zimbabwe_648
  {0x649, TM_GNSS_GLO_CONSTELL},         //Africa_Namibia_649
  {0x650, TM_GNSS_GLO_CONSTELL},          //Africa_Malawi_650
  {0x651, TM_GNSS_GLO_CONSTELL},          //Africa_Lesotho_651
  {0x652, TM_GNSS_GLO_CONSTELL},          //Africa_Botswana_652
  {0x653, TM_GNSS_GLO_CONSTELL},          //Africa_Swaziland_653
  {0x654, TM_GNSS_GLO_CONSTELL},           //Africa_Comoros_654
  {0x657, TM_GNSS_GLO_CONSTELL},           //Africa_Eritrea_657
  {0x702, TM_GNSS_GLO_CONSTELL},          //North America_Belize_702
  {0x704, TM_GNSS_GLO_CONSTELL},          //North America_Guatemala_704
  {0x706, TM_GNSS_GLO_CONSTELL},          //North America_El Salvador_706
  {0x708, TM_GNSS_GLO_CONSTELL},           //North America_Honduras_708
  {0x710, TM_GNSS_GLO_CONSTELL},          //North America_Nicaragua_710
  {0x712, TM_GNSS_GLO_CONSTELL},                //North America_Costa Rica_712
  {0x714, TM_GNSS_GLO_CONSTELL},           //North America_Panama_714
  {0x716, TM_GNSS_GLO_CONSTELL},            //South America_Peru_716
  {0x722, TM_GNSS_GLO_CONSTELL},        //South America_Argentina_722
  {0x730, TM_GNSS_GLO_CONSTELL},            //South America_Chile_730
  {0x732, TM_GNSS_GLO_CONSTELL},          //South America_Colombia_732
  {0x734, TM_GNSS_GLO_CONSTELL},           //South America_Venezuela_734
  {0x736, TM_GNSS_GLO_CONSTELL},         //South America_Bolivia_736
  {0x738, TM_GNSS_GLO_CONSTELL},           //South America_Guyana_738
  {0x740, TM_GNSS_GLO_CONSTELL},             //South America_Ecuador_740
  {0x742, TM_GNSS_GLO_CONSTELL},           //South America_French Guiana_742
  {0x744, TM_GNSS_GLO_CONSTELL},         //South America_Paraguay_744
  {0x746, TM_GNSS_GLO_CONSTELL},           //South America_Suriname_746
  {0x748, TM_GNSS_GLO_CONSTELL},         //South America_Uruguay_748
  {0x750, TM_GNSS_GLO_CONSTELL}             //South America_Falkland_Islands_Malvinas_750
  
};
  
/* 
 * The struct will contain a specification for the range of valid file names
 * that are allowed to be written to the EFS. Valid file name ranges are
 * specified using the pattern below
 *
 *     <Common Alphabetical Prefix><Optional Numerical Suffix> 
 *
 * E.g. "/GNSS/PE/AlmFile00", .. "/GNSS/PE/AlmFile31"
 *
 * The minimum and the maximum value of the suffix is specified and all file 
 * names with the common alphabetical prefix followed by a suffix between the
 * min and max suffix are valid.
 *
 * In case only a single alphabetical file name is to be specified, the  
 * suffix length is set to 0.
 */

/*===========================================================================
FUNCTION tm_util_nearest_long

DESCRIPTION
    tm_util_nearest_long() rounds the double floating point var and returns long

RETURN VALUE
    Rounded long signed integer

DEPENDENCIES

===========================================================================*/
int32 tm_util_nearest_long(DBL d_X)
{
  return(d_X - floor(d_X)) < 0.5 ? (S32)floor(d_X) : (S32)ceil(d_X);
}
/*===========================================================================
FUNCTION tm_util_select_pos_unc

DESCRIPTION
    tm_util_select_pos_unc() Encodes a Standard deviation for position 
    uncertainty value into an integer based on table 3.2.4.2-10 of the 
    IS-801 spec.

RETURN VALUE
    Rounded long signed integer

DEPENDENCIES

===========================================================================*/
int16 tm_util_select_pos_unc(float Std)
{
  int16 i;
  for (i = MAX_POS_UNC_IND - 2; i >= 0; i--)
  {
    if (fabs(Std) > f_PosUncArr[i])
    {
      return (i + 1);
    }
  }

  if (fabs(Std) <= f_PosUncArr[0])
  {
    return 0;
  }

  /* This should never happen. Include to avoid compiler warning of
     implicit return. */
  return 31;
}


void tm_util_xlate_nav_soln_to_qmi_pos_info
(

   qmiLocEventPositionReportIndMsgT_v02  *pos_info,
   const sm_NavPositionStructType    *nav_ptr
   )
{

  if ((nav_ptr != NULL) && (pos_info != NULL))
  {

    pos_info->technologyMask |= (nav_ptr->z_PosFlags.b_IsGpsUsed) ?
       QMI_LOC_POS_TECH_MASK_SATELLITE_V02 : 0;

    pos_info->technologyMask |= (nav_ptr->z_PosFlags.b_IsGloUsed) ?
       QMI_LOC_POS_TECH_MASK_SATELLITE_V02 : 0;

    pos_info->technologyMask |= (nav_ptr->z_PosFlags.b_IsBdsUsed) ?
       QMI_LOC_POS_TECH_MASK_SATELLITE_V02 : 0;

    pos_info->technologyMask |= (nav_ptr->z_PosFlags.b_IsGalUsed) ?
       QMI_LOC_POS_TECH_MASK_SATELLITE_V02 : 0;

    pos_info->technologyMask |= (nav_ptr->z_PosFlags.b_IsQzssUsed) ?
       QMI_LOC_POS_TECH_MASK_SATELLITE_V02 : 0;

    pos_info->technologyMask |= (nav_ptr->z_SmPosFlags.b_IsRefLoc) ?
       QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION_V02 : 0;

    pos_info->technologyMask |= (nav_ptr->z_SmPosFlags.b_IsCoarsePos) ?
       QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION_V02     : 0;

   
    /* Set gpsUtcOffset in milliseconds */
   
    }
  /*  nav_ptr != NULL */
}

boolean tm_util_xlate_nav_reliability_to_qmi( qmiLocReliabilityEnumT_v02        *pe_Target,
                                  pdsm_lite_pd_reliability_e_type         e_Reliability)
{
    boolean v_result = FALSE;

   if(NULL == pe_Target)
   {
      return FALSE ;
   }

   v_result = TRUE;
   switch(e_Reliability)
   {
      case PDSM_LITE_POSITION_RELIABILITY_NOT_SET:
         *pe_Target = eQMI_LOC_RELIABILITY_NOT_SET_V02;
      break;
      case PDSM_LITE_POSITION_RELIABILITY_VERY_LOW:
         *pe_Target = eQMI_LOC_RELIABILITY_VERY_LOW_V02;
      break;
      case PDSM_LITE_POSITION_RELIABILITY_LOW:
         *pe_Target = eQMI_LOC_RELIABILITY_LOW_V02;
      break;
      case PDSM_LITE_POSITION_RELIABILITY_MEDIUM:
         *pe_Target = eQMI_LOC_RELIABILITY_MEDIUM_V02;
      break;
      case PDSM_LITE_POSITION_RELIABILITY_HIGH:
         *pe_Target = eQMI_LOC_RELIABILITY_HIGH_V02;
      break;
      default:
         v_result = FALSE;
      break;
   }

   if(TRUE != v_result)
   {
      //LOC_MSG_ERROR("locPd_ConvertReliabilityFromPdapi failed",0,0,0);
   }
   return v_result;
}
                          
static boolean tm_util_xlate_timeSrc_to_qmi
(
   qmiLocTimeSourceEnumT_v02 * pe_QmiLocTimeSource,
   pdsm_lite_extended_time_src_e_type e_ExtTimeSrc
)
{
   boolean v_RetVal = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   do
   {
      if(NULL == pe_QmiLocTimeSource)
      {
         v_RetVal = FALSE;
         break;
      }

      switch(e_ExtTimeSrc)
      {
         case PDSM_LITE_GNSS_TIME_SRC_RESERVED: /* Fall Through */
         case PDSM_LITE_GNSS_TIME_SRC_INVALID:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_INVALID_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_UNKNOWN:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_UNKNOWN_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TAGGING:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_EXTERNAL_INPUT:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_EXTERNAL_INPUT_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_GPS_TOW_DECODE:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_TOW_DECODE_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_TOW_CONFIRMED:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_TOW_CONFIRMED_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_NAV_SOLUTION:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_NAV_SOLUTION_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_SOLVE_FOR_TIME:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_GLO_TOW_DECODE:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_GLO_TOW_DECODE_V02 ;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_TIME_TRANSFORM:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_TIME_TRANSFORM_V02 ;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V02 ;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_GSM_SLEEP_TIME_TAGGING:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V02 ;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_QZSS_TOW_DECODE:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_BDS_TOW_DECODE:
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_BDS_TOW_DECODE_V02;
         break;
         case PDSM_LITE_GNSS_TIME_SRC_GAL_TOW_DECODE :
            *pe_QmiLocTimeSource = eQMI_LOC_TIME_SRC_GAL_TOW_DECODE_V02;
         break;
         default:
            v_RetVal = FALSE;
         break;
      }
   } while(0);
   return v_RetVal;
}


static void tm_util_xlate_gnssNavSvInfo_to_qmi
(
   qmiLocEventPositionReportIndMsgT_v02  *pz_PositionReportIndMsg,
   const pdsm_lite_GnssNavSvInfo               *pz_NavSvInfo
)
{
   uint32_t i = 0, q_SvListIdx = 0;
   

   if(NULL == pz_PositionReportIndMsg ||
      NULL == pz_NavSvInfo )
   {
      return;
   }
   

   /** convert the sv used info to gnssSvUsedList */

   /* Populate the GPS satellite info */
   for (i = 0; (i < PDSM_LITE_N_ACTIVE_GPS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
   {
      /* Check whether GPS SVs were used in calculating position */
      if(pz_NavSvInfo->z_GpsSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX|PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
         pz_PositionReportIndMsg->gnssSvUsedList[q_SvListIdx++] = 
            pz_NavSvInfo->z_GpsSvInfo[i].w_SvId;
      }
   }

   /* Populate the GLO satellite info */
   for (i = 0; (i < PDSM_LITE_N_ACTIVE_GLO_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
   {
      /* Check whether GLO SVs were used in calculating position */
      if(pz_NavSvInfo->z_GloSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX|PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
         pz_PositionReportIndMsg->gnssSvUsedList[q_SvListIdx++] = 
            pz_NavSvInfo->z_GloSvInfo[i].w_SvId;
      }
   }

   /* Populate the QZSS satellite info */
   for (i = 0; (i < PDSM_LITE_N_ACTIVE_QZSS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
   {
      /* Check whether QZSS SVs were used in calculating position */
      if(pz_NavSvInfo->z_QzssSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX|PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
         pz_PositionReportIndMsg->gnssSvUsedList[q_SvListIdx++] = 
            pz_NavSvInfo->z_QzssSvInfo[i].w_SvId;
      }
   }

   /* Populate the BDS satellite info */
   for (i = 0; (i < PDSM_LITE_N_ACTIVE_BDS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
   {
      /* Check whether GLO SVs were used in calculating position */
      if(pz_NavSvInfo->z_BdsSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX|PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
         pz_PositionReportIndMsg->gnssSvUsedList[q_SvListIdx++] = 
            pz_NavSvInfo->z_BdsSvInfo[i].w_SvId;
      }
   }

   /* Populate the GAL satellite info */
   for (i = 0; (i < PDSM_LITE_N_ACTIVE_GAL_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
   {
      /* Check whether GAL SVs were used in calculating position */
      if(pz_NavSvInfo->z_GalSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX|PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
         pz_PositionReportIndMsg->gnssSvUsedList[q_SvListIdx++] = 
            pz_NavSvInfo->z_GalSvInfo[i].w_SvId;
      }
   }

   /** set the valid flag only if the list is non-empty */
   if(q_SvListIdx > 0 )
   {
      pz_PositionReportIndMsg->gnssSvUsedList_len = q_SvListIdx;
      pz_PositionReportIndMsg->gnssSvUsedList_valid = TRUE;
   }

   
}

void tm_util_get_timestamp_from_gps_time(sm_FixInfoStructType    *input_lr_buf_ptr, me_Clock         * z_Clock, unsigned long * timeStamp )
{
 qword  milliseconds, timeStamp_temp, fix_time;
 uint16 time_ref_cdma;
 int16 timestamp_msec;
  uint32   orig_time = 0;
  gps_TimeStructType z_Clock_local;
  z_Clock_local.w_GpsWeek = input_lr_buf_ptr->z_NavPos.w_GpsWeek;
  z_Clock_local.q_GpsMsec = input_lr_buf_ptr->z_NavPos.q_GpsTimeMs;
  timestamp_msec = (int16)(input_lr_buf_ptr->z_NavPos.q_GpsTimeMs % 1000);
  
  gps_to_cdma_time(&z_Clock_local, timeStamp);
  input_lr_buf_ptr->z_NavPos.q_RefFCount = input_lr_buf_ptr->z_NavPos.w_GpsWeek * MS_PER_WEEK + input_lr_buf_ptr->z_NavPos.q_GpsTimeMs;

  gps_total_msec(timeStamp, milliseconds); //  For getting correct timestamp_tow_ms and timestamp_gps_week
  gps_total_msec(timeStamp, fix_time);  // For getting correct timestamp

  
  fix_time[0] = input_lr_buf_ptr->z_NavPos.q_RefFCount;
  (void)qw_div(fix_time, fix_time, 1000);
  time_ref_cdma = (uint16)(fix_time[0] & 0x3FFF);

  gps_week_ms(milliseconds, &(z_Clock->z_Time.w_GpsWeek),
          &(z_Clock->z_Time.q_GpsMsec));
  /* The above gps time conversion routines may cause some rounding errors
  * that cause the millisecond value not to match what had originally been
  * sent in the PE structure.  The following code checks the value and makes
  * an adjustment if needed
  */
  if (z_Clock->z_Time.q_GpsMsec % 1000 != timestamp_msec)
  {
    uint32 lTempMS = z_Clock->z_Time.q_GpsMsec / 1000;
    lTempMS *= 1000;
    z_Clock->z_Time.q_GpsMsec = lTempMS + timestamp_msec;
  }
  /* Check for week rollover */
   gps_MsecWeekLimit((int32 *)&z_Clock->z_Time.q_GpsMsec, &z_Clock->z_Time.w_GpsWeek);
  
  (void)qw_div_by_power_of_2(timeStamp, timeStamp, 16); /* obtain time in 1.25 ms */
  
  /* obtain time in second's */
  (void)qw_div(timeStamp, timeStamp, 800);

  /* Save the current time, so we can check for underflow/overflow */
  orig_time = timeStamp[0];

  /* Now clear the lower 14 bits of time stamp */
  timeStamp[0] &= 0xffffc000;

  /* Now copy the time reference provided from PDE.
  */
  timeStamp[0] |= (time_ref_cdma & 0x3fff);

  /* Check for Overflow/Underflow
  */
  if ((int32)(timeStamp[0] - orig_time) > 0x2000)
  {
    timeStamp[0] -= 0x4000;
  }
  else if ((int32)(orig_time - timeStamp[0]) > 0x2000)
  {
    timeStamp[0] += 0x4000;
  }
  return ;
}

float tm_util_get_geoidal_separation (double latitude, double longitude)
{
   double x1, y1, dZ0, dZ1, dZ2, dZ3, dXn, dYn, a0, a1, a2, a3, iGH;
   int i, j;

   // Interpolates between table entries
   // First gather up the table entries surrounding the position
   if (latitude < -90.0 || latitude > 90.0) return 0.0;
   if (longitude < -180.0 || longitude > 180.0) return 0.0;
   i = ((int)latitude / 10);
   x1 = i * 10;
   i = 9 - i;
   if (i >= 19 || i < 0) i = 0;
   j = ((int)longitude / 10);
   y1 = j * 10;
   j += 18;
   if (j >= 36 || j < 0) j = 0;
      dZ0 = GeoSep[i][j];
   if (--i < 0) i = 0;
      dZ3 = GeoSep[i][j];
   if (++j >= 36) j = 0;
      dZ2 = GeoSep[i][j];
   if (++i >= 19) i = 0;
      dZ1 = GeoSep[i][j];

   // This algorithm was taken from CLSInterpolator::Bilinear
   // Step 1. calculate dXn, and dYn which are non-dimensional (normalized quantities)
   dYn = (longitude - y1) / 10.0;
   dXn = (latitude - x1) / 10.0;

   // Step 2. compute the coefficients of the polynomial
   a0 = dZ0;                          //  SW corner
   a1 = dZ1 - dZ0;                    //  difference along X axis
   a2 = dZ3 - dZ0;                    //  difference along Y axis
   a3 = dZ0 + dZ2 - dZ1 - dZ3;  //  cross - derivative

   // Step 3. evaluate the polynomial
   iGH = a0 + a1 * dYn + a2 * dXn + a3 * dXn * dYn;

   return (float)iGH;
}




/*===========================================================================
FUNCTION tm_util_calculate_heading

DESCRIPTION

  This function is used to calculate the heading value from the PE position report

DEPENDENCIES 
  None.

PARAMETERS:
  sm_GpsFixRptStructType  *fix_ptr,
  

RETURN VALUE
  Heading 
    
SIDE EFFECTS
  None

===========================================================================*/

void tm_util_calculate_heading
(const sm_FixInfoStructType *fix_ptr,
 DBL *heading_angle
 )
{

  DBL  d_Azimuth;

  if (fix_ptr->z_NavPos.z_PosFlags.b_IsHeadingValid)
  {
    d_Azimuth = fix_ptr->z_NavPos.f_HeadingRad * RAD2DEG;
  }
  else
  {
    d_Azimuth = 0;
  }

  *heading_angle = d_Azimuth;        /* [x10 deg] */
}
 
/*============================================================================
FUNCTION tm_util_convert_lr #9x05 remove

DESCRIPTION
    Function to convert navsolution into IS801 Location response structure

RETURN VALUE
    None

DEPENDENCIES

LIMITATIONS

=============================================================================*/
boolean tm_util_convert_lr
(
   sm_FixInfoStructType    *input_lr_buf_ptr,
   tm_lr_resp_type         *output_lr_buf_ptr
   )

{

  DBL                     heading_angle;
  DBL                     vel_east;
  DBL                     vel_north;
  qword                   fix_time;
  gps_TimeStructType      z_Clock;
  DBL                     f_tmp;

  if ((input_lr_buf_ptr == NULL) || (output_lr_buf_ptr == NULL))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null pointer recv'd", 0, 0, 0);
    return FALSE;
  }

  z_Clock.w_GpsWeek = input_lr_buf_ptr->z_NavPos.w_GpsWeek;
  z_Clock.q_GpsMsec = input_lr_buf_ptr->z_NavPos.q_GpsTimeMs;
  gps_to_cdma_time(&z_Clock, output_lr_buf_ptr->timestamp);

  if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed &&
      input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)
  {
    /* Since both constellations were used, GPS to Glonass Bias is valid */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_GPS | PDSM_GNSS_TIME_GLO;
    /* convert bias from meters to ms by dividing it with LIGHT_MSEC (distance light travels in a msec) */
    output_lr_buf_ptr->f_GGTBMs    = (FLT)(input_lr_buf_ptr->z_NavPos.z_InterGnssTb[GNSS_TB_GPSGLO].f_FiltMeters    * ONE_OVER_LIGHT_MSEC);
    output_lr_buf_ptr->f_GGTBUncMs = (FLT)(input_lr_buf_ptr->z_NavPos.z_InterGnssTb[GNSS_TB_GPSGLO].f_FiltUncMeters * ONE_OVER_LIGHT_MSEC);
  }
  else if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed)
  {
    /* Only GPS constellation was used in fix, impossible that we know GPS to Glonass time bias */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_GPS;
  }
  else if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)
  {
    /* Only Glo constellation was used in fix, impossible that we know Glo to Glonass time bias */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_GLO;
  }
  else if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsBdsUsed)
  {
    /* Only BDS constellation was used in fix */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_BDS;   // TODO FIXME
  }
  else if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGalUsed)
  {
    /* Only GAL constellation was used in fix */
    output_lr_buf_ptr->time_src  = PDSM_GNSS_TIME_GAL;
  }

  /* Update the local copy of qRefFCount with the fix time also because
   * it will be used to generate the time_ref_cdma.
  */
  input_lr_buf_ptr->z_NavPos.q_RefFCount = input_lr_buf_ptr->z_NavPos.w_GpsWeek * MS_PER_WEEK + input_lr_buf_ptr->z_NavPos.q_GpsTimeMs;

  /* Position CDMA time stamp in seconds modulo 16384.
   * Assumes that q_RefFCount was derived from CDMA time.
  */
  /* Convert CDMA timestamp to milliseconds */
  gps_total_msec(output_lr_buf_ptr->timestamp, fix_time);

  /* Use only the upper half of the current time along
   * q_RefFCount measured in milliseconds.
  */
  fix_time[0] = input_lr_buf_ptr->z_NavPos.q_RefFCount;

  /* Save msec portion of LR fix time */
  output_lr_buf_ptr->timestamp_msec = (int16)(input_lr_buf_ptr->z_NavPos.q_GpsTimeMs % 1000);
  output_lr_buf_ptr->msec_incl = 1;

  /* Convert to seconds */
  (void)qw_div(fix_time, fix_time, 1000);
  /* Modulo 16384 */
  output_lr_buf_ptr->time_ref_cdma = (uint16)(fix_time[0] & 0x3FFF);

  /* Convert latitude from radians to degrees and scale. */
  output_lr_buf_ptr->latitude = tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[0] * RAD2DEG *
                                                     LAT_LONG_SCALE);

  /* Convert longitude from radians to degrees and scale. */
  output_lr_buf_ptr->longitude = tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[1] * RAD2DEG *
                                                      LAT_LONG_SCALE);

#ifdef FEATURE_REDUCE_PDAPI_POSREP_RESOLUTION_TO_GAD
  output_lr_buf_ptr->latitude =
     tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[0] * C_LAT_RAD_TO_GAD);
  output_lr_buf_ptr->latitude *= 2;


  output_lr_buf_ptr->longitude =
     tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[1] * C_LON_RAD_TO_GAD);
  output_lr_buf_ptr->longitude *= 4;
#endif

  /* Set axis to True North for position uncertainty. */
  output_lr_buf_ptr->loc_uncrtnty_ang = (byte)(input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[0] / 5.625F);

  /* Format the LOC_UNCRTNTY_A field. */
  output_lr_buf_ptr->loc_uncrtnty_a = (uint8)tm_util_select_pos_unc(input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[1]);

  /* Format the LOC_UNCRTNTY_P field. */
  output_lr_buf_ptr->loc_uncrtnty_p = (uint8)tm_util_select_pos_unc(input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[2]);

  /* 0: 2D, 1: 3D */
  if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_Is3D)
  {
    output_lr_buf_ptr->fix_type = 1;
  }
  else
  {
    output_lr_buf_ptr->fix_type = 0;
  }

  /* Velocity information is included */
  if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsVelValid)
    output_lr_buf_ptr->velocity_incl = 1;
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Velocity fix failed", 0, 0, 0);
    output_lr_buf_ptr->velocity_incl = 0;
  }

  /* Compute horizontal velocity from components and scale
   * to units of 0.25 m/s.
  */
  vel_east = input_lr_buf_ptr->z_NavPos.f_VelEnu[0];
  vel_north = input_lr_buf_ptr->z_NavPos.f_VelEnu[1];

  f_tmp = sqrt(vel_east * vel_east + vel_north * vel_north);


  if (f_tmp < TM_CORE_MIN_VELOCTY)
  {
    output_lr_buf_ptr->velocity_hor = 0;
  }
  else
  {
    output_lr_buf_ptr->velocity_hor =
       (uint16)tm_util_nearest_long(4.0 * f_tmp);
  }

  tm_util_calculate_heading(input_lr_buf_ptr, &heading_angle);

  output_lr_buf_ptr->heading = (uint16)tm_util_nearest_long(heading_angle * HEADING_SCALE);
  /* heading & uncertainty in radian */
  output_lr_buf_ptr->f_HeadingRad = input_lr_buf_ptr->z_NavPos.f_HeadingRad;
  output_lr_buf_ptr->f_HeadingUncRad = input_lr_buf_ptr->z_NavPos.f_HeadingUncRad;
  output_lr_buf_ptr->f_GnssHeadingRad = input_lr_buf_ptr->z_NavPos.f_GnssHeadingRad;
  output_lr_buf_ptr->f_GnssHeadingUncRad = input_lr_buf_ptr->z_NavPos.f_GnssHeadingUncRad;

  if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsHeadingValid)
  {
    output_lr_buf_ptr->heading_valid = TRUE;
  }
  else
  {
    output_lr_buf_ptr->heading_valid = FALSE;
  }

  /* If fix_type = 1, set the vertical velocity in units of 0.5 m/s. */
  if (output_lr_buf_ptr->fix_type)
  {
    output_lr_buf_ptr->velocity_ver =
       (int1)tm_util_nearest_long(2.0 * input_lr_buf_ptr->z_NavPos.f_VelEnu[2]);
  }

  /* Include clock information. */
  output_lr_buf_ptr->clock_incl = 1;
  output_lr_buf_ptr->clock_drift = (sint15)(-input_lr_buf_ptr->z_NavPos.f_ClockDriftRate *
                                            1.0E9 / LIGHT_SEC);

  /* Indicate that the HEIGHT field is included. */
  output_lr_buf_ptr->height_incl = 1;

  /* HEIGHT field */
  /* height above the WGS-84 reference ellipsoid hence add 500m */
  output_lr_buf_ptr->f_altitude = input_lr_buf_ptr->z_NavPos.d_PosLla[2];
  output_lr_buf_ptr->f_altitude += 500.0;
  output_lr_buf_ptr->height = (sint15)tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[2]);
  /* Format the LOC_UNCRTNTY_V field. */
  output_lr_buf_ptr->height += 500;

  /* Format the LOC_UNCRTNTY_V field. */
  output_lr_buf_ptr->loc_uncrtnty_v = (uint8)tm_util_select_pos_unc(input_lr_buf_ptr->z_NavPos.f_PuncVertMeters);

  output_lr_buf_ptr->f_clock_bias_unc = input_lr_buf_ptr->z_NavPos.f_ClockBiasUncMs;
  /* Time source parameters: pdsm_extended_time_src_e_type */
  output_lr_buf_ptr->u_TimeSource = input_lr_buf_ptr->z_NavPos.u_InitGpsTimeSrc;

  output_lr_buf_ptr->e_HoriRelIndicator = (pdsm_lite_pd_reliability_e_type)input_lr_buf_ptr->z_NavPos.e_HoriRelIndicator;
  output_lr_buf_ptr->e_VertRelIndicator = (pdsm_lite_pd_reliability_e_type)input_lr_buf_ptr->z_NavPos.e_VertRelIndicator;
  
  /* Check the flag which indicates if the SV use in fix report is valid */
  if (input_lr_buf_ptr->z_NavPos.z_SmPosFlags.b_IsSatellite)
  {
    output_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid = TRUE;
  }
  else
  {
    output_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid = FALSE;
  }

  /* Copy the entire SVInfo structure */
  output_lr_buf_ptr->z_NavSvInfo = input_lr_buf_ptr->z_NavSvInfo;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_util_convert_lr: nv_use_rpt_valid=%d", output_lr_buf_ptr->z_NavSvInfo.u_SvInfoValid, 0, 0);

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "GpsSvMask used=0x%08x usable=0x%08x",
        output_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsed,
        output_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsable);

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "GloSvMask used=0x%08x usable=0x%08x",
        output_lr_buf_ptr->z_NavSvInfo.q_GloSvMaskUsed,
        output_lr_buf_ptr->z_NavSvInfo.q_GloSvMaskUsable);

  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "BdsSvMask used=0x%08x%08x usable=0x%08x%08x",
        (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_BdsSvMaskUsed >> 32),
        (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_BdsSvMaskUsed),
        (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_BdsSvMaskUsable >> 32),
        (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_BdsSvMaskUsable));

  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "GalSvMask used=0x%08x%08x usable=0x%08x%08x",
        (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_GalSvMaskUsed >> 32),
        (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_GalSvMaskUsed),
        (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_GalSvMaskUsable >> 32),
        (uint32)(output_lr_buf_ptr->z_NavSvInfo.t_GalSvMaskUsable));

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "QzssSvMask used=0x%02x usable=0x%02x",
        output_lr_buf_ptr->z_NavSvInfo.u_QzssSvMaskUsed,
        output_lr_buf_ptr->z_NavSvInfo.u_QzssSvMaskUsable);

  MSG_5(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,
        "Final fix Used SVs (GPS=%u, GLO=%u, BDS=%u, GAL=%u, QZSS=%u)",
        output_lr_buf_ptr->z_NavSvInfo.u_GpsNumSvPosFix,
        output_lr_buf_ptr->z_NavSvInfo.u_GloNumSvPosFix,
        output_lr_buf_ptr->z_NavSvInfo.u_BdsNumSvPosFix,
        output_lr_buf_ptr->z_NavSvInfo.u_GalNumSvPosFix,
        output_lr_buf_ptr->z_NavSvInfo.u_QzssNumSvPosFix);

  /* Sensor (gyro/accel) bias parameter structure */
  return TRUE;

  return FALSE;
}


/*===========================================================================
FUNCTION tm_util_post_non_exist_svs #9x05 not called anywhere

Enum for MSG trace in following two functions */
enum
{
  LSM_CONF_CONVERT_FROM = 0,
  LSM_CONF_CONVERT_TO = 1
};

/*===========================================================================

FUNCTION
  convert_pct_limited

DESCRIPTION
  This function converts a float % in the range from 0.00 to 1.00 to a
    to a U8 index of percents in the range from 1-99, 
    warning if the input was out of that range.

RETURN: U8 percent

Note: this function is for the use of tm_util_get_cep_scale_ratio_from_pct
      only and is not exposed outside this module.

===========================================================================*/
static uint8 convert_pct_limited(float f_input_percent, uint8 u_case)
{
  f_input_percent *= (float)100.0;
  f_input_percent += (float)0.5; /* 1/2 a quanta of the bins, s.t. we round to
                               nearest int. */
  if (f_input_percent <= (float)1.0)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%% conversion input too small %d %%, converting to 1%% (case %d)",
          (S32)(f_input_percent), u_case, 0);
    return 1;
  }
  else if (f_input_percent >= (float)99.0)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "%% conversion input too big %d %%, converting to 99%% (case %d)",
          (S32)(f_input_percent), u_case, 0);
    return 99;
  }
  else
  {
    return(uint8)(f_input_percent);
  }
}

/*
 ******************************************************************************
 *
 * tm_util_get_cep_scale_ratio_from_pct
 *
 * Function description:
 *  This function gives the ratio to scale circular uncertainties (CEP's) from
 *  different percentiles.  E.g. 39% conf. "standard" to 99% conf.
 *
 * Parameters:
 *
 *  f_from_percentile: CEP percentile to convert from (range 0.00 to 1.00)
 *  f_to_percentile: CEP percentile to convert to (range 0.00 to 1.00)
 *
 * Return value:
 *
 *  The scale factor to multiply by to scale the CEP<from>% to a CEP<to>% 
 *   value (in meters.)
 *
 ******************************************************************************
*/
float tm_util_get_cep_scale_ratio_from_pct(float f_from_percentile,
                                           float f_to_percentile)
{
  static const float f_cep_percentile_scalars[100] =
  {
    /* Value for 0% should be 0f, but preventing divide by zero */
    /*  0% - 9% */
    0.0001f, 0.1418f, 0.2010f, 0.2468f, 0.2857f,
    0.3203f, 0.3518f, 0.3810f, 0.4084f, 0.4343f,
    /* 10% */
    0.4590f, 0.4828f, 0.5056f, 0.5278f, 0.5492f,
    0.5701f, 0.5905f, 0.6105f, 0.6300f, 0.6492f,
    /* 20% */
    0.6680f, 0.6866f, 0.7049f, 0.7230f, 0.7409f,
    0.7585f, 0.7760f, 0.7934f, 0.8106f, 0.8276f,
    /* 30% */
    0.8446f, 0.8615f, 0.8783f, 0.8950f, 0.9116f,
    0.9282f, 0.9448f, 0.9613f, 0.9778f, 0.9943f,
    /* 40% */
    1.0108f, 1.0273f, 1.0438f, 1.0603f, 1.0769f,
    1.0935f, 1.1101f, 1.1268f, 1.1436f, 1.1605f,
    /* 50% */
    1.1774f, 1.1944f, 1.2116f, 1.2288f, 1.2462f,
    1.2637f, 1.2814f, 1.2992f, 1.3172f, 1.3354f,
    /* 60% */
    1.3537f, 1.3723f, 1.3911f, 1.4101f, 1.4294f,
    1.4490f, 1.4689f, 1.4891f, 1.5096f, 1.5305f,
    /* 70% */
    1.5518f, 1.5735f, 1.5956f, 1.6182f, 1.6414f,
    1.6651f, 1.6894f, 1.7145f, 1.7402f, 1.7667f,
    /* 80% */
    1.7941f, 1.8225f, 1.8519f, 1.8825f, 1.9145f,
    1.9479f, 1.9830f, 2.0200f, 2.0593f, 2.1011f,
    /* 90% - 99% */
    2.1460f, 2.1945f, 2.2475f, 2.3062f, 2.3721f,
    2.4477f, 2.5373f, 2.6482f, 2.7971f, 3.0349f
  };

  uint8 u_from_pct, u_to_pct;

  u_from_pct = convert_pct_limited(f_from_percentile, LSM_CONF_CONVERT_FROM);
  u_to_pct =   convert_pct_limited(f_to_percentile,   LSM_CONF_CONVERT_TO);

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "conversion value %d, %d", u_to_pct, u_from_pct, 0);
  return (f_cep_percentile_scalars[u_to_pct] /
          f_cep_percentile_scalars[u_from_pct]);
}

/*===========================================================================
FUNCTION tm_util_check_numerical_suffix

DESCRIPTION
  This function that the input string is a numerical value, that is a 
  specific number of bytes in length and is within the min and max values
  provided.
  

DEPENDENCIES
  None.

RETURN VALUE
   TRUE - Numerical value within bounds
   FALSE - Incorrect string passed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean tm_util_check_numerical_suffix
(
   const char *p_path,
   uint32      min,
   uint32      max,
   uint8       bytes_to_convert
   )
{
  uint8  index   = 0;
  uint64 value   = 0;

  /* If NULL string, or string is longer than bytes_to_convert, return FALSE */
  if (p_path == NULL || p_path[bytes_to_convert] != '\0')
  {
    return FALSE;
  }

  /* If no bytes_to_convert, return TRUE */
  if (bytes_to_convert == 0)
  {
    return TRUE;
  }

  /* examine each byte in string */
  while (p_path[index] != '\0')
  {
    /* If any unexpected characters, return FALSE */
    if (p_path[index] > '9' || p_path[index] < '0')
    {
      return FALSE;
    }
    else
    {
      value += value * 10 + p_path[index] - '0';
    }
    index++;
  }

  /* Convert value must be within the bounds given */
  if (value < min || value > max)
  {
    return FALSE;
  }

  return TRUE;
}

/*
 ******************************************************************************
 *
 * tm_util_num_bits_set_in_long
 *
 * Function description:
 *  This function counts the number of bits set in a uint32 bit mask and returns
 *  the result.
 *
 * Parameters:
 *
 *  q_long_word: bit mask;
 *
 * Return value:
 *
 * number of bits set.
 ******************************************************************************
*/
uint8 tm_util_num_bits_set_in_long(uint32 q_long_word)
{
  uint8 u_cnt = 0;

  while (q_long_word != 0)
  {
    if (q_long_word & ((uint32)0x1 << 31))
    {
      u_cnt++;
    }
    q_long_word = q_long_word << 1;
  }
  return u_cnt;
}

/*
 ******************************************************************************
 *
 * tm_util_num_bits_set_in_long64
 *
 * Function description:
 *  This function counts the number of bits set in a uint64 bit mask and returns
 *  the result.
 *
 * Parameters:
 *
 *  t_long [IN]: bit mask;
 *
 * Return value:
 *
 *  Number of bits set.
 ******************************************************************************
*/
uint8 tm_util_num_bits_set_in_long64(uint64 t_long_bits)
{
  uint8 u_cnt = 0;

  for (u_cnt = 0; t_long_bits; u_cnt++)
  {
    t_long_bits &= t_long_bits - 1; // clear the least significant bit set
  }
  return (u_cnt);
}

/*
 ******************************************************************************
 *
 * tm_util_calc_vel_horiz_uncertainty
 *
 * Function description:
 *  This function calculate the horizontal velocity uncertainty and
 *  return the result
 *
 * Return value:
 *
 *  horizontal velocity uncertainty (m/s)
 ******************************************************************************
*/
FLT tm_util_calc_vel_horiz_uncertainty(const sm_NavPositionStructType *nav_ptr)
{
  if (nav_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "null ptr passed %x", nav_ptr, 0, 0);
    return 0;
  }

  // use 3D velocity uncertainty directly
  //return (FLT)(nav_ptr->f_Vunc3dMps);

  /* Calculate velUncHoriz */
  return (FLT)(sqrt(nav_ptr->f_VuncEastNorthMps[0] *
                    nav_ptr->f_VuncEastNorthMps[0] +
                    nav_ptr->f_VuncEastNorthMps[1] *
                    nav_ptr->f_VuncEastNorthMps[1]));
}

/*
 ******************************************************************************
 *
 * tm_util_calc_vel_vert_uncertainty
 *
 * Function description:
 *  This function calculate the vertical velocity uncertainty and
 *  return the result
 *
 * Return value:
 *
 *  vertical velocity uncertainty (m/s)
 ******************************************************************************
*/
FLT tm_util_calc_vel_vert_uncertainty(const sm_NavPositionStructType *nav_ptr)
{
  if (nav_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "null ptr passed %x", nav_ptr, 0, 0);
    return 0;
  }

  // use 3D velocity uncertainty directly
  //return (FLT)(nav_ptr->f_Vunc3dMps);

  return (FLT)(nav_ptr->f_VuncVertMps);
}

/*
 ******************************************************************************
 *
 * FUNCTION:
 * tm_util_calculate_position_print
 *
 * DESCRIPTION:
 *  This function calculates position using fix report
 *  Position is calcuated in Latitude and Longitude
 *
 * PARAMETERS:
 *  p_fix:          [Input] ptr to fix report
 *  p_Str:          [Output]ptr to the string where data will be written
 *  q_buf_len:      length of string
 *
 * RETURN VALUE:
 *  q_len           length of string written
 *
 ******************************************************************************
*/
int tm_util_calculate_position_print(const sm_FixInfoStructType *p_fix, char *p_Str, int q_buf_len)
{
  double d_Lat, d_Long, d_LatMins, d_LongMins = 0.0;
  char c_LatHem, c_LongHem;
  int q_len = 0;

  if ((NULL == p_fix) || (NULL == p_Str) || (q_buf_len <= 0))
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_util_calculate_position(): Invalid Input");
    return (0);
  }

  /* Latitude and longitude: */
  d_Lat = p_fix->z_NavPos.d_PosLla[0] * (180.0 / C_PI);
  d_Long = p_fix->z_NavPos.d_PosLla[1] * (180.0 / C_PI);

  if (d_Lat > 0.0)
  {
    c_LatHem = 'N';
  }
  else
  {
    c_LatHem = 'S';
    d_Lat *= -1.0;
  }

  d_LatMins = fmod(d_Lat * 60.0, 60.0);

  if (d_LatMins > 59.999999)
  {
    d_LatMins = 0.0;
    d_Lat += 1.0;
  }

  if (d_Lat > 90.0)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_util_calculate_position_print: Error in latitude calculation");
    return (0);
  }

  if (d_Long < 0.0)
  {
    c_LongHem = 'W';
    d_Long *= -1.0;
  }
  else
  {
    c_LongHem = 'E';
  }

  d_LongMins = fmod(d_Long * 60.0, 60.0);

  if (d_LongMins > 59.999999)
  {
    d_LongMins = 0.0;
    d_Long += 1.0;
  }

  if (d_Long > 180.0)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_util_calculate_position_print: Error in logitude calculation");
    return (0);
  }

  q_len = snprintf(p_Str, q_buf_len, "%02d%09.6lf,%c,%03d%09.6lf,%c,",
                   (uint8)floor(d_Lat), d_LatMins, c_LatHem,
                   (uint8)floor(d_Long), d_LongMins, c_LongHem);

  if (q_len <= 0 || q_len >= q_buf_len)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_util_calculate_position_print: Error in string formatting", 0, 0, 0);
    return (0);
  }
  return q_len;

}

/*
 ******************************************************************************
 *adding for loc mw to compile #9x05
 * tm_util_get_rand dummy
 *
 * Function description:
 *   Gets a random number from security module.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *  uint32 random number.
 *
 ******************************************************************************
*/
uint32 tm_util_get_rand
(
   void
   )
{
  uint32 rand_num;
  rand_num = rand();

  return (rand_num);
}


/*===========================================================================

FUNCTION tm_util_translate_glo_intcodephase

DESCRIPTION
 Compute the Glonass milli-second of the week from interger code phase &
 Glonass Time Of Day in milli-seconds.

DEPENDENCIES

RETURN VALUE
  GNSS milli-second of the week

SIDE EFFECTS
  None

===========================================================================*/
uint32 tm_util_translate_glo_intcodephase(uint32 const q_integer_code_phase, uint32 const q_GloMsecInDay)
{
  if (q_GloMsecInDay < q_integer_code_phase)
  {
    return ((uint32)((q_GloMsecInDay + DAY_MSECS - q_integer_code_phase) % 80));
  }
  else
  {
    return ((uint32)((q_GloMsecInDay - q_integer_code_phase) % 80));
  }
}
/*===========================================================================
FUNCTION tm_util_calculate_offset_print

DESCRIPTION

 tm_util_calculate_offset_print() is used to Calcullate offset for PQDTM. It is difference of Lat/Long/alt between WGS84 and PZ90 cooordinate system 

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
int tm_util_calculate_offset_print(const double *d_PosLlaWGS84, const double *d_PosLlaPZ90, char *p_Str, int q_buf_len )
{
  double d_Lat, d_Long, d_Alt, d_LatMins, d_LongMins = 0.0;
  char c_LatHem, c_LongHem;
  int q_len = 0;
  
  if ( ( NULL == d_PosLlaWGS84)  || ( NULL == d_PosLlaPZ90) || (NULL == p_Str) || (q_buf_len <= 0))
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_util_calculate_position(): Invalid Input");
    return (0);
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "WGS84 Values     Lat : %d Long : %d Alt : %d",d_PosLlaWGS84[0],d_PosLlaWGS84[1],d_PosLlaWGS84[2]);
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "PZ90 Values     Lat : %d Long : %d Alt : %d",d_PosLlaPZ90[0],d_PosLlaPZ90[1],d_PosLlaPZ90[2]);

  
  /* Latitude offset */
  d_Lat =  (d_PosLlaPZ90[0] - d_PosLlaWGS84[0]) * (180.0 / C_PI);

  /* Longitude offset */
  d_Long = fmod(((d_PosLlaPZ90[1] - d_PosLlaWGS84[1]) * (180.0 / C_PI)), 360.0);
  if (d_Long < -180.0)
  {
    d_Long += 360.0;
  }
  else if (d_Long > 180.0)
  {
    d_Long -= 360.0;
  }

  /* Altitude offset */
  d_Alt =  (d_PosLlaPZ90[2] - d_PosLlaWGS84[2]);

  if ( d_Lat > 0.0 )
  {
    c_LatHem = 'N';
  }
  else
  {
    c_LatHem = 'S';
    d_Lat *= -1.0;
  }

  d_LatMins = fmod( d_Lat * 60.0 , 60.0 );

  if ( d_Long < 0.0 )
  {
    c_LongHem = 'W';
    d_Long *= -1.0;
  }
  else
  {
    c_LongHem = 'E';
  } 

  d_LongMins = fmod( d_Long * 60.0 , 60.0 );
  
  q_len = snprintf(p_Str, q_buf_len, "%02d%09.6lf,%c,%03d%09.6lf,%c,%.3lf,",
                        (uint8) floor( d_Lat ) , d_LatMins , c_LatHem ,
                        (uint8) floor( d_Long ) , d_LongMins , c_LongHem,
                        d_Alt);
  
  if(q_len <= 0 || q_len >= q_buf_len)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_util_calculate_position_print: Error in string formatting");
    return (0);
  }
                       
  return q_len;
}   
/*===========================================================================
 FUNCTION tm_util_late_meas_rpt_to_qmi_loc_SV_Info_Ind
 
 DESCRIPTION
       Used to populate QMI indication from SM structure

 DEPENDENCIES
 
 RETURN VALUE none
 
 SIDE EFFECTS
  
===========================================================================*/

uint16 tm_util_xlate_meas_rpt_to_qmi_loc_SV_Info_Ind(gnss_MeasBlkStructType              *lr_data, qmiLocEventGnssSvInfoIndMsgT_v02 *pz_SVInfoIndMsg,uint16 sv_index)
{
   uint16 u_in=0,sv_id,u_localid=0;
   if ((NULL == lr_data) || (lr_data->u_NumSvs > N_ACTIVE_CHAN))
   {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Error tm_util_xlate_meas_rpt_to_qmi_loc_SV_Info_Ind ");
	  return 0;
   }

   /**Getting SV info for all SVs*/
   for (u_in=sv_index; (u_in < ((lr_data->u_NumSvs)+sv_index)) && (u_in < QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02) ; u_in++,u_localid++)
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"xlate function for %d sv",lr_data->z_Gnss[u_localid].w_Sv,0,0);  
	  
      sv_id = lr_data->z_Gnss[u_localid].w_Sv;
	  pz_SVInfoIndMsg->svList[u_in].gnssSvId = sv_id;
	  pz_SVInfoIndMsg->svList[u_in].validMask = 0;
	  pz_SVInfoIndMsg->svList[u_in].svInfoMask = 0;

	  if (sv_id >= PDSM_LITE_PD_SV_ID_GPS_MIN && sv_id <= PDSM_LITE_PD_SV_ID_GPS_MAX)
	  {
	      
          pz_SVInfoIndMsg->svList[u_in].system = eQMI_LOC_SV_SYSTEM_GPS_V02;
		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02|QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02;
	   
	      if ((tm_core_info.session_info.eph_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_GPS_MIN))) != 0)
	      {
	          pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
	  		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
	      }
		  if ((tm_core_info.session_info.alm_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_GPS_MIN))) != 0)
		  {
		     pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
			 pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
		  }
		  if (tm_core_info.session_info.health_svmask)
		  {
		     pz_SVInfoIndMsg->svList[u_in].validMask |= QMI_LOC_SV_INFO_MASK_VALID_HEALTH_STATUS_V02;
		     pz_SVInfoIndMsg->svList[u_in].healthStatus = (uint8)((tm_core_info.session_info.health_svmask >> (sv_id - PDSM_LITE_PD_SV_ID_GPS_MIN)) & 1); 
		  }
		  
	 }
	 else if ((sv_id >= PDSM_LITE_PD_SV_ID_GLO_MIN) && (sv_id <= PDSM_LITE_PD_SV_ID_GLO_MAX))
	 {
	 
     	  pz_SVInfoIndMsg->svList[u_in].system = eQMI_LOC_SV_SYSTEM_GLONASS_V02;
		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02|QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02;
		  if ((tm_core_info.session_info.glo_eph_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_GLO_MIN))) != 0)
		  {
		      pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
	  		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
		  }
		  if ((tm_core_info.session_info.glo_alm_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_GLO_MIN))) != 0)
		  {
		     pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
			 pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
		  }	 
	 }
	 else if ((sv_id >= PDSM_LITE_PD_SV_ID_BDS_MIN) && (sv_id <= PDSM_LITE_PD_SV_ID_BDS_MAX))
	 {
	 
	      pz_SVInfoIndMsg->svList[u_in].system = eQMI_LOC_SV_SYSTEM_BDS_V02;
		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02|QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02;
		  if ((tm_core_info.session_info.bds_eph_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_BDS_MIN))) != 0)
		  {
		      pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
	  		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
		  }
		  if ((tm_core_info.session_info.bds_alm_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_BDS_MIN))) != 0)
		  {
		     pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
			 pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
		  }	 
	 }
	 else if ((sv_id >= PDSM_LITE_PD_SV_ID_GAL_MIN) && (sv_id <= PDSM_LITE_PD_SV_ID_GAL_MAX))
	 {
	 
	      pz_SVInfoIndMsg->svList[u_in].system = eQMI_LOC_SV_SYSTEM_GALILEO_V02;
		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02|QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02;
		  if ((tm_core_info.session_info.gal_eph_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_GAL_MIN))) != 0)
		  {
		      pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
	  		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
		  }
		  if ((tm_core_info.session_info.gal_alm_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_GAL_MIN))) != 0)
		  {
		     pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
			 pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
		  }
	 }
	 else if ((sv_id >= PDSM_LITE_PD_SV_ID_QZSS_MIN) && (sv_id <= PDSM_LITE_PD_SV_ID_QZSS_MAX))
	 {
	 
	      pz_SVInfoIndMsg->svList[u_in].system = eQMI_LOC_SV_SYSTEM_QZSS_V02;
		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02|QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02;
		  if ((tm_core_info.session_info.qzss_eph_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_QZSS_MIN))) != 0)
		  {
		      pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
	  		  pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
		  }
		  if ((tm_core_info.session_info.qzss_alm_svmask & (1 << (sv_id - PDSM_LITE_PD_SV_ID_QZSS_MIN))) != 0)
		  {
		     pz_SVInfoIndMsg->svList[u_in].svInfoMask = pz_SVInfoIndMsg->svList[u_in].svInfoMask | QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
			 pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
		  }
	 }

	 if (lr_data->z_Gnss[u_localid].u_SvDirValid && (lr_data->z_Gnss[u_localid].f_Elevation > 0) && (lr_data->z_Gnss[u_localid].f_Azimuth > 0))
	 {
	     pz_SVInfoIndMsg->svList[u_in].elevation = lr_data->z_Gnss[u_localid].f_Elevation * RAD2DEG;
		 pz_SVInfoIndMsg->svList[u_in].azimuth = lr_data->z_Gnss[u_localid].f_Azimuth * RAD2DEG;
		 pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_AZIMUTH_V02|QMI_LOC_SV_INFO_MASK_VALID_ELEVATION_V02;
	 }
	 if (lr_data->z_Gnss[u_localid].w_Cno)
	 {
	    pz_SVInfoIndMsg->svList[u_in].snr = (float)((lr_data->z_Gnss[u_localid].w_Cno) + (lr_data->z_Gnss[u_localid].w_RfLoss)) / 10;
	 }
	 pz_SVInfoIndMsg->svList[u_in].svStatus = lr_data->z_Gnss[u_localid].q_SvState;
	 pz_SVInfoIndMsg->svList[u_in].validMask = pz_SVInfoIndMsg->svList[u_in].validMask | QMI_LOC_SV_INFO_MASK_VALID_SNR_V02 | QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02;
     pz_SVInfoIndMsg->svList[u_in].gnssSignalType = 0; // TBD 
   }   
   
   return u_in;
}

/*===========================================================================
 FUNCTION tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind
 
 DESCRIPTION
       Used to populate QMI indication from SM structure

 DEPENDENCIES
 
 RETURN VALUE none
 
 SIDE EFFECTS
  
===========================================================================*/
 pdsm_lite_pd_event_type tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind(sm_FixInfoStructType *lr_data,
                                                        qmiLocEventPositionReportIndMsgT_v02* pz_PositionReportIndMsg , 
                                                        prtl_sess_info_e_type orig_sess_info_type )
{
   /*Session status and session Id can be filled in loc_mw*/
   /*Lat Long is valid */
   double latitude  = 0.0;
   double longitude = 0.0;
   byte loc_uncrtnty_ang;
   pdsm_lite_pd_event_type   pd_event;
   sint31 latitude_temp;
   sint31 longitude_temp;
   int64 latitude_temp_2;
   int64 longitude_temp_2;
   int1 velocity_ver;
   uint16 velocity_hor;
   float f_alongAxisUnc;
   float f_perpAxisUnc;
   double vel_east;
   double vel_north;
   double f_tmp;
   float f_VelocityUncENUMps[3] = {0,0,0};
   byte                                        uncertainty_a;
   byte                                        uncertainty_p;
   DBL  d_Azimuth;
   float                                       heading = 0.0; 
   boolean is_best_avail_fix = (PRTL_SESS_INFO_NOFIX == orig_sess_info_type)?TRUE:FALSE;

   uncertainty_a = (uint8)tm_util_select_pos_unc(lr_data->z_NavPos.f_ErrorEllipse[1]);
   uncertainty_p = (uint8)tm_util_select_pos_unc(lr_data->z_NavPos.f_ErrorEllipse[2]);
   f_alongAxisUnc = lr_data->z_NavPos.f_ErrorEllipse[1];
   f_perpAxisUnc = lr_data->z_NavPos.f_ErrorEllipse[2];
   loc_uncrtnty_ang = (byte)(lr_data->z_NavPos.f_ErrorEllipse[0] / 5.625F);
   pd_event = PDSM_LITE_PD_EVENT_POSITION;
   
   tm_util_calculate_heading(lr_data, &d_Azimuth);

  /*Treat BestAvail Fix similar to intermiate fix*/
  if(PRTL_SESS_INFO_INTERM_LR == orig_sess_info_type || PRTL_SESS_INFO_NOFIX == orig_sess_info_type)
  {
    pz_PositionReportIndMsg->sessionStatus = eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02;

   latitude_temp = tm_util_nearest_long(lr_data->z_NavPos.d_PosLla[0] * RAD2DEG * LAT_LONG_SCALE); 
   latitude = ((double) ((signed) latitude_temp)) * 180.0 / (1 << 25);

   longitude_temp = tm_util_nearest_long(lr_data->z_NavPos.d_PosLla[1] * RAD2DEG * LAT_LONG_SCALE); 
   longitude = ((double) ((signed) longitude_temp)) * 180.0 / (1 << 25);

    /********************Velocity related calculations for intermediate fix******************************************************/
    if(lr_data->z_NavPos.z_PosFlags.b_IsVelValid)
    {
      
      pz_PositionReportIndMsg->speedHorizontal_valid = TRUE;
      pz_PositionReportIndMsg->speedVertical_valid = TRUE;
      pz_PositionReportIndMsg->speedUnc_valid = TRUE;

      /*heading related calculation specific to intermediate fix*/
      heading  = ((float) ((tm_util_nearest_long(d_Azimuth * HEADING_SCALE))*360)/ (1<<10));
    }
  }
  else if (PRTL_SESS_INFO_LR == orig_sess_info_type)
  { 
    /* Final Fix */
    pz_PositionReportIndMsg->sessionStatus = eQMI_LOC_SESS_STATUS_SUCCESS_V02;

    /*#9x05: mimicked tm_core_generate_ext_pd_data(const sm_FixInfoStructType * fix_ptr) for final position*/
    latitude_temp_2  = (int64)(lr_data->z_NavPos.d_PosLla[0] * (180.0 / C_PI) * (double)PDSM_LITE_LATLONG_SCALE + 0.5);
    latitude = ((double) latitude_temp_2) / PDSM_LITE_LATLONG_SCALE;
    longitude_temp_2  = (int64)(lr_data->z_NavPos.d_PosLla[1] * (180.0 / C_PI) * (double)PDSM_LITE_LATLONG_SCALE + 0.5);
    longitude = ((double) longitude_temp_2) / PDSM_LITE_LATLONG_SCALE;

    /********************Velocity related calculations for final fix******************************************************/
    if(lr_data->z_NavPos.z_PosFlags.b_IsVelValid)
    {
      pd_event |= PDSM_LITE_PD_EVENT_VELOCITY;
       velocity_ver = 0;
       velocity_hor = 0;
      if(lr_data->z_NavPos.z_PosFlags.b_Is3D)
      {
         velocity_ver = (int1)tm_util_nearest_long(2.0 * lr_data->z_NavPos.f_VelEnu[2]);
      }
  
      vel_east = lr_data->z_NavPos.f_VelEnu[0];
      vel_north = lr_data->z_NavPos.f_VelEnu[1];
      f_tmp = sqrt(vel_east * vel_east + vel_north * vel_north);
      
      if (f_tmp < TM_CORE_MIN_VELOCTY)
      {
        velocity_hor = 0;
      }
      else
      {
        velocity_hor =(uint16)tm_util_nearest_long(4.0 * f_tmp);
      }
     
      pz_PositionReportIndMsg->speedHorizontal_valid = TRUE;
      pz_PositionReportIndMsg->speedVertical_valid = TRUE;
	  pz_PositionReportIndMsg->speedUnc_valid = TRUE;
      pz_PositionReportIndMsg->speedHorizontal   = ((float) velocity_hor)/4;
      pz_PositionReportIndMsg->speedVertical  = ((float) velocity_ver)/2;

      pz_PositionReportIndMsg->magneticDeviation_valid = TRUE;      
      pz_PositionReportIndMsg->magneticDeviation = ((float) (lr_data->z_NavPos.f_MagDeviation * 10.0 + 0.5)) / 10;
      
      pz_PositionReportIndMsg->velEnu_valid = TRUE;
      memscpy(pz_PositionReportIndMsg->velEnu,sizeof(pz_PositionReportIndMsg->velEnu),
      lr_data->z_NavPos.f_VelEnu, sizeof(lr_data->z_NavPos.f_VelEnu ));

      pz_PositionReportIndMsg->velUncEnu_valid = TRUE;
     /*Copy lr_data->z_NavPos.f_VuncEastNorthMps on the first two index of pz_PositionReportIndMsg->velUncEnu*/
      memscpy(pz_PositionReportIndMsg->velUncEnu,sizeof(lr_data->z_NavPos.f_VuncEastNorthMps),
            lr_data->z_NavPos.f_VuncEastNorthMps, sizeof(lr_data->z_NavPos.f_VuncEastNorthMps));
      pz_PositionReportIndMsg->velUncEnu[2] = lr_data->z_NavPos.f_VuncVertMps;

     /*heading calculation specific to final fix*/
      heading = (d_Azimuth * 10.0 + 0.5) / 10; 
    }
  }

  /***************************** GNSS SV used in fix ***************************************************************/
  if (lr_data->z_NavPos.z_SmPosFlags.b_IsSatellite)
  {
    tm_util_xlate_gnssNavSvInfo_to_qmi(pz_PositionReportIndMsg, (pdsm_lite_GnssNavSvInfo *)&(lr_data->z_NavSvInfo));
  }

  /*****************************Longitude lattitude translation*****************************************************************/

  if ( (is_best_avail_fix && lr_data->z_NavPos.z_PosFlags.b_IsValid) || (!is_best_avail_fix))
  {
    // For GNSS intermediate fix and Final fix , PDSM_PD_LAT_LONG_VALID is always true, no check on b_IsValid
  pz_PositionReportIndMsg->latitude_valid = TRUE;
  pz_PositionReportIndMsg->latitude = latitude;

  pz_PositionReportIndMsg->longitude_valid = TRUE;
  pz_PositionReportIndMsg->longitude = longitude;

  }

  /*****************************GPS and UTC time********************************************************************************/


  if ( ((PRTL_SESS_INFO_NOFIX == orig_sess_info_type) && (lr_data->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid) && (lr_data->z_NavPos.w_GpsWeek != C_GPS_WEEK_UNKNOWN)) || 
       ((PRTL_SESS_INFO_INTERM_LR == orig_sess_info_type) && lr_data->z_NavPos.w_GpsWeek != C_GPS_WEEK_UNKNOWN) ||
       ((PRTL_SESS_INFO_LR == orig_sess_info_type) && (lr_data->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid))
     )
     /*Time validity condition is added to final fix and it is specific only to GTP server fix that satifsfies HePe criteria, to avoid special case of GTP WWAN based fix reported as final fix with invalid time*/
  {
    // PDSM_PD_GPS_TIME_VALID,PDSM_PD_UTC_TIME_VALID,PDSM_PD_MSEC_VALID are always true for GNSS intermediate and final fixes
    
  /*Populate GPS time*/
  pz_PositionReportIndMsg->gpsTime.gpsWeek = lr_data->z_NavPos.w_GpsWeek;
  pz_PositionReportIndMsg->gpsTime.gpsTimeOfWeekMs = lr_data->z_NavPos.q_GpsTimeMs;
  pz_PositionReportIndMsg->gpsTime_valid = TRUE;

  /*populate UTC offset*/
  sm_GetGpsUtcOffset((uint8 *)&(pz_PositionReportIndMsg->leapSeconds)); //GPS_UTC_OFFSET;
  pz_PositionReportIndMsg->leapSeconds_valid = TRUE;
 
  /*Populate UTC time*/ 
  gnss_ConvertGpstoUtcMsTime(&(pz_PositionReportIndMsg->timestampUtc),
                              (pz_PositionReportIndMsg->leapSeconds),
                              (pz_PositionReportIndMsg->gpsTime.gpsWeek),
                              (pz_PositionReportIndMsg->gpsTime.gpsTimeOfWeekMs));
  pz_PositionReportIndMsg->timestampUtc_valid = TRUE;

  }

  /*****************************Altitude translation****************************************************************************/

  if((is_best_avail_fix && lr_data->z_NavPos.z_PosFlags.b_Is3D) || (!is_best_avail_fix) )
  {
     //To fix later in case of GNSS intermediate and final fix, should there be dependcy on b_Is3d or unconditionally set
  pd_event |= PDSM_LITE_PD_EVENT_HEIGHT;
  pz_PositionReportIndMsg->altitudeWrtEllipsoid_valid = TRUE;


      if(is_best_avail_fix)
      {
        pz_PositionReportIndMsg->altitudeWrtEllipsoid = 0.0;
      }
      else
      {
  pz_PositionReportIndMsg->altitudeWrtEllipsoid = lr_data->z_NavPos.d_PosLla[2];
      }



    if((is_best_avail_fix && lr_data->z_NavPos.z_PosFlags.b_IsValid) || (!is_best_avail_fix))
    {

  pz_PositionReportIndMsg->altitudeWrtMeanSeaLevel_valid = TRUE;
  pz_PositionReportIndMsg->altitudeWrtMeanSeaLevel = pz_PositionReportIndMsg->altitudeWrtEllipsoid -
               tm_util_get_geoidal_separation (pz_PositionReportIndMsg->latitude, pz_PositionReportIndMsg->longitude);

    }

  }
  
  /*****************************Velocity translation*************************************************************************/

   if(lr_data->z_NavPos.z_PosFlags.b_IsVelValid)
   {
   //final report , no need to check this flag
     float horizVel = (float) sqrt(SQR(lr_data->z_NavPos.f_VelEnu[0]) + SQR(lr_data->z_NavPos.f_VelEnu[1])); 
     float vertVel =  (float) lr_data->z_NavPos.f_VelEnu[2];
     float horizVeltUnc = (FLT)(sqrt(lr_data->z_NavPos.f_VuncEastNorthMps[0] *
                                     lr_data->z_NavPos.f_VuncEastNorthMps[0] +
                                     lr_data->z_NavPos.f_VuncEastNorthMps[1] *
                                     lr_data->z_NavPos.f_VuncEastNorthMps[1]));
     float vertVelUnc =  (FLT)(lr_data->z_NavPos.f_VuncVertMps);

     if(horizVel*100 < (float)(TM_CORE_MIN_VELOCTY))
     {
       horizVel = 0.0;
     }
     if(horizVeltUnc * 100 > (float)CGPS_MAX_UINT16)
     {
       horizVeltUnc = (float)CGPS_MAX_UINT16/100;
     }

     if(vertVelUnc  * 100 > (float)CGPS_MAX_UINT16 )
     {
       vertVelUnc = (float)CGPS_MAX_UINT16/100;
     }

      // Do not set valid mask for intermediate positions
     pz_PositionReportIndMsg->speedHorizontal  = horizVel;  /* These are in m/s */
     pz_PositionReportIndMsg->speedVertical  = vertVel;   /* These are in m/s */
                  
     pz_PositionReportIndMsg->speedUnc = sqrt(
            ((float) horizVeltUnc) * horizVeltUnc +
            ((float) vertVelUnc) * vertVelUnc ); /* These are in m/s */

   }

  /******************************Heading Valid ************************************************************************************/
   if(lr_data->z_NavPos.z_PosFlags.b_IsHeadingValid)
   {
   //This check is same for all type of position report
   
     if (pz_PositionReportIndMsg->speedHorizontal >= 0.1)
     {
       pz_PositionReportIndMsg->heading_valid = TRUE;
       pz_PositionReportIndMsg->heading = heading; 
       pz_PositionReportIndMsg->headingUnc_valid = TRUE;
       pz_PositionReportIndMsg->headingUnc = lr_data->z_NavPos.f_HeadingUncRad * RAD2DEG;
     }
   }

  /*****************************************************************************************************************************/

   // Is always true regardless of fix type .
   pz_PositionReportIndMsg->horUncEllipseSemiMajor_valid = TRUE;
   pz_PositionReportIndMsg->horUncEllipseSemiMinor_valid = TRUE;

   if (f_alongAxisUnc > CGPS_WHOLE_EARTH_UNC)
  {
    f_alongAxisUnc = CGPS_WHOLE_EARTH_UNC;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Error Ellipse along greater than earth radius", 0, 0, 0);
  }
  if (f_perpAxisUnc > CGPS_WHOLE_EARTH_UNC)
  {
    f_perpAxisUnc = CGPS_WHOLE_EARTH_UNC;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Error Ellipse perpendicular greater than earth radius", 0, 0, 0);
  }
   
   if((is_best_avail_fix && lr_data->z_NavPos.z_SmPosFlags.b_IsPuncValid) || (!is_best_avail_fix))
   {   

       pz_PositionReportIndMsg->horUncEllipseOrientAzimuth_valid = TRUE;
       pz_PositionReportIndMsg->horUncCircular_valid = TRUE;
       pz_PositionReportIndMsg->vertUnc_valid = TRUE;
       pz_PositionReportIndMsg->horConfidence_valid = TRUE;

       
   if(uncertainty_a >= uncertainty_p)
   {
      pz_PositionReportIndMsg->horUncEllipseSemiMajor = f_alongAxisUnc; /* meters */
      pz_PositionReportIndMsg->horUncEllipseSemiMinor = f_perpAxisUnc;  /* meters */
      pz_PositionReportIndMsg->horUncEllipseOrientAzimuth = loc_uncrtnty_ang * 5.625;
   }
   else
   {
      pz_PositionReportIndMsg->horUncEllipseSemiMinor = f_alongAxisUnc; /* meters */
      pz_PositionReportIndMsg->horUncEllipseSemiMajor = f_perpAxisUnc;  /* meters */
      pz_PositionReportIndMsg->horUncEllipseOrientAzimuth = loc_uncrtnty_ang * 5.625 + 90;
   }
   pz_PositionReportIndMsg->horUncCircular =  sqrt((f_alongAxisUnc *f_alongAxisUnc) 
                                                                          + (f_perpAxisUnc *f_perpAxisUnc)); /* meters */
   // Vertical uncertainty
   if(UNCERTAINTY_TABLE_SIZE > tm_util_select_pos_unc(lr_data->z_NavPos.f_PuncVertMeters))
   {
     pz_PositionReportIndMsg->vertUnc = lr_data->z_NavPos.f_PuncVertMeters;
   }
   else
   {
     MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Vertical Punc outside UNC table index : %d , Dont populate VetUnc",
                                              tm_util_select_pos_unc(lr_data->z_NavPos.f_PuncVertMeters));
   }
   // Horizontal confidence
   pz_PositionReportIndMsg->horConfidence = LOC_UNC_CONFIDENCE;


  }
  else
  {
      pz_PositionReportIndMsg->horUncEllipseSemiMajor =
         pz_PositionReportIndMsg->horUncEllipseSemiMinor =
          INV_SQRT_2 * (sqrt((f_alongAxisUnc *f_alongAxisUnc)+ (f_perpAxisUnc *f_perpAxisUnc)));

  }
   
  /*****************************************************************************************************************************/
   tm_util_xlate_nav_soln_to_qmi_pos_info(pz_PositionReportIndMsg, &(lr_data->z_NavPos));
   if(0 != pz_PositionReportIndMsg->technologyMask)
   {
       pz_PositionReportIndMsg->technologyMask_valid = TRUE;
   }

   pz_PositionReportIndMsg->altitudeAssumed_valid = TRUE;
   if(TRUE == lr_data->z_NavPos.z_PosFlags.b_Is3D)
   {
     pz_PositionReportIndMsg->altitudeAssumed = FALSE;
   }
   else
   {
     pz_PositionReportIndMsg->altitudeAssumed = TRUE;
   }
    

   pz_PositionReportIndMsg->horReliability_valid = tm_util_xlate_nav_reliability_to_qmi(
                                                    &(pz_PositionReportIndMsg->horReliability),
                                                    (pdsm_lite_pd_reliability_e_type)lr_data->z_NavPos.e_HoriRelIndicator);
   pz_PositionReportIndMsg->vertReliability_valid = tm_util_xlate_nav_reliability_to_qmi(
                                                      &(pz_PositionReportIndMsg->vertReliability),
                                                      (pdsm_lite_pd_reliability_e_type)lr_data->z_NavPos.e_VertRelIndicator); 
   pz_PositionReportIndMsg->timeSrc_valid =
               tm_util_xlate_timeSrc_to_qmi(&(pz_PositionReportIndMsg->timeSrc), (pdsm_lite_extended_time_src_e_type)lr_data->z_NavPos.u_InitGpsTimeSrc);
   
   if((is_best_avail_fix && lr_data->z_NavPos.z_PosFlags.b_IsValid) || (!is_best_avail_fix)) 
   {
     //PDSM_PD_LAT_LONG_VALID always true for non best_avail_fix
    pz_PositionReportIndMsg->DOP_valid = TRUE;
    pz_PositionReportIndMsg->DOP.PDOP = (float)(lr_data->z_NavPos.f_PDOP * 10.0 + 0.5) / 10.0f;
    pz_PositionReportIndMsg->DOP.HDOP = (float)(lr_data->z_NavPos.f_HDOP * 10.0 + 0.5) / 10.0f;
    pz_PositionReportIndMsg->DOP.VDOP = (float)(lr_data->z_NavPos.f_VDOP * 10.0 + 0.5) / 10.0f;
    
    pz_PositionReportIndMsg->extDOP_valid = TRUE;
    pz_PositionReportIndMsg->extDOP.PDOP = pz_PositionReportIndMsg->DOP.PDOP;
    pz_PositionReportIndMsg->extDOP.HDOP = pz_PositionReportIndMsg->DOP.HDOP;
    pz_PositionReportIndMsg->extDOP.VDOP = pz_PositionReportIndMsg->DOP.VDOP;
    pz_PositionReportIndMsg->extDOP.TDOP = (float)(lr_data->z_NavPos.f_TDOP * 10.0 + 0.5) / 10.0f;
    pz_PositionReportIndMsg->extDOP.GDOP = (float)(lr_data->z_NavPos.f_GDOP * 10.0 + 0.5) / 10.0f;
  
   }
   
   return pd_event;
}

/*===========================================================================

FUNCTION tm_util_get_constellation_from_mcc()

DESCRIPTION
  This function provides the constellation supported based on the mcc of 
  serving cell..  

DEPENDENCIES

RETURN VALUE 
  secondary constellation id

SIDE EFFECTS

===========================================================================*/



void tm_util_get_constellation_from_mcc(uint32 mcc, uint32 *p_gnss_config, uint8 *p_constellation_id)
{
    uint8 idx  = 0;
    *p_constellation_id = (uint8)TM_GNSS_MAX_CONSTELL;
    if(mcc != TM_CM_INVALID_MCC)
    {
      while((mcc_constelaltion_map[idx].w_MCC!= mcc))
      { 
        idx++;
        if(idx>= MAX_MCC_ENTRIES)
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "MCC = %u not found in MCC_ID constellation map",mcc, 0, 0);
          break;
        }
      }
      if(idx < MAX_MCC_ENTRIES)
      {
        *p_constellation_id = (uint8)mcc_constelaltion_map[idx].gnns_contstellation;
      }
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "MCC.= %u sending constellation id = %u ",mcc ,*p_constellation_id, 0);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "INVALID MCC. sending constellation id = %u ",*p_constellation_id,0, 0);
    }
    
    switch (*p_constellation_id)
    {
      case TM_GNSS_GPS_CONSTELL:
        {
          *p_gnss_config = C_RCVR_GNSS_CONFIG_GPS_ENABLED;
          break;
        }
      case TM_GNSS_GLO_CONSTELL:
        {
          *p_gnss_config = C_RCVR_GNSS_CONFIG_GLO_ENABLED;
          break;
        }
      case TM_GNSS_BDS_CONSTELL:
        {
          *p_gnss_config = C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE;
          break;
        }
      case TM_GNSS_GAL_CONSTELL:
        {
          *p_gnss_config = C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE;
          break;
        }
      case TM_GNSS_QZSS_CONSTELL:
        {
          *p_gnss_config = C_RCVR_GNSS_CONFIG_QZSS_ENABLED_WORLDWIDE;
          break;
        }
      default:
          *p_gnss_config = 0;
      break;
    }
	/* If QZSS was not already enabled , below logic enables it */
	if( 0 == (*p_gnss_config & C_RCVR_GNSS_CONFIG_QZSS_ENABLED_WORLDWIDE))
	{
		*p_gnss_config |= C_RCVR_GNSS_CONFIG_QZSS_ENABLED_OUTSIDE_OF_US;
	}
    *p_gnss_config |= C_RCVR_GNSS_CONFIG_GPS_ENABLED;
    
    return; 
}

/*===========================================================================

FUNCTION tm_util_get_constellation_info_from_efs()

DESCRIPTION
  This function provides the constellation supported as secondary constellation 
  based on the configuration stored in EFS.

DEPENDENCIES

RETURN VALUE 
  secondary constellation id

SIDE EFFECTS

===========================================================================*/

void tm_util_get_constellation_info_from_efs(uint32  *p_gnss_config, uint8 * p_constellation_id)
{

  uint32 q_EfsNvGnssConfig = tm_core_info.efs_nv_gnss_config;

  *p_constellation_id = 0;
  *p_gnss_config = 0;
  
  if((q_EfsNvGnssConfig & TM_GNSS_GAL_ENABLE_MASK)&&
      (0 == (q_EfsNvGnssConfig & TM_GNSS_BDS_ENABLE_MASK)))
  {
    *p_constellation_id  = (uint8)TM_GNSS_GAL_CONSTELL;
    if(q_EfsNvGnssConfig & C_RCVR_GNSS_CONFIG_GAL_ENABLED_OUTSIDE_OF_US)
    {
      *p_gnss_config = C_RCVR_GNSS_CONFIG_GAL_ENABLED_OUTSIDE_OF_US;
    }
    else
    {
      *p_gnss_config = C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE;
    }
  }
  else if((q_EfsNvGnssConfig & TM_GNSS_BDS_ENABLE_MASK)&&
           (0 == (q_EfsNvGnssConfig & TM_GNSS_GAL_ENABLE_MASK)))
  {
  	*p_constellation_id  = (uint8)TM_GNSS_BDS_CONSTELL;
    if(q_EfsNvGnssConfig & C_RCVR_GNSS_CONFIG_BDS_ENABLED_OUTSIDE_OF_US)
    {
      *p_gnss_config = C_RCVR_GNSS_CONFIG_BDS_ENABLED_OUTSIDE_OF_US;
    }
    else
    {
     *p_gnss_config = C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE;
     }
  }
 /* This is the default block which will enable GLO as secondary constellation,
  will also handle all other suprious constellation configurations */
  else
  {
	*p_constellation_id = (uint8)TM_GNSS_GLO_CONSTELL;
	*p_gnss_config |= C_RCVR_GNSS_CONFIG_GLO_ENABLED;
  }
  
  /* THis if condition handles QZSS enablement in NV */
   if(q_EfsNvGnssConfig & TM_GNSS_QZSS_ENABLE_MASK)
	{
	  *p_gnss_config |= C_RCVR_GNSS_CONFIG_QZSS_ENABLED_OUTSIDE_OF_US;
	  if(q_EfsNvGnssConfig & C_RCVR_GNSS_CONFIG_QZSS_ENABLED_WORLDWIDE)
	  {
	    *p_gnss_config &= ~(C_RCVR_GNSS_CONFIG_QZSS_ENABLED_OUTSIDE_OF_US);
		*p_gnss_config |= C_RCVR_GNSS_CONFIG_QZSS_ENABLED_WORLDWIDE;
	  }
				 
	  if(0==(q_EfsNvGnssConfig & (TM_GNSS_GLO_ENABLE_MASK|TM_GNSS_GAL_ENABLE_MASK|TM_GNSS_BDS_ENABLE_MASK)))
	  {
	    *p_constellation_id  = (uint8)TM_GNSS_QZSS_CONSTELL;	 
		 /* Reached here, that means GPS+QZSS are only enabled in NV and below negation
		    will take of explicit disablement of GLO in config mask*/
		*p_gnss_config &= ~(C_RCVR_GNSS_CONFIG_GLO_ENABLED);
	  }
	}
  
  *p_gnss_config|= C_RCVR_GNSS_CONFIG_GPS_ENABLED;
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Read GNSS config NV.= %u sending constellation id = %u gnss_config = %u",q_EfsNvGnssConfig ,*p_constellation_id ,*p_gnss_config );
  
  return; 
}

/*===========================================================================

FUNCTION tm_util_update_constellation_on_mcc_change()

DESCRIPTION
  This function provides the constellation supported as secondary constellation 
  

DEPENDENCIES

RETURN VALUE 
  secondary constellation id

SIDE EFFECTS

===========================================================================*/

void tm_util_update_constellation_on_mcc_change(uint32 mcc)
{

  uint8 u_constellation_id = (uint8)TM_GNSS_GLO_CONSTELL;
  uint32 q_gnss_config = 0;

  if (!(tm_core_info.efs_nv_gnss_config & MCC_ENABLED))
    {
    return;
    }
  
  tm_util_get_constellation_from_mcc(mcc, &q_gnss_config, &u_constellation_id);
  if( TM_GNSS_MAX_CONSTELL <= u_constellation_id )
  {
    tm_util_get_constellation_info_from_efs(&q_gnss_config, &u_constellation_id);
  }
  
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Sending constellation id = %u gnss_config = %u",u_constellation_id ,q_gnss_config, 0);
  
  if(tm_core_info.u_sm_curr_serving_sec_gnss_constellation != u_constellation_id )
  {
    tm_core_info.u_sm_curr_serving_sec_gnss_constellation  = u_constellation_id ;
    tm_core_info.q_mgp_curr_gnss_constellation_config = q_gnss_config;
    /*Clear the flag since config and secondary constellation has been updated*/
    tm_core_info.v_is_update_config_needed = FALSE;
    tm_xtra_notify_updated_gnss_constellation(u_constellation_id );
}

  return;
}

void tm_util_set_nmea_type(void)
{
   TM_CORE_CONFIG_DD(nmea_output_format) = PDAPI_LITE_NMEA_DEBUG;
   return;
}

/*===========================================================================

FUNCTION tm_util_update_constellation_config_if_in_US

DESCRIPTION
  This function update the constellation config and constellation ID
  when receiver is in US.
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_util_update_constellation_config_if_in_US(boolean * v_is_config_update_needed)
{
   tm_core_info.v_is_update_config_needed = *v_is_config_update_needed;
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GLO v_is_update_config_needed=%d ",tm_core_info.v_is_update_config_needed );
}
