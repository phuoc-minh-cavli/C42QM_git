/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/mminfo.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/14   mks     Created the file
07/08/03   jca     Now call function mm_convert_time_zone() instead of GMM function.
09/19/03   jca     mm_convert_time_zone() now compliant with 23.040 section 9.1.3.11.
03/11/04   ks      Fixed the IE_length to extract the correct NW Name. 
07/07/04   cd      Fixed Lint errors
07/07/04   cd      Corrected the length of the MMR_MM_INFORMATION_IND message
08/18/11   gps     Sending a STATUS message to the network indicating the Scemantic error, 
                   if the ie_length field is zero in IEI_NETWORK_FULL_NAME and IEI_NETWORK_SHORT_NAME.
08/18/11   gps     Checking index with actual message length rather than max_octets_l3_message
                    in all cases throughout function mm_handle_information and truncating ie length to
                    mm_max_network_name_len if it is long.
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if 0

#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include "customer.h"
#include "mminfo.h"
#include "reg_mm_v.h"
#include "gmm_msg.h"
#include "err.h"
#include "cause.h"
#include "bit.h"
#include "gmm_int_v.h"

/*===========================================================================

FUNCTION MM_CONVERT_SEMI_OCTETS

DESCRIPTION
  This function converts a byte into a semi-octet representation
  as per 3GPP TS 23.040 section 9.1.2.3.

DEPENDENCIES
  None

RETURN VALUE
  Semi-octet representation of input byte (bit stream).

SIDE EFFECTS
  None

===========================================================================*/
uint8 mm_convert_semi_octets( byte value )
{
   return (10 * (value & 0x0F)) + (value / 0x10);
}

/*===========================================================================

FUNCTION MM_CONVERT_TIME_ZONE

DESCRIPTION
  This function converts a byte into a time zone value
  as per 3GPP TS 23.040 sections 9.1.2.3 and 9.1.3.11.

DEPENDENCIES
  None

RETURN VALUE
  Time zone in semi-octet representation

SIDE EFFECTS
  None

===========================================================================*/
int mm_convert_time_zone( byte value )
{
   int time_zone = (10 * (value & 0x07)) + ((value & 0xF0) / 0x10);

   if (value & 0x08)
   {
      /* ---------------
      ** Sign bit is set
      ** --------------- */
     time_zone = -time_zone;
   }

   return time_zone;
}
#ifdef FEATURE_NAS_GW
#ifndef FEATURE_NAS_CS_DISABLED


