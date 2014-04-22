"""
 common_Parse

Description: This file Takes the input from PRE Processor.Helps in Post processing the 
             Input to producr symbol Information
             Used by mapparser.

 Copyright (c) 2016 - 2017 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: common_parse.py

 when            who        what, where, why
 --------      ---------  ------------------------------------------------
 July 7, 2016   jsumit         First Draft
 ==========================================================================
 """

import sys
import dataformat


class HandleException(Exception):
	def __init__(self, value):
		self.value = value

class MainParser:
	
	def __init__(self,buffer,sectype_dict,build_type):
		##buffer to read after 1st pass
		self.buffer=buffer
		self.address_to_sectype_dict=sectype_dict
		self.build_type=build_type
		## Section Data
		self.section_name=None
		self.Section_start_address=None
		self.Section_end_address=None
		self.section_dictionary=dict()
		self.buffer=buffer
		self.section_index=None
		## Sub Section Data
		self.Sub_Section_name=None
		self.Sub_Section_start_address=None
		self.Sub_Section_end_address=None
		self.previous_path=None
		##final dictionary to store the data , to be passed for TREE data structure
		self.symbol_dict=dict()
		self.section_dict=dict()
		## previous address storing for address calculation
		self.previous_address=None
		self.previous_key=None
		##TCM SYMBOLS FILTERING
		self.found_tcm_static_section_start = False
		self.found_tcm_overlay_text_start = False
		self.found_tcm_overlay_data_start = False
		##ISLAND SECTION FILTER
		self.is_section_island=False
		##Static TLB Report
		self.static_tlb_report=dict()
		self.PageSizeLst = [16*1024*1024 ,4*1024*1024, 1024*1024, 256*1024, 64*1024, 16*1024, 4*1024]

	def check_candidate_section(self,line):
		if "__MMU_region" in line or line.startswith("end") or "PROVIDE(end" in line:
			self.static_tlb_generate_register(line)
			return
		if "PROVIDE" in line:
			return
		if "__swapped_range_rodata_start__" in line:
			self.section_index=1
		elif "__swapped_segments_bss_start__" in line:
			self.section_index=3
		##Added External paging support 
		elif "__swapped_segments_extpl" in line:
			if "text" in line:
				self.section_index=0
			elif "rodata" in line:
				self.section_index=1
			elif "rwdata" in line:
				self.section_index=2
			elif "bss" in line:
				self.section_index=3
		if "+" in line or "-" in line:
			return
		self.check_tcm_start(line)
		self.check_end_received(line)

	def static_tlb_generate_register(self,line):
		if line.startswith("                0x"):
			line=line.split("                ")
			address=line[1]
			if "PROVIDE(end" in line[-1]:
				line[-1]="end"
			self.static_tlb_report[address]=line[-1].strip("=.").split("_")
		else:
			line = line.split(" = .( ")
			address = line[1].strip(" );")
			if "PROVIDE(end" in line[0]:
				line[0]="end"
			self.static_tlb_report[address]=line[0].lstrip().split("(")[0].split("_")

	def generate_final_tlb_report(self):
		file=open("summary","w")
		if len(self.static_tlb_report)<=1:
			return
		file.write("STATIC TLB REPORT\n============================\n\n\n")
		start_address=0
		for i in sorted(self.static_tlb_report):
			# print self.static_tlb_report[i]
			if "end" in self.static_tlb_report[i]:
				while int(start_address,16)<int(i,16):
						for j in self.PageSizeLst:
							if int(start_address,16) + int(j) <= int(i,16):
								file.write(start_address+"--->Size--->"+str(hex(j).strip("L"))+"\n")
								# print start_address+"--->Size--->"+str(hex(j).strip("L"))
								start_address= str(hex(int(start_address,16) + int(j)).strip("L"))
							if (int(i,16) - int(start_address,16)) >0 and (int(i,16) - int(start_address,16)) < self.PageSizeLst[-1] :
								start_address= str(hex(int(start_address,16) + int(self.PageSizeLst[-1])).strip("L"))
								file.write(start_address+"--->Size--->"+str(hex(self.PageSizeLst[-1]).strip("L"))+"\n")
								# print start_address+"--->Size--->"+str(hex(self.PageSizeLst[-1]).strip("L"))
			elif self.static_tlb_report[i][4]=="start":
				if start_address==0:
					file.write("\nName --> "+self.static_tlb_report[i][6]+"  Permissions   Read "+self.static_tlb_report[i][10]+"  Write "+self.static_tlb_report[i][12]+"  Execute "+self.static_tlb_report[i][14]+"\n")
					# print "\nName --> "+self.static_tlb_report[i][6]+"  Permissions   Read "+self.static_tlb_report[i][10]+"  Write "+self.static_tlb_report[i][12]+"  Execute "+self.static_tlb_report[i][14]
					start_address=i;
					continue
				else:
					while int(start_address,16)<int(i,16):
						for j in self.PageSizeLst:
							if int(start_address,16) + int(j) <= int(i,16):
								file.write(start_address+"--->Size--->"+str(hex(j).strip("L"))+"\n")
								# print start_address+"--->Size--->"+str(hex(j).strip("L"))
								start_address= str(hex(int(start_address,16) + int(j)).strip("L"))
							if (int(i,16) - int(start_address,16)) >0 and (int(i,16) - int(start_address,16)) < self.PageSizeLst[-1] :
								start_address= str(hex(int(start_address,16) + int(self.PageSizeLst[-1])).strip("L"))
								# print start_address+"--->Size--->"+str(hex(self.PageSizeLst[-1]).strip("L"))
								file.write(start_address+"--->Size--->"+str(hex(self.PageSizeLst[-1]).strip("L"))+"\n")
					start_address=i
					# print "\nName --> "+self.static_tlb_report[i][6]+"  Permissions   Read "+self.static_tlb_report[i][10]+"  Write "+self.static_tlb_report[i][12]+"  Execute "+self.static_tlb_report[i][14]
					file.write("\nName --> "+self.static_tlb_report[i][6]+"  Permissions   Read "+self.static_tlb_report[i][10]+"  Write "+self.static_tlb_report[i][12]+"  Execute "+self.static_tlb_report[i][14]+"\n")

	def check_tcm_start(self,line):
		if "__tcm_overlay_data_start__" in line:
			self.found_tcm_static_section_start=False
			self.found_tcm_overlay_text_start=False
			self.found_tcm_overlay_data_start=True
			# print line
		if "__tcm_overlay_text_start__" in line:
			self.found_tcm_static_section_start=False
			self.found_tcm_overlay_text_start=True
			self.found_tcm_overlay_data_start=False
			# print line
		if "__tcm_static_section_start__" in line:
			self.found_tcm_static_section_start=True
			self.found_tcm_overlay_text_start=False
			self.found_tcm_overlay_data_start=False
			# print line

	def check_end_received(self,line):
		if "__tcm_overlay_data_end__" in line or "__tcm_overlay_text_end__" in line or "__tcm_static_section_end__" in line:
			self.found_tcm_static_section_start=False
			self.found_tcm_overlay_text_start=False
			self.found_tcm_overlay_data_start=False
			# print line

	##start processing the file
	def start_parsing(self):
		for line in self.buffer:
			#print line
			if "=" in line or "PROVIDE" in line:
				self.check_candidate_section(line)
				continue
			line=line.split()
			line_length=len(line)
			try:
				if line_length==0:
					continue
				elif line_length==3:
					self.section_process(line)
				## To avoid the processing of the QSR_STRING , blocking the call of 4 and 2 types
				## segments , this way hole is not missed and symbols are also missed
				elif self.section_name=="QSR_STRING":
					continue
				elif line_length==4:
					self.sub_section_process(line)
				elif line_length==2:
					self.symbol_process(line)
				else:
					raise HandleException(line)
			except HandleException as Exp:
				file=open("summary","w")
				for i in range(0,len(Exp.value)):
					file.write(Exp.value[i]+" ")
				file.write("\n")
				file.close()
		##Case for Last line in the section as we are back Processing the size last size needs to be handled - START
		else:
			#print "self.Sub_Section_end_address",self.Sub_Section_end_address,"self.previous_address:-",self.previous_address
			if not self.Sub_Section_end_address==None:
				size=int(self.Sub_Section_end_address,16)-int(self.previous_address,16)
				self.symbol_dict[self.previous_key].set_size(size)
		self.check_section_hole()
		# self.show_dict()
		self.generate_final_tlb_report();
		return self.symbol_dict
	
	def get_elf_sec_info(self):
		# for i in self.section_dict:
			 # print i,"==>",self.section_dict[i]
		return self.section_dict

	##HOLE detection between the different sections
	def check_section_hole(self):
		previous_end_address=None
		previous_section=None
		previous_index=None
		if len(self.section_dictionary)>1:
			for i in sorted(self.section_dictionary):
				#print self.section_dictionary[i]
				if previous_end_address==None and previous_section==None:
					previous_end_address=self.section_dictionary[i][1]
					previous_section=self.section_dictionary[i][2]
					previous_index=self.section_dictionary[i][3]
				else:
					size=int(self.section_dictionary[i][0],16)-int(previous_end_address,16)
					#print size
					if  size<=4194304 and size>0:
						#section_name,symbol_section,symbol_name,address,size,path)
						if self.found_tcm_static_section_start==True or self.found_tcm_overlay_text_start==True or self.found_tcm_overlay_data_start==True:
							directory="ROOT/TCM/SECTION_HOLE/"
						else:
							directory="ROOT/SECTION_HOLE/"
						data=dataformat.object_info(previous_section,"","SECTION_HOLE",previous_end_address,size,str(directory)+str(previous_section),previous_index)
						self.symbol_dict[previous_end_address]=data
					previous_end_address=self.section_dictionary[i][1]
					previous_section=self.section_dictionary[i][2]
					previous_index=self.section_dictionary[i][3]

	##def detect section type
	def setindex(self,type):
		if type=="ELF_SECTION_TEXT":
			self.section_index=0
		elif type=="ELF_SECTION_RO":
			self.section_index=1
		elif type=="ELF_SECTION_DATA":
			self.section_index=2
		elif type=="ELF_SECTION_BSS":
			self.section_index=3
		else:
			self.section_index=5 ##unknown section type


	##Section Processing Function
	def section_process(self,line):
		##Previous Section Clean Up -- START
		# In Back Processing when lat line of section came we are not able to proceess its size
		# with the below if block all the section except the last sections last line is updated
		#need to change this for all sections 
		if not self.Sub_Section_end_address==None and not self.previous_address==None:
			size=int(self.Sub_Section_end_address,16)-int(self.previous_address,16)
			self.symbol_dict[self.previous_key].set_size(size)
		##Previous Section Clean Up -- END
		self.section_name = line[0]
		if ".island" in self.section_name and(self.build_type=="SLPI" or self.build_type=="slpi"):
			self.is_section_island=True
		else:
			self.is_section_island=False
		self.Section_start_address=line[1]
		##Create Section Dict - START
		# print line
		if self.Section_start_address+"___"+str(int(line[2],16)) in self.address_to_sectype_dict:
			key=self.Section_start_address+self.section_name
			self.address_to_sectype_dict[self.Section_start_address+"___"+str(int(line[2],16))][0]=self.section_name
			self.section_dict[key]=self.address_to_sectype_dict[self.Section_start_address+"___"+str(int(line[2],16))]
			self.setindex(self.section_dict[key][5])
			# print self.address_to_sectype_dict[self.Section_start_address+"___"+str(int(line[2],16))]
		elif self.Section_start_address in self.address_to_sectype_dict:
			if int(line[2],16)==int(self.address_to_sectype_dict[self.Section_start_address][3]):
				key=self.Section_start_address+self.section_name
				self.address_to_sectype_dict[self.Section_start_address][0]=self.section_name
				self.section_dict[key]=self.address_to_sectype_dict[self.Section_start_address]
				self.setindex(self.section_dict[key][5])
			# print self.address_to_sectype_dict[self.Section_start_address]
		##Create Section Dict - END
		self.Section_end_address=hex(int(self.Section_start_address,16)+int(line[2],16)).strip("L")
		self.previous_address=self.Section_start_address
		self.previous_path="/SECTION_START"
		section_info=[self.Section_start_address,self.Section_end_address,self.section_name,self.section_index]
		# print "section_name:- ",self.section_name,"Section_start_address:-  ",self.Section_start_address,"Section_end_address:-  ",self.Section_end_address
		##Dictionary for section hole detection based on sorting algorithm
		self.section_dictionary[self.Section_start_address]=section_info
		##New Section Start Clean Up for the next symbols
		self.Sub_Section_end_address=None
		self.Sub_Section_start_address=None
		self.Sub_Section_name=None


	##Sub Section Processing Function
	def sub_section_process(self,line):
		#print line
		if line[1]=="0x00000000":
			return
		if not self.Sub_Section_end_address==None:
			size=int(self.Sub_Section_end_address,16)-int(self.previous_address,16)
			self.symbol_dict[self.previous_key].set_size(size)
			self.previous_address=self.Sub_Section_end_address
		self.Sub_Section_start_address=line[1]
		#print "jsumit:-  self.previous_address:- ", self.previous_address , "self.Sub_Section_start_address" , self.Sub_Section_start_address
		self.Detect_Hole(self.previous_address,self.Sub_Section_start_address,"SUB_SECTION_HOLE",self.previous_path)
		self.Sub_Section_end_address=hex(int(self.Sub_Section_start_address,16)+int(line[2],16)).strip("L")
		self.Sub_Section_name=self.getSubSectionName(line[0])
		self.previous_path=self.Trim_Path(line[3])
		self.previous_address=line[1]
		self.previous_key=str(self.previous_address)+str(self.section_name)
		#section_type,section_name,symbol_section,symbol_name,address,size,path)
		if self.found_tcm_static_section_start==True or self.found_tcm_overlay_text_start==True or self.found_tcm_overlay_data_start==True:
			directory="ROOT/TCM/"
		elif self.is_section_island==True:
				directory="ROOT/Island/"
		else:
			directory="ROOT/"
		if self.Sub_Section_name=="_FATAL_COUNT_":
			data=dataformat.object_info(self.section_name,self.Sub_Section_name,self.Sub_Section_name,self.Sub_Section_start_address,"",str(directory)+self.previous_path,self.section_index)
		elif self.Sub_Section_name=="_F3_COUNT_":
			data=dataformat.object_info(self.section_name,self.Sub_Section_name,self.Sub_Section_name,self.Sub_Section_start_address,"",str(directory)+self.previous_path,self.section_index)
		else:
			data=dataformat.object_info(self.section_name,self.Sub_Section_name,self.Sub_Section_name,self.Sub_Section_start_address,"",str(directory)+self.previous_path,self.section_index)
		self.symbol_dict[self.previous_key]=data
		self.symbol_dict[self.previous_key].set_size(int(line[2],16))
		
	def Trim_Path(self,line):
		# print line
		if "modem_proc" in line:
			line="modem_proc"+line.split("modem_proc",1)[1]
		if "HEXAGON_Tools" in line or "hexagon/" in line:
			if "HEXAGON_Tools" in line:
				line="HEXAGON_Tools"+line.split("HEXAGON_Tools",1)[1]
			elif "hexagon/" in line:
				line="hexagon/"+line.split("hexagon/",1)[1]
		if "build" in line:
			line=line.split("build")[0]+line.split("build/")[-1].split("/")[-1]
		if ".lib" in line or ".a" in line and ".o)" in line:
			line=line.replace("(","/")
			line=line.strip(")")
		##mpss build handling
		if ("../.." in line or "../" in line) and (self.build_type=="MPSS" or self.build_type=="mpss"):
			if "../.." in line:
				line=line.replace("../..","modem_proc")
			if "../" in line:
				line=line.replace("../","modem_proc/")
		##wcnss buils handled
		if "wlan_proc" in line:
			line="wlan_proc"+line.split("wlan_proc",1)[1]
		if ("../.." in line or "../" in line) and (self.build_type=="WCNSS" or self.build_type=="wcnss"):
			if "../.." in line:
				line=line.replace("../..","wlan_proc")
			if "../" in line:
				line=line.replace("../","wlan_proc/")
		##WDSP builds Handled
				##wcnss buils handled
		if "wdsp_proc" in line:
			line="wdsp_proc"+line.split("wdsp_proc",1)[1]
		if ("../.." in line or "../" in line) and (self.build_type=="WDSP" or self.build_type=="wdsp"):
			if "../.." in line:
				line=line.replace("../..","wdsp_proc")
			if "../" in line:
				line=line.replace("../","wdsp_proc/")
		##adsp buils handled
		if "adsp_proc" in line:
			line="adsp_proc"+line.split("adsp_proc",1)[1]
		if ("../.." in line or "../" in line) and (self.build_type=="ADSP" or self.build_type=="adsp"):
			if "../.." in line:
				line=line.replace("../..","adsp_proc")
			if "../" in line:
				line=line.replace("../","adsp_proc/")
		##slpi builds handling
		if "slpi_proc" in line:
			line="slpi_proc"+line.split("slpi_proc",1)[1]
		if ("../.." in line or "../") and (self.build_type=="SLPI" or self.build_type=="slpi"):
			if "../.." in line:
				line=line.replace("../..","slpi_proc")
			if "../" in line:
				line=line.replace("../","slpi_proc/")
		##aop builds handling
		if "aop_proc" in line:
			line="aop_proc"+line.split("aop_proc",1)[1]
		if ("../.." in line or "../") and (self.build_type=="ADSP" or self.build_type=="adsp"):
			if "../.." in line:
				line=line.replace("../..","aop_proc")
			if "../" in line:
				line=line.replace("../","aop_proc/")
		##TZ builds handling
		if "trustzone_images" in line:
			line="trustzone_images"+line.split("trustzone_images",1)[1]
		if ("../.." in line or "../") and (self.build_type=="TZ" or self.build_type=="tz"):
			if "../.." in line:
				line=line.replace("../..","trustzone_images")
			if "../" in line:
				line=line.replace("../","trustzone_images/")
			line="trustzone_images"+line.split("trustzone_images")[-1].replace("\\","/")
		##LTO builds special handling
		if "/tmp/" in line:
			line=line.replace("/tmp/","tmp/")
		return line

	##SubSection Name Processing
	def getSubSectionName(self,name):
		if len(name.split("."))==1:
			return name
		elif len(name.split("."))==2:
			return name.split(".")[1]
		else:
			return name.split(".",2)[2]

	## Symbol Processing Function
	def symbol_process(self,line):
		size=int(line[0],16)-int(self.previous_address,16)
		if line[0]+str(self.section_name) in self.symbol_dict:
			self.symbol_dict[self.previous_key].set_size(size)
			self.symbol_dict[self.previous_key].set_symbol_name(line[1])
			self.previous_address=line[0]
		else:
			if self.found_tcm_static_section_start==True or self.found_tcm_overlay_text_start==True or self.found_tcm_overlay_data_start==True:
				directory="ROOT/TCM/"
			elif self.is_section_island==True:
				directory="ROOT/Island/"
			else:
				directory="ROOT/"
			self.symbol_dict[self.previous_key].set_size(size)
			data=dataformat.object_info(self.section_name,self.Sub_Section_name,line[1],line[0],size,str(directory)+self.previous_path,self.section_index)
			self.previous_key=line[0]+str(self.section_name)
			self.symbol_dict[self.previous_key]=data
			self.previous_address=line[0]


	##Hole Detection Function
	def Detect_Hole(self,end_address,start_address,symbol_name,path):
		size=int(start_address,16) - int(end_address,16)
		if size==0:
			return
		else:
			#section_type,section_name,symbol_section,symbol_name,address,size,path)
			if self.found_tcm_static_section_start==True or self.found_tcm_overlay_text_start==True or self.found_tcm_overlay_data_start==True:
				directory="ROOT/TCM/"
			elif self.is_section_island==True:
				directory="ROOT/Island/"
			else:
				directory="ROOT/"
			data=dataformat.object_info(self.section_name,"",symbol_name,end_address,size,str(directory)+path,str(self.section_index))
			key=end_address+str(self.section_name)
			self.symbol_dict[key]=data

	def show_dict(self):
		write=open("final.log","w")
		for i in sorted(self.symbol_dict):
			write.write(self.symbol_dict[i].display())

def main():
	obj=MainParser()

if __name__ == "__main__":
	main()