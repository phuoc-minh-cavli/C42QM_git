# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import abc

from six import add_metaclass


@add_metaclass(abc.ABCMeta)
class BaseValidator(object):
    """
    validator base class
    """

    def __init__(self):
        """
        Constructor
        """
        print('I am ' + self.val_type)

    @abc.abstractproperty
    def val_type(self):
        return 'BASE'

    @abc.abstractproperty
    def validate(self):
        pass


__all__ = ['secdat_validator',
          ]
