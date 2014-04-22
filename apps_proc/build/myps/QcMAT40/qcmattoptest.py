import qcmat3toplevel
import sys

# mapfile, elffile
data = qcmat3toplevel.memory_top_level_data(sys.argv[1],sys.argv[2])
print str(data)
