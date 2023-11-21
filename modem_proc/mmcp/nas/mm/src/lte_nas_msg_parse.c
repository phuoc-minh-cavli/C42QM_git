
/*==============================================================================

==============================================================================*/

/*Author: hnam
  Reference Document: 3GPP TS 24.301 V1.1.1 (2008-10)
  START DATE: 12/15/2008*/
/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/lte_nas_msg_parse.c#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when         who                          what                              
--------   --------  ------------------------------------------------
02/02/15   RI        Fixed the processing of SMC header.
03/16/10   hnam      Added a parameter to get_nas_sec_info()
01/21/10   zren      Added EMM/ESM STATUS feature
01/05/10   zren      Fixed OTA message buffer memory overflow
11/10/09   hnam      Added additional checks for UL/DL int and enc.
10/05/09   hnam      Added TLB error handler support
09/25/09   hnam      Added TLB support
08/28/09   hnam      Added support for secure EMM and ESM messages.
06/15/09   hnam      Added the code to support security with EMM 
05/17/09   hnam      Added the code to support security
04/30/09   hnam      Added a switch case to write message ID, for Independant ESM Message parsing
04/16/09   MNK       Enabled TAU processing again
04/15/09   hnam      Added support for multiple bearers.
04/10/09   hnam      Deleted authentication reject function being called in the code.
03/20/09   hnam      Restructured the entire code, moved all the code in here to other source files in MSG_LIB

04/07/09   hnam      Fix m_tmsi and guti's mme group id encoding and decoding
03/31/09   hnam      Added small code fix for CR 166518 (GUTI length was unrecognized previously), in ATTACH ACCEPT
03/25/09   hnam      Added a default case (error handler)for switch statement if ESM Message Id is incorrect. 
03/18/09   hnam      Changed all IEI values in decimal to hex and NAS_ESM_ACT_DEF_REQ_MIN_LEN value from 8 to 7
03/05/09   hnam      Added the code to parse Security mode cmd, security mode complete, 
                                            service request and service reject.
02/27/09   MNK       Added code to validate the mandatory paramters
02/26/09   MNK       Fix the processing of decoding of protocol config options
02/26/09   MNK       Remove parsing TAI IEI in attach accept
02/12/09   hnam      Modified the IMSI_ID encoding
02/05/09   MNK       Changed the buf_len_bits in decode_nas_msg to word
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "comdef.h"
#include "amssassert.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_esm_message_type.h"
#include "lte_nas_emm_message_description.h"
#include "msg_lib_encode_esm.h"
#include "msg_lib_encode_emm.h"
#include "msg_lib_decode_esm.h"
#include "msg_lib_decode_emm.h"
#include "msg_lib_declarations.h"
#include "err.h"
#include "lte.h"
#include "lte_nas_security_info.h"
#include "lte_log_codes.h"
#include "lte_nas_log.h"
#include "emm_database.h"
#include "event_defs.h"
#include <stringl/stringl.h>
#ifdef TEST_FRAMEWORK
#error code not present
#endif
#include "emm_mem.h" 

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
static byte security_hdr_type = 0;
static boolean decipher_success = FALSE;
static boolean integrity_check_passed = FALSE;
byte skip_sec_hdr = 6; /* PD + SEC (1) + MAC(4) + SQN(1)*/
static word orig_buffer_len;
word buf_len_bits = 0;
boolean msg_lib_encode_error;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
#define MIN_SEC_MSG_LEN 7
#define MIN_PLAIN_MSG_LEN 2
#endif
#define MIN_SMC_LEN 7

