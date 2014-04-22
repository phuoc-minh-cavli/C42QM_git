"""
 qcmat.py

Description: qcmat parser main.

 Copyright (c) 2016 - 2017 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: qcmat.py

 when            who           what, where, why
 --------      ---------    ------------------------------------------------
 7 July 2016                    First Draft

 """
 
import mapparser
import common_parse
import qmaffile
import elf
import elf64
import argparse
import sys
import time
import ArmMemoryAnalyser
import subprocess
import struct
import qcmatreport

class qcmat4:
	def __init__ (self, mapfilename,elffilename,outputfile,debug_enable,build_type):
		self.mapfilename = mapfilename
		self.elffilename = elffilename
		self.build_type = build_type
		elf_pars_time=time.time()
		#print "Time Taken in Elf Parsing - " ,time.time() - elf_pars_time
		map_pars_time=time.time()
		self.m_parser = mapparser.mapparser(self.mapfilename,debug_enable,self.build_type)
		self.buffer=self.m_parser.start_first_parsing()
		if self.buffer==None:
			ArmMemoryAnalyser.ArmMemoryAnalysis(self.mapfilename,outputfile)
			return
		if self.check_elf_type():
			self.e_parser=elf64.elfFile(elffilename,False)
			print "ELF TYPE IS 64 BIT"
		else:
			self.e_parser = elf.elfFile(elffilename,0)
		#print "Time Taken in Pre Processing Map Parsing - " , time.time()-map_pars_time
		map_post_parse=time.time()
		commonparse=common_parse.MainParser(self.buffer,self.e_parser.get_addr_to_sectype_dict(),self.build_type)
		self.memoryAnalysis=commonparse.start_parsing()
		self.elfAnalysis=commonparse.get_elf_sec_info()
		qmaffile.qmafFileCreate(self.memoryAnalysis,self.elfAnalysis,outputfile)

	def check_elf_type(self):
		file=open(self.elffilename,"rb")
		file.seek(0)
		data=file.read(50)
		elf_type=str(struct.unpack_from("<H",data,18)[0])
		if elf_type=="164":
			return False
		else:
			return True
		file.close()

def main():
	version = 'Qualcomm Memory Analyzer Tool (qcmat) v4 June 30, 2016'
	print version
	argParser = argparse.ArgumentParser(description=version)
	argParser.add_argument('-m', type=str, required=True, action='store', dest='map_file_path',help='Map file with path')
	argParser.add_argument('-e', type=str, required=True, action='store', dest='elf_file_path',help='Elf file with path')
	argParser.add_argument('-o', type=str, required=True, action='store', dest='output_file',help='Output file with path')
	argParser.add_argument('-bt', type=str, required=True, action='store', dest='build_file',help='Build Type Supported(case sensitive). (E.g. MPSS, WCNSS, LPASS etc.')
	argParser.add_argument('-filter', type=str, required=False, action='store', dest='module_filter',help='module filter')
	argParser.add_argument('-csv', type=str, required=False, action='store', dest='csv_report',help='Generate csv report, please provide csv file including path and ext')
	argParser.add_argument('-html', type=str, required=False, action='store', dest='html_report',help='Generate html report, please provide html output directory')
	argParser.add_argument('-diff', type=str, required=False, action='store', dest='qmaf_diff',help='Qmaf File path to compare this build with already generated Qmaf')
	argParser.add_argument('-debug', type=str, required=False, action='store', dest='debug_enable',help='Enable parser debugging command "-debug 1"')
	args = argParser.parse_args()
	map_file_path = args.map_file_path
	elf_file_path = args.elf_file_path
	debug_enable = args.debug_enable
	outputfile = args.output_file
	build_type=args.build_file
	csv_file_path = args.csv_report
	module_filter = args.module_filter
	html_report= args.html_report
	qmaf2=args.qmaf_diff
	parser = qcmat4(map_file_path,elf_file_path,outputfile,debug_enable,build_type)
	if csv_file_path or html_report:
		qcmatreport.main1(outputfile,qmaf2,module_filter,csv_file_path,html_report)

if __name__ == "__main__":
	print str(sys.argv)
	main()