/*===========================================================================

FUNCTION MM_HANDLE_INFORMATION

DESCRIPTION
  This function handles the MM INFORMATION message from the network

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_handle_information ( mm_cmd_type *msg_p )
{
  byte message_id   = msg_p->cmd.rr_data_ind.layer3_message[1];
  uint16 message_len = 0;

  GET_IMH_LEN(message_len, &msg_p->cmd.rr_data_ind.message_header);

  if ((msg_p->cmd.hdr.message_set != MS_MM_RR && msg_p->cmd.hdr.message_set != MS_MM_TDSRRC) ||
       message_id != MM_INFORMATION ||
       message_len < MM_INFORMATION_MSG_MIN_LENGTH )
  {
    if (msg_p->cmd.hdr.message_set != MS_MM_RR && msg_p->cmd.hdr.message_set != MS_MM_TDSRRC)
    {
      MSG_ERROR_DS( MM_SUB, "=MM= Unexpected Message Set (%d)", msg_p->cmd.hdr.message_set, 0, 0 );
    }
    else if ( message_id != MM_INFORMATION )
    {
      MSG_ERROR_DS( MM_SUB, "=MM= Unexpected Message Id (%d)", message_id, 0, 0 );
    }
    else if ( message_len < MM_INFORMATION_MSG_MIN_LENGTH )
    {
      MSG_ERROR_DS( MM_SUB, "=MM= Invalid MM_INFORMATION message", 0, 0, 0);
    }

    /* ---------------------------------------------------------
    ** Send a STATUS message to the network indicating the error
    ** --------------------------------------------------------- */
    mm_send_status ( INVALID_MANDATORY_INFORMATION );
  }
  else
  {
    uint16                         index = MM_INFORMATION_MSG_MIN_LENGTH;
    byte                           ie_length = 0;
    boolean                        valid_msg = TRUE;
    mmr_mm_information_ind_s_type  *mm_information;
    sys_mm_information_s_type      *info;

#ifndef FEATURE_MODEM_HEAP
    mm_information = (mmr_mm_information_ind_s_type*)gs_alloc(sizeof(mmr_mm_information_ind_s_type));
    info = (sys_mm_information_s_type *)gs_alloc(sizeof(sys_mm_information_s_type));
#else
    mm_information = (mmr_mm_information_ind_s_type*)modem_mem_calloc(1, sizeof(mmr_mm_information_ind_s_type), MODEM_MEM_CLIENT_NAS);
    info = (sys_mm_information_s_type *)modem_mem_calloc(1, sizeof(sys_mm_information_s_type), MODEM_MEM_CLIENT_NAS);
#endif

    mm_check_for_null_ptr((void*)mm_information);
    mm_check_for_null_ptr((void*)info);
   /* ----------------------------------------------
    **  Initialize the MM information data structure
    ** ---------------------------------------------- */
    memset(info,0,sizeof(sys_mm_information_s_type));
    info->plmn_avail = FALSE;
    info->full_name_avail = FALSE;
    info->full_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_NONE;
    info->short_name_avail = FALSE;
    info->short_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_NONE;
    info->univ_time_and_time_zone_avail = FALSE;
    info->time_zone_avail = FALSE;
    info->lsa_identity_avail = FALSE;
    info->daylight_saving_adj_avail = FALSE;
    info->rat = SYS_RAT_NONE;
    info->ciot_lte_mode = SYS_SYS_MODE_NONE;

    /* ------------------------------------------------
    ** Extract the MM Information IEs from the message
    ** ------------------------------------------------ */
    if(message_len <= gmm_max_L3_msg_length(MM_CS_DOMAIN))
    {
      while ( (index < message_len) && (valid_msg) )
      {
        switch (msg_p->cmd.rr_data_ind.layer3_message[index])
        {
 
          case IEI_NETWORK_FULL_NAME :

            /* --------------------------------------------------------------------
            ** Extract the Full Name of the network (Section 10.5.3.5a of TS24.008)
            ** -------------------------------------------------------------------- */
            if((index < message_len-1))
            {
              ie_length = MIN(msg_p->cmd.rr_data_ind.layer3_message[index+1] , MM_MAX_NETWORK_NAME_LEN);
            }
            else
            {
              MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
              valid_msg = FALSE;
              break;
            }

            if ( ie_length == 0) 
            {
              valid_msg = FALSE;
              MSG_HIGH_DS_1(MM_SUB ,"=MM= Unexpected length(NULL) %d",ie_length);
              break;
            }
            if((index < message_len-2))
            {
              info->full_name.spare_bits = (msg_p->cmd.rr_data_ind.layer3_message[index+2] & 0x07);
              info->full_name.add_country_initials = ((msg_p->cmd.rr_data_ind.layer3_message[index+2] & 0x08) == 0x08);
            }
            else
            {
              MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
              valid_msg = FALSE;
              break;
            }

            switch ( msg_p->cmd.rr_data_ind.layer3_message[index+2] & 0x70 )
            {
              case 0x00 :
                info->full_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET;
                break;

              case 0x10 :
                info->full_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_UCS2;
                break;

              default :
                MSG_ERROR_DS(MM_SUB, "=MM= Invalid Network Full Name IE", 0, 0, 0 );
                valid_msg = FALSE;
                break;
            }

            if ( valid_msg )
            {
              if(((index+1+ie_length) < message_len) && ((ie_length-1) <= sizeof(info->full_name.name)) )
              {
                memscpy(info->full_name.name, SYS_MAX_FULL_NAME_CHARS, &(msg_p->cmd.rr_data_ind.layer3_message[index+3]), (ie_length-1));
              }
              else
              {
                MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
                valid_msg = FALSE;
                break;
              }

              info->full_name.length = ie_length - 1;
              info->full_name_avail  = TRUE;
              index += msg_p->cmd.rr_data_ind.layer3_message[index+1] + 2;
            }

            break;

          case IEI_NETWORK_SHORT_NAME :

            /* ----------------------------------------------------------------------
            ** Extract the Short Name of the network (Section 10.5.3.5a of TS24.008)
            ** ---------------------------------------------------------------------- */
            if((index < message_len-1))
            {
              ie_length = MIN(msg_p->cmd.rr_data_ind.layer3_message[index+1] , MM_MAX_NETWORK_NAME_LEN);
            }
            else
            {
              MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
              valid_msg = FALSE;
              break;
            }

            if ( ie_length == 0) 
            {
              valid_msg = FALSE;
              MSG_HIGH_DS_1(MM_SUB ,"=MM= Unexpected length(NULL) %d",ie_length);
              break;
            }

            if((index < message_len-2))
            {

              info->short_name.spare_bits = (msg_p->cmd.rr_data_ind.layer3_message[index+2] & 0x07);

              info->short_name.add_country_initials = ((msg_p->cmd.rr_data_ind.layer3_message[index+2] & 0x08) == 0x08);

            }
            else
            {
              MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
              valid_msg = FALSE;
              break;
            }

            switch ( msg_p->cmd.rr_data_ind.layer3_message[index+2] & 0x70 )
            {
              case 0x00 :
                info->short_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET;
                break;

              case 0x10 :
                info->short_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_UCS2;
                break;

              default :
                MSG_ERROR_DS(MM_SUB, "=MM= Invalid Network Short Name IE", 0, 0, 0 );
                valid_msg = FALSE;
                break;
            }

            if ( valid_msg )
            {
              if(((index+1+ie_length) < message_len) && ((ie_length-1) <= sizeof(info->short_name.name)))
              {
                memscpy(info->short_name.name, SYS_MAX_SHORT_NAME_CHARS, &(msg_p->cmd.rr_data_ind.layer3_message[index+3]), (ie_length-1));
              }
              else
              {
                MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
                valid_msg = FALSE;
                break;
              }

              info->short_name.length = ie_length - 1;
              info->short_name_avail  = TRUE;
              index += msg_p->cmd.rr_data_ind.layer3_message[index+1] + 2;

            }

            break;

          case IEI_TIME_ZONE :

            /* -----------------------------------------------
            ** Extract the Time Zone information - the purpose
            ** of  this  information  is to  encode the offset
            ** between universal time & local time in steps of
            ** 15 minutes ( see section 10.5.3.8 of TS24.008 )
            ** ----------------------------------------------- */
            if(index < message_len-1)
            {
              info->time_zone = mm_convert_time_zone(
                                 msg_p->cmd.rr_data_ind.layer3_message[index+1] );
            }
            else
            {
              MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
              valid_msg = FALSE;
              break;
            }
            info->time_zone_avail = TRUE;
            index += 2;
            break;

          case IEI_TIME_ZONE_AND_TIME :

            /* -----------------------------------------------
            ** Extract the Time Zone & Time information - the
            ** purpose of time zone information is to  encode
            ** the offset between universal time & local time
            ** in  steps  of  15 minutes. The purpose of time
            ** part  of  this  information  is  to encode the
            ** universal time at which the information element
            ** may have been seen by the network( see section
            ** 10.5.3.9 of TS24.008 )
            ** ----------------------------------------------- */
            if((index < message_len-7))
            {
              info->univ_time_and_time_zone.year      =
                mm_convert_semi_octets( msg_p->cmd.rr_data_ind.layer3_message[index+1] );
              info->univ_time_and_time_zone.month     =
                mm_convert_semi_octets( msg_p->cmd.rr_data_ind.layer3_message[index+2] );
              info->univ_time_and_time_zone.day       =
                mm_convert_semi_octets( msg_p->cmd.rr_data_ind.layer3_message[index+3] );
              info->univ_time_and_time_zone.hour      =
                mm_convert_semi_octets( msg_p->cmd.rr_data_ind.layer3_message[index+4] );
              info->univ_time_and_time_zone.minute    =
                mm_convert_semi_octets( msg_p->cmd.rr_data_ind.layer3_message[index+5] );
              info->univ_time_and_time_zone.second    =
                mm_convert_semi_octets( msg_p->cmd.rr_data_ind.layer3_message[index+6]);
              info->univ_time_and_time_zone.time_zone =
                mm_convert_time_zone( msg_p->cmd.rr_data_ind.layer3_message[index+7] );
              info->univ_time_and_time_zone_avail     = TRUE;
            }
            else
            {
              MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
              valid_msg = FALSE;
              break;
            }
            index += 8;
            break;

          case IEI_LSA_ID :

            /* --------------------------------------------------
            ** Extract the Localized Service Area Identification -
            24.008 10.5.3.11: Octets 3-5 are coded as specified in 
            3GPP TS 23.003 [10], 'Identification of Localised Service Area'.
            Bit 8 of octet 3 is the most significant bit.

            23.003 Sec11:The LSA ID consists of 24 bits,
            numbered from 0 to 23, with bit 0 being the LSB. 
            Bit 0 indicates whether the LSA is a PLMN significant number 
            or a universal LSA. If the bit is set to 0 the LSA 
            is a PLMN significant number; if it is set to 1 it is a universal LSA.
            ** ------------------------------------------------- */
            if((index < message_len-1))
            {
              ie_length = msg_p->cmd.rr_data_ind.layer3_message[index+1];
            }
            else
            {
              MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
              valid_msg = FALSE;
              break;
            }


            if ( ie_length != MM_LSA_ID_SIZE && ie_length != 0 )
            {
              valid_msg = FALSE;
              break;
            }

            if((index < message_len-4))
            {
              /********************************************************** 
              * From section 10.5.3.11 of 24.008 If the Length = 0, 
              * then no LSA ID is included 
              ***********************************************************/
              if (ie_length != 0)
              {
                if (msg_p->cmd.rr_data_ind.layer3_message[index+4] & 0x01 )
                {
                  info->lsa_identity.type = SYS_LSA_ID_TYPE_UNIVERSAL;
                }
                else
                {
                  info->lsa_identity.type = SYS_LSA_ID_TYPE_PLMN_SIG_NUM;
                }
                info->lsa_identity.identity[0] = msg_p->cmd.rr_data_ind.layer3_message[index+4];
                info->lsa_identity.identity[1] = msg_p->cmd.rr_data_ind.layer3_message[index+3];
                info->lsa_identity.identity[2] = msg_p->cmd.rr_data_ind.layer3_message[index+2];
                info->lsa_identity_avail = TRUE;
              }
            }
            else
            {
              MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
              valid_msg = FALSE;
              break;
            }

            if ( ie_length == MM_LSA_ID_SIZE )
            {
              index += 5;
            }
            else
            {
              index += 2;
            }

            break;

          case IEI_DAYLIGHT_SAVING_TIME :

            /* ------------------------------------
            ** Extract the Daylight Savings Time
            ** ---------------------------------- */
            if((index < message_len-2))
            {
              switch ( msg_p->cmd.rr_data_ind.layer3_message[index+2] & 0x03 )
              {
                case 0:
                  info->daylight_saving_adj = SYS_DAY_SAV_ADJ_NO_ADJUSTMENT;
                  break;

                case 1:
                  info->daylight_saving_adj = SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR;
                  break;

                case 2:
                  info->daylight_saving_adj = SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR;
                  break;

                default:
                  MSG_ERROR_DS(MM_SUB, "=MM= Invalid Daylight Saving Adjustment IE", 0, 0, 0 );
                  valid_msg = FALSE;
                  break;
              }
            }
            else
            {
              MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
              valid_msg = FALSE;
              break;
            }

            if ( valid_msg )
            { 
              info->daylight_saving_adj_avail = TRUE;
              index += 3;
            }

            break;

          default :

            /* ------------------------------------------------------------
            ** Ignore unknown IE - increment index per TS 24.007 11.2.4
            ** ------------------------------------------------------------ */
            if ((msg_p->cmd.rr_data_ind.layer3_message[index] & 0x80) == 0x80)
            {
              /* --------------------------------------------------------------
              ** If bit 8 is 1, the IE is one octet long
              ** -------------------------------------------------------------- */
              index ++;
            }
            else
            {
              /* ---------------------------------------------------------------------
              ** If bit 8 is 0, it indicates that the following octet is a length octet
              ** --------------------------------------------------------------------- */
              if(index < (message_len-1))
              {
                index += msg_p->cmd.rr_data_ind.layer3_message[index+1] + 2;
              }
              else
              {
                MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
                valid_msg = FALSE;
              }
            }
            break;
        }
      }
    }
    else
    {
      MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
      valid_msg = FALSE;
    }
    if ( valid_msg )
    {
       /* --------------------------------------
       **  Send MMR_MM_INFORMATION_IND to REG
       ** -------------------------------------- */

       mm_information->message_header.message_set = MS_MM_REG;
       mm_information->message_header.message_id  = (int)MMR_MM_INFORMATION_IND;
       info->plmn_avail       = sys_plmn_id_is_valid ( mm_serving_plmn.info.plmn );
       info->plmn             = mm_serving_plmn.info.plmn;
       info->lac_avail        = ((mm_serving_plmn.lac.lac[0] != 0xFF) && (mm_serving_plmn.lac.lac[1] != 0xFF))?TRUE:FALSE;
       info->lac_id           = b_unpackw(mm_serving_plmn.lac.lac,0,16);
#ifdef FEATURE_CIOT
        if(mm_is_rat_lte(mm_serving_plmn.info.active_rat))
        {
            info->rat              = SYS_RAT_LTE_RADIO_ACCESS;
            info->ciot_lte_mode    = mm_convert_sys_rat_to_acq_mode(mm_serving_plmn.info.active_rat);
        }
        else
#endif  
        {
            info->rat              = mm_serving_plmn.info.active_rat;
        }


       memscpy((void *)&mm_information->info,sizeof(mm_information->info),
                       (void *)info,sizeof(sys_mm_information_s_type)) ;

       PUT_IMH_LEN( sizeof (mmr_mm_information_ind_s_type) - sizeof (IMH_T),
              &mm_information->message_header );

       MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMR_MM_INFORMATION_IND");

       mm_send_message ( (IMH_T *)mm_information, GS_QUEUE_REG );
    }
    else
    {
      /* ---------------------------------------------------------
      ** Send a STATUS message to the network indicating the error
      ** --------------------------------------------------------- */
      mm_send_status ( SEMANTICALLY_INCORRECT_MESSAGE );
    }
#ifndef FEATURE_MODEM_HEAP
    gs_free(info);
    gs_free(mm_information)
#else
    modem_mem_free(info, MODEM_MEM_CLIENT_NAS);
    modem_mem_free(mm_information, MODEM_MEM_CLIENT_NAS);
#endif

  }

}
#endif
#endif

#endif

