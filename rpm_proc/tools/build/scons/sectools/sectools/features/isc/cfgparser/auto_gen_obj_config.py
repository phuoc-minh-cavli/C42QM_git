"""
Auto generated classes
"""


class Cfg_Secimage:

    def __init__(self):
        self.parsegen = Cfg_Parsegen()
        self.signing = Cfg_Signing()
        self.images_list = Cfg_Images_List()
        self.data_provisioning = Cfg_Data_Provisioning()
        self.general_properties = Cfg_General_Properties()
        self.post_process = Cfg_Post_Process()
        self.metadata = Cfg_Metadata()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Parsegen:

    def __init__(self):
        self.image_types_list = Cfg_Image_Types_List()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Image_Types_List:

    def __init__(self):
        self.image_type = []

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Image_Type:

    def __init__(self):
        self.file_type = ''
        self.ewm_properties = Cfg_Ewm_Properties()
        self.mbn_properties = Cfg_Mbn_Properties()
        self.pmbl_properties = Cfg_Pmbl_Properties()
        self.elf_properties = Cfg_Elf_Properties()
        self.id = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Ewm_Properties:

    def __init__(self):
        self.image_entry = ''
        self.relocatable = False

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Mbn_Properties:

    def __init__(self):
        self.header_size = 0

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Pmbl_Properties:

    def __init__(self):
        self.min_size_with_pad = 0
        self.preamble_size = 0
        self.page_size = 0
        self.has_magic_num = False
        self.num_of_pages = 0
        self.ota_enabled = False

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Elf_Properties:

    def __init__(self):
        self.has_multi_image_segment = False
        self.has_license_manager_segment = False
        self.image_type = 0
        self.has_sections = False
        self.validate_vir_addrs = False
        self.max_elf_segments = 0
        self.hash_seg_placement = ''
        self.validate_ph_addrs = True

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Signing:

    def __init__(self):
        self.signer_attributes = Cfg_Signer_Attributes()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Signer_Attributes:

    def __init__(self):
        self.cass_signer_attributes = Cfg_Cass_Signer_Attributes()
        self.remote_client_signer_attributes = Cfg_Remote_Client_Signer_Attributes()
        self.remote_signer_attributes = Cfg_Remote_Signer_Attributes()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Cass_Signer_Attributes:

    def __init__(self):
        self.server = Cfg_Server()
        self.user_identity = Cfg_User_Identity()

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Server:

    def __init__(self):
        self.trust_keystore = ''
        self.trust_keystore_password = ''
        self.host = ''
        self.host_validation_mode = ''
        self.trust_keystore_type = ''
        self.port = 0

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_User_Identity:

    def __init__(self):
        self.token_identity_slot = 0
        self.keystore_type = ''
        self.keystore_file = ''
        self.token_password = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Remote_Client_Signer_Attributes:

    def __init__(self):
        self.send_signing_overrides = False

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Remote_Signer_Attributes:

    def __init__(self):
        self.allow_signing_overrides = False

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Images_List:

    def __init__(self):
        self.image = []

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Image:

    def __init__(self):
        self.post_process_commands = ''
        self.name = ''
        self.general_properties_overrides = Cfg_General_Properties_Overrides()
        self.meta_build_location = ''
        self.image_type = ''
        self.pil_split = False
        self.cert_config = ''
        self.sign_id = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_General_Properties_Overrides:

    def __init__(self):
        self.dsa_type = ''
        self.rot_en = ''
        self.lib_id = ''
        self.soc_vers = ''
        self.app_id = ''
        self.multi_image_segment_addr = ''
        self.msm_part = ''
        self.max_cert_size = 2048
        self.uie_key_switch_enable = ''
        self.UIE_root_key_type = ''
        self.use_serial_number_in_signing = ''
        self.in_use_soc_hw_version = ''
        self.exponent = 3
        self.num_certs_in_certchain = 2
        self.ecdsa_curve = ''
        self.object_id = Cfg_Object_Id()
        self.anti_rollback_version = ''
        self.serial_number = ''
        self.UIE_server_cert_path = ''
        self.model_id = ''
        self.mask_soc_hw_version = ''
        self.root_revoke_activate_enable = ''
        self.segment_hash_algorithm = ''
        self.UIE_server_url = ''
        self.qti_sign = False
        self.cass_attest_cert = ''
        self.num_root_certs = 0
        self.debug = ''
        self.UIE_capability = ''
        self.sw_id = ''
        self.client_id = ''
        self.activation_enablement = ''
        self.max_num_root_certs = 1
        self.revocation_enablement = ''
        self.oem_sign = True
        self.soc_hw_version = ''
        self.oem_id_independent = ''
        self.UIE_feature_id = ''
        self.crash_dump = ''
        self.rsa_padding = ''
        self.UIE_key = ''
        self.key_size = 2048
        self.mrc_index = 0
        self.hash_pageseg_as_segment = False
        self.hash_algorithm = ''
        self.oem_id = ''
        self.testsig_serialnum = ''
        self.hmac = True
        self.multi_serial_numbers = Cfg_Multi_Serial_Numbers()
        self.secboot_version = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Object_Id:

    def __init__(self):
        self.max = ''
        self.name = ''
        self.min = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Multi_Serial_Numbers:

    def __init__(self):
        self.serial = []

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Data_Provisioning:

    def __init__(self):
        self.base_path = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_General_Properties:

    def __init__(self):
        self.rot_en = ''
        self.ecdsa_curve = ''
        self.cass_capability = ''
        self.lib_id = ''
        self.soc_vers = ''
        self.app_id = ''
        self.multi_image_segment_addr = ''
        self.msm_part = ''
        self.max_cert_size = 2048
        self.uie_key_switch_enable = ''
        self.selected_cert_config = ''
        self.UIE_root_key_type = ''
        self.use_serial_number_in_signing = ''
        self.in_use_soc_hw_version = ''
        self.exponent = 3
        self.num_certs_in_certchain = 2
        self.dsa_type = ''
        self.object_id = Cfg_Object_Id()
        self.anti_rollback_version = ''
        self.serial_number = ''
        self.crash_dump = ''
        self.model_id = ''
        self.UIE_server_cert_path = ''
        self.root_revoke_activate_enable = ''
        self.segment_hash_algorithm = ''
        self.UIE_server_url = ''
        self.qti_sign = False
        self.cass_attest_cert = ''
        self.num_root_certs = 0
        self.debug = ''
        self.UIE_capability = ''
        self.sw_id = ''
        self.selected_signer = ''
        self.client_id = ''
        self.activation_enablement = ''
        self.max_num_root_certs = 1
        self.revocation_enablement = ''
        self.selected_encryptor = ''
        self.oem_sign = True
        self.soc_hw_version = ''
        self.oem_id_independent = ''
        self.UIE_feature_id = ''
        self.hash_pageseg_as_segment = False
        self.mask_soc_hw_version = ''
        self.rsa_padding = ''
        self.UIE_key = ''
        self.image_entry = ''
        self.key_size = 2048
        self.mrc_index = 0
        self.hash_algorithm = ''
        self.oem_id = ''
        self.testsig_serialnum = ''
        self.hmac = True
        self.multi_serial_numbers = Cfg_Multi_Serial_Numbers()
        self.secboot_version = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Post_Process:

    def __init__(self):
        self.pil_splitter = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))



class Cfg_Metadata:

    def __init__(self):
        self.chipset = ''
        self.version = ''

    def __str__(self):
        return 'Instance of ' + self.__class__.__name__

    def __repr__(self):
        retval = ''
        for attr in sorted(self.__dict__.keys()):
            value = self.__dict__[attr]
            retval += (str(attr) + ': ' + '\n'
                       '    ' + repr(value) + '\n')
        return (self.__class__.__name__ + ':' + '\n' +
                '\n'.join(['  ' + line for line in retval.split('\n')]))


