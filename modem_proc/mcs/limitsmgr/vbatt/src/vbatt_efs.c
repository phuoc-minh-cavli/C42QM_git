/*!
  @file
  vbatt_efs.c

  @brief
  This file implements reading and looking up VBATT limits from EFS
*/

/*=============================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcs.mpss/5.1/limitsmgr/vbatt/src/vbatt_efs.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/31/16   btl     Add new EFS items: mode and generic vbatt records
05/29/15   tl      Initial revision

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "vbatt_i.h"
#include "vbatt_efs.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_translation.h"
#include "lmtsmgr_diag.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "queue.h"
#include "mcsprofile.h"

/*=============================================================================
                                   TYPEDEFS
=============================================================================*/

typedef struct
{
  q_link_type           link;
  vbatt_record_type     record;
} vbatt_queue_record_type;

/*=============================================================================
                              INTERNAL VARIABLES
=============================================================================*/

q_type vbatt_efs_records;
uint16 vbatt_efs_mode;

/*=============================================================================
                              INTERNAL FUNCTIONS
=============================================================================*/

/*=============================================================================

  FUNCTION:  vbatt_efs_read_mode

=============================================================================*/
/*!
    @brief
    Reads the VBATT records from the specified EFS file for the specified tech

    @param[in]  file    The file name to read
    @param[in]  tech    The technology corresponding to the file name

    @return
    None
*/
/*===========================================================================*/
static uint16 vbatt_efs_read_mode(const char *file)
{
  int32 fd;
  int result;
  uint16 mode = VBATT_OP_MODE_DEFAULT;
  /*-----------------------------------------------------------------------*/
  fd = efs_open(file, O_RDONLY, ALLPERMS);
  if(fd < 0)
  {
    VBATT_MSG_1(ERROR, "Error %d opening EFS VBATT mode", fd);
  }
  else
  {
    /* Read the VBATT operating mode number from EFS */
    result = efs_read(fd, &mode, sizeof(mode));
    if(result <= 0 || result != sizeof(mode) || mode >= VBATT_OP_MODE_MAX)
    {
      VBATT_MSG_1(ERROR, "Error %d reading EFS VBATT mode", efs_errno);
      efs_close(fd);
      mode = VBATT_OP_MODE_DEFAULT;
    }

    VBATT_MSG_1(HIGH, "Read EFS VBATT mode: %d", mode);
  }

  return mode;
}

/*=============================================================================

  FUNCTION:  vbatt_efs_add_record

=============================================================================*/
/*!
    @brief
    Reads the VBATT records from the specified EFS file for the specified tech

    @param[in]  efs_record  The packed-form record, read from EFS
    @param[in]  tech        The technology corresponding to the EFS file

    @return
    count of successful records put
*/
/*===========================================================================*/
static int vbatt_efs_enqueue_record(
  vbatt_efs_record_type *efs_record,
  cxm_tech_type          tech,
  cxm_lte_sub_tech_type  sub_tech
)
{
  int i;
  vbatt_queue_record_type *queue_record;
  /*-----------------------------------------------------------------------*/
  /* Allocate memory for the new record */
  queue_record =
    modem_mem_alloc(sizeof(vbatt_queue_record_type), MODEM_MEM_CLIENT_MCS);
  if(queue_record == NULL)
  {
    VBATT_MSG_2(HIGH, "Out of memory allocating %d bytes for tech %d",
               sizeof(vbatt_queue_record_type), tech);
    return 0;
  }

  q_link(queue_record, &queue_record->link);

  /* Copy the data from EFS to memory */
  if(tech == CXM_TECH_DFLT_INVLD)
  {
    queue_record->record.tech_band = 0;
    queue_record->record.time_hysteresis = VBATT_EFS_GEN_HYSTERESIS;
  }
  else
  {
    queue_record->record.tech_band = VBATT_EFS_DEFINE_TECH_BAND(tech, sub_tech, efs_record->band);
    queue_record->record.time_hysteresis = efs_record->time_hysteresis;
  }
  for(i = 0; i < VBATT_STAGES_MAX; i++)
  {
    queue_record->record.stages[i].tx_power_limit =
      efs_record->stages[i].tx_power_limit;
    queue_record->record.stages[i].voltage_up =
      efs_record->stages[i].voltage_up;
    queue_record->record.stages[i].voltage_down =
      efs_record->stages[i].voltage_down;
  }

  /* Insert the new record into the queue */
  q_put(&vbatt_efs_records, &queue_record->link);

  return 1;
}

