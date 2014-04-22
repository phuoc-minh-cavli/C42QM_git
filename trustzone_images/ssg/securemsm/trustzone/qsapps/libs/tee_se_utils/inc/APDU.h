#ifndef APDU_H_INCLUDED
#define APDU_H_INCLUDED
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                                   <APDU.h>
  DESCRIPTION
    Definitions and declarations for APDU handling.

  Copyright (c) 2015-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================
                                EDIT HISTORY

  $Header: //components/rel/ssg.tz/1.1.3/securemsm/trustzone/qsapps/libs/tee_se_utils/inc/APDU.h#1 $

    when     who     what, where, why
  --------   ---     -------------------------------------------------------
  18/08/15   ADG     Initial version
  ==========================================================================*/

/*==========================================================================*
 *                             INCLUDE FILES                                *
 *==========================================================================*/
#include "BER.h"
#include "DGI.h"

#ifdef __cplusplus
extern "C" {
#endif
/*==========================================================================*
 *                              DATA TYPES                                  *
 *==========================================================================*/
typedef BEROctet APDUOctet;
typedef BERSize  APDUSize;

/* Extract a byte from an integer value - LS is byte 0 */
#define APDUBYTE(b, v)                ( (APDUOctet) (((v) >>  ((b) * 8)) & 0xFF) )

/** @brief Structure to hold information derived from a C-APDU */
typedef struct
{
  APDUOctet  CLA;                             /**< CLA byte */
  APDUOctet  INS;                             /**< INS byte */
  APDUOctet  P1;                              /**< P1 byte */
  APDUOctet  P2;                              /**< P2 byte */
  APDUOctet  logChan;                         /**< Logical Channel number from CLA */
  APDUSize   Lc;                              /**< DATA bytes (Nc) */
  APDUSize   Le;                              /**< Expected maximum response bytes (Ne) */

  APDUOctet *pData;                           /**< Pointer to start of DATA, NULL if absent */
  APDUOctet *pLe;                             /**< Pointer to Le, or NULL if absent */

  APDUOctet  extended;                        /**< Lc/Le extended if non zero */
  APDUOctet *pAPDUStart;                      /**< Pointer to start of APDU */
  APDUSize   apduLength;                      /**< Total APDU length */

} APDUInfo;

/** @brief APDU field offsets */
typedef enum
{
  /* Command APDU (C-APDU) */
  APDUOFFS_CLA                        = 0,    /**< Single byte, always present */
  APDUOFFS_INS                        = 1,    /**< Single byte, always present */
  APDUOFFS_P1                         = 2,    /**< Single byte, always present */
  APDUOFFS_P2                         = 3,    /**< Single byte, always present */
  APDUOFFS_LC                         = 4,    /**< 0, 1, or 3 bytes. Absent if 0 */
  APDUOFFS_LENOLC                     = 4,    /**< Le if present without Lc */
  APDUOFFS_CDATA_LC1                  = 5,    /**< Command data with short Lc */
  APDUOFFS_CDATA_LC3                  = 7,    /**< Command data with extended Lc */

  /* Response APDU (R-APDU) */
  APDUOFFS_RDATA                      = 0,    /**< Response data (length <= Le) */
  APDUOFFS_SW1                        = (-2), /**< Status Word 1 */
  APDUOFFS_SW2                        = (-1), /**< Status Word 2 */

} APDUOffset;

/** @brief C-APDU CLA field bits */
typedef enum
{
  APDUCLA_INTERINDUSTRY               = 0x00, /**< Command is Interindustry */
  APDUCLA_GLOBALPLATFORM              = 0x80, /**< Command is GlobalPlatform */
  APDUCLA_FURTHERINTERINDUSTRY        = 0x40, /**< Command is Further Interindustry */

  /* First Interindustry coding (Logical Channels 0-3) */
  APDUCLA_1II_RFU                     = 0x20, /**< Unused bit */

  APDUCLA_1II_MORE                    = 0x10, /**< More APDUs in chain */

  APDUCLA_1II_SM_MASK                 = 0x0C, /**< Secure messaging bits */
  APDUCLA_1II_SM_NONE                 = 0x00, /**< Secure messaging: None */
  APDUCLA_1II_SM_PROPRIETARY          = 0x04, /**< Secure messaging: Proprietary */
  APDUCLA_1II_SM_7816_NO_CMAC         = 0x08, /**< Secure messaging: No authentication */
  APDUCLA_1II_SM_7816_CMAC            = 0x0C, /**< Secure messaging: Authentication */

  APDUCLA_1II_LOGCHAN_0_3_MASK        = 0x03, /**< Logical Channel number (0-3) */

  /* Further Interindustry coding (Logical Channels 4-19) */
  APDUCLA_2II_SM                      = 0x20, /**< Secure messaging */

  APDUCLA_2II_MORE                    = 0x10, /**< More APDUs in chain */

  APDUCLA_1II_LOGCHAN_4_19_MASK       = 0x0F, /**< Logical Channel number (4-19) */

} APDUCLAValue;

/** @brief C-APDU INS field values involved in Secure Channel management */
typedef enum
{
  /* Defined in ISO/IEC 7816-4 */
  APDUINS_VERIFY_PIN                  = 0x20,
  APDUINS_MANAGE_CHANNEL              = 0x70,
  APDUINS_EXTERNAL_AUTHENTICATE       = 0x82,
  APDUINS_MUTUAL_AUTHENTICATE         = 0x82,
  APDUINS_INTERNAL_AUTHENTICATE       = 0x88,
  APDUINS_SELECT                      = 0xA4,
  APDUINS_GET_RSP                     = 0xC0,
  APDUINS_GET_DATA                    = 0xCA,
  APDUINS_PUT_KEY                     = 0xD8,

  /* Defined in ISO/IEC 7816-8 and GlobalPlatform Card Specification */
  APDUINS_PERFORM_SECURITY_OPERATION  = 0x2A,

  /* Defined in GlobalPlatform Card Specification */
  APDUINS_INITIALIZE_UPDATE           = 0x50,
  APDUINS_END_RMAC_SESSION            = 0x78,
  APDUINS_BEGIN_RMAC_SESSION          = 0x7A,
  APDUINS_STORE_DATA                  = 0xE2,
  APDUINS_INSTALL                     = 0xE6,
  APDUINS_SET_STATUS                  = 0xF0,

  /* Proprietary */
  APDUINS_GENERATE_KEY                = 0x66,
  APDUINS_AUTHENTICATE                = 0x67

} APDUINSValue;

/** @brief P1 and P2 values for C-APDUs involved in Secure Channel management */
typedef enum
{
  /* Defined in ISO/IEC 7816-4 */
  APDUP1_MANAGE_CHANNEL_OPEN          = 0x00,
  APDUP1_MANAGE_CHANNEL_CLOSE         = 0x80,
  APDUP1_SELECT                       = 0x04,

  APDUP2_SELECT_NEXT                  = 0x02,

  /* As defines in Secure Element Access Control Specification */
  APDUP1_GET_ACCESS_RULES             = 0xFF,
  APDUP1_GET_REFRESH_TAG              = 0xDF,

  APDUP2_GET_ALL_ACCESS_RULES         = 0x40,

  /* Defined in GlobalPlatform Card Specification */
  APDUP1_INSTALL_LASTBLOCK_MASK       = 0x80,     /**< Mask to isolate Last Block indicator */
  APDUP1_INSTALL_LASTBLOCK_NO         = 0x80,     /**< Last (or only) block */
  APDUP1_INSTALL_LASTBLOCK_YES        = 0x00,     /**< More blocks to follow */

  APDUP1_INSTALL_PURPOSE_MASK         = 0x7F,     /**< Mask to isolate Purpose bits */
  APDUP1_INSTALL_PURPOSE_REGUPDATE    = 0x40,     /**< GP registry update */
  APDUP1_INSTALL_PURPOSE_PERSO        = 0x20,     /**< Personalization */
  APDUP1_INSTALL_PURPOSE_EXTRADITION  = 0x10,     /**< Extradition */
  APDUP1_INSTALL_PURPOSE_MAKESELECT   = 0x08,     /**< Make Selectable */
  APDUP1_INSTALL_PURPOSE_INSTALL      = 0x04,     /**< Install */
  APDUP1_INSTALL_PURPOSE_LOAD         = 0x02,     /**< Load  */

  APDUP2_INSTALL_COMBINED_NOINFO      = 0x00,     /**< Combined Selectable/Install/Load: no info */
  APDUP2_INSTALL_COMBINED_START       = 0x01,     /**< Combined Selectable/Install/Load: start */
  APDUP2_INSTALL_COMBINED_END         = 0x03,     /**< Combined Selectable/Install/Load: end */

  APDUP1_STOREDATA_LASTBLOCK_MASK     = 0x80,     /**< Mask to isolate Last Block indicator */
  APDUP1_STOREDATA_LASTBLOCK_NO       = 0x00,     /**< Last (or only) block */
  APDUP1_STOREDATA_LASTBLOCK_YES      = 0x80,     /**< More blocks to follow */

  APDUP1_STOREDATA_ENC_MASK           = 0x60,     /**< Mask to isolate encryption indicator */
  APDUP1_STOREDATA_ENC_NOINFO         = 0x00,     /**< No information, or no encryption */
  APDUP1_STOREDATA_ENC_APPDEPENDENT   = 0x20,     /**< Application dependent encryption */
  APDUP1_STOREDATA_ENC_RFU            = 0x40,     /**< RFU */
  APDUP1_STOREDATA_ENC_ENCRYPTED      = 0x60,     /**< Encrypted */

  APDUP1_STOREDATA_STRUCTURE_MASK     = 0x18,     /**< Mask to isolate DATA structure indicator */
  APDUP1_STOREDATA_STRUCTURE_NOINFO   = 0x00,     /**< No information */
  APDUP1_STOREDATA_STRUCTURE_DGI      = 0x08,     /**< DGI format */
  APDUP1_STOREDATA_STRUCTURE_BERTLV   = 0x10,     /**< BER-TLV format */
  APDUP1_STOREDATA_STRUCTURE_RFU      = 0x18,     /**< RFU */

  APDUP1_STOREDATA_ISOCASE_MASK       = 0x01,     /**< Mask to isolate response DATA expectation */
  APDUP1_STOREDATA_ISOCASE_3          = 0x00,     /**< No response DATA expected */
  APDUP1_STOREDATA_ISOCASE_4          = 0x01      /**< Response DATA expected */

} APDUP1P2Value;

/** @brief Maximum lengths for APDUs involved in Secure Channel Management */
typedef enum
{
  /* Generic */
  APDULEN_C_HEADER                    = (4),                              /* CLA + INS + P1 + P2 */
  APDUMAXLEN_C_FRAMING_LC             = (APDULEN_C_HEADER + 1),           /* CLA + INS + P1 + P2 + Lc */
  APDUMAXLEN_C_FRAMING                = (APDUMAXLEN_C_FRAMING_LC + 1),    /* CLA + INS + P1 + P2 + Lc + Le */
  APDUMAXLEN_C_DATA                   = (255),
  APDUMAXLEN_C_APDU                   = (APDUMAXLEN_C_FRAMING + APDUMAXLEN_C_DATA),
  APDULEN_R_STATUS                    = (2),                              /* SW1 + SW2 */
  APDUMAXLEN_R_DATA                   = (256),
  APDUMAXLEN_R_APDU                   = (APDUMAXLEN_R_DATA + APDULEN_R_STATUS),

  /* Proprietary GENERATE_KEY APDU */
  APDUMAXLEN_C_GENERATE_KEY           = (APDULEN_C_HEADER + 1),           /* Header, Le */
  APDUMAXLEN_R_GENERATE_KEY           = (1 + 32 + 32 + APDULEN_R_STATUS),

  /* Proprietary AUTHENTICATE APDU */
  APDUMAXLEN_C_AUTHENTICATE           = (APDULEN_C_HEADER + 1 + 8 + 1),   /* Header, Lc, DATA, Le */
  APDUMAXLEN_R_AUTHENTICATE           = (8 + APDULEN_R_STATUS),

  /* SET_STATUS APDU variant in GP Card Specification Amendment C - Contactless Availability */
  APDUMAXLEN_C_SET_STATUS             = (APDULEN_C_HEADER + 1 + (1 + 1 + 16) + 1),  /* Header, Lc, DATA, Le */
  APDUMAXLEN_R_SET_STATUS             = (1 + 1 + (1 + 1 + 16) + APDULEN_R_STATUS),

  /* GET_DATA APDU variant in GP Card Specification Amendment F - ECKA Certificate */
  APDUMAXLEN_C_GET_DATA               = (APDULEN_C_HEADER + 1 + 6 + 1),   /* Header, Lc, DATA, Le */
  APDUMINLEN_R_GET_DATA               = (APDULEN_R_STATUS),

  /* INSTALL (for Perso) APDU variant in GP Card Specification Amendment */
  APDUMAXLEN_C_INSTALL_PERSO          = (APDULEN_C_HEADER + 1 + 6 + 1),   /* Header, Lc, DATA, Le */
  APDUMINLEN_R_INSTALL_PERSO          = (APDULEN_R_STATUS + 1),

  /* STORE_DATA APDU variant in GP Card Specification Amendment F - ECKA Certificate */
  APDUMAXLEN_C_STORE_DATA_ECKACERT    = (APDULEN_C_HEADER + 1 + (1 + 1 + 4) + (2 + 1 + 2 + 1) + 1),   /* Header, Lc, DATA, Le */
  APDUMINLEN_R_STORE_DATA_ECKACERT    = (APDULEN_R_STATUS),

  /* STORE_DATA APDU variant in GP Card Specification - ECC Key */
  APDUMAXLEN_C_STORE_DATA_ECCKEY      = (APDULEN_C_HEADER + 1 + (2 + 1) + (2 + 1) + 1),   /* Header, Lc, DATA, Le */
  APDUMINLEN_R_STORE_DATA_ECCKEY      = (APDULEN_R_STATUS),

  /* PERFORM_SECURITY_OPERATION APDU in GP Card Specification Amendment F */
  APDUMINLEN_C_PERFORM_SECURITY_OP    = (APDULEN_C_HEADER + 1 + 1),       /* Header, Lc, DATA, Le */
  APDULEN_R_PERFORM_SECURITY_OP       = (APDULEN_R_STATUS),

  /* MUTUAL_AUTHENTICATE APDU in GP Card Specification Amendment F */
  APDUMINLEN_C_MUTUAL_AUTHENTICATE    = (APDULEN_C_HEADER + 1 + 1),       /* Header, Lc, DATA, Le */
  APDUMINLEN_R_MUTUAL_AUTHENTICATE    = (APDULEN_R_STATUS),

  /* INTERNAL_AUTHENTICATE APDU in GP Card Specification Amendment F */
  APDUMINLEN_C_INTERNAL_AUTHENTICATE  = (APDUMINLEN_C_MUTUAL_AUTHENTICATE),
  APDUMINLEN_R_INTERNAL_AUTHENTICATE  = (APDUMINLEN_R_MUTUAL_AUTHENTICATE),

  /* Adjustments relating to MAC and encryption */
  APDUMAXLEN_C_ENC_PADDING            = (16),                             /* Pad up to multiple of 16 bytes */
  APDUMAXLEN_R_ENC_PADDING            = (APDUMAXLEN_C_ENC_PADDING),       /* Pad up to multiple of 16 bytes */
  APDULEN_C_MAC_SIZE                  = (8),
  APDULEN_R_MAC_SIZE                  = (APDULEN_C_MAC_SIZE),
  APDUMAXLEN_CR_ENC_MAC_OVERHEAD      = (APDUMAXLEN_C_ENC_PADDING + APDULEN_C_MAC_SIZE)

} APDUMaxLength;

/** @brief TLV Tag values used in SCP APDU DATA. All TLVs are BER-TLV encoded unless stated otherwise */
typedef enum
{
  /* Tags for SET_STATUS (Contactless Availability), used in SCPX */
  APDUTAG_SETSTATUS_AID               = 0x004F,   /**< SET_STATUS (Contactless Availability): AID value */

  /* Tags for GET_DATA (ECKA Certificate), used in SCP11 */
  APDUTAG_GETDATA_TEMPLATE_ECKA       = 0x00A6,   /**< GET_DATA (ECKA Cert): Control Reference Template */
  APDUTAG_GETDATA_KEYINFO             = 0x0083,   /**< GET_DATA (ECKA Cert): Key Information */

  APDUTAG_GETDATA_SCP11CERTSTORE      = 0xBF21,   /**< GET_DATA (ECKA Cert): SCP11 Certificate Store */
  APDUTAG_GETDATA_CERTSDECKA          = 0x7F21,   /**< GET_DATA (ECKA Cert): CERT.SD.ECKA */
  APDUTAG_GETDATA_CERTOCEECKA         = 0x7F22,   /**< GET_DATA (ECKA Cert): CERT.OCE.ECKA */
  APDUTAG_GETDATA_CERTSERIALNUM       = 0x0093,   /**< GET_DATA (ECKA Cert): Certificate Serial Number */
  APDUTAG_GETDATA_CAID                = 0x0042,   /**< GET_DATA (ECKA Cert): CA Identifier */
  APDUTAG_GETDATA_SUBJECTID           = 0x5F20,   /**< GET_DATA (ECKA Cert): Subject Identifier */
  APDUTAG_GETDATA_KEYUSAGE            = 0x0095,   /**< GET_DATA (ECKA Cert): Key Usage */
  APDUTAG_GETDATA_STARTDATE           = 0x5F25,   /**< GET_DATA (ECKA Cert): Start Date (BCD) */
  APDUTAG_GETDATA_ENDDATE             = 0x5F24,   /**< GET_DATA (ECKA Cert): End Date (BCD) */
  APDUTAG_GETDATA_CASDIMAGENUMBER     = 0x0045,   /**< GET_DATA (ECKA Cert): CASD Image Number */
  APDUTAG_GETDATA_DISCDATAUNSPEC      = 0x0053,   /**< GET_DATA (ECKA Cert): Discretionary Data (no format) */
  APDUTAG_GETDATA_DISCDATABERTLV      = 0x0073,   /**< GET_DATA (ECKA Cert): Discretionary Data (BER-TLV) */
  APDUTAG_GETDATA_PKDATAOBJECT        = 0x7F49,   /**< GET_DATA (ECKA Cert): Public Key Data Object */
  APDUTAG_GETDATA_ECCPK               = 0x00B0,   /**< GET_DATA (ECKA Cert): ECC Public Key (Q) */
  APDUTAG_GETDATA_ECCCURVE            = 0x00F0,   /**< GET_DATA (ECKA Cert): ECC Curve */
  APDUTAG_GETDATA_SIGNATURE           = 0x5F37,   /**< GET_DATA (ECKA Cert): Signature */

  /* Tags for PERFORM_SECURITY_OPERATION, used in SCP11 */
  APDUTAG_PSO_CERTOCEECKA             = 0x7F21,   /**< PERFORM_SECURITY_OPERATION: CERT.OCE.ECKA */
  APDUTAG_PSO_CERTSERIALNUM           = 0x0093,   /**< PERFORM_SECURITY_OPERATION: Certificate Serial Number */
  APDUTAG_PSO_CAKLOCID                = 0x0042,   /**< PERFORM_SECURITY_OPERATION: CA-KLOC Identifier */
  APDUTAG_PSO_SUBJECTID               = 0x5F20,   /**< PERFORM_SECURITY_OPERATION: Subject Identifier */
  APDUTAG_PSO_KEYUSAGE                = 0x0095,   /**< PERFORM_SECURITY_OPERATION: Key Usage */
  APDUTAG_PSO_STARTDATE               = 0x5F25,   /**< PERFORM_SECURITY_OPERATION: Start Date (BCD) */
  APDUTAG_PSO_ENDDATE                 = 0x5F24,   /**< PERFORM_SECURITY_OPERATION: End Date (BCD) */
  APDUTAG_PSO_DISCDATAUNSPEC          = 0x0053,   /**< PERFORM_SECURITY_OPERATION: Discretionary Data (no format) */
  APDUTAG_PSO_DISCDATABERTLV          = 0x0073,   /**< PERFORM_SECURITY_OPERATION: Discretionary Data (BER-TLV) */
  APDUTAG_PSO_PKDATAOBJECT            = 0x7F49,   /**< PERFORM_SECURITY_OPERATION: Public Key Data Object */
  APDUTAG_PSO_ECCPK                   = 0x00B0,   /**< PERFORM_SECURITY_OPERATION: ECC Public Key (Q) */
  APDUTAG_PSO_ECCCURVE                = 0x00F0,   /**< PERFORM_SECURITY_OPERATION: ECC Curve */
  APDUTAG_PSO_SIGNATURE               = 0x5F37,   /**< PERFORM_SECURITY_OPERATION: Signature */

  /* Tags for MUTUAL_AUTHENTICATE and INTERNAL_AUTHENTICATE, used in SCP11 */
  APDUTAG_AUTH_TEMPLATEECKA           = 0x00A6,   /**< MUTUAL/INTERNAL_AUTHENTICATE: Control Reference Template */
  APDUTAG_AUTH_SCP_ID_PARAMS          = 0x0090,   /**< MUTUAL/INTERNAL_AUTHENTICATE: SCP Type and Parameters */
  APDUTAG_AUTH_KEYUSAGE               = 0x0095,   /**< MUTUAL/INTERNAL_AUTHENTICATE: Session Key usage */
  APDUTAG_AUTH_KEYTYPE                = 0x0080,   /**< MUTUAL/INTERNAL_AUTHENTICATE: Session Key Type */
  APDUTAG_AUTH_KEYLENGTH              = 0x0081,   /**< MUTUAL/INTERNAL_AUTHENTICATE: Session Key Length (bytes) */
  APDUTAG_AUTH_HOSTID                 = 0x0084,   /**< MUTUAL/INTERNAL_AUTHENTICATE: Host ID, if indicated in SCP Parameters */
  APDUTAG_AUTH_ECCPKECKA              = 0x5F49,   /**< MUTUAL/INTERNAL_AUTHENTICATE: ECC Public Key for ECKA */
  APDUTAG_AUTH_RECEIPT                = 0x0086,   /**< MUTUAL/INTERNAL_AUTHENTICATE: Receipt */

  /* Tags for STORE_DATA (ECKA Certificate), used in SCP11 Provisioning */
  APDUTAG_STOREDATA_TEMPLATE_ECKA     = APDUTAG_GETDATA_TEMPLATE_ECKA,
  APDUTAG_STOREDATA_KEYINFO           = APDUTAG_GETDATA_KEYINFO,

  APDUTAG_STOREDATA_SCP11CERTSTORE    = APDUTAG_GETDATA_SCP11CERTSTORE,
  APDUTAG_STOREDATA_CERTSDECKA        = APDUTAG_GETDATA_CERTSDECKA,
  APDUTAG_STOREDATA_CERTOCEECKA       = APDUTAG_GETDATA_CERTOCEECKA,
  APDUTAG_STOREDATA_CERTSERIALNUM     = APDUTAG_GETDATA_CERTSERIALNUM,
  APDUTAG_STOREDATA_CAID              = APDUTAG_GETDATA_CAID,
  APDUTAG_STOREDATA_SUBJECTID         = APDUTAG_GETDATA_SUBJECTID,
  APDUTAG_STOREDATA_KEYUSAGE          = APDUTAG_GETDATA_KEYUSAGE,
  APDUTAG_STOREDATA_STARTDATE         = APDUTAG_GETDATA_STARTDATE,
  APDUTAG_STOREDATA_ENDDATE           = APDUTAG_GETDATA_ENDDATE,
  APDUTAG_STOREDATA_CASDIMAGENUMBER   = APDUTAG_GETDATA_CASDIMAGENUMBER,
  APDUTAG_STOREDATA_DISCDATAUNSPEC    = APDUTAG_GETDATA_DISCDATAUNSPEC,
  APDUTAG_STOREDATA_DISCDATABERTLV    = APDUTAG_GETDATA_DISCDATABERTLV,
  APDUTAG_STOREDATA_PKDATAOBJECT      = APDUTAG_GETDATA_PKDATAOBJECT,
  APDUTAG_STOREDATA_ECCPK             = APDUTAG_GETDATA_ECCPK,
  APDUTAG_STOREDATA_ECCCURVE          = APDUTAG_GETDATA_ECCCURVE,
  APDUTAG_STOREDATA_SIGNATURE         = APDUTAG_GETDATA_SIGNATURE,

  /* Tags for STORE_DATA (ECC Key), used in SCP11 Provisioning - DGI encoded */
  APDUTAG_STOREDATA_ECCKEYCRT         = 0x00B9,
  APDUTAG_STOREDATA_ECCKEYUSAGE       = APDUTAG_STOREDATA_KEYUSAGE,
  APDUTAG_STOREDATA_ECCKEYACCESS      = 0x0096,
  APDUTAG_STOREDATA_ECCKEYTYPE        = APDUTAG_AUTH_KEYTYPE,
  APDUTAG_STOREDATA_ECCKEYLENGTH      = APDUTAG_AUTH_KEYLENGTH,
  APDUTAG_STOREDATA_ECCKEYID          = 0x0082,
  APDUTAG_STOREDATA_ECCKEYVERSION     = 0x0083,
  APDUTAG_STOREDATA_ECCKEYPARAMREF    = 0x0085,
  APDUTAG_STOREDATA_ECCKEYPUBLIC      = 0x0036,
  APDUTAG_STOREDATA_ECCKEYPRIVATE     = 0x8137,

  /* Invalid tag */
  APDUTAG_INVALID                     = 0xFFFF

} APDUTLVTag;

/** @brief Status Type values for SET_STATUS C-APDU P1 */
typedef enum
{
  /* Values from GP card Specification */
  APDUSST_GP_ISSUER_SD                = 0x80, /**< Issuer SD */
  APDUSST_GP_APP_OR_SUPP_SD           = 0x40, /**< Application or Supplementary SD */
  APDUSST_GP_SD_AND_APPS              = 0x60, /**< SD and its associated Applications */

  /* Values from GP Card Specification Amendment C */
  APDUSST_GPC_CONTACTLESSAVAILIBILITY = 0x01, /**< Availability over Contactless Interface */
  APDUSST_GPC_SELPRIORITYORDER        = 0x02, /**< Application Selection Priority Order */
  APDUSST_GPC_COMMINTERFACEACCESS     = 0x04, /**< Communication Interface Access */
  APDUSST_GPC_REMAININGRSPDATA        = 0x80  /**< Remaining Response Data */

} APDUSetStatusType;

/** @brief State Control values for SET_STATUS C-APDU P2 */
typedef enum
{
  /* Executable Load File Life Cycle coding */
  APDUSSS_EXEC_LOADED                 = 0x01, /**< Executable Load File loaded */

  /* Application Life Cycle coding */
  APDUSSS_APP_INSTALLED               = 0x03, /**< Application installed */
  APDUSSS_APP_SELECTABLE              = 0x07, /**< Application selectable */
  APDUSSS_APP_LOCKED                  = 0x83, /**< Application locked (mask) */

  /* Security Domain Life Cycle coding */
  APDUSSS_SD_INSTALLED                = 0x03, /**< Security Domain installed */
  APDUSSS_SD_SELECTABLE               = 0x07, /**< Security Domain selectable */
  APDUSSS_SD_PERSONALISED             = 0x0F, /**< Security Domain personalised */
  APDUSSS_SD_LOCKED                   = 0x83, /**< Security Domain locked (mask) */

  /* Card Life Cycle coding */
  APDUSSS_CARD_OP_READY               = 0x01, /**< Card OP_READY */
  APDUSSS_CARD_INITIALISED            = 0x07, /**< Card INITIALISED */
  APDUSSS_CARD_SECURED                = 0x0F, /**< Card SECURED */
  APDUSSS_CARD_CARD_LOCKED            = 0x7F, /**< Card CARD_LOCKED */
  APDUSSS_CARD_TERMINATED             = 0xFF, /**< Card TERMINATED */

  /* GP Card Specification Amendment C - Contactless Availability */
  APDUSSS_AVAIL_ACTIVATED             = 0x01, /**< Activate AID */
  APDUSSS_AVAIL_DEACTIVATED           = 0x00, /**< Deactivate AID */
  APDUSSS_AVAIL_NONACTIVATABLE        = 0x80  /**< Set AID non-activatable */

} APDUSetStatusState;

/** @brief Key Usage values for GET_DATA R-APDU */
typedef enum
{
  /* First byte, defined in GP Card Specification section 11.1.9 */
  APDUGDKU_0_VERIFICATION_ENC         = 0x80,
  APDUGDKU_0_COMPUTATION_DEC          = 0x40,
  APDUGDKU_0_SM_RSP_DATA              = 0x20,
  APDUGDKU_0_SM_CMD_DATA              = 0x10,
  APDUGDKU_0_CONFIDENTIALITY          = 0x08,
  APDUGDKU_0_CRYPTOGRAPHIC_CHECKSUM   = 0x04,
  APDUGDKU_0_DIGITAL_SIGNATURE        = 0x02,
  APDUGDKU_0_CRYPTOGRAPHIC_AUTH       = 0x01,

  /* Second byte, defined in GP Card Specification Amendment E section 3.3 */
  APDUGDKU_1_KEYAGREEMENT             = 0x80,
  APDUGDKU_1_RFU                      = 0x7F

} APDUGetDataKeyUsage;

/*==========================================================================*
 *                              DEFINITIONS                                 *
 *==========================================================================*/

/**
 *  @brief  Determine a C-APDU DATA field location and length.
 *
 *  <table>
 *    <tr>
 *      <th> C-APDU Length </th>
 *      <th> Fields Present   </th>
 *    </tr>
 *    <tr>
 *      <td align="center"> 0 to 3 </td>
 *      <td> Malformed C-APDU </td>
 *    </tr>
 *    <tr>
 *      <td align="center"> 4 </td>
 *      <td> CLA, INS, P1, P2 </td>
 *    </tr>
 *    <tr>
 *      <td align="center"> 5 </td>
 *      <td> CLA, INS, P1, P2, Le1 </td>
 *    </tr>
 *    <tr>
 *      <td align="center"> L >= 6 </td>
 *      <td>
 *        L = 7  : CLA, INS, P1, P2, Le3                     <br>
 *        L >= 8 : CLA, INS, P1, P2, Lc3=L-7, Data(L-7)      <br>
 *        L >= 10: CLA, INS, P1, P2, Lc3=L-9, Data(L-9), Le2 <br>
 *        L >= 7 : CLA, INS, P1, P2, Lc1=L-6, Data(L-6), Le1 <br>
 *        All L  : CLA, INS, P1, P2, Lc1=L-5, Data(L-5)
 *      </td>
 *    </tr>
 *  </table>
 *
 *  @param  [in]  pAPDU           Pointer to the buffer holding the C-APDU.
 *  @param  [in]  apduLength      The C-APDU length, in bytes.
 *  @param  [out] pInfo           Pointer to a structure to receive the output.
 *
 *  @return pInfo, having been filled in, if successful, or NULL otherwise.
 */
APDUInfo *APDUGetInfo(APDUOctet *pAPDU,
                      APDUSize   apduLength,
                      APDUInfo  *pInfo);

/**
 *  @brief  Append data to a C-APDU DATA field, or make room for
 *          additional data to be added. Adds, adjusts and moves
 *          fields as necessary.
 *
 *  A number of transformations are possible, depending on the
 *  sizes and values of various C-APDU fields (Lc, Le), and
 *  the amount of additional data (XD) being appended.
 *
 *  Note the Lc and Le columns show the change in size of these fields,
 *  not the change in value.
 *
 * | N  |        APDU before         |           APDU After          | Lc | Le |
 * | -- | -------------------------- | ----------------------------- | -- | -- |
 * | 1a | CLA INS P1 P2              | CLA INS P1 P2 Lc1 XD          | +1 |    |
 * | 1b | CLA INS P1 P2              | CLA INS P1 P2 Lc3 XD          | +3 |    |
 * | 2a | CLA INS P1 P2 Le1          | CLA INS P1 P2 Lc1 XD Le1      | +1 |    |
 * | 2b | CLA INS P1 P2 Le1          | CLA INS P1 P2 Lc3 XD Le2      | +3 | +1 |
 * | 3  | CLA INS P1 P2 Le3          | CLA INS P1 P2 Lc3 XD Le2      | +3 | -1 |
 * | 4a | CLA INS P1 P2 Lc1 DATA     | CLA INS P1 P2 Lc1 DATA+XD     |    |    |
 * | 4b | CLA INS P1 P2 Lc1 DATA     | CLA INS P1 P2 Lc3 DATA+XD     | +2 |    |
 * | 5  | CLA INS P1 P2 Lc3 DATA     | CLA INS P1 P2 Lc3 DATA+XD     |    |    |
 * | 6a | CLA INS P1 P2 Lc1 DATA Le1 | CLA INS P1 P2 Lc1 DATA+XD Le1 |    |    |
 * | 6b | CLA INS P1 P2 Lc1 DATA Le1 | CLA INS P1 P2 Lc3 DATA+XD Le2 | +2 | +1 |
 * | 7  | CLA INS P1 P2 Lc3 DATA Le2 | CLA INS P1 P2 Lc3 DATA+XD Le2 |    |    |
 *
 *  @param  [in,out] pInfo        C-APDU descriptor previously populated
 *                                by APDUGetInfo().
 *  @param  [in]     pExtraData   The data to be appended to whatever
 *                                data is already present. May be NULL,
 *                                in which case fields will be adjusted
 *                                but no data will be appended.
 *  @param  [in]     extraSize    The size of the extra data.
 *  @param  [in]     bufferSize   The size of the buffer containing
 *                                the C-APDU.
 *
 *  @return The new length of the C-APDU, if successful, or 0 otherwise.
 */
APDUSize APDUAppend(APDUInfo        *pInfo,
                    const APDUOctet *pExtraData,
                    APDUSize         extraSize,
                    APDUSize         bufferSize);

/**
 *  @brief  Construct a GENERATE_KEY C-APDU in the buffer given.
 *
 *  @param  [out] pBuffer         Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize      The buffer size, in bytes.
 *  @param  [in]  logicalChannel  The Logical Channel value to go into
 *                                the CLA byte.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 */
APDUSize APDUGenerateKey(APDUOctet *pBuffer,
                         APDUSize   bufferSize,
                         APDUOctet  logicalChannel);

/**
 *  @brief  Construct a AUTHENTICATE C-APDU in the buffer given.
 *
 *  @param  [out] pBuffer           Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize        The buffer size, in bytes.
 *  @param  [in]  logicalChannel    The Logical Channel value to go into
 *                                  the CLA byte.
 *  @param  [in]  pHostChallenge    Pointer to the Host Challenge.
 *  @param  [in]  hostChallengeSize The size of the Host Challenge.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 */
APDUSize APDUAuthenticate(APDUOctet       *pBuffer,
                          APDUSize         bufferSize,
                          APDUOctet        logicalChannel,
                          const APDUOctet *pHostChallenge,
                          APDUSize         hostChallengeSize);

/**
 *  @brief  Construct a SET_STATUS C-APDU in the buffer given.
 *
 *          The form of the SET_STATUS C-APDU is as described in the GP
 *          Card Specification Amendment C (Contactless Services) to be sent
 *          to the CRS applet to control access to the contactless
 *          interface for a specified applet.
 *
 *  @param  [out] pBuffer         Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize      The buffer size, in bytes.
 *  @param  [in]  logicalChannel  The Logical Channel value to go into
 *                                the CLA byte.
 *  @param  [in]  statusType      The entity to which the new status applies.
 *  @param  [in]  statusState     The new status value.
 *  @param  [in]  pAID            Pointer to the AID.
 *  @param  [in]  aidSize         The size of the AID.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 */
APDUSize APDUSetStatus(APDUOctet          *pBuffer,
                       APDUSize            bufferSize,
                       APDUOctet           logicalChannel,
                       APDUSetStatusType   statusType,
                       APDUSetStatusState  statusState,
                       const APDUOctet    *pAID,
                       APDUSize            aidSize);

/**
 *  @brief  Construct a GET_DATA C-APDU in the buffer given.
 *
 *          The form of the GET_DATA C-APDU is as described in the GP
 *          Card Specification Amendment F (SCP11) to retrieve the
 *          ECKA Certificate from the eSE.
 *
 *  @param  [out] pBuffer         Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize      The buffer size, in bytes.
 *  @param  [in]  logicalChannel  The Logical Channel value to go into
 *                                the CLA byte.
 *  @param  [in]  keyIdentifier   Which key is required.
 *  @param  [in]  keyVersion      The required version of the key.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 */
APDUSize APDUGetData(APDUOctet *pBuffer,
                     APDUSize   bufferSize,
                     APDUOctet  logicalChannel,
                     APDUOctet  keyIdentifier,
                     APDUOctet  keyVersion);

/**
 *  @brief  Construct a INSTALL (for Perso) C-APDU in the buffer given.
 *
 *  @param  [out] pBuffer         Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize      The buffer size, in bytes.
 *  @param  [in]  logicalChannel  The Logical Channel value to go into
 *                                the CLA byte.
 *  @param  [in]  pAID            Pointer to the AID to install for.
 *  @param  [in]  aidSize         The AID size, in bytes.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 */
APDUSize APDUInstallPerso(APDUOctet       *pBuffer,
                          APDUSize         bufferSize,
                          APDUOctet        logicalChannel,
                          const APDUOctet *pAID,
                          APDUSize         aidSize);

/**
 *  @brief  Construct a STORE_DATA (ECKA Certificate) C-APDU
 *          in the buffer given.
 *
 *          The form of the STORE_DATA C-APDU is as described in the
 *          GP Card Specification Amendment F (SCP11) to provision
 *          the SD ECKA Certificate in the eSE.
 *
 *  @param  [out] pBuffer         Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize      The buffer size, in bytes.
 *  @param  [in]  logicalChannel  The Logical Channel value to go into
 *                                the CLA byte.
 *  @param  [in]  blockNumber     The Block Number to go into the P2 byte.
 *  @param  [in]  keyIdentifier   Which key to attach to.
 *  @param  [in]  keyVersion      The required version of the key.
 *  @param  [in]  pCertificate    Pointer to the ECKA certificate.
 *  @param  [in]  certificateSize The certificate size, in bytes.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 */
APDUSize APDUStoreDataECKACert(APDUOctet       *pBuffer,
                               APDUSize         bufferSize,
                               APDUOctet        logicalChannel,
                               APDUOctet        blockNumber,
                               APDUOctet        keyIdentifier,
                               APDUOctet        keyVersion,
                               const APDUOctet *pCertificate,
                               APDUSize         certificateSize);

/**
 *  @brief  Construct a STORE_DATA (ECC Key) C-APDU in the buffer given.
 *
 *          The form of the STORE_DATA C-APDU is as described in the
 *          GP Card Specification Amendment F (SCP11) to provision
 *          an ECC Public or Private Key in the eSE.
 *
 *  @param  [out] pBuffer         Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize      The buffer size, in bytes.
 *  @param  [in]  logicalChannel  The Logical Channel value to go into
 *                                the CLA byte.
 *  @param  [in]  blockNumber     The Block Number to go into the P2 byte.
 *  @param  [in]  keyIdentifier   Which key to set.
 *  @param  [in]  keyIsPublic     TRUE if the key is public, otherwise FALSE.
 *  @param  [in]  keyVersion      The required version of the key.
 *  @param  [in]  keyParamRef     The ECC curve.
 *  @param  [in]  pKeyBuffer      Pointer to the key buffer.
 *  @param  [in]  keySize         The key size, in bytes.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 */
APDUSize APDUStoreDataECCKey(APDUOctet       *pBuffer,
                             APDUSize         bufferSize,
                             APDUOctet        logicalChannel,
                             APDUOctet        blockNumber,
                             APDUOctet        keyIdentifier,
                             APDUOctet        keyVersion,
                             bool             keyIsPublic,
                             APDUOctet        keyParamRef,
                             const APDUOctet *pKeyBuffer,
                             APDUSize         keySize);

/**
 *  @brief  Construct a PERFORM_SECURITY_OPERATION C-APDU in the buffer given.
 *
 *          The form of the PERFORM_SECURITY_OPERATION C-APDU is as described
 *          in the GP Card Specification Amendment F (SCP11) to pass the Host
 *          ECKA Certificate to the eSE.
 *
 *  @param  [out] pBuffer         Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize      The buffer size, in bytes.
 *  @param  [in]  logicalChannel  The Logical Channel value to go into
 *                                the CLA byte.
 *  @param  [in]  keyIdentifier   Which key is required.
 *  @param  [in]  keyVersion      The required version of the key.
 *  @param  [in]  pCertificate    Pointer to the ECKA certificate.
 *  @param  [in]  certificateSize The certificate size, in bytes.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 */
APDUSize APDUPerformSecurityOp(APDUOctet       *pBuffer,
                               APDUSize         bufferSize,
                               APDUOctet        logicalChannel,
                               APDUOctet        keyIdentifier,
                               APDUOctet        keyVersion,
                               const APDUOctet *pCertificate,
                               APDUSize         certificateSize);

/**
 *  @brief  Construct a MUTUAL_AUTHENTICATE C-APDU in the buffer given.
 *
 *          The form of the MUTUAL_AUTHENTICATE C-APDU is as described in
 *          the GP Card Specification Amendment F (SCP11) to pass Host ECKA
 *          information to the eSE and to elicit corresponding information.
 *
 *          This APDU is used during link setup for SCP11a
 *
 *  @param  [out] pBuffer         Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize      The buffer size, in bytes.
 *  @param  [in]  logicalChannel  The Logical Channel value to go into
 *                                the CLA byte.
 *  @param  [in]  pInfo           SCP options and key information.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 *
 *  @see    APDUInternalAuthenticate
 */
typedef struct
{
  APDUOctet  keyIdentifier;           /**< Which eSE key is required */
  APDUOctet  keyVersion;              /**< The required version of the eSE key */
  APDUOctet  scpParams[2];            /**< SCP type and parameters */
  APDUOctet  keyUsage;                /**< Session Key usage: MAC or MAC+ENC */
  APDUOctet  keyType;                 /**< Session Key type */
  APDUOctet  keyLengthBytes;          /**< Session Key length, in bytes */
  APDUOctet *pHostId;                 /**< Host ID for Session Key derivation */
  APDUOctet  hostIdLength;            /**< Host ID length, in bytes */
  APDUOctet *pPublicKeyECKA;          /**< Ephemeral or Static Public Key for ECKA */
  APDUOctet  publicKeyECKALength;     /**< Length of Public Key, in bytes */

} APDUAuthenticateInfo;

APDUSize APDUMutualAuthenticate(APDUOctet                  *pBuffer,
                                APDUSize                    bufferSize,
                                APDUOctet                   logicalChannel,
                                const APDUAuthenticateInfo *pInfo);

/**
 *  @brief  Construct a INTERNAL_AUTHENTICATE C-APDU in the buffer given.
 *
 *          The form of the INTERNAL_AUTHENTICATE C-APDU is as described in
 *          the GP Card Specification Amendment F (SCP11) to pass Host ECKA
 *          information to the eSE and to elicit corresponding information.
 *
 *          This APDU is used during link setup for SCP11b
 *
 *  @param  [out] pBuffer         Pointer to the buffer to hold the C-APDU.
 *  @param  [in]  bufferSize      The buffer size, in bytes.
 *  @param  [in]  logicalChannel  The Logical Channel value to go into
 *                                the CLA byte.
 *  @param  [in]  pInfo           SCP options and key information.
 *
 *  @return The length of the C-APDU, if successful, or 0 otherwise.
 *
 *  @see    APDUMutualAuthenticate
 */
APDUSize APDUInternalAuthenticate(APDUOctet                  *pBuffer,
                                  APDUSize                    bufferSize,
                                  APDUOctet                   logicalChannel,
                                  const APDUAuthenticateInfo *pInfo);

/*==========================================================================*
 *                              END OF FILE                                 *
 *==========================================================================*/
#ifdef __cplusplus
}
#endif

#endif /* APDU_H_INCLUDED */

