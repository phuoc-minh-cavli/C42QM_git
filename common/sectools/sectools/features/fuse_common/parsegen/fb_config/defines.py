# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the cfgparser package.

Constants relate to:

#. Common constants
#. Auto-generated FB XML Parser - auto_gen_xml_config
"""

from sectools.common.utils import c_config
from sectools.features.fuse_common.datamodel import defines as dm_def

#------------------------------------------------------------------------------
# Config Data Model Structure
#------------------------------------------------------------------------------
OBJ_STRUCTURE = \
{
    'metadata' :
    {
        'chipset' : '',
        'version' : '',
    },

    'module' : dm_def.OBJ_STRUCTURE['module'],
}


ENTRIES_STRUCTURE = {
                        'metadata' :
                        {
                            'chipset' : '',
                            'version' : '',
                        },

                        'secdat' :
                        {
                            'file_info' : '',
                            'file_version' : (0,),
                            'fuse_version' : (0,),
                        },

                        'elf' :
                        {
                            c_config.DICT.DictHandlerKey_Attr : [c_config.DICT.DictHandlerAttr_Optional],
                            'elf_class' : ((32, 64),),
                            'phys_addr' : (c_config.HEX),
                        },

                        'module' :
                        {
                            'entry':
                            [
                                {
                                    'description':'',
                                    'name' : '',
                                    'value' : '',
                                    'ignore' : (True,),
                                }
                            ]
                        }
                    }

# FEC bits are 7 bits long, mask 0x7F : 0111 1111
FEC_BIT_MASK=0x7F

# FEC DATA region bit mask, 32 lower bits, mask 0xFFFFFFFF
FEC_DATA_32BIT_MASK=0xFFFFFFFF

# FEC DATA region bit mask, 48 lower bits, mask 0xFFFFFFFFFFFF
FEC_DATA_48BIT_MASK = 0xFFFFFFFFFFFF

# FEC DATA region bit mask, 56 lower bits, mask 0xFFFFFFFFFFFFFF
FEC_DATA_56BIT_MASK=0xFFFFFFFFFFFFFF

# FEC DATA region bit mask, 64 lower bits, mask 0xFFFFFFFFFFFFFFFF
FEC_DATA_64BIT_MASK=0xFFFFFFFFFFFFFFFF

# FEC bits are 5 bits long, mask 0x1F : 0001 1111
FEC_15_10_BIT_MASK=0x1F

# chipsets that have FEC_EN bit in row instead of FEC_EN region
FEC_IN_ROW = ['8016','8916','8039','8939','8909','8929','8952','401x','8909W', '8905', '8009lat']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region & different from
# other 8084, 8994 chipsets and so on.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region11
# OEM_PK_HASH FEC bit in region13
DIFF_FEC_REG = ['8976','8937','8953','8076','8940','9607','8917','8920','807x','9206',
                'sdm450', '9x50', '9206tx', '8053lat', 'sdm632', 'sdm439', 'sdx50m', 'qm215']

# Same as DIFF_FEC_REG where additionally shadow region also needs to be set
FEC_SHADOW = ['9607', '9206', '9206tx']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Nazgul. This includes
# Nazgul (8998), Starlord (sdm660) and Starhawk (sdm630) and so on.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region20
# PK_HASH1 FEC bit in region25
# IMAGE_ENCR_KEY1 FEC bit in region21
DIFF_FEC_REG_NAZGUL = ['8998', 'sdm660', 'sdm630', 'sdm845', 'sdm670', '8998-sdx50m', 'sm8150', 'sdx24', 'sm6150', 'sc8180x','sm7150']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Cypress(ipq6018).
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region11
# PK_HASH0 FEC bit in region13
# IMAGE_ENCR_KEY0 FEC bit in region21
DIFF_FEC_REG_CYP = ['ipq6018']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Nicobar.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region22
# PK_HASH0 FEC bit in region15
# IMAGE_ENCR_KEY FEC bit in region21
DIFF_FEC_REG_NIC = ['nicobar', 'kamorta']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Saipan.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region22
# No PK_HASH0 FEC Support
# IMAGE_ENCR_KEY FEC bit in region21
DIFF_FEC_REG_SAI = ['saipan', 'rennell', 'bitra', 'agatti', 'mannar']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Vipertooth.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region19
# PK_HASH0 FEC bit in region12
# IMAGE_ENCR_KEY FEC bit in region18
DIFF_FEC_REG_VT = ['qcs405']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Twizy.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region21
# PK_HASH0 FEC bit in region15(OEM_PK_HASH)
# IMAGE_ENCR_KEY FEC bit in region20
DIFF_FEC_REG_TW = ['9205']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Kona and Huracan.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region23
# No PK_HASH0 FEC Support
# IMAGE_ENCR_KEY FEC bit in region22
DIFF_FEC_KONA_HURACAN_FUSION = ['sdx55', 'sm8250', 'lahaina', 'cedros']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Olympic.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region23
# No PK_HASH0 FEC Support
# No IMAGE_ENCR_KEY Support
DIFF_FEC_OLYMPIC = ['chitwan', 'olympic']


# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Makena.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region31
# No PK_HASH0 FEC Support
# IMAGE_ENCR_KEY FEC bit in region30
DIFF_FEC_MAKENA = ['makena']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Hastings.
# These chipsets have:
# PK_HASH0 FEC bit in region5(OEM_PK_HASH)
DIFF_FEC_HASTINGS = ['6390']

# For Keyprovision: chipsets that have FEC_EN bit in FEC_EN region & different from other 8976, 8937 and so on.
KP_FEC_REG = ['8953','8937','8940','8917','8920','sdm450', '8053lat', 'sdm632', 'sdm439', 'qm215']

# Chipsets that support ROT feature
ROT_SUPPORTED = ['8992','8994','8996','8998', '8998-sdx50m']

IPQ_CHIPSET = ['806x']

# Chipsets that does not support FEC bits/Region
IPQ_CHIPSET_NO_FEC_REG = ['ipq5018']

# MRC 2.0 supported chipsets
MRC_2_0_CHIPSETS = ['sdm845', 'sdm670', 'sm8150', 'sdx24', 'sm6150', 'sc8180x', 'nicobar',
                    'sdx55', 'sm8250', 'chitwan', 'cedros', 'mannar', 'makena', 'olympic']