/*=============================================================================

  FUNCTION:  vbatt_efs_read_generic_record

=============================================================================*/
/*!
    @brief
    Reads the VBATT records from the specified EFS file for the specified tech

    @param[in]  fd    The file handle

    @return
    None
*/
/*===========================================================================*/
static void vbatt_efs_read_generic_record(const char *file)
{
  int32                    fd;
  int                      result, count = 0;
  int16                    version;
  vbatt_efs_record_type    efs_record;
  vbatt_queue_record_type *queue_record;
  /*-----------------------------------------------------------------------*/
  fd = efs_open(file, O_RDONLY, ALLPERMS);
  if(fd < 0)
  {
    VBATT_MSG_1(ERROR, "Error %d opening EFS file", fd);
  }
  else
  {
    /* all good so far! */
    /* Read the EFS version number */
    result = efs_read(fd, &version, sizeof(version));
    if(result <= 0 || result != sizeof(version))
    {
      VBATT_MSG_1(ERROR, "Error %d reading EFS version", efs_errno);
    }
    else if(version != VBATT_EFS_GEN_VERSION)
    {
      VBATT_MSG_2(ERROR,
          "EFS version mismatch: Expected version %d read version %d",
          VBATT_EFS_GEN_VERSION, version);
    }
    else
    {
      /* all good so far! */
      /* Read the EFS records -- just stage information for generic version */
      result = efs_read(fd, &efs_record.stages[0], VBATT_EFS_GEN_SIZE);
      if(result <= 0 || result != VBATT_EFS_GEN_SIZE)
      {
        VBATT_MSG_1(HIGH, "EFS error %d while reading VBATT record",
            efs_errno);
      }
      else
      {
        /* put the record into the queue */
        result = vbatt_efs_enqueue_record(&efs_record, CXM_TECH_DFLT_INVLD, CXM_LTE_SUB_INVALID);
        count += result;
      }
    }
    efs_close(fd);
    VBATT_MSG_1(LOW, "Read %d VBATT record(s)", count);
  }
}

/*=============================================================================

  FUNCTION:  vbatt_efs_read_tech_records

=============================================================================*/
/*!
    @brief
    Reads the VBATT records from the specified EFS file for the specified tech

    @param[in]  file    The file name to read
    @param[in]  tech    The technology corresponding to the file name

    @return
    None
*/
/*===========================================================================*/
static void vbatt_efs_read_tech_records(
  const char *file,
  cxm_tech_type tech,
  cxm_lte_sub_tech_type sub_tech
)
{
  int32                    fd;
  int                      result, count = 0;
  int16                    version;
  vbatt_efs_record_type    efs_record;
  vbatt_queue_record_type *queue_record;
  /*-----------------------------------------------------------------------*/
  fd = efs_open(file, O_RDONLY, ALLPERMS);
  if(fd >= 0)
  {
    /* all good so far! */
    /* Read the EFS version number */
    result = efs_read(fd, &version, sizeof(version));
    if(result <= 0 || result != sizeof(version))
    {
      VBATT_MSG_1(ERROR, "Error %d reading EFS version", efs_errno);
    }
    else if(version != VBATT_EFS_DEF_VERSION)
    {
      VBATT_MSG_3(ERROR,
          "EFS version mismatch for tech %d: Expected version %d read version %d",
          tech, VBATT_EFS_DEF_VERSION, version);
    }
    else
    {
      /* all good so far! */
      /* Read the EFS records */
      for(;;)
      {
        /* Default implementation */
        result = efs_read(fd, &efs_record, sizeof(efs_record));

        /* Check to see if the read failed. If it did, then determine
         * why it failed. */
        if(result <= 0 || result != sizeof(efs_record))
        {
          VBATT_MSG_2(HIGH,
              "EFS error %d while reading VBATT record for tech %d",
              efs_errno, tech);
          break;
        }
        else
        {
          result = vbatt_efs_enqueue_record(&efs_record, tech, sub_tech);
          if(result)
          {
            ++count;
          }
          else
          {
            /* no more records to read */
            break;
          }
        }
      }
    }
    efs_close(fd);
    VBATT_MSG_2(LOW, "Read %d VBATT record(s) for tech %d", count, tech);
  }
}

