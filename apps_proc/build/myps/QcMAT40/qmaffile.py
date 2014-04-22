import os
import sys
import zipfile



class qmafFileCreate :

	def __init__(self,memoryAnalysis,elfsec,outputfile):
		self.classify={"TEXT":0 ,"RO":1 ,"DATA":2 , "BSS":3}
		self.create_elfanalysis(elfsec)
		self.create_memoryanalysis(memoryAnalysis)
		self.create_info_and_compress()
		create_qmaffile(outputfile)

	def create_memoryanalysis(self,memoryAnalysis):
		memoryfile=open("memoryAnalysis","w")
		for i in sorted(memoryAnalysis):
			if memoryAnalysis[i].get_section_index()==None:
				continue
			else:
				memoryfile.write(memoryAnalysis[i].display())
		memoryfile.close()

	def create_elfanalysis(self,elfsec):
		elfsecfile=open("elfsec","w")
		sections=open("sections","w")
		sections.write("Name\tType\tAddr\tSize\tES\tSection Type\tLk\tInfo\tAlignment\tisSectionOverlaid\n")
		for i in sorted(elfsec):
			if elfsec[i][5]=="None":
				continue
			# print i.split(str(elfsec[i][2]))[-1]
			section_name=i.split(str(elfsec[i][2]))[-1]
			# print section_name
			# print "\t".join(elfsec[i])
			sections.write(section_name+"\t"+elfsec[i][2]+"\t"+elfsec[i][3]+"\t"+elfsec[i][5]+"\t"+elfsec[i][9]+"\n")
			string=section_name+","+str(elfsec[i][2])+"L"+","+elfsec[i][3]+","+str(hex(int(elfsec[i][2],16)+int(elfsec[i][3])))+","+str(elfsec[i][5][12:])+","+str(self.classify[elfsec[i][5][12:]])+","+str(elfsec[i][9])+"\n"
			elfsecfile.write(string)
		elfsecfile.close()
		sections.close()
	
	def create_info_and_compress(self):
		info=open("info","w")
		info.write("v3.0\nMPSS\n/modem_proc/\nHexagon(Q6)")
		compressionSectionList=open("compressionSectionList","w")
		compressionSectionList.write("Q6Zip Sections Present in the build:\n.rw_candidate_compress_section, RW, .candidate_compress_section, RO")
		info.close()
		compressionSectionList.close()



def create_qmaffile(outputfile):
	qmaf = zipfile.ZipFile(outputfile,"w", zipfile.ZIP_DEFLATED)
	qmaf.write("summary")
	qmaf.write("memoryAnalysis")
	qmaf.write("info")
	qmaf.write("sections")
	qmaf.write("compressionSectionList")
	qmaf.write("elfsec")
	qmaf.close()
	os.remove("summary")
	os.remove("memoryAnalysis")
	os.remove("info")
	os.remove("sections")
	os.remove("compressionSectionList")
	os.remove("elfsec")