/*===========================================================================
                        FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      LTE_NAS_DECODE_SECURITY_HDR

DESCRIPTION
  This function decodes the security header information.

DEPENDENCIES
  None

RETURN VALUE
 TRUE if security header decoding is successful
 FALSE if security header decoding is unsuccessful
SIDE EFFECTS
  None
===========================================================================*/
boolean lte_nas_decode_security_hdr
(
  byte                          *src,
  word                          *pos,
  lte_nas_dl_security_info_type *dl_security_info,
  lte_nas_incoming_msg_type     *nas_incoming_msg_ptr
)
{
  byte first_octet = 8; /* PD (4 bits) + Security Hdr type (4 bits) */
  byte decipher_check_reqd = TRUE;
  boolean security_check_rqd = FALSE;
  boolean reserved_security = FALSE;
  byte index = 0;
  byte octet = 0;
  word num_bits_to_read = 0;
  byte pd = 0;
  byte message_id = 0;
  emm_state_type emm_state = EMM_INVALID_STATE;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  byte plain_msg_pd = 0;  //to store PD of the NAS msg in security protected NAS msg
  byte plain_msg_pd_offset = 52;  //plain msg PD is at 52 position from src
  //<PD + Sec HDR> <MAC1> <MAC2> <MAC3> <MAC4> <SQN> <PD + EPS bearer ID> <PTI>  <MSG ID>
  byte plain_msg_esm_msg_id_offset = 64;
  //<PD + Sec HDR> <MAC1> <MAC2> <MAC3> <MAC4> <SQN> <PD + Sec HDR> <MSG ID>
  byte plain_msg_emm_msg_id_offset = 56;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
  
  
  pd = nas_incoming_msg_ptr[nas_emm_index].nas_hdr.pd;

  if((pd == (byte)EPS_MOBILITY_MANAGEMENT_MESSAGES) ||
     (pd == (byte)RESERVED_FOR_TESTS_PROCEDURES))
  {
    /* Check for Security */
    num_bits_to_read = 4;
    security_hdr_type = (byte)b_unpackb(src, *pos, num_bits_to_read);
    buf_len_bits -= first_octet;
    *pos += first_octet;
  }
  else
  {
    if( dl_security_info != NULL )
    {
      /* Get Security Header Type */
      num_bits_to_read = 4;
      security_hdr_type = (byte)b_unpackb(src, *pos, num_bits_to_read);
      buf_len_bits -= first_octet;
      *pos += first_octet;
    }
  }

  switch(security_hdr_type)
  {
    case PLAIN_NAS_MSG:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if((orig_buffer_len/8) < MIN_PLAIN_MSG_LEN)
      {
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received PLAIN NAS MSG with len less than min %d bytes",
                       MIN_PLAIN_MSG_LEN);
        buf_len_bits = 0;
        total_num_messages = 0;
        report_emm_failure(nas_incoming_msg_ptr);
        return FALSE;
      }
#endif
      if(pd == (byte)RESERVED_FOR_TESTS_PROCEDURES)
      {
        MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= TLB cannot be a PLAIN NAS message");
        buf_len_bits = 0;
        total_num_messages = 0;
        report_emm_failure(nas_incoming_msg_ptr);
        return FALSE;
      }
      num_bits_to_read = 8;
      message_id = (byte)b_unpackb(src, *pos, num_bits_to_read);
      MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= MSG ID %d received as a PLAIN message", message_id);
      if(dl_security_info != NULL)
      {
        if(!dl_security_info->nas_connected_mode)
        {
          switch(message_id)
          {
          /*===================================================================================================
           Except the messages listed below, no NAS signalling messages shall be processed by the receiving EMM 
           entity in the UE or forwarded to the ESM entity, unless the secure exchange of NAS messages has been 
           established for the NAS signalling connection:
           Ref section: 4.4.4.2 of spec  24.301 (March version)
          ====================================================================================================*/
          case EMM_IDENTITY_REQUEST:
            /*if requested identification parameter is IMSI, then this Message is permitted to be a PLAIN NAS message*/
            if((LTE_NAS_EMM_IMEI == (mobile_id_type)b_unpackb(src, (word)(*pos+13), 3))||
               (LTE_NAS_EMM_IMEISV == (mobile_id_type)b_unpackb(src,(word)(*pos+13), 3)) ||
               (LTE_NAS_EMM_TMSI == (mobile_id_type)b_unpackb(src,(word)(*pos+13), 3)))
            {
              MSG_HIGH_DS_0(MM_SUB, "Received IDENTITY_REQ without IMSI as identity parameter, discarding this message");
              /*Messages received with incorrect security header shall be discarded*/
              total_num_messages  =  0;          
              buf_len_bits = 0;
              return FALSE;
            }
            MSG_HIGH_DS_1(MM_SUB, "NAS Received MSG_TYPE %d as a PLAIN NAS message and this is permissible",
                          message_id);
            break;
          case EMM_AUTHENTICATION_REQUEST:
          case EMM_AUTHENTICATION_REJECT:
          case DETACH_ACCEPT:
            MSG_HIGH_DS_1(MM_SUB, "NAS Received MSG_TYPE %d as a PLAIN NAS message and this is permissible",
                          message_id);
            break;

            case ATTACH_REJECT:
            case SERVICE_REJECT:
            case TRACKING_AREA_REJECT:
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
              /* 24.301 Section 4.4.4.2 states to discard the plain message if EMM Cause is #25 (Not authorized for this CSG)*/
              if(LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG == (lte_nas_emm_cause_type)b_unpackb(src,(word)(*pos+8), 8))
              {
                MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received plain Message ID %d with EMM Cause #25, discarding this message",
                              message_id);
                total_num_messages = 0;
                buf_len_bits = 0;
                return FALSE;
              }
#endif
              break;

            default:
              MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Valid security context is established in NAS, message with Message ID %d cannot be a PLAIN NAS message",
                             message_id);
              total_num_messages = 0;
              report_emm_failure(nas_incoming_msg_ptr);
              buf_len_bits = 0;
              return FALSE;
          } /* end switch(message_id) */
        } /* end if(!dl_security_info->nas_connected_mode) */
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Valid security context is established in NAS, message with Message ID %d cannot be a PLAIN NAS message",
                         message_id);
          /* Messages received with incorrect security header shall be discarded */
          total_num_messages = 0;
          buf_len_bits = 0;
          return FALSE;
        }
      } /* end if(dl_security_info != NULL) */
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= No valid Security context present");
        switch(message_id)
        {
          case EMM_IDENTITY_REQUEST:
            /* allow Identity request in EMC scenario */
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
            if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
            {
              /* TS 33.401, Section 5.1.1
               The UE shall not send IMEI or IMEISV to the network on a network request before the NAS security has been activated
              */		
              if((LTE_NAS_EMM_IMEI == (mobile_id_type)b_unpackb(src, (word)(*pos+13), 3))||
                 (LTE_NAS_EMM_IMEISV == (mobile_id_type)b_unpackb(src,(word)(*pos+13), 3)))
              {
                MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Ignore Identity Request for IMEI/IMEISV as no security context exists in UE");
                total_num_messages = 0;
                buf_len_bits = 0;
                return FALSE;
              }
            }
            break;

          case ATTACH_REJECT:
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
            /* 24.301 Section 4.4.4.2 states to discard the plain message if EMM Cause is #25 (Not authorized for this CSG)*/
            if(LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG == (lte_nas_emm_cause_type)b_unpackb(src,(word)(*pos+8), 8))
            {
              MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received plain Attach Reject message with EMM Cause #25, discarding this message",
                            message_id);
              total_num_messages = 0;
              buf_len_bits = 0;
              return FALSE;
            }
#endif
            break;

          case EMM_INFORMATION:
          case EMM_STATUS:
          case DETACH_REQUEST:
          case SECURITY_MODE_COMMAND:
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= No security context is established in NAS, message with Message ID %d cannot be a PLAIN NAS message",
                           message_id);
            total_num_messages = 0;
            buf_len_bits = 0;
            return FALSE;

          default:
            break;
        } /* end switch(message_id) */
      }

      if(pd ==(byte)EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_OTA_IN_MSG_LOG_C,(word)(orig_buffer_len/8),src);
      }
      else
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C,(word)(orig_buffer_len/8),src);
      }
      break;

    case INTEGRITY_PROTECT:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if((orig_buffer_len/8) < MIN_SEC_MSG_LEN)
      {
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received INTEGRITY PROTECT MSG < min length %d bytes",
                       MIN_SEC_MSG_LEN);
        buf_len_bits = 0;
        total_num_messages = 0;
        return FALSE;
      }