/*=============================================================================

  FUNCTION:  vbatt_lookup_func

=============================================================================*/
/*!
    @brief
    Comparator function for vbatt_lookup() to look up a VBATT record in the
    queue by the specified band

    @param[in]  item_ptr        Pointer to the record, which is expected to be
                                of type vbatt_queue_record_type
    @param[in]  compare_val     Pointer to the band to compare the record to,
                                which is expected to be of type
                                sys_band_class_e_type

    @return
    Non-zero if item_ptr matches compare_val; zero otherwise
*/
/*===========================================================================*/
static int vbatt_lookup_func(void *item_ptr, void *compare_val)
{
  const vbatt_queue_record_type *record;
  const uint32 *compare_tech_band;

  ASSERT(item_ptr);
  ASSERT(compare_val);

  record = (const vbatt_queue_record_type *)item_ptr;
  compare_tech_band = (const uint32 *)compare_val;

  return (record->record.tech_band == *compare_tech_band);
}

/*=============================================================================

  FUNCTION:  vbatt_validate_record

=============================================================================*/
/*!
    @brief
    Checks the contents of the record to verify that it is internally
    consistent

    @param[in]  record  The vbatt record to validate

    @return
    TRUE if the record meets the validation criteria; FALSE otherwise
*/
/*===========================================================================*/
static boolean vbatt_validate_record(const vbatt_record_type *record)
{
  if(record->stages[0].voltage_down > record->stages[0].voltage_up)
  {
    VBATT_MSG_3(ERROR, "Tech band 0x%x: stage 1 voltage_down %d > voltage_up %d",
        record->tech_band,
        record->stages[0].voltage_down,
        record->stages[0].voltage_up);
    return FALSE;
  }

  if(record->stages[1].voltage_down > record->stages[1].voltage_up)
  {
    VBATT_MSG_3(ERROR, "Tech band 0x%x: stage 2 voltage_down %d > voltage_up %d",
        record->tech_band,
        record->stages[1].voltage_down,
        record->stages[1].voltage_up);
    return FALSE;
  }

  if(record->stages[1].voltage_up > record->stages[0].voltage_up)
  {
    VBATT_MSG_3(ERROR,
        "Tech band 0x%x: stage 2 voltage_up %d > stage 1 voltage_up %d",
        record->tech_band,
        record->stages[1].voltage_up,
        record->stages[0].voltage_up);
    return FALSE;
  }

  if(record->stages[1].voltage_down > record->stages[0].voltage_down)
  {
    VBATT_MSG_3(ERROR,
        "Tech band 0x%x: stage 2 voltage_down %d > stage 1 voltage_down %d",
        record->tech_band,
        record->stages[1].voltage_down,
        record->stages[0].voltage_down);
    return FALSE;
  }

  return TRUE;
}

/*=============================================================================
                               PUBLIC FUNCTIONS
=============================================================================*/

