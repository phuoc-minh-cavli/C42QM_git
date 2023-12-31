# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Nov 25, 2013

@author: hraghav
'''

from sectools.features.isc.parsegen.base_encdec import SecParseGenBaseEncDec


# Bin file type
BIN_FILE_TYPE = 'bin'


class SecParseGenBin(SecParseGenBaseEncDec):

    def __init__(self, data, imageinfo=None,
                 bin_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 validating=False,
                 signing=False,
                 parsegens=None,
                 sign_attr=False):

        super(SecParseGenBin, self).__init__(data,
                                             imageinfo=imageinfo,
                                             general_properties=general_properties,
                                             encdec=encdec,
                                             debug_dir=debug_dir,
                                             debug_prefix=debug_prefix,
                                             debug_suffix=debug_suffix,
                                             validating=validating,
                                             signing=signing,
                                             parsegens=parsegens,
                                             sign_attr=sign_attr)

        """
        # Check the arguments
        if imageinfo is not None:
            bin_properties = imageinfo.image_type.bin_properties
        if bin_properties is None:
            raise RuntimeError('BIN properties must not be None.')
        """

        # Check if file is encrypted
        encryption_params = ''
        if self.encdec is not None:
            data, encryption_params = self.encdec.extract_encryption_parameters(data)
            if encryption_params:
                decryptor = self.encdec.get_decryptor(encryption_params, self.encdec.get_dummy_key())
                data = decryptor.decrypt_segment(data, 0)

        self.data, self.encryption_params = data, encryption_params

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def file_type(cls):
        return BIN_FILE_TYPE

    def get_wrapped_data(self):
        return self.data

    def set_wrapped_data(self, data):
        self.data = data

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt

        if integrity_check:
            raise RuntimeError('Bin Images do not support integrity check.')
        if sign:
            raise RuntimeError('Bin Images do not support signing.')

        data = self.data
        if encrypt:
            data = self.encryption_params + self.encrypt_segment(data, 0)
        return data

    @property
    def data_to_sign(self):
        raise RuntimeError('Bin Images do not support signing')

    def contains_integrity_check(self):
        return False