#endif
      //reading PD of NAS msg
      plain_msg_pd = (byte)b_unpackb(src, plain_msg_pd_offset, 4);
      if(plain_msg_pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        //if sec protected NAS msg contains EMM msg, then msg id offset is 56 from src
        message_id = (byte)b_unpackb(src, plain_msg_emm_msg_id_offset, 8);
      }
      else if(plain_msg_pd == EPS_SESSION_MANAGEMENT_MESSAGES)
      {
        //PD is ESM, if sec protected NAS msg contains ESM msg, then msg id offset is 64 from src
        message_id = (byte)b_unpackb(src, plain_msg_esm_msg_id_offset, 8);
      }
      if(message_id == SECURITY_MODE_COMMAND)
      {
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received SMC msg %d in INTEGRITY PROTECT header", message_id);
        report_emm_failure(nas_incoming_msg_ptr);
        total_num_messages = 0;
        buf_len_bits = 0;
        return FALSE;
      }
      emm_state = EMM_GET_STATE();

      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Rx INTEGRITY_PROTECT msg, emm_state = %d , nas_secure_exchange = %d ",
                    emm_state, emm_ctrl_data_ptr->nas_secure_exchange);

      /* TS 24.301: Section 4.4.5 Ciphering of NAS signalling messages
         Once the encryption of NAS messages has been started between the MME and the UE, the receiver
         shall discard the unciphered NAS messages which shall have been ciphered according
         to the rules described in this specification 
         Whenever a DL NAS message is received, during the function call emm_get_dl_security_info()
         nas_security->nas_connected_mode is assigned the value of emm_ctrl_data_ptr->nas_secure_exchange */
      if((dl_security_info != NULL) &&
         (dl_security_info->nas_connected_mode == TRUE))
      {
        MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= NAS secure exchange is established but NAS message is not ciphered. Discarding the message.");
        total_num_messages = 0;
        buf_len_bits = 0;
        return FALSE;
      }

      if((emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
         (emm_state == EMM_REGISTERED_INITIATED) ||
         (emm_state == EMM_DEREGISTERED_INITIATED))
      {
          /* 24.301, Section 4.4.2.3, Bullet 1 */
          /*   Process Integrity protected and Ciphered message and Set the Secure Exchange to TRUE */
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= nas_secure_exchange = %d, Set nas_secure_exchange to TRUE and process the INTEGRITY_PROTECT msg, in middle of Attach/TAU/Detach",
                        emm_ctrl_data_ptr->nas_secure_exchange);
          emm_ctrl_data_ptr->nas_secure_exchange = TRUE;
      }
      else if(emm_state != EMM_SERVICE_REQUEST_INITIATED)
      {
        /* Discard the Integrity protected and Ciphered message */
         if((dl_security_info == NULL) || (emm_ctrl_data_ptr->nas_secure_exchange == FALSE))
         {
           MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= NAS received INTEGRITY_PROTECT message when not in EMM_CONNECTED mode, discarding this message");
           total_num_messages = 0;
           return FALSE;
         }
      }

      security_check_rqd = TRUE;
      decipher_check_reqd = FALSE;
      if(pd ==(byte)EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_SEC_OTA_IN_MSG_LOG_C,(word)(orig_buffer_len/8),src);
      }
      else
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_SEC_OTA_IN_MSG_LOG_C,(word)(orig_buffer_len/8),src);
      }
      for(octet = 0; octet < 4; octet++)
      {
        num_bits_to_read = 8;
        if(dl_security_info != NULL)
        {
          dl_security_info->maci[octet] = b_unpackb(src, *pos, num_bits_to_read);
        }
        *pos = *pos + num_bits_to_read;
      }
       /*lint -e628 */
      integrity_check_passed = nas_msg_integrity_check(src,
                                                       pos,
                                                       &orig_buffer_len,
                                                       dl_security_info,
                                                       TRUE,
                                                       FALSE);

      if(integrity_check_passed == FALSE)
      {
        /* Report EMM Failure */
        MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Integrity check failed");
        nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = FALSE;
        total_num_messages = 0;
        return FALSE;
      }

      /*lint +e628 */
      num_bits_to_read = 32;
      buf_len_bits -= num_bits_to_read;
      /* Get the SQN Value */
      num_bits_to_read = 8;
      nas_sec_params.sqn = b_unpackb(src, *pos, num_bits_to_read);
      *pos += num_bits_to_read;
      /* PD is already read. Skip the first byte of EMM (PD + SECURITY HEADER TYPE) */
      *pos += num_bits_to_read;
      buf_len_bits -= num_bits_to_read;
      buf_len_bits -= num_bits_to_read;
      break;

    case INTEGRITY_PROTECT_SECURITY_MODE_CMD:
      if((orig_buffer_len/8) < MIN_SMC_LEN)
      {
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received INTEGRITY PROTECT SMC leng < min len %d bytes",
                       MIN_SMC_LEN);
        buf_len_bits = 0;
        total_num_messages = 0;
        return FALSE;
      }

      //reading PD of NAS msg
      plain_msg_pd = (byte)b_unpackb(src, plain_msg_pd_offset, 4);
      if(plain_msg_pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        //if sec protected NAS msg contains EMM msg, then msg id offset is 56 from src
        message_id = (byte)b_unpackb(src, plain_msg_emm_msg_id_offset, 8);
      }
      if((plain_msg_pd != EPS_MOBILITY_MANAGEMENT_MESSAGES)
         || (message_id != SECURITY_MODE_COMMAND))
      {
        //we have either received SMC header for non EMM PD msg, or SMC header for non SMC msg, 
        //or SMC len is less than minimum length
        MSG_ERROR_DS_3(MM_SUB, "=NAS_MSG_LIB= SMC header decode error, msg PD %d, msg ID %d, recvd SMC len %d",
                       plain_msg_pd,
                       message_id,
                       (orig_buffer_len/8));
        report_emm_failure(nas_incoming_msg_ptr);
        total_num_messages = 0;
        buf_len_bits = 0;
        return FALSE;
      }
      /* Get MACI Value*/
      if(pd ==(byte)EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_SEC_OTA_IN_MSG_LOG_C,(word)(orig_buffer_len/8),src);
      }
      else
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_SEC_OTA_IN_MSG_LOG_C,(word)(orig_buffer_len/8),src);
      }
      num_bits_to_read = 8;
      for(index = 0; index < sizeof(nas_sec_params.maci); index++)
      {
        nas_sec_params.maci[index] = b_unpackb(src, *pos, num_bits_to_read);
        *pos += num_bits_to_read;
        buf_len_bits -= num_bits_to_read;
      }
      /* Get the SQN Value */
      num_bits_to_read = 8;
      nas_sec_params.sqn = b_unpackb(src, *pos, num_bits_to_read);
      *pos += num_bits_to_read;
      buf_len_bits -= num_bits_to_read;
      /* PD is already read. Skip the first byte of EMM (PD + SECURITY HEADER TYPE) */
      *pos += num_bits_to_read;
      buf_len_bits -= num_bits_to_read;
      send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_OTA_IN_MSG_LOG_C,
                                  (word)((orig_buffer_len - skip_sec_hdr*8)/8),
                                  src + skip_sec_hdr);
      break;

    case INTEGRITY_PROTECT_CIPHER:
      emm_state = EMM_GET_STATE();
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Rx INTEGRITY_PROTECT_CIPHER msg, emm_state = %d , nas_secure_exchange = %d ",
                    emm_state, emm_ctrl_data_ptr->nas_secure_exchange);

      if((emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
         (emm_state == EMM_REGISTERED_INITIATED) ||
         (emm_state == EMM_DEREGISTERED_INITIATED))
      {
           /* 24.301, Section 4.4.2.3, Bullet 1 */
          /*   Process Integrity protected and Ciphered message and Set the Secure Exchange to TRUE */
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= nas_secure_exchange = %d, Set nas_secure_exchange to TRUE and process the INT_PROT_CIPHERED msg, in middle of Attach/TAU/Detach ",
                        emm_ctrl_data_ptr->nas_secure_exchange);
          emm_ctrl_data_ptr->nas_secure_exchange = TRUE;
      }
      else if(emm_state != EMM_SERVICE_REQUEST_INITIATED)
      {
        /* Discard the Integrity protected and Ciphered message */
         if((dl_security_info == NULL) || (emm_ctrl_data_ptr->nas_secure_exchange == FALSE))
         {
           MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= NAS received INTEGRITY_PROTECT_CIPHER message when not in EMM_CONNECTED mode, discarding this message");
           total_num_messages = 0;
           return FALSE;
         }
      }

