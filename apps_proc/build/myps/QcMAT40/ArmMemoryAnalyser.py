import sys
import qmaffile


class ArmMemoryAnalysis:

	def __init__(self,mapfilename,outpufile):
		self.outpufile=outpufile
		self.mapfilename=mapfilename
		self.Image_Symbol_Table=False
		self.Memory_Map_of_the_image=False
		self.Image_component_sizes=False
		self.global_symbol_start=False
		self.obj_path_dict=dict()
		self.symbol_dict=dict()
		self.sudo_section_dict=dict()
		self.section_dict=dict()
		self.final_merge_dict=dict()
		self.start_parsing(self.mapfilename)
		# self.dump_all_data()
		self.previous_path=None
		self.previous_section=None
		self.previous_type=None


	def start_parsing(self,mapfilename):
		mapfile=open(mapfilename,"r")
		for line in mapfile:
			if line.startswith("Image Symbol Table"):
				self.Image_Symbol_Table=True
				self.Memory_Map_of_the_image=False
				self.Image_component_sizes=False
			elif line.startswith("Memory Map of the image"):
				self.Memory_Map_of_the_image=True
				self.Image_Symbol_Table=False
				self.Image_component_sizes=False
			elif line.startswith("Image component sizes"):
				self.Image_component_sizes=True
				self.Memory_Map_of_the_image=False
				self.Image_Symbol_Table=False
			else:
				if self.Image_Symbol_Table:
					self.call_Image_Symbol_Table(line)
				elif self.Memory_Map_of_the_image:
					self.call_Memory_Map_of_the_image(line)
				elif self.Image_component_sizes:
					self.merge_dict_to_symbols()
					self.call_Image_component_sizes(line)
					break
				else:
					self.Image_Symbol_Table=False
					self.Memory_Map_of_the_image=False
					self.Image_component_sizes=False

	def dump_all_data(self):
		file = open("symbol_dict.txt","w")
		for i in sorted(self.symbol_dict.keys()):
			file.write(str(i)+"  "+str(self.symbol_dict[i][0])+"  "+str(self.symbol_dict[i][1])+"\n")
		file=open("obj_path_dict.txt","w")
		for i in self.obj_path_dict:
			file.write(str(i)+"  "+str(self.obj_path_dict[i])+"\n")
		file=open("section_dict.txt","w")
		for i in self.section_dict:
			file.write(str(i)+" "+str(self.section_dict[i])+"\n")
		file=open("sudo_dict.txt","w")
		for i in sorted(self.sudo_section_dict.keys()):
			file.write(str(i)+" "+str(self.sudo_section_dict[i])+"\n")

	def call_Image_Symbol_Table(self,line):
		# print line
		if "Short Thumb to ARM Veneer to" in line:
			line=line.replace("Short Thumb to ARM Veneer to","")
		if "Inline Thumb to ARM Veneer to" in line:
			line=line.replace("Inline Thumb to ARM Veneer to","")
		if "Inline ARM to Thumb Veneer to" in line:
			line=line.replace("Inline ARM to Thumb Veneer to","")
		if "Global Symbols" in line:
			self.global_symbol_start=True
		if len(line.split())==6 and line.split()[-1]=="ABSOLUTE" and line.split()[1]=="0x00000000" and self.global_symbol_start==False:
			self.obj_path_dict[line.split()[4]]=self.trimpath(line.split()[0])
		elif "0x" in line and "Section" not in line:
			if "Thumb Code" in line:
				decc=1
			else:
				decc=0
			if "Thumb Code" in line or "ARM Code" in line:
				line=line.rsplit("0x",1)
				# print line
				size=line[1].split()[3]
			#    xx_msg_v4_const                          0x10000000   Data           8  aprv2_ipc.o(QSR_4_0_MSG.fmt.rodata.0.((0x00000004)).)
			# __dso_handle                             0x10000000   Number         0  aprv2_ipc.o(QSR_4_0_MSG.fmt.rodata.0.((0x00000004)).)
			else:
				#print line
				if "QSR_4_0_MSG.fmt.rodata" in line:
					line = line.split("(QSR_4_0_MSG")[0]
				line=line.rsplit("0x",1)
				#print line
				size=line[1].split()[2]
				#print size
			if not size=="0":
				self.symbol_dict[hex(int("0x"+line[1].split()[0],16)-decc).strip("L")]=[str(line[0].replace(" ","").split("(",1)[0]),size]
			# print "",hex(int("0x"+line[1].split()[0],16)-decc).strip("L"),"     ",str(line[0].replace(" ","").split("(",1)[0]),"    ",size

	def call_Memory_Map_of_the_image(self,line):
		if line.startswith("    Execution"):
			line=line.split()
			self.section_dict[line[4].strip(",")]=[line[2],line[4],str(int(line[6].strip(","),16)),hex(int(line[4].strip(","),16)+int(line[6].strip(","),16)).strip("L")]
		elif len(line.split())>=3:
			line=line.split()
			if line[0].startswith("0x") and line[1].startswith("0x"):
				if line[2]=="Code" or line[2]=="Ven":
					sectionIndex = 0
				elif line[2]=="Data" and line[3]=="RO":
					sectionIndex = 1
				elif line[2]=="Data" and line[3]=="RW":
					sectionIndex = 2
				elif line[2]=="Zero":
					sectionIndex = 3
				elif line[2]=="PAD":
					sectionIndex = 4
				else:
					sectionIndex=5
				if "QSR_STR.fmt.rodata" in line[-2]:
					line[-2]="QSR_STR.fmt.rodata"
				if sectionIndex!=4:
					if line[-1].split("(")[-1].strip(")") in self.obj_path_dict:
						path=self.obj_path_dict[line[-1].split("(")[-1].strip(")")]
					else:
						path=line[-1].split("(")[-1].strip(")")
					self.previous_path=path
					self.previous_section=line[-2]
					self.previous_type=sectionIndex
					address=line[0][2:]
					if line[0] in self.symbol_dict:
						symbol_name=str(self.symbol_dict[line[0]][0])
					else:
						symbol_name=line[-2]
					size=str(int(line[1],16))
					self.sudo_section_dict[line[0]]=[self.previous_path+" "+address+" "+symbol_name+" "+size+" "+line[-2]+" "+"g F"+" "+str(sectionIndex)+"\n"]
				else:
					address=line[0][2:]
					size=str(int(line[1],16))
					self.sudo_section_dict[line[0]]=[self.previous_path+" "+address+" "+"PADDING"+" "+size+" "+self.previous_section+" "+"g F"+" "+str(sectionIndex)+"\n"]

	def call_Image_component_sizes(self,line):
		# memoryAnalysis=open("memoryAnalysis","w")
		elfsecfile=open("elfsec","w")
		sections=open("sections","w")
		compressionSectionList=open("compressionSectionList","w")
		compressionSectionList.write("Q6Zip Sections Present in the build:\n.rw_candidate_compress_section, RW, .candidate_compress_section, RO")
		info=open("info","w")
		info.write("v3.0\n"+"MPSS\n"+"/modem_proc/\n"+"Hexagon(Q6)")
		summary=open("summary","w")
		# for i in sorted(self.sudo_section_dict.keys()):
			# memoryAnalysis.write(str(self.sudo_section_dict[i][0]))
		for i in sorted(self.section_dict):
			elfsecfile.write(" ".join(self.section_dict[i])+"\n")
			sections.write(self.section_dict[i][0]+"\t"+self.section_dict[i][1].strip(",")+"\t"+self.section_dict[i][2]+"\tELF_SECTION_TEXT"+"\tFalse\n")
		# memoryAnalysis.close()
		sections.close()
		elfsecfile.close()
		compressionSectionList.close()
		info.close()
		summary.close()
		# print "calling qmaf"
		qmaffile.create_qmaffile(self.outpufile)
		return

	def merge_dict_to_symbols(self):
		##['ROOT/ARM/stdlib/source/rwcats.cpp 80000f70 PADDING 144 .init_array g F 4\n']
		start_address=None
		end_address=None
		current_address=None
		file=open("memoryAnalysis","w")
		for i in sorted(self.sudo_section_dict):
			start_address=i
			data=self.sudo_section_dict[i][0].split()
			path=data[0]
			size=data[3]
			section=data[4]
			type=data[7]
			end_address=int(start_address,16)+int(size)
			# print "end_address ",hex(end_address)
			if data[2]=="PADDING":
				self.final_merge_dict[start_address]=str(self.sudo_section_dict[i][0].strip())
			else:
				if i in self.symbol_dict:
					start_address_int=int(start_address,16)
					padding=0
					while  start_address_int < end_address:
						start_address_hex=hex(start_address_int).strip("L")
						if start_address_hex in self.symbol_dict:
							##LOGIC for HOLE has to come here
							if padding>0:
								symbol_data=path+" "+hex(start_address_int-padding).strip("L")[2:]+" PADDING"+" "+str(padding)+" "+section+" g F "+type
								self.final_merge_dict[hex(start_address_int-padding).strip("L")]=symbol_data
								padding=0
							## Continious Symbols map to the Same File and Section
							symbol_data=path+" "+start_address_hex[2:]+" "+self.symbol_dict[start_address_hex][0]+" "+self.symbol_dict[start_address_hex][1]+" "+section+" g F "+type
							self.final_merge_dict[start_address_hex]=symbol_data
							start_address_int=start_address_int+int(self.symbol_dict[start_address_hex][1])
						else:
							padding=padding+1
							start_address_int=start_address_int+1
				else:
					self.final_merge_dict[start_address]=str(self.sudo_section_dict[i][0].strip())
		for j in sorted(self.final_merge_dict):
			# print self.final_merge_dict[j]
			if 'QSR_4_0_STR' in self.final_merge_dict[j]:
				continue
			file.write(self.final_merge_dict[j]+"\n")
		file.close()

	def trimpath(self,line):
		line=line.replace("\\","/")
		if "../../../" in line:
			line=line.replace("../../../","")
			return "ROOT/ARM/"+line
		if "../../" in line:
			line=line.replace("../../","")
			return "ROOT/ARM/"+line
		if "../" in line:
			line=line.replace("../","")
			return "ROOT/ARM/"+line
		if "apps_proc" in line:
			line =line.split("apps_proc",1)[1]
			return "ROOT/apps_proc"+line
		elif "modem_proc" in line:
			line =line.split("modem_proc",1)[1]
			return "ROOT/modem_proc"+line
		elif "wlan_proc" in line:
			line =line.split("wlan_proc",1)[1]
			return "ROOT/wlan_proc"+line
		else:
			return "ROOT/"+line

	def get_memoryanalysis_dict(self):
		return self.sudo_section_dict

def main():
	obj = ArmMemoryAnalysis(filename,output)


if __name__ == "__main__":
	main()