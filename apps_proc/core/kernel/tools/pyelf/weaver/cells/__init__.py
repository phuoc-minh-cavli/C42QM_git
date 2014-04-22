##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2006-2010, Open Kernel Labs, Inc.
# 
# All rights reserved.
# 
# Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
#               National ICT Australia
#               http://www.ertos.nicta.com.au
# 
# Permission is granted by Open Kernel Labs, Inc., free of charge, to
# any person obtaining a copy of this software and any associated
# documentation files (the "Software") to deal with the Software without
# restriction, including (without limitation) the rights to use, copy,
# modify, adapt, merge, publish, distribute, communicate to the public,
# sublicense, and/or sell, lend or rent out copies of the Software, and
# to permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimers.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimers in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of Open Kernel Labs, Inc., nor the names of its
#       contributors, may be used to endorse or promote products derived
#       from this Software without specific prior written permission.
# 
# EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
# PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
# NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
# WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
# REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
# THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
# ERRORS, WHETHER OR NOT DISCOVERABLE.
# 
# TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
# NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
# THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
# LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
# OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
# OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
# OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
# CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
# CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
# DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
# CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
# DAMAGES OR OTHER LIABILITY.
# 
# If applicable legislation implies representations, warranties, or
# conditions, or imposes obligations or liability on Open Kernel Labs, Inc.
# or one of its contributors in respect of the Software that
# cannot be wholly or partly excluded, restricted or modified, the
# liability of Open Kernel Labs, Inc. or the contributor is limited, to
# the full extent permitted by the applicable legislation, at its
# option, to:
# a.  in the case of goods, any one or more of the following:
# i.  the replacement of the goods or the supply of equivalent goods;
# ii.  the repair of the goods;
# iii. the payment of the cost of replacing the goods or of acquiring
#  equivalent goods;
# iv.  the payment of the cost of having the goods repaired; or
# b.  in the case of services:
# i.  the supplying of the services again; or
# ii.  the payment of the cost of having the services supplied again.
# 
# The construction, validity and performance of this licence is governed
# by the laws in force in New South Wales, Australia.

"""
Package description and base class for elfweaver cell support.
"""

import glob
import os

from weaver.parse_spec import XmlCollector
from weaver.util import import_custom

def import_custom_cells(path):
    return import_custom(path, "cells", "cell.py")


class AbstractCell(XmlCollector):
    #pylint: disable-msg=R0913
    """
    Abstract base class for all cell support objects.

    An instance of this class is created for each cell element that is
    found in the XML file.  This object is responsible for parsing the
    subelements of the cell element and generating the relevant data
    structures.

    The protocol driving this class is as followed:

    1) When a new cell element is found an object of the relevant
       subclass will be created.

    2) cell.collect_xml() will be called to subelements of of the
       cell XML.

    3) After all elements have been processed, any cross cell dependencies are
       resolved with resolve_cross_cell_dependencies().

    4) After all elements have been processed,
       generate_dynamic_segments() will be called to allow the cell to
       create any further objects (such as additional memory regions)
       whose values and size depend on access to all of the processed
       data.

    5) image.layout() is called to place all objects in memory.

    6) cell.generate_init() will be called to place any cell specific
       initialisation data into buffers.

    When a subclass is imported is must call Cell.register() to map a
    cell tag to the relevant class.
    """

    element = NotImplemented

    def __init__(self):
        XmlCollector.__init__(self)
        self.name = None
        self.sas = None

    def collect_xml(self, elem, ignore_name, namespace, machine,
                    pools, kernel, image):
        """
        Parse XML elements and create an intermediate representation of
        the system.
        """
        raise NotImplementedError

    def resolve_cross_cell_dependencies(self, machine, image):
        """
        Resolve any references to objects in other cells.
        """
        pass

    def generate_dynamic_segments(self, machine, image):
        """
        Create any segments, such as bootinfo buffers, whose size depends
        on the parsed XML.
        """
        raise NotImplementedError

    def layout_cells_post(self, image, machine):
        raise NotImplementedError

    def generate_init(self, machine, image):
        """
        Generate the init script for the cell, placing it into a segment.
        """
        raise NotImplementedError

    def group_elf_segments(self, image):
        """
        Group ELF segments together intelligently, avoiding potential
        domain conflicts.
        """
        pass

    def get_rights_copy_data(self):
        """
        Return a cap.RightsCopyData object for use when resolving caps from
        other cells.
        """
        raise NotImplementedError
