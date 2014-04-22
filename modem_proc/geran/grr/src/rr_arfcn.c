/*==============================================================================

                             R R   A R F C N

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.

$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_arfcn.c#3 $

==============================================================================*/


/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#include "rr_seg_load.h"
#include "rr_arfcn.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "msg.h"

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/*==============================================================================
  Constants and Macros
==============================================================================*/

/**
 * Hashed values for PGSM 900 ARFCNs are in the range 0 - 123
 */
#define RR_ARFCN_HASH_PGSM_900_MIN \
  (0)

#define RR_ARFCN_HASH_PGSM_900_MAX \
  (RR_ARFCN_HASH_PGSM_900_MIN + RR_ARFCN_PGSM_900_COUNT)

/**
 * Hashed values for EGSM 900 (lower range) ARFCNs are in the range 124 -
 */
#define RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MIN \
  (RR_ARFCN_HASH_PGSM_900_MAX + 1)

#define RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MAX \
  (RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MIN + RR_ARFCN_EGSM_900_LOWER_RANGE_COUNT)

/**
 * Hashed values for Cellular 850 ARFCNs are in the following range:
 */
#define RR_ARFCN_HASH_CELL_850_MIN \
  (RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MAX + 1)

#define RR_ARFCN_HASH_CELL_850_MAX \
  (RR_ARFCN_HASH_CELL_850_MIN + RR_ARFCN_CELL_850_COUNT)

/**
 * Hashed values for DCS 1800 ARFCNs are in the following range:
 */
#define RR_ARFCN_HASH_DCS_1800_MIN \
  (RR_ARFCN_HASH_CELL_850_MAX + 1)

#define RR_ARFCN_HASH_DCS_1800_MAX \
  (RR_ARFCN_HASH_DCS_1800_MIN + RR_ARFCN_DCS_1800_COUNT)

/**
 * Hashed values for PCS 1900 ARFCNs are in the following range:
 */
#define RR_ARFCN_HASH_PCS_1900_MIN \
  (RR_ARFCN_HASH_DCS_1800_MAX + 1)

#define RR_ARFCN_HASH_PCS_1900_MAX \
  (RR_ARFCN_HASH_PCS_1900_MIN + RR_ARFCN_PCS_1900_COUNT)

/**
 * Hashed values for EGSM 900 (upper range) ARFCNs are in the following range:
 */
#define RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MIN \
  (RR_ARFCN_HASH_PCS_1900_MAX + 1)

#define RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MAX \
  (RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MIN + RR_ARFCN_EGSM_900_UPPER_RANGE_COUNT)

/* Offset added to PGSM 900 channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_PGSM_900_OFFSET \
  ((RR_ARFCN_HASH_PGSM_900_MIN - RR_ARFCN_PGSM_900_MIN))

/**
 * The following definitions are for offsets that are applied to channel
 * numbers in each band to calculate the hashed value of the ARFCN.
 *
 * Generally, the offset is calculated by taking the minimum hashed value for
 * the band (defined above) and subtracting the minimum valid channel number
 * for the band, so taking PGSM as an example, the minimum hashed value is
 * 0, and the minimum channel number is 1.  This gives an offset of -1, which
 * maps the PGSM channel numbers in to the range 0 - 123.
 */

/* Offset added to EGSM 900 (lower range) channel numbers to calculate their
hashed value. */
#define RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_OFFSET \
  ((RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MIN - RR_ARFCN_EGSM_900_LOWER_RANGE_MIN))

/* Offset added to DCS 1800 channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_DCS_1800_OFFSET \
  ((RR_ARFCN_HASH_DCS_1800_MIN - RR_ARFCN_DCS_1800_MIN))

/* Offset added to Cellular 850 channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_CELL_850_OFFSET \
  ((RR_ARFCN_HASH_CELL_850_MIN - RR_ARFCN_CELL_850_MIN))

/* Offset added to PCS 1900 channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_PCS_1900_OFFSET \
  ((RR_ARFCN_HASH_PCS_1900_MIN - RR_ARFCN_PCS_1900_MIN))

/* Offset added to EGSM 900 (upper range) channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_OFFSET \
  ((RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MIN - RR_ARFCN_EGSM_900_UPPER_RANGE_MIN))

/**
 * Offsets that should be added to channel numbers to calculate the frequency
 * index of a GSM ARFCN.  This is effectively a one-way hash as band information
 * (PGSM or EGSM) is lost.  This hashing allows an easy comparison of ARFCNs
 * in PGSM and EGSM to see if they correspond to the same frequency.
 *
 * The frequency indexes in the mapped space are arranged in ascending order
 * of absolute frequency.
 *
 * PGSM frequencies resolve to the same indexes as the corresponding EGSM
 * frequencies.
 *
 * These values map the supported ARFCNs as follows:
 *
 *               ARFCN RANGE     MAPPED VALUE   OFFSET
 * Cellular 850  128 -> 251      0 -> 123       -128
 * EGSM 900      975 -> 1023     124 -> 172     -851
 * PGSM 900      1 -> 124        174 -> 297     173
 * EGSM 900      0 -> 124        173 -> 297     173
 * DCS 1800      512 -> 885      298 -> 671     -214
 * PCS 1900      512 -> 810      672 -> 971     160
 */
