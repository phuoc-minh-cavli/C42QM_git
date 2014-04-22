#===============================================================================
#
# Dynamic Loading
#
# GENERAL DESCRIPTION
#    Contains all MBN Utilities for image generation 
#
# Copyright 2010-2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tx/6.0/bsp/build/scripts/modify_dynamic_section.py#1 $
#  $DateTime: 2020/01/30 22:49:35 $
#  $Author: pwbldsvc $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 12/15/14   amitkulk  Created
#===============================================================================

import sys
import os
import shutil
from elfparserutil import *

#------------------------------------------------------------------------------
# Get pasred dynamic section structure
#------------------------------------------------------------------------------
def get_dynamic_section(elf_out_file_fp, dynamic_section_offset, num_entries):
	dynamic_table= []
	elf_out_file_fp.seek(dynamic_section_offset)
	for i in range(num_entries):
		dynamic_table.append(Elf32_dynamic(elf_out_file_fp.read(8)))
		print str(dynamic_table[i].get_tag_str()) + "\t" + str(hex(dynamic_table[i].get_value()))
	print ""
	return dynamic_table

#------------------------------------------------------------------------------
# Get pasred dynamic section from a ELF file, when dynamic section offset is provided
#------------------------------------------------------------------------------
def get_dynamic_section_from_file(elf_out_file_name, dynamic_section_offset, num_entries):
	#Re-open and verify that we did it all correctly
	elf_out_fp = OPEN(elf_out_file_name, "rb")
	dynamic_table_veify = get_dynamic_section(elf_out_fp, dynamic_section_offset, num_entries) 
	return dynamic_table_veify

#------------------------------------------------------------------------------
# Write back to log file
#------------------------------------------------------------------------------
def write_back_to_log(log_out_file_name, log_buffer):
	log_out_fp = OPEN(log_out_file_name, 'w')
	log_out_fp.write(log_buffer)
	log_out_fp.close()
	return 0

#------------------------------------------------------------------------------
# Get the new values to be populated into the .dynamic table 
#------------------------------------------------------------------------------
def get_new_values(elf_out_dict):
	new_vaddr_values =[]
	err_string =""
	
	#search for the new dynamic string table location
	out_shdr_list_dynstr 		= query_section_by_name_raw(elf_out_dict, "DYNAMIC_STRING_TABLE")
	if not out_shdr_list_dynstr:
		err_string = "DYNAMIC_STRING_TABLE not found. Exiting.\n"
		raise RuntimeError, err_string
	else:
		new_vaddr_values.append(out_shdr_list_dynstr[0].sh_addr)

	out_shdr_list_dynsym        = query_section_by_name_raw(elf_out_dict, "DYNAMIC_SYMBOL_TABLE")
	if not out_shdr_list_dynsym:
		err_string = "DYNAMIC_SYMBOL_TABLE not found. Exiting.\n"
		raise RuntimeError, err_string
	else:
		new_vaddr_values.append(out_shdr_list_dynsym[0].sh_addr)

	out_shdr_list_dynhash 		= query_section_by_name_raw(elf_out_dict, "DYNAMIC_HASH_TABLE")
	if not out_shdr_list_dynsym:
		err_string = "DYNAMIC_HASH_TABLE not found. Exiting.\n"
		raise RuntimeError, err_string
	else:
		new_vaddr_values.append(out_shdr_list_dynhash[0].sh_addr)
		
	return new_vaddr_values