/*=============================================================================

  FUNCTION:  vbatt_efs_init

=============================================================================*/
/*!
    @brief
    Initializes this module's internal data structures and reads VBATT records
    from EFS for the supported techs LTE and GSM.

    @return
    None
*/
/*===========================================================================*/
void vbatt_efs_init(void)
{
  q_init(&vbatt_efs_records);

  /* Read mode from NV -- default to tech/band-specific implementation */
  vbatt_efs_mode = vbatt_efs_read_mode(LIMITSMGR_VBATT_MODE);

  /* Now read records from NV. EFS format depends on operating mode */
  switch(vbatt_efs_mode)
  {
    case VBATT_OP_MODE_GENERIC:
      /* Read vbatt records */
      vbatt_efs_read_generic_record(LIMITSMGR_VBATT_GENERIC_LIMIT);
      break;
    case VBATT_OP_MODE_DEFAULT:
    default:
      /* Read vbatt records for LTE/CATM1 */
      vbatt_efs_read_tech_records(LIMITSMGR_VBATT_LTE_CATM_LIMIT, CXM_TECH_LTE, CXM_LTE_SUB_CATM1);
      /* Read vbatt records for all GSM subscriptions */
      vbatt_efs_read_tech_records(LIMITSMGR_VBATT_GSM_LIMIT, CXM_TECH_GSM1, CXM_LTE_SUB_INVALID);
      /* Read vbatt records for LTE/NB1 */
      vbatt_efs_read_tech_records(LIMITSMGR_VBATT_LTE_NB1_LIMIT, CXM_TECH_LTE, CXM_LTE_SUB_NB1);
      break;
  }
}

/*=============================================================================

  FUNCTION:  vbatt_efs_deinit

=============================================================================*/
/*!
    @brief
    Frees all memory allocated by vbatt_efs_init().

    @return
    None
*/
/*===========================================================================*/
void vbatt_efs_deinit(void)
{
  q_link_type *link;

  /* Free all memory used by the queue */
  while((link = q_get(&vbatt_efs_records)))
  {
    modem_mem_free(link, MODEM_MEM_CLIENT_MCS);
  }

  q_destroy(&vbatt_efs_records);
}

/*=============================================================================

  FUNCTION:  vbatt_lookup

=============================================================================*/
/*!
    @brief
    Looks up a VBATT record for the specified band.

    @param[in]  band    The tech/band to look up

    @return
    A pointer to the VBATT record if the band could be found; NULL otherwise.
*/
/*===========================================================================*/
const vbatt_record_type * vbatt_lookup(cxm_tech_type tech,
                                       cxm_lte_sub_tech_type sub_tech,
                                       sys_band_class_e_type band)
{
  const vbatt_queue_record_type *queue_link;
  const vbatt_record_type *record = NULL;
  uint32 tech_band;

  MCSMARKER(VBATT_LOOKUP_I);

  if(vbatt_efs_mode == VBATT_OP_MODE_DEFAULT)
  {
    tech_band = VBATT_EFS_DEFINE_TECH_BAND(tech, sub_tech, band);

    /* default mode -- search for a record matching this band */
    queue_link = (const vbatt_queue_record_type *)
      q_linear_search(&vbatt_efs_records, vbatt_lookup_func, &tech_band);
  }
  else
  {
    /* In generic mode, there's only one record regardless of band/tech */
    queue_link = (const vbatt_queue_record_type *) q_check(&vbatt_efs_records);
  }

  if(queue_link)
  {
    if(vbatt_validate_record(&queue_link->record))
    {
      record = &queue_link->record;
    }
    VBATT_MSG_4(LOW, "Found record %x for tech %d sub_tech %d band %d",
                     record, (uint32)tech, (uint32)sub_tech, (uint32)band);
  }
  else
  {
    VBATT_MSG_3(HIGH, "Record for tech %d sub_tech %d band %d not found",
                      (uint32)tech, (uint32)sub_tech, (uint32)band);
  }

  MCSMARKER(VBATT_LOOKUP_O);

  return record;
}

