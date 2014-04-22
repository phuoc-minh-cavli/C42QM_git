"""
 qcmatreport.py

Description: qcmat csv/html report.

 Copyright (c) 2015 - 2016 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/build.tn/4.0/myps/QcMAT40/qcmatreport.py#1 $
 

 when            who        what, where, why
 --------      ---------  ------------------------------------------------
 Mar 28, 2016   rmandal   qcmatreport - initial version for command-line csv,html report
    
 ==========================================================================
 """

# Imports
import os
import os.path
import sys
import time
import inspect
import argparse
import zipfile
import inspect

import helper.Constants as const
import report.memtree as memtree
import report.htmlheader as htmlheader

import traceback

from os.path import join as pj
from os.path import split as ps
from os.path import exists as pe
from os.path import abspath as ap

debug_enable = False

# Global tree containing both build1(left) and build2(right) build memory data
# Diff is calculated from Build2 - Build1
MemTreeDict = dict()

#  Section Dictionary for mapping section names to TXT/RO/BSS/DATA column in memory report
SectionsDictLeft = dict()
SectionsDictRight = dict()

# Create path if does not exist
def mkpath(folder):
    if folder and not os.path.exists(folder):  
        os.makedirs(folder)

#  Update section  to TXT/RO/BSS/DATA column mapping for Build1
def UpdateSecDictionaryLeft(qmaf_file_path):
    qmaf = zipfile.ZipFile(qmaf_file_path, 'r')
    totalLines = qmaf.read('elfsec').split('\n')
    for line in totalLines:
        line=line.strip()
        #print line
        lineComponents = line.split(',')
        # print lineComponents
        
        if len(lineComponents) >= 5:
            if lineComponents[4] == "TEXT":
                SectionsDictLeft[lineComponents[0]] = 0
            elif lineComponents[4] == "RO":
                SectionsDictLeft[lineComponents[0]] = 1
            elif lineComponents[4] == "DATA":
                SectionsDictLeft[lineComponents[0]] = 2
            elif lineComponents[4] == "BSS":
                SectionsDictLeft[lineComponents[0]] = 3
            # print line, " => ", SectionsDictLeft[lineComponents[0]]    
    del totalLines
    qmaf.close()
    
#  Update section  to TXT/RO/BSS/DATA column mapping for Build2
def UpdateSecDictionaryRight(qmaf_file_path):
    qmaf = zipfile.ZipFile(qmaf_file_path, 'r')
    totalLines = qmaf.read('elfsec').split('\n')
    for line in totalLines:
        line=line.strip()
        #print line
        lineComponents = line.split(',')
        #print lineComponents
        
        if len(lineComponents) >= 5:
            if lineComponents[4] == "TEXT":
                SectionsDictRight[lineComponents[0]] = 0
            elif lineComponents[4] == "RO":
                SectionsDictRight[lineComponents[0]] = 1
            elif lineComponents[4] == "DATA":
                SectionsDictRight[lineComponents[0]] = 2
            elif lineComponents[4] == "BSS":
                SectionsDictRight[lineComponents[0]] = 3
            #print line, " => ", SectionsDictRight[lineComponents[0]]    
    del totalLines
    qmaf.close()

# Update Memory Tree data structure for Build1
def UpdateMemTreeLeft(qmaf_file_path):
    isfatal=False
    isf3=False
    qmaf = zipfile.ZipFile(qmaf_file_path, 'r')
    totalLines = qmaf.read('memoryAnalysis').split('\n')
    for line in totalLines:
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
        section = int(lineComponents[7])
        if (symbol.startswith("_F3_COUNT_") or "xx_msg_" in symbol or "xx_err_" in symbol or "ERR." in symbol or "QSR_F3_" in symbol or "QSR_4_0_MSG" in symbol or "QSR_LEGACY" in symbol) and "ro_fatal" not in lineComponents[4]:
            isf3=True
            # print section
        elif symbol.startswith("_FATAL_COUNT_") or "xx_msg_" in symbol or "xx_err_" in symbol and "ro_fatal" in lineComponents[4]:
            # print section
            isfatal=True
        # print lineComponents
        for part in pathComponents:
            parentPath = currentPath
            currentPath = currentPath + "/" + part
            #print currentPath, ",", section, ",", size
            if currentPath not in MemTreeDict:
                MemTreeDict[currentPath] = memtree.MemTreeNode(currentPath)
                if parentPath:
                    MemTreeDict[parentPath].addChild(MemTreeDict[currentPath])
            size_adjusted = size
            if isf3==True:
                MemTreeDict[currentPath].setLeftSizeAt(6,(MemTreeDict[currentPath].getLeftSizeAt(6) + (size/8)))
            elif isfatal==True:
                MemTreeDict[currentPath].setLeftSizeAt(5,(MemTreeDict[currentPath].getLeftSizeAt(5) + (size/10)))
            # Update section size e.g. TXT, RO, DATA, BSS
            if "QSR_4_0_MSG" not in lineComponents[4]:
                MemTreeDict[currentPath].setLeftSizeAt(section,(MemTreeDict[currentPath].getLeftSizeAt(section) + size_adjusted))
                # Update total size
                MemTreeDict[currentPath].setLeftSizeAt(4,(MemTreeDict[currentPath].getLeftSizeAt(4) + size_adjusted))
                #print MemTreeDict[currentPath]
        isf3=False
        isfatal=False
    del totalLines
    qmaf.close()

