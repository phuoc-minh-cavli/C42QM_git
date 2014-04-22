# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Provides a command line interface to the services provided by kpc.

.. data:: KEYPROVISION_TOOL_NAME

    Name of the tool

.. data:: KEYPROVISION_TOOL_VERSION

    Version of the tool

.. data:: DEF_KEYPROVISION_OUTPUT_DIR_NAME

    Name of the default output directory

.. data:: DEF_KEYPROVISION_OUTPUT_DIR_PATH

    Absolute path of the default output directory
"""

import sys
import traceback

from sectools.common.utils import c_path
from sectools.common.utils.c_base import dynamicToolStatus
from sectools.common.utils.c_logging import logger
from sectools.features.fuse_common.fuse_common import FuseCommonOptionParser, log_to_file
from sectools.features.kpc.kpc import KeyProvisionCore

# Tool name for command arg
CMD_ARG_TOOL_NAME = 'keyprovision'

# Name & version of the tool
KEYPROVISION_TOOL_NAME = 'KeyProvision'
KEYPROVISION_TOOL_VERSION = '3.0'

# Path definitions
DEF_KEYPROVISION_OUTPUT_DIR_PATH = dynamicToolStatus.toolDir

__version__ = KEYPROVISION_TOOL_NAME + ' ' + KEYPROVISION_TOOL_VERSION


class KeyProvisionOptionParser(FuseCommonOptionParser):
    """Parses and validates the command line arguments specific to KeyProvision."""

    _LIST_TAG = 'LIST'

    @property
    def c_description(self):
        """(str) Returns the description of the program."""
        return 'This program generates & validates the sec.dat/sec.elf file.'

    @property
    def c_version(self):
        """(str) Returns the version of the program."""
        return __version__

    def c_add_options(self):
        """Adds the command line args supported by keyprovision."""
        # Generating secdat
        gen_group = self.add_option_group('Generating sec.dat/sec.elf with or without sec.dat/sec.elf as input')
        gen_group.add_option('-u', '--user_config_path', metavar='<file>',
                             help='path to the user config file.')
        gen_group.add_option('-e', '--keymap_config_path', metavar='<file>',
                             help='path to the keyprovision mapping config file.')
        gen_group.add_option('-c', '--secimage_config_path', metavar='<file>',
                             help='path to the fuseblower config file.')
        gen_group.add_option('-s', '--secdat', metavar='<file>',
                             help='path to the sec.dat/sec.elf file as input for common_output or for validation.')
        gen_group.add_option('-p', '--platform', metavar='<platform>',
                             help='specify the platform, to get the config files as input related to platform')

        # Specifying the output location
        output_group = self.add_option_group('Specifying output location')
        output_group.add_option('-o', '--output_dir', metavar='<dir>',
                                help='directory to store output files. DEFAULT: "./"',
                                default=DEF_KEYPROVISION_OUTPUT_DIR_PATH)

        # Specifying the operation
        operations_group = self.add_option_group('Operations')
        operations_group.add_option('-g', '--generate', action='store_true',
                                    default=False, help='1. generates the sec.dat/sec.elf file using config files and the cmd is  "-u, -e, -g" '
                                                        '2. generates the sec.dat/sec.elf file using config files & input sec.dat/sec.elf and the cmd is "-u, -e, -g, -s" '
                                                        '3. generates the sec.dat/sec.elf file using platform and the cmd is "-p <platform> '
                                                        '4. generates the sec.dat/sec.elf file using platform & input sec.dat/sec.elf and the cmd is "-p <platform> -s')
        operations_group.add_option('-a', '--validate', action='store_true',
                                    default=False,
                                    help='validate the sec.dat/sec.elf file and the cmd is "-u, -e, -a, -s')
        operations_group.add_option('--sign', action='store_true',
                                    default=False,
                                    help='sign the signed/unsigned sec.elf file')
        operations_group.add_option('-l', '--verify_inputs', action='store_true',
                                    default=False,
                                    help='verify the command line options.')

    def c_validate(self):
        """Validates the command line args provided by the user.

        :raises: RuntimeError
        """
        args = self.parsed_args
        err = []

        if not args.platform:
            # Check the config paths
            if not (args.user_config_path and args.keymap_config_path):
                err.append('Provide the config files using '
                           '--user_config_path, --keymap_config_path, '
                           'or provide the platform details using -p <platform> ')

        if args.platform and (args.keymap_config_path or args.user_config_path or args.secimage_config_path):
            err.append('Provide either config files or platform as a input. '
                       'Do not provide both types of input for generation of sec.dat/sec.elf')

        err += self.c_validate_common_params() + self.c_validate_paths()
        self.c_raise_errors(err)


def print_summary():
    """Prints the summary of the actions performed by KeyProvision"""
    pass


def main(args):
    """Parses the command line arguments, performs any basic operations based on
    the parsed arguments and starts processing using the fbc module.
    """
    # Log to file
    log_to_file(c_path.join(args.output_dir, KeyProvisionCore.FEATURE_DIR), KEYPROVISION_TOOL_NAME)

    # Print the tool's launch command
    logger.debug('\n\n    KeyProvision launched as: "' + ' '.join(sys.argv) + '"\n')

    # Initialize KeyProvisionCore
    kpc = KeyProvisionCore(debug=args.debug)

    # Configure KeyProvisionCore
    if args.keymap_config_path or args.secimage_config_path:
        kpc.set_config_paths(oem_config_path=args.keymap_config_path,
                             user_config_path=args.user_config_path,
                             secimage_config_path=args.secimage_config_path)
    elif args.platform:
        kpc.set_config_paths(*kpc._config_dir_obj.get_chipset_config(args.platform))

    if args.secdat:
        kpc.secdat = args.secdat

    kpc.output_dir = c_path.join(args.output_dir)

    # Process
    kpc.process(verify_setup=args.verify_inputs,
                generate=args.generate,
                sign=args.sign,
                validate=args.validate)

    # Print summary of operations
    print_summary()


def parse_args(argv):
    return KeyProvisionOptionParser(argv).parsed_args

if __name__ == '__main__':
    try:
        # Call the main of the tool
        main(parse_args(sys.argv))

    except Exception:
        logger.debug(traceback.format_exc())
        logger.error(sys.exc_info()[1])
        sys.exit(1)

    except KeyboardInterrupt:
        print
        logger.error('Keyboard Interrupt Received. Exiting!')
        sys.exit(1)

    sys.exit(0)


