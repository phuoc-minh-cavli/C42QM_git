"""
 qcmattoplevel.py

Description: Top level report

 Copyright (c) 2015 - 2016 by QUALCOMM, Incorporated.  
			 All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/build.tn/4.0/myps/QcMAT40/qcmat3toplevel.py#1 $

========================================================================
"""

# Imports
import os
import sys
import time
import inspect
import argparse
import zipfile
import subprocess
import pprint
import elf
import common_parse
import helper.Constants as const
import report.memtree as memtree
import traceback
import mapparser
import dataformat
import ArmMemoryAnalyser

class QCMAT30TopLevel(object):
	def __init__(self,SortedMemoryAnalysis,buildType):
		self.SortedMemoryAnalysis = SortedMemoryAnalysis
		self.MemTreeDict = dict()
		self.qc_data = {}
		self.unique_dict = {}
		self.buildType=buildType

	def updateMemTreeLeft(self):
		isfatal=False
		isf3=False
		for i in self.SortedMemoryAnalysis:
			if str(self.SortedMemoryAnalysis[i].__class__.__name__)=="object_info":
				line=self.SortedMemoryAnalysis[i].display()
			else:
				line = self.SortedMemoryAnalysis[i][0]
			line=line.strip()
			#skip blank line
			if not line:
				continue
			if "SECTION_HOLE" in line and "SUB_SECTION_HOLE" not in line:
				continue
			# memoryAnalysis  file format
			# modulepath,address,symbol,size,section,visibility(g/l),type(F/O)
			# ROOT/modem_proc/core/services/utils/modem_mem.o c47b6cb0 modemheap_fail_info 160 .bss g O
			lineComponents =  line.split(' ')
			# print lineComponents
			currentPath = ""
			parentPath  = ""
			pathComponents = lineComponents[0].split('/')
			#address = lineComponents[1]
			symbol = lineComponents[2]
			size = int(lineComponents[3])
			sectionname = lineComponents[4]
			section = int(lineComponents[7])
			if (symbol.startswith("_F3_COUNT_") or "xx_msg_" in symbol or "xx_err_" in symbol or "ERR." in symbol or "QSR_F3_" in symbol or "QSR_4_0_MSG" in symbol or "QSR_LEGACY" in symbol) and "ro_fatal" not in lineComponents[4]:
				isf3=True
				# print section
			elif symbol.startswith("_FATAL_COUNT_") or "xx_msg_" in symbol or "xx_err_" in symbol and "ro_fatal" in lineComponents[4]:
				# print section
				isfatal=True
			#print lineComponents
			for part in pathComponents:
				parentPath = currentPath
				currentPath = currentPath + "/" + part
				#print currentPath, ",", section, ",", size
				if currentPath not in self.MemTreeDict:
					self.MemTreeDict[currentPath] = memtree.MemTreeNode(currentPath)
					if parentPath:
						self.MemTreeDict[parentPath].addChild(self.MemTreeDict[currentPath])
				size_adjusted = size
				if isf3==True:
					self.MemTreeDict[currentPath].setLeftSizeAt(6,(self.MemTreeDict[currentPath].getLeftSizeAt(6) + (size/8)))
				elif isfatal==True:
					self.MemTreeDict[currentPath].setLeftSizeAt(5,(self.MemTreeDict[currentPath].getLeftSizeAt(5) + (size/10)))
				if 'QSR_4_0_MSG' not in sectionname:
					# Update section size e.g. TXT, RO, DATA, BSS
					self.MemTreeDict[currentPath].setLeftSizeAt(section,(self.MemTreeDict[currentPath].getLeftSizeAt(section) + size_adjusted))
					# Update total size
					self.MemTreeDict[currentPath].setLeftSizeAt(4,(self.MemTreeDict[currentPath].getLeftSizeAt(4) + size_adjusted))
			isf3=False
			isfatal=False
	
	def get_top_level_key(self):
		rf_first = True
		fw_first = True
		all_rf_subsystem = {}
		all_fw_subsystem = {}
		for key, value in self.qc_data.iteritems():
			#print "keys" + key +"value  " + value[0]
			if key.lower().startswith('rf'):
				
				if rf_first == True:
					#print "found rf first",qc_data[key]
					self.unique_dict['rf'] = value
					rf_first = False
					
				else:
					 
					self.unique_dict['rf'][0] += value[0]
					self.unique_dict['rf'][1] += value[1] 
					self.unique_dict['rf'][2] += value[2] 
					self.unique_dict['rf'][3] += value[3]
					self.unique_dict['rf'][4] += value[4]
					self.unique_dict['rf'][5] += value[5]
					self.unique_dict['rf'][6] += value[6]
				all_rf_subsystem[key] = value	
				continue
			
			if key.lower().startswith('fw'):
			   
				if fw_first == True:
					#print "found first fw", qc_data[key]
					self.unique_dict['fw'] = value
					fw_first = False
				else:
					self.unique_dict['fw'][0] += value[0]
					self.unique_dict['fw'][1] += value[1] 
					self.unique_dict['fw'][2] += value[2] 
					self.unique_dict['fw'][3] += value[3]
					self.unique_dict['fw'][4] += value[4]
					self.unique_dict['fw'][5] += value[5]
					self.unique_dict['fw'][6] += value[6]
				all_fw_subsystem[key] = value 
				continue
			self.unique_dict[key] = value
		
		print""
		print "List of all RF subsystems merged under RF:"
		pprint.pprint(all_rf_subsystem,stream=None, indent=1, width=200, depth=None)
		print""
		print "List of all FW subsystems under fw:"
		pprint.pprint(all_fw_subsystem,stream=None, indent=1, width=200, depth=None)
	
	def MemTree_TopLevel(self,node):
		int_val_list = []
		child_int_val = []
		#print "Module,TXT,RO,DATA,BSS,TOTAL,FatalCount,F3Count\n"
		nodefulname = node.getFullName().replace("/ROOT/","")
		if node.isLeaf:
			print  nodefulname + "," + node.getLeftSizesString()
			leaf_val = node.getLeftSizesString()
			val_list = leaf_val.split(',')
			int_val_list= [int(x) for x in val_list]
			self.qc_data[nodefulname.lower()] = int_val_list
			print "\n" + nodefulname + "," + node.getLeftSizesString()
			return
			
		for child in node.getChilds():
			#print child.getLastName() + "," + child.getLeftSizesString()
			child_val  = child.getLeftSizesString()
			#pprint.pprint(child_val,stream=None, indent=1, width=200, depth=None) 
			#print "child val", child_val
			child_val_list = child_val.split(',')
			#print "child val lst", child_val_list
			child_int_val = [int (x) for x in child_val_list]
			#print "int list", child_int_val
			self.qc_data[child.getLastName().lower()] = child_int_val
			#print "\n"+child.getLastName() + "," + child.getLeftSizesString()
		#print "\nTotal:," + node.getLeftSizesString()
		print ""
		print "Raw qcmat data \n"
		pprint.pprint(self.qc_data,stream=None, indent=1, width=200, depth=None)
		self.get_top_level_key()
		
	def printMemTreeTopLevel(self):
		root = None
		if '/ROOT' in self.MemTreeDict and self.buildType=="MPSS":
			root = self.MemTreeDict['/ROOT/modem_proc']
		elif '/ROOT' in self.MemTreeDict and self.buildType=="ARM": 
			root = self.MemTreeDict['/ROOT/apps_proc']
		self.MemTree_TopLevel(root)

	def GetDataDictionary(self):
		return self.unique_dict

