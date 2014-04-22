# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


import abc

from six import add_metaclass
from sectools.features.fuse_common.datamodel.auto_gen_model import \
    Cfg_Fuseblower


@add_metaclass(abc.ABCMeta)
class BaseDataParseGen(object):

    def __init__(self):
        self._data_model = Cfg_Fuseblower()

    @property
    def data_model(self):
        return self._data_model

    @data_model.setter
    def data_model(self, dm):
        self._data_model = dm

    @abc.abstractmethod
    def read(self, file_path):
        pass

    @abc.abstractmethod
    def write(self, file_path):
        pass
