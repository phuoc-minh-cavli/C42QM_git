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
Support class for a single program cell.
"""

################
# FIXME: Jira ticket OLD-1566 - awiggins.
# Everything in here should go once the build system is in a state to be
# able to generate <cell> with <program> XML tags.
################

from weaver.ezxml import str_attr, bool_attr, long_attr, ParsedElement, \
        update_element, Element
from weaver.memobjs_xml import Memsection_el, Stack_el
from weaver.segments_xml import Patch_el, Segment_el
from weaver.cells.collection.thread import Thread_el
from weaver.cells.collection.irq import IRQ_el
from weaver.cells.collection.mutex import Mutex_el
from weaver.cells.collection.message_queue import MessageQueue_el
from weaver.cells.collection.device import UseDevice_el
from weaver.cells.collection.tzmonitor import TZMonitor_el
from weaver.cells.collection.pd import Program_el, PD_el
from weaver.cells.collection.physmem_item import  PhysMemItem_el
from weaver.cells.collection.attachment import Attachment_el
from weaver.cells.buffer_area import BufferArea_el
from weaver.cells.collection.shared_buffer import SharedBuffer_el

from weaver.cells.cell import Cell, Cell_el

# If the virq module is not present, treat as nops.
try:
    from weaver.cells.collection.virq import VIRQ_el, VInterruptLine_el, \
         VInterruptLines_el
except:
    VIRQ_el = Element('virq')
    VInterruptLine_el = Element('virtual_interrupt_line')
    VInterruptLines_el = Element('virtual_interrupt_lines')

# If the vserver module is not present, treat as nops.
try:
    from weaver.cells.collection.vserver import VServer_el
except:
    VServer_el = Element('vserver')

# If the channel module is not present, treat as nops.
try:
    from weaver.cells.collection.channel import Channel_el
except:
    Channel_el = Element('channel')

ProgramCell_el = update_element(Cell_el,
                                Segment_el, Patch_el, Thread_el, Attachment_el,
                                Memsection_el, Mutex_el, IRQ_el,
                                UseDevice_el, MessageQueue_el, BufferArea_el,
                                Stack_el, TZMonitor_el,
                                Channel_el, SharedBuffer_el, PhysMemItem_el,
                                VServer_el, VInterruptLines_el,
                                file = (str_attr, 'required'),
                                start = (str_attr, 'optional'),
                                caps = (long_attr, 'optional'),
                                threads = (long_attr, 'optional'),
                                priority = (long_attr, 'optional'),
                                timeslice = (long_attr, 'optional'),
                                affinity = (long_attr, 'optional'),
                                max_reply_caps = (long_attr, 'optional'),
                                init = (str_attr, 'optional'))
ProgramCell_el.remove_element(Program_el)
ProgramCell_el.remove_element(PD_el)
ProgramCell_el.set_name('program_cell')

class ProgramCell(Cell):
    """
    Cell for the single program kernel.
    """

    element = ProgramCell_el

    def __init__(self):
        Cell.__init__(self)

    def _get_el(self, el):
        """Handle an simple program cell Compound Object"""

        # Split our combined cell tag into a cell and program tag
        program_el = ParsedElement('program')
        program_el.name = 'MAIN'
        program_el.file = el.file
        program_el.manager = True
        program_el._path = el._path

        # Grab any program specific tag from the cell's XML
        for tag in ('caps', 'threads', 'start',
#                    'priority', 'timeslice', 'affinity',
                    'init', 'max_reply_caps',):
            if hasattr(el, tag):
                setattr(program_el, tag, getattr(el, tag))

        # Grab any relevant children from the cell's XML
        for tag in ('segment', 'patch', 'thread', 'attached', 'memsection',
                    'mutex', 'irq', 'irq', 'use_device', 'stack', 'message_queue',
                    'buffer_area', 'virtual_interrupt_line',
                    'virtual_interrupt_lines', 'channel', 'tzmonitor',
                    'shared_buffer', 'physmem_item', 'vserver', 'platform_control',
                    'processor_control', 'profile_control', 'intlock_control'):
            program_el.add_children(el.find_children(tag))

        return program_el

    def collect_xml(self, el, ignore_name, namespace, machine, pools, kernel,
                    image):
        program_el = self._get_el(el)
        el.add_children([program_el])

        # Call Cell's collect method
        Cell.collect_xml(self, el, ignore_name, namespace, machine, pools,
                         kernel, image)

ProgramCell.register()