def memory_top_level_data(map_file,elf_file):
	try:
		map_file_path= map_file
		elf_file_path= elf_file
		buildType= 'MPSS'
		version = 'Qualcomm Memory Analyzer Tool (qcmat) v4 June 30, 2016'
		print version
		# start time
		start = time.time()
		#print start
		debug_enable= None
		qcmatobj = mapparser.mapparser(map_file,debug_enable,buildType)
		buffer=qcmatobj.start_first_parsing()
		if buffer==None:
			arm=ArmMemoryAnalyser.ArmMemoryAnalysis(map_file,"temp_for_ARM.qmaf")
			memoryAnalysis=arm.get_memoryanalysis_dict()
			buildType='ARM'
		else:
			e_parser = elf.elfFile(elf_file,0)
			commonparse=common_parse.MainParser(buffer,e_parser.get_addr_to_sectype_dict(),buildType)
			memoryAnalysis=commonparse.start_parsing()
		qcmattop = QCMAT30TopLevel(memoryAnalysis,buildType)
		qcmattop.updateMemTreeLeft()
		qcmattop.printMemTreeTopLevel()
		
		done = time.time()
		elapsed = done - start
		#print done
		print "Time took:", elapsed

		print "QCMAT Activity Completed!"
		if os.path.isfile("summary"):
			os.remove("summary")
		if os.path.isfile("temp_for_ARM.qmaf"):
			os.remove("temp_for_ARM.qmaf")
		return qcmattop.GetDataDictionary()
	except:
		print "Quiting Main SL Program"
		print traceback.format_exc()