#define RR_ARFCN_FREQ_INDEX_CELL_850_OFFSET             -128
#define RR_ARFCN_FREQ_INDEX_EGSM_900_UPPER_RANGE_OFFSET -851
#define RR_ARFCN_FREQ_INDEX_PGSM_900_OFFSET             173
#define RR_ARFCN_FREQ_INDEX_EGSM_900_LOWER_RANGE_OFFSET 173
#define RR_ARFCN_FREQ_INDEX_DCS_1800_OFFSET             -214
#define RR_ARFCN_FREQ_INDEX_PCS_1900_OFFSET             160

#define RR_MHZ                            (1000000UL)
#define RR_KHZ                            (1000UL)

/* See 3GPP 45.005 Table 2-2 */

/* Uplink base frequencies per band in Hertz. */
#define RR_UL_FREQUENCY_BASE_PGSM_900  (890000UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_EGSM_900  (890000UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_RGSM_900  (890000UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_DCS_1800 (1710200UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_PCS_1900 (1850200UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_GSM_450   (450600UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_GSM_480   (479000UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_GSM_850   (824200UL * RR_KHZ)

/* Downlink base frequencies per band in Hertz. */
#define RR_DL_FREQUENCY_BASE_PGSM_900 (RR_UL_FREQUENCY_BASE_PGSM_900 + (45UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_EGSM_900 (RR_UL_FREQUENCY_BASE_EGSM_900 + (45UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_RGSM_900 (RR_UL_FREQUENCY_BASE_RGSM_900 + (45UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_DCS_1800 (RR_UL_FREQUENCY_BASE_DCS_1800 + (95UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_PCS_1900 (RR_UL_FREQUENCY_BASE_PCS_1900 + (80UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_GSM_450  (RR_UL_FREQUENCY_BASE_GSM_450  + (10UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_GSM_480  (RR_UL_FREQUENCY_BASE_GSM_480  + (10UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_GSM_850  (RR_UL_FREQUENCY_BASE_GSM_850  + (45UL * RR_MHZ))

/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Variables
==============================================================================*/

#define RR_ARFCN_FREQ_BASE_ARRAY_MAX    4

#define RR_FREQ_LOWEST   ((double) 869.2)
#define RR_FREQ_HIGHEST  ((double) 1989.8)

static const double rr_arfcn_frequencies_base[] =
{
  /* index 0 - 123 */
  869.200000, /* Cellular 850 ARFCN 128 */

  /* index 124 - 297 */
  925.200000, /* EGSM 900 (upper range) ARFCN 975 */

  /* index 298 - 671 */
  1805.200000, /* DCS 1800 ARFCN 512 */

  /* index 672 - 970 */
  1930.200000, /* PCS 1900 ARFCN 512 */
};

static const uint16 rr_arfcn_frequencies_index[] =
{
  0,
  124,
  298,
  672,
  RR_ARFCN_FREQ_INDEX_MAX
};

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/


/**
 * See header file for description.
 */
double rr_arfcn_freq_index_to_freq(rr_arfcn_freq_index_t freq_index)
{
  double freq = 0;

  byte i;

  for (i=0; i<RR_ARFCN_FREQ_BASE_ARRAY_MAX; i++)
  {
    if (freq_index >= rr_arfcn_frequencies_index[i] && freq_index < rr_arfcn_frequencies_index[i+1])
    {
      freq = rr_arfcn_frequencies_base[i] + (freq_index - rr_arfcn_frequencies_index[i]) * 0.2;
      return freq;
    }
  }

  //MSG_ERROR("ARFCN frequency index %d invalid", freq_index, 0, 0);
  return freq;
}

rr_arfcn_freq_index_t rr_arfcn_freq_to_freq_index(double freq)
{
  rr_arfcn_freq_index_t index;
  double f;
  byte i;

  if (freq < RR_FREQ_LOWEST || freq > RR_FREQ_HIGHEST)
  {
    return RR_ARFCN_FREQ_INDEX_MAX;
  }

  for (i=0; i<RR_ARFCN_FREQ_BASE_ARRAY_MAX; i++)
  {
    for (index = rr_arfcn_frequencies_index[i]; index < rr_arfcn_frequencies_index[i+1]; index++)
    {
      f = rr_arfcn_frequencies_base[i] + 0.2 * (index - rr_arfcn_frequencies_index[i]);

      if (f > freq)
      {
        if (index > 0)
        {
          return index - 1;
        }
        else
        {
          return RR_ARFCN_FREQ_INDEX_MAX;
        }
      }
      else if (f == freq)
      {
        return index;
      }
    }
  }

  //MSG_ERROR("ARFCN frequency %d invalid", freq, 0, 0);
  return RR_ARFCN_FREQ_INDEX_MAX;
}

// UTF support

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

