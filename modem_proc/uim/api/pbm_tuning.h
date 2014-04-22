#ifndef PBM_TUNING_H
#define PBM_TUNING_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER FUNCTIONS

GENERAL DESCRIPTION
  This file contains the constants that may be useful to customize
  for a particular phonebook implementation.

  Copyright (c) 2005-2008, 2009, 2016 byQUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/api/pbm_tuning.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   nr      ECC optimizations for IoE devices
10/14/09  krishnac Added support for reduced RAM usage
07/10/09  ashwanik Support for CSIM and Dual Sim Features
03/04/08   cvs     Increase record size for non-EFS enabled PB's
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
03/12/07   cvs     Featurize 4 PB set support
02/09/07   cvs     Support for 4 phonebook sets
11/01/06   cvs     multi pb set fixes
06/12/06   cvs     add feature for 200 entry PB
03/27/06   cvs     Direct featurization to create EFS phonebook
09/22/05   cvs     Initial Revision
===========================================================================*/


/* These control the number of records and record sizes for the EFS phonebooks */
  /* enable main EFS phonebook */
  #define PBM_MAX_SPEED_DIALS 99    /* Allow 99 EFS based Speed Dials. */
  #define PBM_MAX_EFS_REC_LEN 1500  /* Allow the records to become large */
  #define PBM_AVG_EFS_REC_LEN PBM_MAX_EFS_REC_LEN /* All records can be of max size,
                                   * used for allocating heap */
  #define PBM_MAX_FIELD_SIZE 256
  #define PBM_MAX_EFS_RECS     500   /* Allow 500 records */
  #define PBM_MAX_CALL_HISTORY 100
  
#define PBM_MAX_RECORD_SIZE (PBM_MAX_EFS_REC_LEN)

#define PBM_MAX_ADN_RECS                  251
#define PBM_MAX_FDN_RECS                  100

/* These control garbage collection */
#define PBM_MAX_EFS_DELETES       10  /* Number of deletes before doing garbage
                                         collection. */
#define PBM_MAX_DELETE_SIZE       1000 /* When converting old files, if they
                                          are too big, reading the entire
                                          file would take too long. If we try to
                                          do garbage collection with this many
                                          deletes, we return PBM_ERROR*/



/* The following structures are used by the phone book manager's best fit
 * memory allocation system.  The default mem_malloc schemes are not well
 * suited for PBM's allocation needs.  In particular, circular files can
 * cause fragmentation issues in the heap resulting in the need for large
 * areas of extra memory.  The PBM_MEM_HEADERS define is
 * helpful to ensure the memory is not corrupted, however, it causes an
 * additional 8K of memory needs.  These are especially useful during testing.
 */

#define PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT 10 /* max number of clients that can call new PBM session enum rec init simultaneoulsy */
/* Client list will include
ATCOP 
QMI - PBM
QCRIL
below mentioned clients are for fusion architectures 
ATCOP - 9K 
QMI-PBM - 9K
QCRIL - 9K
*/

#endif /* PBM_TUNING_H */