#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if((orig_buffer_len/8) < MIN_SEC_MSG_LEN)
      {
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received INTEGRITY PROTECT CIPHER leng < min len %d bytes",
                       MIN_SEC_MSG_LEN);
        buf_len_bits = 0;
        total_num_messages = 0;
        return FALSE;
      }
#endif
      /*Once the security context is established and NAS connection is released, future ATTACH REQUEST
      messages will get ATTACH ACCEPT with INT+CIPHERING..so this clause needs to be accomodated,
      hence the 'OR' operator is used in the if() condition*/
      if(dl_security_info == NULL)
      {
        MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= NAS received INTEGRITY_PROTECT_CIPHER message when not in EMM_CONNECTED mode, discarding this message");
        total_num_messages = 0;
        return FALSE;
      }
      if(pd ==(byte)EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_SEC_OTA_IN_MSG_LOG_C, (word)(orig_buffer_len/8), src);
      }
      else
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_SEC_OTA_IN_MSG_LOG_C, (word)(orig_buffer_len/8), src);
      }
      security_check_rqd = TRUE;

      num_bits_to_read = 8;
      for(octet = 0; octet < 4; octet++)
      {
        dl_security_info->maci[octet] = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        buf_len_bits -= num_bits_to_read;
      }
      
      integrity_check_passed = nas_msg_integrity_check(src,
                                                       pos,
                                                       &orig_buffer_len,
                                                       dl_security_info,
                                                       TRUE,
                                                       FALSE);
      if(integrity_check_passed == TRUE)
      {
        decipher_success = decipher_nas_msg(src, &buf_len_bits, dl_security_info);
        if(decipher_success == FALSE)
        {
          MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Deciphering failed");
          total_num_messages = 0;
          return FALSE;
        }
      }
      else
      {
        MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Integrity check failed");
        nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = FALSE;
        total_num_messages = 0;
        return FALSE;
      }
      /* Get the SQN Value */
      num_bits_to_read = 8;
      nas_sec_params.sqn = b_unpackb(src, *pos, num_bits_to_read);
      *pos += num_bits_to_read;
      buf_len_bits -= num_bits_to_read;
      /* PD is already read. Skip the first byte of EMM (PD + SECURITY HEADER TYPE) */
      *pos += num_bits_to_read;
      buf_len_bits -= num_bits_to_read;
       //reading PD of NAS msg
      plain_msg_pd = (byte)b_unpackb(src, plain_msg_pd_offset, 4);
      if(plain_msg_pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        //if sec protected NAS msg contains EMM msg, then msg id offset is 56 from src
        message_id = (byte)b_unpackb(src, plain_msg_emm_msg_id_offset, 8);
      }
      else if(plain_msg_pd == EPS_SESSION_MANAGEMENT_MESSAGES)
      {
        //PD is ESM,  if sec protected NAS msg contains ESM msg, then msg id offset is 64 from src
        message_id = (byte)b_unpackb(src, plain_msg_esm_msg_id_offset, 8);
      }
      if(message_id == SECURITY_MODE_COMMAND)
      {
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received SMC msg %d in INTEGRITY PROTECT CIPHER header", message_id);
        report_emm_failure(nas_incoming_msg_ptr);
        total_num_messages = 0;
        buf_len_bits = 0;
        return FALSE;
      }
      break;

  case RESERVED_SECURITY:
      MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Reserved security header type is being sent, dropping this message");
      reserved_security = TRUE;
      total_num_messages = 0;
      break;

    default:
      MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= NAS message with security header type = %d received, dropping this message",
                     security_hdr_type);
      reserved_security = TRUE;
      total_num_messages = 0;
      break;
  } /* end switch(security_hdr_type) */

  if(reserved_security == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= reserved_security is TRUE");
    return FALSE;
  }

  return TRUE;
} /* end lte_nas_decode_security_hdr() */


/*===========================================================================
FUNCTION      LTE_NAS_DECODE_EMM_MESSAGE

DESCRIPTION
 This function decodes the incoming NAS message

DEPENDENCIES
  None

RETURN VALUE
  TRUE if message decoding is successful
  FALSE if message decoding is unsuccessful

SIDE EFFECTS
  None
===========================================================================*/
boolean lte_nas_decode_emm_message
(
  byte                      *src,
  word                      *pos,
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
)
{
  log_lte_nas_ota_in_msg_event_type event;
  word num_bits_to_read = 0;
  byte message_id = 0;
  boolean emm_exists = FALSE;
  buffer_length_check_failed = FALSE;

  /* Security Header */
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  if(buf_len_bits == 0)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Available buffer length is insufficient to decode NAS EMM Message header",
                   MIN_SEC_MSG_LEN);
    buf_len_bits = 0;
    total_num_messages = 0;
    return FALSE;
  }
