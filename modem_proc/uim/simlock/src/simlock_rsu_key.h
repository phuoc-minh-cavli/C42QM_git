#ifndef SIMLOCK_RSU_KEY_H
#define SIMLOCK_RSU_KEY_H
/*===========================================================================


            S I M   L O C K   R S U   K E Y   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/simlock/src/simlock_rsu_key.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/16   bcho    Postpone creation of simlock config files
01/18/16   stv     Remote simlock support
05/17/15   stv     Fix compiler errors
05/15/15   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if defined (FEATURE_SIMLOCK) && (defined (FEATURE_SIMLOCK_RSU) || defined (FEATURE_SIMLOCK_QC_REMOTE_LOCK))

#include "simlock.h"

/*=============================================================================

                   DATA DECLARATIONS

=============================================================================*/
#define SIMLOCK_RSU_KEY_MOD_LEN                32
#define SIMLOCK_RSU_ENCRYPTED_KEY_LEN          256

#define SIMLOCK_RSU_PUBKEY_MODULUS_LEN         256
#define SIMLOCK_RSU_PUBKEY_EXPONENT_LEN        3

typedef uint8  simlock_rsu_keymod_type[SIMLOCK_RSU_KEY_MOD_LEN];

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_PUBKEY_DATA_TYPE

   DESCRIPTION:
     This structure contains the public key data

     version          : File version
     modulus          : Modulus
     exponent         : Public exponent
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  uint8    version;
  uint8    modulus[SIMLOCK_RSU_PUBKEY_MODULUS_LEN];
  uint8    exponent[SIMLOCK_RSU_PUBKEY_EXPONENT_LEN];
} simlock_rsu_pubkey_data_type;


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION SIMLOCK_RSU_READ_AND_STORE_PUBKEY_FILE

DESCRIPTION
  This function reads the public key file from the EFS location and stores it
  in the SFS location. It also deletes file in the EFS location. This is
  expected to happen once at the first power-up

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_key_read_and_store_pubkey_file
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_READ_PUBKEY_DATA

DESCRIPTION
  This function reads the public key file and provides the data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_key_read_pubkey_data
(
  simlock_rsu_pubkey_data_type   *  pubkey_data_ptr
);

#ifdef FEATURE_SIMLOCK_RSU
/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_READ_KEYMOD_FILE

DESCRIPTION
  This function reads the modem key file and provides the key data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_key_read_keymod_file
(
  simlock_rsu_keymod_type     keymod_data
);

/*===========================================================================
FUNCTION SIMLOCK_RSU_KEYMOD_GENERATE_AND_ENCRYPT

DESCRIPTION
  This function generates the modem key for the RSU. It encrypts the key,
  with the public key provisioned in the device.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_keymod_generate_and_encrypt
(
  simlock_data_type          * encrypted_key_ptr
);
#endif /* FEATURE_SIMLOCK_RSU */

/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_CHECK_PUBKEY_FILE_IN_SFS

DESCRIPTION
  This function checks if the public key file is present in the
  SFS and is of the expected size

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_key_check_pubkey_file_in_sfs
(
  void
);

#endif /* FEATURE_SIMLOCK  && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */

#endif /* SIMLOCK_RSU_KEY_H */