# Update Memory Tree data structure for Build2
def UpdateMemTreeRight(qmaf_file_path):
    isfatal=False
    isf3=False
    qmaf = zipfile.ZipFile(qmaf_file_path, 'r')
    totalLines = qmaf.read('memoryAnalysis').split('\n')
    for line in totalLines:
        line=line.strip()
        #skip blank line
        if not line:
            continue
        if "SECTION_HOLE" in line and "SUB_SECTION_HOLE" not in line:
            continue
        # ROOT/modem_proc/core/services/utils/modem_mem.o c47b6cb0 modemheap_fail_info 160 .bss g O
        # modulepath,address,symbol,size,section,visibility(g/l),type(F/O)
        lineComponents =  line.split(' ')
        currentPath = ""
        parentPath  = ""
        pathComponents = lineComponents[0].split('/')
        #address = lineComponents[1]
        symbol = lineComponents[2]
        size = int(lineComponents[3])
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
            if currentPath not in MemTreeDict:
                MemTreeDict[currentPath] = memtree.MemTreeNode(currentPath)
                if parentPath:
                    MemTreeDict[parentPath].addChild(MemTreeDict[currentPath])
            size_adjusted = size
            if isf3==True:
                MemTreeDict[currentPath].setRightSizeAt(6,(MemTreeDict[currentPath].getRightSizeAt(6) + (size/8)))
            elif isfatal==True:
                MemTreeDict[currentPath].setRightSizeAt(5,(MemTreeDict[currentPath].getRightSizeAt(5) + (size/10)))
            # Update section size e.g. TXT, RO, DATA, BSS
            if "QSR_4_0_MSG" not in lineComponents[4]:
                MemTreeDict[currentPath].setRightSizeAt(section,(MemTreeDict[currentPath].getRightSizeAt(section) + size_adjusted))
                # Update total size
                MemTreeDict[currentPath].setRightSizeAt(4,(MemTreeDict[currentPath].getRightSizeAt(4) + size_adjusted))
                #print MemTreeDict[currentPath]
        isf3=False
        isfatal=False
    del totalLines
    qmaf.close()

# Update Diff sizes in case two builds are provided
# Algorithm: Depth First Traversal
# Dependency: This should be called only after both Build1 and Build2 data is populated.
def UpdateMemTreeDiff(node):
    node.updateDiffSizes()
    if node.isLeaf:
        return
    
    for child in node.getChilds():
        UpdateMemTreeDiff(child)
        
# Generate CSV report for Build1
# Algorithm: Depth First Traversal
# Two csv file generated:
#  one having all object file txt/ro/data/bss/total... sizes
#  other having folder level breakup of sizes
def MemTree_CSV(node,csvObjFile,csvLibFile,csvRptFile,filter):
    nodefulname = node.getFullName().replace("/ROOT/","")
    if node.isLeaf:
        csvObjFile.write("\n" + nodefulname + "," + node.getLeftSizesString())
        return
    else:
        csvRptFile.write("\n\nMemory Analysis of :: " + nodefulname)
        if ".lib" in node.getLastName():
            csvLibFile.write("\n" + nodefulname + "," + node.getLeftSizesString())
    
    for child in node.getChilds():
        csvRptFile.write("\n"+child.getLastName() + "," + child.getLeftSizesString())
    csvRptFile.write("\nTotal:," + node.getLeftSizesString())
    
    for child in node.getChilds():
        MemTree_CSV(child,csvObjFile,csvLibFile,csvRptFile,filter)
        
# Generate CSV diff report for Build1 and Build2 (Build2-Build1)
# Algorithm: Depth First Traversal
# Two csv file generated:
#  one having all object file txt/ro/data/bss/total... sizes
#  other having folder level breakup of sizes
def MemTree_CSV_Diff(node,csvObjFile,csvLibFile,csvRptFile,filter):
    nodefulname = node.getFullName().replace("/ROOT/","")
    if node.isLeaf:
        csvObjFile.write(nodefulname + "," + node.getLeftSizesString() + "," + node.getDiffSizesString() + "\n")
        return
    else:
        csvRptFile.write("\n\nComparative Memory Analysis of :: " + nodefulname)
        if ".lib" in node.getLastName():
            csvLibFile.write("\n" + nodefulname + "," + node.getLeftSizesString() + "," + node.getDiffSizesString())
        
    for child in node.getChilds():
        csvRptFile.write("\n"+child.getLastName() + "," + child.getLeftSizesString() + "," + child.getDiffSizesString())
    csvRptFile.write("\nTotal:," + node.getLeftSizesString() + "," + node.getDiffSizesString())
    
    for child in node.getChilds():
        MemTree_CSV_Diff(child,csvObjFile,csvLibFile,csvRptFile,filter)