#endif
  num_bits_to_read = 8;
  message_id = (byte)b_unpackb(src, *pos, num_bits_to_read);

  event.in_msg = message_id;
  event_report_payload(EVENT_LTE_EMM_OTA_INCOMING_MSG,sizeof(event), (void*)&event);
  nas_incoming_msg_ptr[nas_emm_index].nas_hdr.msg_id = (lte_nas_message_id)message_id;
  buf_len_bits -= num_bits_to_read;
  *pos += num_bits_to_read;

  /* Proceed with normal decoding */
  switch(message_id)
  {
    case ATTACH_ACCEPT:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.attach_accept.security_hdr =
                                          (lte_nas_security_hdr_type) security_hdr_type;
      emm_exists = decode_attach_accept(src,
                                        pos,
                                        &buf_len_bits,
                                        &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.attach_accept,
                                        emm_ctrl_data_ptr,
                                        nas_incoming_msg_ptr);
      total_num_messages = nas_esm_index + 1;
      if((emm_exists == TRUE) &&
         (report_invalid_guti_or_nas_security(nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.attach_accept.guti_type_exists,
                                              nas_incoming_msg_ptr,
                                              emm_ctrl_data_ptr) == FALSE))
      {
        emm_exists = FALSE;
      }
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= ATTACH ACCEPT decoding passed/Failed = %d security_hdr_type %d ",emm_exists,security_hdr_type);
      break;

    case ATTACH_REJECT: 
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.attach_rej.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_attach_reject(src,
                                        pos,
                                        &buf_len_bits,
                                        &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.attach_rej,
                                        nas_incoming_msg_ptr);
      total_num_messages = nas_esm_index + 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= ATTACH REJECT decoding Passed/Failed = %d security_hdr_type %d",emm_exists,security_hdr_type);

      break;

    case DETACH_REQUEST:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.mt_detach_req.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_detach_request(src,
                                         pos,
                                         &buf_len_bits,
                                         &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.mt_detach_req);
      total_num_messages = nas_esm_index + 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= DETACH REQUEST decoding PASSED(1)/FAILED(0) = %d security_hdr_type %d",emm_exists,security_hdr_type);
      break;

    case DETACH_ACCEPT:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.mt_detach_accept.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = TRUE;
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= DETACH ACCEPT(SHDR = %d) decoding passed security_hdr_type %d", emm_exists,security_hdr_type);
      break;

    case SECURITY_MODE_COMMAND:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.security_mode_cmd.security_hdr =
                                            (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_security_mode_cmd(src,
                                            pos,
                                            &buf_len_bits,
                                            &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.security_mode_cmd);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= SECURITY MODE COMMAND(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);
      break;

    case SERVICE_REJECT:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.service_rej.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_service_rej(src,
                                      pos,
                                      &buf_len_bits,
                                      &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.service_rej);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= SERVICE REJECT decoding passed/Failed %d",emm_exists);
      break;
#ifdef FEATURE_CIOT
    case SERVICE_ACCEPT:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= EMM Message ID --> SERVICE ACCEPT");
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.service_rej.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_service_accept(src,
                                           pos,
                                           &buf_len_bits,
                                           &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.service_accept);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= SERVICE REJECT(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);
      break;
#endif
    case EMM_AUTHENTICATION_REQUEST:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.auth_req.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_authentication_req(src,
                                             pos,
                                             &buf_len_bits,
                                             &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.auth_req);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= AUTHENTICATION REQUEST(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);
      break;

    case EMM_AUTHENTICATION_REJECT:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.auth_rej.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      total_num_messages = 1;
      emm_exists = TRUE;
      buf_len_bits = 0;
      MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= AUTHENTICATION REJECT(SHDR = %d) decoding passed", security_hdr_type);
      break;

    case TRACKING_AREA_UPDATE_ACCEPT:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.tau_accept.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_tau_accept(src,
                                     pos,
                                     &buf_len_bits,
                                     &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.tau_accept);
      total_num_messages = 1;
      if((emm_exists == TRUE) &&
         (report_invalid_guti_or_nas_security(nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.tau_accept.guti_type_exists,
                                              nas_incoming_msg_ptr,
                                              emm_ctrl_data_ptr) == FALSE))
      {
        emm_exists = FALSE;
      }
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= TRACKING AREA UPDATE ACCEPT(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);
      break;

    case TRACKING_AREA_REJECT:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.tau_rej.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_tau_reject(src,
                                     pos,
                                     &buf_len_bits,
                                     &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.tau_rej);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= TRACKING AREA UPDATE REJECT(SHDR = %d) decoding PASSED(1)/FAILED(0) %d ", security_hdr_type, emm_exists);
      break;

    case EMM_IDENTITY_REQUEST:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.identity_req.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_identity_req(src,
                                       pos,
                                       &buf_len_bits,
                                       &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.identity_req);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= IDENTITY REQUEST(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);

      break;

    case GUTI_REALLOCATION_COMMAND:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.guti_realloc_cmd.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_guti_realloc_cmd(src,
                                           pos,
                                           &buf_len_bits,
                                           &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.guti_realloc_cmd);
      total_num_messages = 1;
      /* GUTI is mandatory parameter in this message, since this message is decoded
         successfully GUTI must be valid */
      if((emm_exists == TRUE) &&
         (report_invalid_guti_or_nas_security(TRUE, nas_incoming_msg_ptr,
                                                emm_ctrl_data_ptr) == FALSE))
      {
        emm_exists = FALSE;
      }
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= GUTI REALLOCATION COMMAND(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);
      
      break;

    case EMM_INFORMATION:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.emm_info.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_emm_info(src,
                                   pos,
                                   &buf_len_bits,
                                   &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.emm_info);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= EMM INFORMATION(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);
      break;

    case EMM_STATUS:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.mt_emm_status.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_emm_status(src,
                                     pos,
                                     &buf_len_bits,
                                     &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.mt_emm_status);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= EMM STATUS(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);
      break;

    case DOWNLINK_NAS_TRANSPORT:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.dl_nas_transport.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_dl_nas_transport(src,
                                           pos,
                                           &buf_len_bits,
                                           &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.dl_nas_transport);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= DOWNLINK NAS TRANSPORT(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d ", security_hdr_type, emm_exists);
      break;

#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
   case CS_SERV_NOTIFICATION:
      nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.emm_info.security_hdr =
                                          (lte_nas_security_hdr_type)security_hdr_type;
      emm_exists = decode_cs_serv_notification(src,
                                               pos,
                                               &buf_len_bits,
                                               &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.cs_serv_notification);
      total_num_messages = 1;
      buf_len_bits = 0;
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= CS SERVICE NOTIFICATION(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);
      break;
#endif

#if defined (FEATURE_LTE_REL9)
#ifdef FEATURE_NAS_GPS_ENABLE

   case DL_GENERIC_NAS_TRANSPORT:
     if((emm_ctrl_data_ptr->emm_3gpp_spec_ver) >= LTE_3GPP_REL9)
     {
       nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.dl_generic_nas_transport_msg.security_hdr =
                                           (lte_nas_security_hdr_type)security_hdr_type;
       emm_exists = decode_dl_generic_nas_transport(src,
                                                    pos,
                                                    &buf_len_bits,
                                                    &nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.dl_generic_nas_transport_msg);
       total_num_messages = 1;
       buf_len_bits = 0;
       MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= DOWNLINK GENERIC NAS TRANSPORT(SHDR = %d) decoding PASSED(1)/FAILED(0) = %d", security_hdr_type, emm_exists);
     } 
     break;
#endif
#endif
   default:
     /* Report EMM Failure */
     MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown EMM Message ID = %d received", message_id);
     unknown_emm_msg_received(nas_incoming_msg_ptr);
     total_num_messages = 1;
     buf_len_bits = 0;
     break;
  } /* end switch(message_id) */

  if(emm_exists == TRUE)
  {
    report_emm_success(nas_incoming_msg_ptr);
  }
  else
  {
    /* Default value of hdr.valid is TRUE before decoding starts */
    if(nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid == TRUE)
    {
      /* If hdr.valid flag is FALSE, someone has set it to FALSE with a cause.
         Do not override the cause */
      report_emm_failure(nas_incoming_msg_ptr);
    }
  }

  return TRUE;
} /* end lte_nas_decode_emm_message() */


/*===========================================================================
FUNCTION      DECODE_NAS_MSG

DESCRIPTION
  This function decodes all the incoming NAS message and puts it into the 'C' structure

DEPENDENCIES
  None

RETURN VALUE
  byte - number of messages decoded

SIDE EFFECTS
  None
===========================================================================*/
byte decode_nas_msg
(
  byte                          *src,
  word                          buf_len,
  lte_nas_dl_security_info_type *dl_security_info,
  struct emm_ctrl_data_tag      *emm_ctrl_data_ptr,
  lte_nas_incoming_msg_type     *nas_incoming_msg_ptr
)
{
  lte_nas_dl_security_info_type nas_dl_sec_info;

  /* Reset all variables */
  word pos = 0;
  boolean emm_exists = FALSE, esm_exists = FALSE;
  byte message_id = 0;
  word num_bits_to_read = 0;
  byte pd = 0;
  word esm_len = 0;
  word esm_offset = 6;
  byte pd_offset = 4;
  buf_len_bits = buf_len*8;
  orig_buffer_len = buf_len_bits;
  nas_esm_index = 0;
  buffer_length_check_failed = FALSE;
  memset(&nas_dl_sec_info, 0, sizeof(lte_nas_dl_security_info_type));
  security_hdr_type = 0;
  decipher_success = FALSE;
  integrity_check_passed = FALSE;

  if(buf_len <= 0)
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Incoming buffer length to NAS Message library is NULL,returning 0 elements to EMM");
  }
  /* Reset incoming message structure */
  memset((void *)nas_incoming_msg_ptr, 0, sizeof(lte_nas_incoming_msg_type)*2);

  report_emm_success(nas_incoming_msg_ptr);
  while(buf_len_bits)
  {
    /* Protocol Discriminator */
    num_bits_to_read = 4;
    pd = b_unpackb(src, (word)(pos+pd_offset), num_bits_to_read);

    if((pd != (byte)EPS_MOBILITY_MANAGEMENT_MESSAGES)&&
       (pd != (byte)EPS_SESSION_MANAGEMENT_MESSAGES) &&
       (pd != (byte)RESERVED_FOR_TESTS_PROCEDURES))
    {
      MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received unknown PD = %d", pd);
      report_emm_failure(nas_incoming_msg_ptr);
      total_num_messages = 0;
      return total_num_messages;
    }
    nas_incoming_msg_ptr[nas_emm_index].nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
   
    /* Decode the security header for the incoming message */
    if(lte_nas_decode_security_hdr(src,&pos,dl_security_info,
                                   nas_incoming_msg_ptr) == FALSE)
    {
      return total_num_messages;
    } 

    /* Continue parsing the plain message as security things are taken care of */
    if(dl_security_info != NULL)
    {
      num_bits_to_read = 4;
      pd = (byte)b_unpackb(src,(word)(pos-pd_offset), num_bits_to_read);
      nas_incoming_msg_ptr[nas_emm_index].nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
    }

    /* Log plain incoming OTA Message */
    if((security_hdr_type == INTEGRITY_PROTECT)|| (security_hdr_type == INTEGRITY_PROTECT_CIPHER))
    {
      if(pd ==(byte)EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_OTA_IN_MSG_LOG_C,
                                    (word)((orig_buffer_len - skip_sec_hdr*8)/8),
                                     src + skip_sec_hdr);
      }
      else
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C,
                                    (word)((orig_buffer_len - skip_sec_hdr*8)/8),
                                    src + skip_sec_hdr);
      }
    }

    /* Process the message based on Protocol Discriminator */
    switch(pd)
    {
      case EPS_MOBILITY_MANAGEMENT_MESSAGES:
        if(lte_nas_decode_emm_message(src, &pos, nas_incoming_msg_ptr) == FALSE)
        {
          return total_num_messages;
        }
        break;
      case EPS_SESSION_MANAGEMENT_MESSAGES:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        /* Message ID is in octect 3 for ESM messages */
        num_bits_to_read = 8;
        if(dl_security_info != NULL)
        {
          message_id = (byte)b_unpackb(src, (word)(pos+8), num_bits_to_read);
        }
        else
        {
          message_id = (byte)b_unpackb(src,(word)(pos+16), num_bits_to_read);
        }
#endif
        if(dl_security_info != NULL)
        {
          pos = esm_offset*8;
          buf_len_bits = orig_buffer_len - (esm_offset*8);
        }
        esm_len = buf_len_bits/8;
        esm_exists = decode_esm_msg(src,
                                    &pos,
                                    &buf_len_bits,
                                    esm_len,
                                    &nas_incoming_msg_ptr[nas_esm_index]);
        if(esm_exists == TRUE)
        {
          report_esm_success(nas_incoming_msg_ptr);
          total_num_messages = 1;
        }
        else
        {
          report_esm_failure(nas_incoming_msg_ptr);
        }
        buf_len_bits = 0;
        total_num_messages = 1;
        break;

      case RESERVED_FOR_TESTS_PROCEDURES:
        if(dl_security_info == NULL)
        {
          /* Point the source buffer to PLAIN message */
          nas_incoming_msg_ptr[nas_emm_index].nas_hdr.ota_msg_ptr = src;
          /* TLB length is only the length of PLAIN TLB message */
          nas_incoming_msg_ptr[nas_emm_index].nas_hdr.ota_msg_len = buf_len;
          buf_len_bits = 0;
          total_num_messages = 1;
        }
        else
        {
          if((integrity_check_passed == TRUE)&& (decipher_success == TRUE))
          {
            /* Point the source buffer to PLAIN message (skipping the first 6 bytes of NAS header) */
            nas_incoming_msg_ptr[nas_emm_index].nas_hdr.ota_msg_ptr = src + skip_sec_hdr;
            /* TLB length is only the length of PLAIN TLB message (skip the first 6 bytes of NAS header)*/
            nas_incoming_msg_ptr[nas_emm_index].nas_hdr.ota_msg_len = buf_len - skip_sec_hdr;
            buf_len_bits = 0;
            total_num_messages = 1;
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= RESERVED_FOR_TESTS_PROCEDURES is received without security");
            report_emm_failure(nas_incoming_msg_ptr);
            buf_len_bits = 0;
            total_num_messages = 1;
          }
        }
        break;

      default:
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Unknown Protocol discriminator =%d received in Message ID = %d",
                       pd, message_id);
        MSG_ERROR_DS_1(MM_SUB, "Message ID = %d", message_id);
        report_emm_failure(nas_incoming_msg_ptr);
        buf_len_bits = 0;
        break;
    } /* end switch(pd) */
  } /* end while(buf_len_bits) */

  /* This case does not come, just to avoid lint errors */
  if((emm_exists)&&(esm_exists))
  {
    total_num_messages = 2;
  }
  MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Message decoding completed, passing the control to EMM");
  return total_num_messages;
} /* end decode_nas_msg() */ 


