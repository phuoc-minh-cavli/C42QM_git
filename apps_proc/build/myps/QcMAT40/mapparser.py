"""
 mapparser.py

Description: Preprocessor - Input is map File , Prodive common format 
             file for post processing
             Uses dataformat for storing symbol related data

 Copyright (c) 2016 - 2017 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: mapparser.py

   when            who        what, where, why
  --------       ---------  ------------------------------------------------
 7 July 2016    jsumit        First Draft
    
 ==========================================================================
 """

import dataformat


class mapparser:

	def __init__ (self, mapfilename, debug_enable,build_type):
		self.mapfile = open(mapfilename,"r")
		self.build_type = build_type
		self.buffer = []
		self.bufferindex = 0
		self.mapversion = self.CheckMapVersion()
		self.debug=debug_enable
		self.filters_6=["	0x0	0x0","KEEP ((",".sbss*)","/*/","*modem","*audio","*core","COMMON)","*/()","*platform","EXCLUDE_FILE","*(",".*)", "(*COMMON",".sdata*","(size before relaxing)", "PROVIDE (_","FILL mask","BYTE ","LOAD ","END GROUP","START GROUP"]

	def start_first_parsing(self):
		if self.mapversion == 7 :
			return self.parse_7x()
		elif self.mapversion==6:
			return self.parse_6x()
		elif self.mapversion==9:
			return self.parse_8x()
		else:
			return None

	def check_filter_6(self,line):
		for i in self.filters_6:
			if i in line:
				return True
		return False

	def parse_1_length_line(self,line,previous_line):
		if "QSR_STR.fmt.rodata." in line:
			previous_line="QSR_STR.fmt.rodata."
		elif "fatal.struct.rodata." in line:
			previous_line="_FATAL_COUNT_"
		elif "QSR_F3_MSG" in line or "QSR_F3_ERR" in line or "QSR_LEGACY" in line:
			previous_line="_F3_COUNT_"
		elif "fatal.fmt.rodata" in line:
			previous_line="fatal.fmt.rodata"
		elif ".fatal.file.rodata." in line:
			previous_line="__THIS_FILENAME__"
		else:
			previous_line=line
		return previous_line


	def display_2nd_file(self):
		master=open("2nd_Pass.log","w")
		for i in self.memoryAnalysis:
			master.write(str(i)+"		"+self.memoryAnalysis[i].display())

	def display_1st_file(self):
		file = open("1st_Pass.log", "w")
		for line in self.buffer:
			file.write(line + "\n")
		file.close()

	def CheckMapVersion(self):
		linecount = 4
		for i in range(linecount):
			line = self.mapfile.readline()
			# Linux
			## Linker from QuIC LLVM Hexagon Clang version Version 3.1
			## Linker based on LLVM version: 3.5
			# Windows
			## Linker from Clang Version 3.1
			## Linker based on LLVM version: 3.5
			if "Pre-LTO Map" in line or self.build_type=="WCNSS" or "Clang version Version 8." in line or "LLVM ARM Compiler Version 3" in line:
				print "Hexagon Legacy 8x"
				return 9
			elif line.startswith("# Linker from QuIC LLVM") or line.startswith("# Linker from Clang Version") or line.startswith("# Linker based on LLVM version"):
				print 'Hexagon QuIC LLVM'
				return 7
			elif "ARM" in line:
				print "ARM Image , Arm Processing"
				return 8
		else:
			print 'Hexagon Legacy'
			return 6


	def parse_6x(self):
		start_processing=False
		previous_line=None
		start_processing=False
		previous_path=None

		for line in self.mapfile:
			if self.check_filter_6(line):
				continue
			if line.lower().startswith("linker script and memory map"):
				start_processing=True
				continue
			if start_processing is True:
				if line.startswith(".stab") or line.startswith(".debug") or line.startswith(".comment") or line.startswith(".debug_info"):
					break
				if line.startswith("("):
					continue
				if '\r' in line:
					line="/".join(line.split())
				if "load address" in line:
					line=str(line.split("load address")[0])
				if "QSR_4_0_MSG.fmt.rodata" in line:
					line="QSR_4_0_MSG.fmt.rodata"
				if "QSR_4_0_STR.fmt.rodata" in line:
					line="QSR_4_0_STR.fmt.rodata"
				if len(line.split())==3 and "build\\" in line.split()[2]:
					continue
				if len(line.split())==1:
					previous_line=self.parse_1_length_line(line,previous_line)
					found_symbol=True
					continue
				if line.startswith("                ") and found_symbol==True and not previous_line==None:
					line1=previous_line.strip()+"    "+line.strip()
					if len(line.split())>3:
						continue
					if len(line.split())==3:
						previous_path=line.split()[-1]
					elif len(line.split())==2:
						previous_path="Unknown"
					self.buffer.append(line1)
					found_symbol=False
					previous_line=None
					line1=None
					continue
				if line.startswith("                ") and len(line.split("                "))==3:
					line1=line.split("                ")
					line="                "+line1[1]+"                "+line1[2].replace(" ","")
					self.buffer.append(line.strip("\n"))
					continue
				else:
					found_symbol=False
					previous_line=None
					line1=line.split()
					if len(line1)==4:
						if line1[0]=="*fill*":
							if "/" in previous_path:
								line1[3]=previous_path
							else:
								continue
							line=line1[0]+"  "+line1[1]+"  "+line1[2]+"  "+line1[3]
							self.buffer.append(line)
						else:
							previous_path=line1[3]
							self.buffer.append(line.strip("\n"))
					elif len(line1)==3 and line1[0]=="*fill*": ##TEN SILICA
						if "/" in previous_path:
							line=line1[0]+"  "+line1[1]+"  "+line1[2]+"  "+str(previous_path)
							self.buffer.append(line)
					else:
						self.buffer.append(line.strip("\n"))
		if self.debug=='1':
			self.display_1st_file()
		return self.buffer


	def parse_7x(self):
		start_processing=False
		found_4_segment_line=False
		previous_path = None
		
		temp_4_seg_line=None
		#Trampoline has no field of file association need to update it from our side
		trampoline_path=None
		
		for line in self.mapfile:
			if line.lower().startswith("linker script and memory map"):
				start_processing=True
				continue
			if line == "Linker scripts used (including INCLUDE command)":
				self.buffer[:]=[]
			if "* )" in line or " )" in line and "QSR_4_0_MSG.fmt.rodata" not in line and "=" not in line:
				continue
			if "=" in line:
				self.buffer.append(line.strip('\n'))
				continue
			# Stop processing debug/comment section
			if line.startswith(".stab") or line.startswith(".debug") or line.startswith(".comment") or line.startswith(".debug_info"):
				break
			# filetrs to strip out the line in 	
			if "/*/" in line or "*)" in line or line.startswith("(*") or "(size before relaxing)" in line or ("PROVIDE (" in line) or line.startswith("LOAD "):
				continue
			if "(." in line or "EXCLUDE_FILE" in line:
				continue
			if line.startswith("START") or  line.startswith("END"):
				continue
			if len(line.split())==0 or len(line.split())==1:
				continue
			if "QSR_string_ptr" in line:
				continue
			if start_processing is True:
				if line.startswith("KEEP"):
					continue
				if len(line.split())==4 and line.startswith("	0x"):
					previous_path=line.split()[2]
					found_4_segment_line=True
					temp_4_seg_line=line
					continue
				if found_4_segment_line==True:
					line1=line.split()
					keyline=temp_4_seg_line.split()
					line2=line1[1]+" "+keyline[0]+" "+keyline[1]+" "+keyline[2]
					self.buffer.append(line2)
					previous_path=keyline[2]
					del line1,line2
					temp_4_seg_line=None
					found_4_segment_line=False
					continue
				if line.startswith("PADDING"):
					line=line.split()
					line1=line[0]+" "+ line[1]+" "+line[2]+" "+previous_path
					self.buffer.append(line1)
					line1=None
					continue
				if line.startswith("QSR_4_0_STR.fmt.rodata."):
					line1=line.split("\t")
					if len(line1)==5:
						line1[0] = "QSR_4_0_STR.fmt.rodata."
						line=str(line1[0])+" "+str(line1[1])+" "+str(line1[2])+" "+str(line1[3])
						previous_path=str(line1[3])
						self.buffer.append(line)
					continue
				if line.startswith("QSR_4_0_MSG.fmt.rodata"):
					line1=line.split("\t")
					if len(line1)==5:
						line1[0] = "QSR_4_0_MSG.fmt.rodata"
						line=str(line1[0])+" "+str(line1[1])+" "+str(line1[2])+" "+str(line1[3])
						previous_path=str(line1[3])
						self.buffer.append(line)
					continue	
				if "# Offset" in line:
					#.rw_candidate_compress_section	0xd10e6000	0x652000 # Offset: 0x3b33000, LMA: 0xd10e6000
					line1=line.split()
					line2=str(line1[0])+" "+str(line1[1])+" "+str(line1[2])
					self.buffer.append(line2)
					line1=None
					line2=None
					continue
				if len(line.split())==5:
					line=line.split()
					if "/" in line[3]:
						line1=str(line[0])+" "+str(line[1])+" "+str(line[2])+" "+str(line[3])
					elif "TRAMPOLINE" in line[3]:
						line[3]=previous_path
						line1=str(line[0])+" "+str(line[1])+" "+str(line[2])+" "+str(line[3])
					else:
						line1=str(line[0])+" "+str(line[1])+" "+str(line[2])+" "
					self.buffer.append(line1)
					previous_path=str(line[3])
					line1=None
				elif len(line.split()) == 2:
					self.buffer.append(line.strip('\n'))
				else:
					self.buffer.append(line.strip('\n'))
			else:	
				continue
		if self.debug=='1':
			self.display_1st_file()
		return self.buffer


	def parse_8x(self):
		start_processing=False
		SHT_NOBITS_line=None
		SHT_NOBITS_found=False
		SHT_PROGBITS_found=False
		previous_path=None
		for line in self.mapfile:
			#Filters to remove the unwanted lines
			# print line
			if "* )" in line:
				continue
			#Start Point of the Parsing
			if line.lower().startswith("linker scripts used (including include command)"):
				start_processing=True
				print line
				continue
			if start_processing is True:
				if "* )" in line or ".lst" in line or ".lcs" in line:
					continue
				elif"# Offset:" in line and "LMA:" in line: 
					line=line.split("# Offset:")
					self.buffer.append(line[0])
				elif "#SHT_PROGBITS" in line :
					SHT_PROGBITS_found=True
					line=line.split("#SHT_PROGBITS")[0]
					if "# [" in line and "]"  in line:
						line=line.split()
						line=line[0]+"	"+line[1]+"	"+line[2]+"	"+line[6]
					if "TRAMPOLINE" in line:
						line=line.split()
						line=line[0]+"	"+line[1]+"	"+line[2]+"	"+previous_path
					previous_path=line.split()[-1]
					self.buffer.append(line)
				elif "PADDING_ALIGNMENT" in line:
					line=line.split()
					line=line[0]+"	"+line[1]+"	"+line[2]+"	"+str(previous_path)
					self.buffer.append(line.strip())
				elif "#SHT_NOBITS" in line and line.startswith("	0x"):
					SHT_NOBITS_line=line.split("#SHT_NOBITS")[0]
					SHT_NOBITS_found=True
				elif "#SHT_NOBITS" in line:
					# print line
					line=line.split("#SHT_NOBITS")[0]
					# print line
					if "# [" in line and "]"  in line:
						line=line.split()
						line=line[0]+"	"+line[1]+"	"+line[2]+"	"+line[6]
					previous_path=line.split()[-1]
					self.buffer.append(line)
				elif "#(Bitcode origin:" in line and SHT_NOBITS_found==True:
					line=line.split()
					self.buffer.append(line[1]+"	"+SHT_NOBITS_line)
					previous_path=SHT_NOBITS_line.split()[-1]
					SHT_NOBITS_found=False
					SHT_NOBITS_line=None
				elif len((str(line)).split())==2 and SHT_NOBITS_found==True:
					line=line.split()
					self.buffer.append(line[1]+"	"+SHT_NOBITS_line)
					previous_path=SHT_NOBITS_line.split()[-1]
					SHT_NOBITS_found=False
					SHT_NOBITS_line=None
				elif "#(Bitcode origin:" in line and SHT_PROGBITS_found==True:
					line=line.split("#(Bitcode origin:")[0]
					line=line.replace(" ","")
					self.buffer.append(line)
					SHT_PROGBITS_found=False
				#Symbol Names filtering
				elif len((str(line)).split())==2 and str(line).split()[0].startswith("0x"):
					line=line.split()
					line="		"+line[0]+"	"+line[1]
					self.buffer.append(line)
				##Break Point of the Parsing
				elif line.startswith(".stab") or line.startswith(".debug") or line.startswith(".comment") or line.startswith(".debug_info"):
					break
				#for TCM processing
				elif "=" in line: 
					self.buffer.append(line.strip('\n'))
				else:
					continue
		if self.debug=='1':
			self.display_1st_file()
		return self.buffer

