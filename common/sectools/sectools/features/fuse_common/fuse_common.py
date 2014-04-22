# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Classes and functions common across fuseblower and keyprovision features
"""
import abc
import os
from glob import glob

from sectools.common.utils import c_path
from sectools.common.utils.c_base import CoreOptionParser
from sectools.common.utils.c_logging import logger
from sectools.features.fbc.parsegen.fb_config import ConfigParser as FuseblowerConfigParser
from sectools.features.fbc.parsegen.fb_config.defines import ROOTNODE_NAME as FUSEBLOWER
from sectools.features.fuse_common import defines
from sectools.features.fuse_common.defines import SECDAT_TYPE_ELF, SECDAT_TYPE_BIN
from sectools.features.fuse_common.fcc import get_secdat_type
from sectools.features.isc.secimage.secimage_cfgparser import SecImageConfigParser
from sectools.features.kpc.parsegen.fb_config import ConfigParser as KeyprovisionConfigParser

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
search_path = os.path.join(CURRENT_DIR, '..', '..', '..', 'config')


class FuseCommonOptionParser(CoreOptionParser):

    @abc.abstractproperty
    def c_description(self):
        """(abstractproperty) (str) Returns the description of the program."""
        pass

    @abc.abstractproperty
    def c_version(self):
        """(abstractproperty) (str) Returns the version of the program."""
        pass

    @abc.abstractmethod
    def c_validate(self):
        """(abstractmethod) Validates the command line arguments that have been
        parsed. If there is any errors in the arguments, raises a RuntimeError.

        :raises: RuntimeError
        """
        pass

    @abc.abstractmethod
    def c_add_options(self):
        """(abstractmethod) Adds the command line arguments to the parser.
        The arguments should be added to "self" as :class:`CoreOptionParser`
        derives from OptionParser.

        ::

            self.add_option('-s', '--source', help='source directory')

        """
        pass

    """validates common command line parameters by features, validates paths and raises errors if any"""
    def c_validate_common_params(self):
        """Validates signing parameters passed by the user
        :raises: RuntimeError
        """
        args = self.parsed_args
        err = list()

        secelf_supporting_chipsets = get_chipsets_supporting_secelf()
        secdat_type = None
        if args.secdat:
            secdat_type = get_secdat_type(args.secdat)

        if not args.platform:
            xmls = ['user_config_path',
                    'oem_config_path',
                    'qti_config_path',
                    'keymap_config_path',
                    'secimage_config_path'
                     ]
            current_chipset = get_chipset_from_config_xmls(self.c_prog,  # Feature
                                                           getattr(args, xmls[0], None),   # User xml
                                                           getattr(args, xmls[1], None),   # Oem xml
                                                           getattr(args, xmls[2], None),   # Qti xml
                                                           getattr(args, xmls[3], None),   # Keymap xml
                                                           getattr(args, xmls[4], None),   # Secimage xml
                                                           )
            if current_chipset in secelf_supporting_chipsets:
                if args.sign and not args.secimage_config_path:
                    err.append('--secimage_config_path not provided. If signed sec.elf is desired,'
                               ' provide --secimage_config_path as well along with --sign.'
                               ' Targets supporting --sign and/or --secimage_config_oath '
                               'are: \n\t' + str(secelf_supporting_chipsets) +
                               '\n If not sure, use -p to generate sec.dat/sec.elf')
                if not args.sign and args.secimage_config_path:
                    err.append('--sign not provided. If signed sec.elf is desired,'
                               ' provide --sign along with --secimage_config_path. '
                               'Targets supporting --sign and/or --secimage_config_path '
                               'are: \n\t' + str(secelf_supporting_chipsets) +
                               '\n If not sure, use -p to generate sec.dat/sec.elf')
                if args.secdat:
                    if secdat_type == SECDAT_TYPE_BIN:
                        err.append('Input sec.dat is not supported for chipset ' + str(current_chipset) +
                                   '. Provide input sec.elf/sec_unsigned.elf only')
            else:
                if args.sign or args.secimage_config_path:
                    err.append('--secimage_config_path or --sign not supported for ' + str(current_chipset) +
                               '. Targets supporting --sign and/or --secimage_config_path '
                               'are: \n\t' + str(get_chipsets_supporting_secelf()))
                if args.secdat:
                    if secdat_type == SECDAT_TYPE_ELF:
                        err.append('Input sec.elf is not supported for chipset ' + str(current_chipset) +
                                   '. Provide input sec.dat only')

        if not (args.generate or args.validate or args.verify_inputs or args.sign):
            err.append('Specify one or more operations to perform')
        if (args.validate or args.sign) and not args.generate:
            if not args.secdat:
                err.append('Provide path to the sec.dat/sec.elf file using -s for signing/validation')
        if args.secdat and args.platform:
            if args.platform in secelf_supporting_chipsets:
                if secdat_type == SECDAT_TYPE_BIN:
                    err.append('Input sec.dat is not supported for chipset ' + str(args.platform) +
                               '. Provide input sec.elf/sec_unsigned.elf only')
            else:
                if args.sign:
                    err.append('--sign not supported for ' + str(args.platform) +
                               '. Targets supporting --sign are: \n\t' + str(get_chipsets_supporting_secelf()))
                if secdat_type == SECDAT_TYPE_ELF:
                    err.append('Input sec.elf is not supported for chipset ' + str(args.platform) +
                               '. Provide input sec.dat only')
        return err

    def c_validate_paths(self):
        args = self.parsed_args
        err = list()
        paths = ['user_config_path',
                 'oem_config_path',
                 'qti_config_path',
                 'secimage_config_path',
                 'keymap_config_path'
                 'secdat']

        for path in paths:
                path_val = getattr(args, path, None)
                if path_val:
                    path_val = c_path.normalize(path_val)
                    if not c_path.validate_file(path_val):
                        err.append('Cannot access ' + path + ' at: ' + path_val)
                    setattr(args, path, path_val)

        # Check and sanitize output directory for write access
        for path in ['output_dir']:
            path_val = getattr(args, path, None)
            if path_val:
                path_val = c_path.normalize(path_val)
                try:
                    c_path.create_dir(path_val)
                except Exception as e:
                    err.append('Cannot write at: ' + path_val + '\n'
                               '    ' + 'Error: ' + str(e))
        return err

    def c_raise_errors(self, err):
        if err:
            if len(err) > 1:
                err = [('  ' + str(idx + 1) + '. ' + error) for idx, error in enumerate(err)]
                err = 'Check the command line args:\n\n' + '\n'.join(err)
            else:
                err = 'Check the command line args:\n\n' + err[0]
            raise RuntimeError(err)


#----------------------------------------------------------
# Common Utility functions
#----------------------------------------------------------
def log_to_file(folder, filename):
    """Configures the logger to log to filesystem
    :param str folder: Directory to generate the logs in.
    """
    folder = c_path.normalize(folder)
    try:
        c_path.create_dir(folder)
    except Exception as e:
        raise RuntimeError('Unable to create directory for logging: ' + folder + '\n'
                           '    ' + 'Error: ' + str(e))
    logger.enable_file_logging(filename, num_logs=1, log_dir=folder)


def get_chipsets_supporting_secelf():
    files = [y for x in os.walk(search_path) for y in glob(os.path.join(x[0], '*' + defines.XML_NAME_END_SECIMAGE))]
    targets_supported = list()
    for f in files:
        base, extn = os.path.splitext(os.path.basename(f))
        targets_supported.append(base.split('_')[0])
    return targets_supported


def get_chipset_from_secimage_config(secimage_config_path):
    chipset_from_file = None
    secimage_config_path = c_path.normalize(secimage_config_path)
    if c_path.validate_file(secimage_config_path):
        try:
            chipset_from_file = SecImageConfigParser(secimage_config_path).chipset
        except Exception:
            pass
    return chipset_from_file


def get_chipset_from_feature_config(feature, config_path):
    chipset_from_file = None
    configParser = FuseblowerConfigParser if feature == FUSEBLOWER else KeyprovisionConfigParser
    if c_path.validate_file(config_path):
        try:
            chipset_from_file = configParser.get_chipset_from_file(config_path)
        except Exception:
            pass
    return chipset_from_file


def get_chipset_from_config_xmls(feature, user_config, oem_config, qti_config, keymap_config, secimage_config):
    if not user_config:
        raise RuntimeError("Provide user config file. It is not provided")
    user_config = c_path.normalize(user_config)
    if c_path.validate_file(user_config):
        reference_chipset = get_chipset_from_feature_config(feature, user_config)
        if not reference_chipset:
            raise RuntimeError("Failed to determine chipset from " + user_config)
    else:
        raise RuntimeError("Failed to load user config file")

    xmls = [oem_config,
            qti_config,
            keymap_config
            ]
    for f in xmls:
        if f is not None:
            f = c_path.normalize(f)
            if c_path.validate_file(f):
                chipset_from_feature_config = get_chipset_from_feature_config(feature, f)
                if not chipset_from_feature_config:
                    raise RuntimeError("Failed to load the file".format(f))
                if chipset_from_feature_config != reference_chipset:
                    raise RuntimeError("Chipset [" + str(reference_chipset) +
                                       "] in " + str(user_config) + "does not match " +
                                       "chipset [" + str(chipset_from_feature_config) + "] in " + str(f))

    if secimage_config:
        chipset_from_secimage_config = get_chipset_from_secimage_config(secimage_config)
        if not chipset_from_secimage_config:
            raise RuntimeError("Failed to load the file".format(secimage_config))
        if reference_chipset != chipset_from_secimage_config:
            raise RuntimeError("Chipset [" + str(reference_chipset) +
                               "] in all config xmls do not match " +
                               "chipset [" + str(chipset_from_secimage_config) + "] in " + str(secimage_config))
    return reference_chipset