/*===========================================================================
FUNCTION  ENCODE_NAS_MSG

DESCRIPTION
  This function encodes all the outgoing NAS message and puts it into the OTA buffer.

DEPENDENCIES
  None

RETURN VALUE
  word - number of octets encoded

SIDE EFFECTS
  None
===========================================================================*/
word encode_nas_msg
(
  lte_nas_outgoing_msg_type     *out_buf,
  size_t                        out_buf_len,
  byte                          *ota_buf,
  size_t                        ota_buf_len,
  lte_nas_ul_security_info_type *security_info
)
{
  log_lte_nas_ota_out_msg_event_type event;
  word pos = 0;
  byte skip_offset_for_normal_encoding = 6*8;
  boolean encoding_present = FALSE;
  word encoded_msg_len = 0;
  byte num_bits_to_write = 0, sqn_offset = 5*8;

#ifdef FEATURE_LTE_REL14
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  msg_lib_encode_error = FALSE;
  service_request = FALSE;

  memset((void *)ota_buf, 0, EMM_MAX_MESSAGE_LENGTH);

#ifdef TEST_FRAMEWORK
  #error code not present
#endif 

  if(security_info != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= NAS message is security protected");
    encoding_present = TRUE;
    if((out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES) &&
       (out_buf->emm_outgoing_msg.service_req.security_hdr == SERV_REQ))
    {
      MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= SERV_REQ message, populate KSI (%d) and SQN (%d). Not skipping the bytes for normal encoding for Service Request message ",
                    emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.service_req.ksi_seqnum.keyset_id,
                    (security_info->nas_count[0])&0x1F);
    }
    else
    {
      if(security_info->int_valid)
      {
        pos += skip_offset_for_normal_encoding;
      }
    }
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= NAS message is not security protected");
  }

  switch(out_buf->nas_hdr.pd)
  {
    case EPS_MOBILITY_MANAGEMENT_MESSAGES:
      encode_emm_msg(out_buf, ota_buf, ota_buf_len, &pos,security_info);
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Encoding EMM message");
      if(encoding_present == TRUE)
      {
        if(out_buf->emm_outgoing_msg.service_req.security_hdr != SERV_REQ)
        {
          /* If message is security protected, skip the first 6 octets (related to security) and pass the plain message alone */
          send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C,(word)(pos/8 -6),ota_buf + (skip_offset_for_normal_encoding/8));
        }
      }
      else
      {
        if(out_buf->emm_outgoing_msg.service_req.security_hdr == SERV_REQ)
        {
          send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C,(word)(4), ota_buf);
        }
        else
        {
          /* Message is a PLAIN message, pass this buffer for logging */
          send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C,(word)(pos/8), ota_buf);
        }
        if(out_buf->emm_outgoing_msg.service_req.security_hdr != SERV_REQ)
        {
          event.out_msg = out_buf->nas_hdr.msg_id;
          event_report_payload(EVENT_LTE_EMM_OTA_OUTGOING_MSG, sizeof(event), (void*)&event);
        }
      }
      break;

    case EPS_SESSION_MANAGEMENT_MESSAGES:
      encode_esm_msg(out_buf, ota_buf, ota_buf_len, &pos, FALSE);
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Encoding ESM message");
      if(encoding_present == TRUE)
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_OTA_OUT_MSG_LOG_C,(word)(pos/8-6),ota_buf + (skip_offset_for_normal_encoding/8));
      }
      else
      {
        send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_OTA_OUT_MSG_LOG_C,(word)(pos/8), ota_buf);
        event.out_msg = out_buf->nas_hdr.msg_id;
        event_report_payload(EVENT_LTE_ESM_OTA_OUTGOING_MSG, sizeof(event), (void*)&event);
      }
      break;

    case RESERVED_FOR_TESTS_PROCEDURES:
      /* Just memcpy the TLB message payload to the ota buffer and continue adding the security header */
      if(encoding_present)
      {
        if(EMM_MAX_MESSAGE_LENGTH <= (pos/8))
        {
          MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Error!!! Destination buffer has exhausted");
          msg_lib_encode_error = TRUE;
          break;
        }
        else
        {
          memscpy((ota_buf+(pos/8)),
                  EMM_MAX_MESSAGE_LENGTH - (pos/8),
                  out_buf->tlb_outgoing_msg.tlb_msg.msg_payload,
                  out_buf->tlb_outgoing_msg.tlb_msg.tlb_len);
        }
      }
      else
      {
        memscpy(ota_buf,
                EMM_MAX_MESSAGE_LENGTH,
                out_buf->tlb_outgoing_msg.tlb_msg.msg_payload,
                out_buf->tlb_outgoing_msg.tlb_msg.tlb_len);
      }
      /* Length of the TLB message (with security header)  = rx TLB length + header length.
         Header length is already accounted before coming into this position, so just add
         the PLAIN TLB message length */
      pos += (out_buf->tlb_outgoing_msg.tlb_msg.tlb_len)*8;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= RESERVED_FOR_TESTS_PROCEDURES message encoded");
      break;

    default:
      MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown Protocol Discriminator = %d received",
                    (byte)out_buf->nas_hdr.pd);
      pos = 0;
      msg_lib_encode_error = FALSE;
      break;
  } /* end switch(out_buf->nas_hdr.pd) */

  encoded_msg_len = pos/8;
  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Encoding present = %d", encoding_present);
  if(encoding_present)
  {
    if((out_buf->emm_outgoing_msg.service_req.security_hdr == SERV_REQ) && 
       (out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES))
    {
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Encoding Service Request, not adding SQN");
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Adding SQN");
      /* Add SQN to OTA buffer */
      num_bits_to_write = 8;
      b_packb((byte)security_info->nas_count[0], ota_buf, sqn_offset, num_bits_to_write);
    }
    encode_security_protected_nas_hdr(out_buf, ota_buf, security_info, &encoded_msg_len);
    if((out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES) &&
       (out_buf->emm_outgoing_msg.service_req.security_hdr != SERV_REQ))
    {
      event.out_msg = out_buf->nas_hdr.msg_id;
      event_report_payload(EVENT_LTE_EMM_OTA_OUTGOING_MSG, sizeof(event), (void*)&event);
      send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_SEC_OTA_OUT_MSG_LOG_C, (word)(pos/8), ota_buf);
    }
    else if ((out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES) &&
             (out_buf->emm_outgoing_msg.service_req.security_hdr == SERV_REQ))
    {
      send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C,
                                  (word)(encoded_msg_len+2), ota_buf);
    }
    else if(out_buf->nas_hdr.pd == EPS_SESSION_MANAGEMENT_MESSAGES)
    {
      event.out_msg = out_buf->nas_hdr.msg_id;
      event_report_payload(EVENT_LTE_ESM_OTA_OUTGOING_MSG, sizeof(event), (void*)&event);
      send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_SEC_OTA_OUT_MSG_LOG_C, (word)(pos/8), ota_buf);
    }
  } /* end if(encoding_present) */

  if(msg_lib_encode_error)
  {
    encoded_msg_len = 0;
  }
  if(out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)
  {
    if(out_buf->emm_outgoing_msg.service_req.security_hdr == SERV_REQ)
    {
      encoded_msg_len++; /* Just to be LSTI compliant, we need to support MACI with zeroes */
      encoded_msg_len++;
      MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Length of Service Request message = %d", encoded_msg_len);
      event.out_msg = SERV_REQ;
      event_report_payload(EVENT_LTE_EMM_OTA_OUTGOING_MSG, sizeof(event), (void*)&event);
    }
  }