# Generate html report for Build1
# Algorithm: Depth First Traversal
# Output browsable similar to QCMAT GUI interface, easy to share HTML report
def MemTree_HTML(node,htmlroot,filter):
    if node.isLeaf:
        return
    
    htmlfilename = node.getFullName().replace("/ROOT",htmlroot)
    mkpath(htmlfilename)
    
    if htmlfilename == htmlroot:
        htmlfilename = htmlroot + "/index.html"
    else:
        htmlfilename = htmlfilename + ".html"
        
    htmlfile = open(htmlfilename,"w")
    
    #print "node: "  + node.getFullName() + ", html: " + htmlfilename
    htmlfile.write(htmlheader.htmlcssheader)
    htmlfile.write(htmlheader.htmldirtable)
    for child in node.getChilds():
        htmlfile.write("<tr>")
        linkpath = child.getLastTwoName().replace("ROOT/","") + ".html"
        linkdisp = child.getFullName().replace("/ROOT/","")
        #print "    child: " + child.getFullName() + ", link: " + linkpath + ", disp: " + linkdisp
        htmldirtd = ""
        if child.isLeaf:
            htmldirtd = """<td>{name}</td>""".format(name = linkdisp )
        else:
            htmldirtd = """<td><a href="{link}">{name}</a></td>""".format(link = linkpath,name = linkdisp )
        htmlfile.write(htmldirtd)
        for i in range(0,7):
            htmlsizetd = """<td align="right">{size}</td>""".format(size = child.getLeftSizeAt(i))
            htmlfile.write(htmlsizetd)
        htmlfile.write("</tr>\n")
    htmlfile.write("</tbody>\n<tfoot><tr><td>Total</td>")
    for i in range(0,7):
        htmlsizetd = """<td align="right">{size}</td>""".format(size = node.getLeftSizeAt(i))
        htmlfile.write(htmlsizetd)
    htmlfile.write("</tr></tfoot>\n</table></body></html>\n")
    htmlfile.close()
    
    for child in node.getChilds():
        MemTree_HTML(child,htmlroot,filter)

# Generate html diff report for Build1 and Build2 ( Build2 - Build1)
# Algorithm: Depth First Traversal
# Output browsable similar to QCMAT GUI interface, easy to share HTML report
def MemTree_HTML_Diff(node,htmlroot,filter):
    if node.isLeaf:
        return
    
    htmlfilename = node.getFullName().replace("/ROOT",htmlroot)
    mkpath(htmlfilename)
    
    if htmlfilename == htmlroot:
        htmlfilename = htmlroot + "/index.html"
    else:
        htmlfilename = htmlfilename + ".html"
        
    htmlfile = open(htmlfilename,"w")
    
    #print "node: "  + node.getFullName() + ", html: " + htmlfilename
    htmlfile.write(htmlheader.htmlcssheader)
    htmlfile.write(htmlheader.htmldirdifftable)
    for child in node.getChilds():
        htmlfile.write("<tr>")
        linkpath = child.getLastTwoName().replace("ROOT/","") + ".html"
        linkdisp = child.getFullName().replace("/ROOT/","")
        #print "    child: " + child.getFullName() + ", link: " + linkpath + ", disp: " + linkdisp
        htmldirtd = ""
        if child.isLeaf:
            htmldirtd = """<td>{name}</td>""".format(name = linkdisp )
        else:
            htmldirtd = """<td><a href="{link}">{name}</a></td>""".format(link = linkpath,name = linkdisp )
        htmlfile.write(htmldirtd)
        for i in range(0,7):
            htmlsizetd = """<td align="right">{size}</td>""".format(size = child.getLeftSizeAt(i))
            htmlfile.write(htmlsizetd)
        for i in range(0,7):
            htmlsizetd = """<td align="right">{size}</td>""".format(size = child.getDiffSizeAt(i))
            htmlfile.write(htmlsizetd)
        htmlfile.write("</tr>\n")
    htmlfile.write("</tbody>\n<tfoot><tr><td>Total</td>")
    for i in range(0,7):
        htmlsizetd = """<td align="right">{size}</td>""".format(size = node.getLeftSizeAt(i))
        htmlfile.write(htmlsizetd)
    for i in range(0,7):
        htmlsizetd = """<td align="right">{size}</td>""".format(size = node.getDiffSizeAt(i))
        htmlfile.write(htmlsizetd)
    htmlfile.write("</tr></tfoot>\n</table></body></html>\n")
    htmlfile.close()
    
    for child in node.getChilds():
        MemTree_HTML_Diff(child,htmlroot,filter)
        
