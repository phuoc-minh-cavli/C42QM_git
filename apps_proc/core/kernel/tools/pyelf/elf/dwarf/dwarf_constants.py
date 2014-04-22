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
Contains constants for the values found in DWARF debugging sections.

Where applicable, names and values are taken from dwarf.h of libdwarf.
"""

from elf.util import IntString


class DwarfChildren(IntString):
    "Symbolically represent whether or not a DIE has children"
    _show = {}
DW_CHILDREN_no   = DwarfChildren(0, "[no children]")
DW_CHILDREN_yes  = DwarfChildren(1, "[has children]")

class DwarfTag(IntString):
    _show = {}
    "Symbolically represent the tag of a DIE"
TAG_padding                 = DwarfTag(0x0000,"DW_TAG_padding               ")
TAG_array_type              = DwarfTag(0x0001,"DW_TAG_array_type            ")
TAG_class_type              = DwarfTag(0x0002,"DW_TAG_class_type            ")
TAG_entry_point             = DwarfTag(0x0003,"DW_TAG_entry_point           ")
TAG_enumeration_type        = DwarfTag(0x0004,"DW_TAG_enumeration_type      ")
TAG_formal_parameter        = DwarfTag(0x0005,"DW_TAG_formal_parameter      ")
TAG_global_subroutine       = DwarfTag(0x0006,"DW_TAG_global_subroutine     ")
TAG_global_variable         = DwarfTag(0x0007,"DW_TAG_global_variable       ")
TAG_label                   = DwarfTag(0x000a,"DW_TAG_label                 ")
TAG_lexical_block           = DwarfTag(0x000b,"DW_TAG_lexical_block         ")
TAG_local_variable          = DwarfTag(0x000c,"DW_TAG_local_variable        ")
TAG_member                  = DwarfTag(0x000d,"DW_TAG_member                ")
TAG_pointer_type            = DwarfTag(0x000f,"DW_TAG_pointer_type          ")
TAG_reference_type          = DwarfTag(0x0010,"DW_TAG_reference_type        ")
TAG_compile_unit            = DwarfTag(0x0011,"DW_TAG_compile_unit          ")
TAG_string_type             = DwarfTag(0x0012,"DW_TAG_string_type           ")
TAG_structure_type          = DwarfTag(0x0013,"DW_TAG_structure_type        ")
TAG_subroutine              = DwarfTag(0x0014,"DW_TAG_subroutine            ")
TAG_subroutine_type         = DwarfTag(0x0015,"DW_TAG_subroutine_type       ")
TAG_typedef                 = DwarfTag(0x0016,"DW_TAG_typedef               ")
TAG_union_type              = DwarfTag(0x0017,"DW_TAG_union_type            ")
TAG_unspecified_parameters  = DwarfTag(0x0018,"DW_TAG_unspecified_parameters")
TAG_variant                 = DwarfTag(0x0019,"DW_TAG_variant               ")
TAG_common_block            = DwarfTag(0x001a,"DW_TAG_common_block          ")
TAG_common_inclusion        = DwarfTag(0x001b,"DW_TAG_common_inclusion      ")
TAG_inheritance             = DwarfTag(0x001c,"DW_TAG_inheritance           ")
TAG_inlined_subroutine      = DwarfTag(0x001d,"DW_TAG_inlined_subroutine    ")
TAG_module                  = DwarfTag(0x001e,"DW_TAG_module                ")
TAG_ptr_to_member_type      = DwarfTag(0x001f,"DW_TAG_ptr_to_member_type    ")
TAG_set_type                = DwarfTag(0x0020,"DW_TAG_set_type              ")
TAG_subrange_type           = DwarfTag(0x0021,"DW_TAG_subrange_type         ")
TAG_with_stmt               = DwarfTag(0x0022,"DW_TAG_with_stmt             ")
TAG_access_declaration      = DwarfTag(0x0023,"DW_TAG_access_declaration    ")
TAG_base_type               = DwarfTag(0x0024,"DW_TAG_base_type             ")
TAG_const_type              = DwarfTag(0x0026,"DW_TAG_const_type            ")
TAG_enumerator              = DwarfTag(0x0028,"DW_TAG_enumerator            ")
TAG_subprogram              = DwarfTag(0x002e,"DW_TAG_subprogram            ")
TAG_variable                = DwarfTag(0x0034,"DW_TAG_variable              ")
TAG_volatile_type           = DwarfTag(0x0035,"DW_TAG_volatile_type         ")

class DwarfForm(IntString):
    "Symbolically represent the form of a DIE's attributes"
    _show = {}
FORM_ADDR      = DwarfForm(0x1,"FORM_ADDR     ")
FORM_REF       = DwarfForm(0x2,"FORM_REF      ")
FORM_BLOCK2    = DwarfForm(0x3,"FORM_BLOCK2   ")
FORM_BLOCK4    = DwarfForm(0x4,"FORM_BLOCK4   ")
FORM_DATA2     = DwarfForm(0x5,"FORM_DATA2    ")
FORM_DATA4     = DwarfForm(0x6,"FORM_DATA4    ")
FORM_DATA8     = DwarfForm(0x7,"FORM_DATA8    ")
FORM_STRING    = DwarfForm(0x8,"FORM_STRING   ")
FORM_BLOCK     = DwarfForm(0x9,"FORM_BLOCK    ")
FORM_BLOCK1    = DwarfForm(0xa,"FORM_BLOCK1   ")
FORM_DATA1     = DwarfForm(0xb,"FORM_DATA1    ")
FORM_FLAG      = DwarfForm(0xc,"FORM_FLAG     ")
FORM_SDATA     = DwarfForm(0xd,"FORM_SDATA    ")
FORM_STRP      = DwarfForm(0xe,"FORM_STRP     ")
FORM_UDATA     = DwarfForm(0xf,"FORM_UDATA    ")
FORM_REF_ADDR  = DwarfForm(0x10,"FORM_REF_ADDR ")
FORM_REF1      = DwarfForm(0x11,"FORM_REF1     ")
FORM_REF2      = DwarfForm(0x12,"FORM_REF1     ")
FORM_REF4      = DwarfForm(0x13,"FORM_REF4     ")
FORM_REF8      = DwarfForm(0x14,"FORM_REF8     ")
FORM_REF_UDATA = DwarfForm(0x15,"FORM_REF_UDATA")
FORM_INDIRECT  = DwarfForm(0x16,"FORM_INDIRECT ")

class DwarfAttribute(IntString):
    "Symbolically represent the meaning of a DIE's attributes"
    _show = {}
AT_sibling       = DwarfAttribute(0x01,"DW_AT_sibling     ")
AT_location      = DwarfAttribute(0x02,"DW_AT_location    ")
AT_name          = DwarfAttribute(0x03,"DW_AT_name        ")
AT_fund_type     = DwarfAttribute(0x05,"DW_AT_fund_type   ")
AT_mod_fund_type = DwarfAttribute(0x06,"DW_AT_mod_fund_type")
AT_user_def_type = DwarfAttribute(0x07,"DW_AT_user_def_type")
AT_mod_u_d_type  = DwarfAttribute(0x08,"DW_AT_mod_u_d_type")
AT_ordering      = DwarfAttribute(0x09,"DW_AT_ordering    ")
AT_subscr_data   = DwarfAttribute(0x0a,"DW_AT_subscr_data ")
AT_byte_size     = DwarfAttribute(0x0b,"DW_AT_byte_size   ")
AT_bit_offset    = DwarfAttribute(0x0c,"DW_AT_bit_offset  ")
AT_bit_size      = DwarfAttribute(0x0d,"DW_AT_bit_size    ")
AT_element_list  = DwarfAttribute(0x0f,"DW_AT_element_list")
AT_stmt_list     = DwarfAttribute(0x10,"DW_AT_stmt_list   ")
AT_low_pc        = DwarfAttribute(0x11,"DW_AT_low_pc      ")
AT_high_pc       = DwarfAttribute(0x12,"DW_AT_high_pc     ")
AT_language      = DwarfAttribute(0x13,"DW_AT_language    ")
AT_member        = DwarfAttribute(0x14,"DW_AT_member      ")
AT_discr         = DwarfAttribute(0x15,"DW_AT_discr       ")
AT_discr_value   = DwarfAttribute(0x16,"DW_AT_discr_value ")
AT_comp_dir      = DwarfAttribute(0x1b,"DW_AT_comp_dir    ")
AT_const_value   = DwarfAttribute(0x1c,"DW_AT_const_value ")
AT_inline        = DwarfAttribute(0x20,"DW_AT_inline      ")
AT_producer      = DwarfAttribute(0x25,"DW_AT_producer    ")
AT_prototyped    = DwarfAttribute(0x27,"DW_AT_prototyped  ")
AT_upper_bound   = DwarfAttribute(0x2f,"DW_AT_upper_bound ")
AT_abstract_origin      = DwarfAttribute(0x31,"DW_AT_abstract_origin")
AT_artificial    = DwarfAttribute(0x34,"DW_AT_artificial  ")
AT_data_member_location = DwarfAttribute(0x38,"DW_AT_data_member_location")
AT_decl_file     = DwarfAttribute(0x3a,"DW_AT_decl_file   ")
AT_decl_line     = DwarfAttribute(0x3b,"DW_AT_decl_line   ")
AT_declaration   = DwarfAttribute(0x3c,"DW_AT_declaration ")
AT_encoding      = DwarfAttribute(0x3e,"DW_AT_encoding    ")
AT_external      = DwarfAttribute(0x3f,"DW_AT_external    ")
AT_frame_base    = DwarfAttribute(0x40,"DW_AT_frame_base  ")
AT_type          = DwarfAttribute(0x49,"DW_AT_type        ")
AT_ranges        = DwarfAttribute(0x55,"DW_AT_ranges      ")
AT_call_file     = DwarfAttribute(0x58,"DW_AT_call_file   ")
AT_call_line     = DwarfAttribute(0x59,"DW_AT_call_line   ")