#ifdef FEATURE_LTE_REL14
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14) &&
     (encoded_msg_len != 0) &&
     (out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES) &&
     ((out_buf->nas_hdr.msg_id == ATTACH_REQUEST) ||
      (out_buf->nas_hdr.msg_id == TRACKING_AREA_UPADTE_REQUEST)))
  {
    word message_len = 0;

    if(encoding_present == TRUE)
    {
      message_len = encoded_msg_len - (word)(skip_offset_for_normal_encoding/8);
    }
    else
    {
      message_len = encoded_msg_len;
    }

    emm_reset_replay_message(emm_ctrl_data_ptr);

    if(message_len > 0)
    {
      emm_ctrl_data_ptr->replayed_msg.msg_ptr = 
                 (byte*)emm_modem_mem_alloc(message_len, emm_ctrl_data_ptr);
      mm_check_for_null_ptr(emm_ctrl_data_ptr->replayed_msg.msg_ptr);
      emm_ctrl_data_ptr->replayed_msg.msg_len = message_len;
      if(encoding_present == TRUE)
      {
        memscpy((void*)emm_ctrl_data_ptr->replayed_msg.msg_ptr,
                emm_ctrl_data_ptr->replayed_msg.msg_len,
                (void*)(ota_buf + skip_offset_for_normal_encoding/8),
                message_len);
      }
      else
      {
        memscpy((void*)emm_ctrl_data_ptr->replayed_msg.msg_ptr,
                emm_ctrl_data_ptr->replayed_msg.msg_len,
                (void*)(ota_buf),
                message_len);
      }
    }
  }