def main1(qmaf1_file,qmaf2_file,module_filter,csv_report,html_report):
    try:
        # Build1 qmaf file
        qmaf1_file_path      = const.Constants.EMPTY_STR
        # Build2 qmaf file
        qmaf2_file_path      = const.Constants.EMPTY_STR
        # module filter, single path as of now
        module_filter        = const.Constants.EMPTY_STR
        
        version = 'Qualcomm Memory Analyzer Tool (report)\nGenerate csv/html report from qmaf file'
        diff_mode = 0
        print version
        qmaf1_file_path         = qmaf1_file.replace(const.Constants.BACKSLASH_STR,     const.Constants.BACKSLASH_STR + const.Constants.BACKSLASH_STR)
        print qmaf1_file_path
        if qmaf2_file:
            qmaf2_file_path         = qmaf2_file.replace(const.Constants.BACKSLASH_STR,     const.Constants.BACKSLASH_STR + const.Constants.BACKSLASH_STR)
            diff_mode = 1
        csv_file_name           = csv_report
        html_root                = html_report
        if not csv_file_name:
            csv_file_name = "memory.csv"
        else:
            csv_file_name.replace(const.Constants.BACKSLASH_STR,     const.Constants.BACKSLASH_STR + const.Constants.BACKSLASH_STR)
        print csv_file_name
        if module_filter:
            module_filter = module_filter.replace(const.Constants.BACKSLASH_STR,     const.Constants.SLASH_STR + const.Constants.SLASH_STR)
        
        UpdateSecDictionaryLeft( qmaf1_file_path)
        if diff_mode:
            UpdateSecDictionaryRight(qmaf2_file_path)
        
        UpdateMemTreeLeft( qmaf1_file_path)
        if diff_mode:
            UpdateMemTreeRight(qmaf2_file_path)
            UpdateMemTreeDiff(MemTreeDict['/ROOT'])
        
        root = None
        if '/ROOT' in MemTreeDict:
            root=MemTreeDict['/ROOT']
    
            if module_filter:
                root = MemTreeDict['/ROOT/'+module_filter]
                mkpath(os.path.dirname(csv_file_name))
            csvObjFile  = open(csv_file_name.replace(".csv","_obj.csv"),"w")
            csvLibFile  = open(csv_file_name.replace(".csv","_lib.csv"),"w")
            csvRptFile  = open(csv_file_name,"w")
            if diff_mode:
                csvObjFile.write("Module,TXT1,RO1,DATA1,BSS1,TOTAL1,FatalCount1,F3Count1,TXT2-TXT1,RO2-RO1,DATA2-DATA1,BSS2-BSS1,TOTAL2-TOTAL1,FatalCount2-FatalCount1,F3Count2-F3Count1\n")
                csvLibFile.write("Module,TXT1,RO1,DATA1,BSS1,TOTAL1,FatalCount1,F3Count1,TXT2-TXT1,RO2-RO1,DATA2-DATA1,BSS2-BSS1,TOTAL2-TOTAL1,FatalCount2-FatalCount1,F3Count2-F3Count1\n")
                csvRptFile.write("Module,TXT1,RO1,DATA1,BSS1,TOTAL1,FatalCount1,F3Count1,TXT2-TXT1,RO2-RO1,DATA2-DATA1,BSS2-BSS1,TOTAL2-TOTAL1,FatalCount2-FatalCount1,F3Count2-F3Count1\n")
                MemTree_CSV_Diff(root,csvObjFile,csvLibFile,csvRptFile,module_filter)
            else:
                csvObjFile.write("Module,TXT,RO,DATA,BSS,TOTAL,FatalCount,F3Count")
                csvLibFile.write("Module,TXT,RO,DATA,BSS,TOTAL,FatalCount,F3Count")
                csvRptFile.write("Module,TXT,RO,DATA,BSS,TOTAL,FatalCount,F3Count")
                MemTree_CSV(root,csvObjFile,csvLibFile,csvRptFile,module_filter)
        
            if html_root and diff_mode:
                MemTree_HTML_Diff(root,html_root,module_filter)
            elif html_root:
                MemTree_HTML(root,html_root,module_filter)

            csvObjFile.close()
            csvLibFile.close()
            csvRptFile.close()
        
    except:
        print "Quiting Main Program"
        print traceback.format_exc()
def main():
    pass
if __name__ == "__main__":
    main()
