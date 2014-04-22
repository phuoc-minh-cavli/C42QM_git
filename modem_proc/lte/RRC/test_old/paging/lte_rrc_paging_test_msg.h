/*!
  @file lte_rrc_paging_test_msg.h

  @brief
  Holds message buffers for encoded Paging messages needed for unit tests.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/paging/lte_rrc_paging_test_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_PAGING_TEST_MSG_H
#define LTE_RRC_PAGING_TEST_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*
  Paging Test Message1
  Paging Record List, 1 UE Id, S-tmsi

  PagingEmpty:value PCCH-Message ::= 
  {
      message c1 : paging : 
      {
        pagingRecordList 
        {
          {
            ue-Identity s-TMSI : 
              {
                mmec '00010010'B,
                m-TMSI '00110100 01010110 01111000 10010000'B
              },
            cn-Domain ps,
          }
        }
      }
  }
*/
#define DECLARE_LTE_RRC_PAGING_TEST_MSG1 \
static const uint8 lte_rrc_paging_test_msg1[] = \
{ \
  0x40, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00 \
}


/*
  Paging Test Message2
  Paging Record List, 1 UE Id, imsi

  PagingImsi:value PCCH-Message ::= 
  {
      message c1 : paging : 
      {
        pagingRecordList 
        {
          {
            ue-Identity imsi : 
              {
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                0
              },
            cn-Domain ps,
          }
        }
      }
  }
*/
#define DECLARE_LTE_RRC_PAGING_TEST_MSG2 \
static const uint8 lte_rrc_paging_test_msg2[] = \
{ \
  0x40, 0x14, 0x12, 0x34, 0x56, 0x78, 0x90, 0x00 \
}


/*
  Paging Test Message3
  Paging Record List, 2 UE Id, imsi, s-tmsi

  PagingImsi:value PCCH-Message ::= 
  {
  message c1 : paging : 
      {
        pagingRecordList 
        {
          {
            ue-Identity s-TMSI : 
              {
                mmec '00010010'B,
                m-TMSI '00110100 01010110 01111000 10010000'B
              },
            cn-Domain ps
          },
          {
            ue-Identity imsi : 
              {
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                0
              },
            cn-Domain ps
          }
        },
        systemInfoModification true,
        etws-Indication true
      }
  }
*/
#define DECLARE_LTE_RRC_PAGING_TEST_MSG3 \
static const uint8 lte_rrc_paging_test_msg3[] = \
{ \
  0x40, 0x81, 0x23, 0x45, 0x67, 0x89, 0x01, 0x41, 0x23, 0x45, 0x67, 0x89, 0x00, \
}


/*
  Paging Test Message4
  Si Update notification

  SiUpdate:value PCCH-Message ::= 
  {
  message c1 : paging : 
      {
        systemInfoModification true
      }
  }

*/
#define DECLARE_LTE_RRC_PAGING_TEST_MSG4 \
static const uint8 lte_rrc_paging_test_msg4[] = \
{ \
  0x20 \
}


/*
  Paging Test Message5
  ETWS notification

  Etws:value PCCH-Message ::= 
  {
  message c1 : paging : 
      {
        etws-PrimaryNotificationIndication true
      }
  }

*/
#define DECLARE_LTE_RRC_PAGING_TEST_MSG5 \
static const uint8 lte_rrc_paging_test_msg5[] = \
{ \
  0x10 \
}


/*
  Paging Test Message6
  ETWS notification

  Etws:value PCCH-Message ::= 
  {
  message c1 : paging : 
      {
        systemInfoModification true,
        etws-PrimaryNotificationIndication true
      }
  }

*/
#define DECLARE_LTE_RRC_PAGING_TEST_MSG6 \
static const uint8 lte_rrc_paging_test_msg6[] = \
{ \
  0x30 \
}


/*
  Paging Test Message7
  ETWS notification

  PagingImsi:value PCCH-Message ::= 
  {
  message c1 : paging : 
      {
        pagingRecordList 
        {
          {
            ue-Identity s-TMSI : 
              {
                mmec '00010010'B,
                m-TMSI '00110100 01010110 01111000 10010000'B
              },
            cn-Domain ps
          },
          {
            ue-Identity imsi : 
              {
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                0
              },
            cn-Domain ps
          }
        },
        systemInfoModification true,
        etws-Indication true
      }
  }
*/
#define DECLARE_LTE_RRC_PAGING_TEST_MSG7 \
static const uint8 lte_rrc_paging_test_msg7[] =  \
{ \
  0x70, 0x81, 0x23, 0x45, 0x67, 0x89, 0x01, 0x41, 0x23, 0x45, 0x67, 0x89, 0x00, \
}



/*
  Paging Test CMAS
  CMAS notification

value PCCH-Message ::= 
{
  message c1 : paging : 
      {
        nonCriticalExtension 
        {
          nonCriticalExtension 
          {
            cmas-Indication-r9 true
          }
        }
      }
}

*/
#define DECLARE_LTE_RRC_PAGING_TEST_CMAS \
static const uint8 lte_rrc_paging_test_cmas[] = \
{ \
  0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
}

#define DECLARE_LTE_RRC_PAGING_TEST_CS \
static const uint8 lte_rrc_paging_test_msg_cs[] = \
{ \
  0x40, 0x81, 0x23, 0x45, 0x67, 0x89, 0x09, 0x41, 0x23, 0x45, 0x67, 0x89, 0x08 \
}

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* LTE_RRC_PAGING_TEST_MSG_H */