#endif
  return encoded_msg_len;
} /* end of encode_nas_msg() */


/*===========================================================================
FUNCTION      NAS_RETRIEVE_MSG

DESCRIPTION
  This function is called from EMM, to retrieve all the incoming OTA mesasges
  that were decoded & populated in the 'C' structure.
DEPENDENCIES
  None

RETURN VALUE
  lte_nas_protocol_discriminator - PROTOCOL_DISCRIMINATOR

SIDE EFFECTS
  None
===========================================================================*/
lte_nas_protocol_discriminator nas_retrieve_msg
(
  byte index,
  lte_nas_incoming_msg_type *buffer
)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();


  if(index > total_num_messages)
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Called indexed buffer does not exist - buffer exhausted");
    return (lte_nas_protocol_discriminator)0;
  }
  memscpy(buffer,
          sizeof(lte_nas_incoming_msg_type),
          &emm_ctrl_data_ptr->nas_incoming_msg_ptr[index],
          sizeof(lte_nas_incoming_msg_type));
  total_num_messages = total_num_messages - 1;
  return emm_ctrl_data_ptr->nas_incoming_msg_ptr[index].nas_hdr.pd;
} /* end nas_retrieve_msg() */


/*===========================================================================
FUNCTION  GET_NAS_SEC_INFO

DESCRIPTION
  This function is called from EMM, to retrieve the security related params
  like count & SQN - required for SECURITY procedure.

DEPENDENCIES
  None

RETURN VALUE
  lte_nas_sec_info_params_type - structure_type

SIDE EFFECTS
  None
===========================================================================*/
lte_nas_sec_info_params_type *get_nas_sec_info
(
  byte *sec_hdr
)
{
  /* EMM now requires the security_hdr, to process the UL messages */
  *sec_hdr = security_hdr_type;
  return &nas_sec_params;
} /* end get_nas_sec_info() */


/*===========================================================================
FUNCTION  GET_DL_NAS_SQN

DESCRIPTION
  This function saves SQN from the received DL message to NAS DL CNT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void get_dl_nas_sqn
(
  byte *buffer,
  byte *nas_dl_cnt
)
{
  byte security_hdr;

  security_hdr = (byte)((*buffer & 0xF0) >> 4);

  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Rcved DL MSG w/ security head %d", security_hdr);

  switch(security_hdr)
  {
    case INTEGRITY_PROTECT:
    case INTEGRITY_PROTECT_SECURITY_MODE_CMD:
    case INTEGRITY_PROTECT_CIPHER:
      /* SQN is in the 6th byte of the DL_NAS message */
      *nas_dl_cnt = (byte)(*(buffer + 5) & 0x0000000000FF); 
      MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Derived SQN %d from DL MSG", *nas_dl_cnt);
      break;

    default:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= No need to derive the DL CNT");
      break;
  } /* end switch(security_hdr)() */
}/* end get_dl_nas_sqn() */

#endif /* FEATURE_LTE */