#------------------------------------------------------------------------------
# mapped to ElfDynamicUtilBuilder in elfdynamiutils.py
# Basically repopulates the newly patched .dynamic section in the ELF 
#------------------------------------------------------------------------------
def modify_dynamic_section(elf_file_name, elf_out_file_name):
	
	log_file_name = "dynamic_mod_script_log.txt"
	log_buffer = ""

	#Create a duplicate of the ELF, open for read/write, pre-process the ELF
	try:
		shutil.copyfile(elf_file_name, elf_out_file_name)
	except IOError:
		print "Unable to copy file " + elf_file_name + "to  destination " + elf_out_file_name
	
	elf_out_fp = OPEN(elf_out_file_name, "r+b")
	
    #Parse ELF
	elf_out_dict = preprocess_elf_file(elf_out_fp, elf_out_file_name)

	#Check if the .dynamic section is present in output ELF, we use this to get it's size, number of entries
	out_shdr_list = query_section_by_name_raw(elf_out_dict, ".dynamic")
	if not out_shdr_list:
		err_string = "Dynamic section not found\n"
		log_buffer+= err_string
		raise RuntimeError, err_str
		exit()
	
	#check if multiple of dynamic sections are present
	if 1 != len(out_shdr_list):
		err_string = str(len(out_shdr_list)) + "number of dynamic sections found\n"
		log_buffer+= err_string
		raise RuntimeError, err_str
	
	#Calculate Dynamic section details, prepare for parsing
	dynamic_section_offset       = out_shdr_list[0].sh_offset
	dynamic_section_size         = out_shdr_list[0].sh_size
	dynamic_section_num_entries  = (dynamic_section_size/8 -1)
	err_string = ".dynamic section offset: " + str(hex(dynamic_section_offset)) + "| section size:  " + str(hex(dynamic_section_size)) + "\n"
	DEBUG_PRINT(err_string)
	log_buffer += err_string

	#look for DYNAMIC_DATA,.dynstr,.dynsym and .hash sections for their sizes
	out_shdr_list_DYNAMIC_DATA  = query_section_by_name_raw(elf_out_dict, "DYNAMIC_DATA")
	if not out_shdr_list_DYNAMIC_DATA:
		err_string = "DYNAMIC_DATA not found. Exiting.\n"
		log_buffer+= err_string
		raise RuntimeError, err_str
	else:
		#All new v-addr will be relative of this section
		DYNAMIC_DATA_vaddr          = out_shdr_list_DYNAMIC_DATA[0].sh_addr
		DYNAMIC_DATA_offset         = out_shdr_list_DYNAMIC_DATA[0].sh_offset

	out_shdr_list_dynstr 		= query_section_by_name_raw(elf_out_dict, ".dynstr")
	if not out_shdr_list_dynstr:
		err_string = ".dynstr not found. Exiting.\n"
		log_buffer+= err_string
		raise RuntimeError, err_str

	out_shdr_list_dynsym        = query_section_by_name_raw(elf_out_dict, ".dynsym")
	if not out_shdr_list_dynsym:
		err_string = ".dynsym not found. Exiting.\n"
		log_buffer+= err_string
		raise RuntimeError, err_str

	out_shdr_list_dynhash 		= query_section_by_name_raw(elf_out_dict, ".hash")
	if not out_shdr_list_dynsym:
		err_string = ".hash not found. Exiting.\n"
		log_buffer+= err_string
		raise RuntimeError, err_str

	new_vaddr_values = get_new_values(elf_out_dict)

	dynstr_section_size 	     =  out_shdr_list_dynstr[0].sh_size
	dynstr_section_offset_new    =  dynamic_section_offset + dynstr_section_size
	dynstr_section_vaddr_new     =  new_vaddr_values[0]
	DEBUG_PRINT(".dynstr size: " + str(hex(dynstr_section_size)))
	DEBUG_PRINT(".dynstr offs: " + str(hex(dynstr_section_offset_new)))
	DEBUG_PRINT(".dynstr addr: " + str(hex(dynstr_section_vaddr_new)))
	
	dynsym_section_size          =  out_shdr_list_dynsym[0].sh_size
	dynsym_section_offset_new    =  dynamic_section_offset + dynsym_section_size
	dynsym_section_vaddr_new     =  new_vaddr_values[1]
	DEBUG_PRINT(".dynsym size: " + str(hex(dynsym_section_size)))
	DEBUG_PRINT(".dynsym offs: " + str(hex(dynsym_section_offset_new)))
	DEBUG_PRINT(".dynsym addr: " + str(hex(dynsym_section_vaddr_new)))
	 
	dynhash_section_size          =  out_shdr_list_dynhash[0].sh_size
	dynhash_section_offset_new    =  dynamic_section_offset + dynhash_section_size
	dynhash_section_vaddr_new     =  new_vaddr_values[2]
	DEBUG_PRINT(".dynhash size: " + str(hex(dynhash_section_size)))
	DEBUG_PRINT(".dynhash offs: " + str(hex(dynhash_section_offset_new)))
	DEBUG_PRINT(".dynhash addr: " + str(hex(dynhash_section_vaddr_new)))
	 
	#Parse '.dynamic' section
	dynamic_table = get_dynamic_section(elf_out_fp, dynamic_section_offset, dynamic_section_num_entries) 

	for i in range(dynamic_section_num_entries):
		log_buffer += (str(dynamic_table[i].get_tag_str()) + "\t" + str(hex(dynamic_table[i].get_value())) + '\n')

	#Overwrite the values with new ones within parsed data
	for i in range(dynamic_section_num_entries):
		if(dynamic_table[i].get_tag_str() == "DT_HASH"):
			dynamic_table[i].val = dynhash_section_vaddr_new
		elif(dynamic_table[i].get_tag_str() == "DT_STRTAB"):
			dynamic_table[i].val = dynstr_section_vaddr_new
		elif(dynamic_table[i].get_tag_str() == "DT_SYMTAB"):
			dynamic_table[i].val = dynsym_section_vaddr_new

	#Pack the parsed data and write back	
	elf_out_fp.seek(DYNAMIC_DATA_offset)
	for i in range(dynamic_section_num_entries):
		elf_out_fp.write(dynamic_table[i].get_packed_data())
    
	#Close file
	elf_out_fp.close()

	#Verify changes to make sure everything well alright
	dynamic_table_verify = get_dynamic_section_from_file(elf_out_file_name, DYNAMIC_DATA_offset, dynamic_section_num_entries)
	log_buffer_verify = ""
	for i in range(dynamic_section_num_entries):
		log_buffer_verify += (str(dynamic_table_verify[i].get_tag_str()) + "\t" + str(hex(dynamic_table_verify[i].get_value())) + '\n')

	#log everything for debug purposes
	write_back_to_log(log_file_name, log_buffer + "\n\n" + log_buffer_verify)

	return 0


