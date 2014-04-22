
import os
import sys


def main(args):

	with open('offsets.txt', 'r') as infile:
		offsets = []
		for line in infile:
			line = line.strip()
			splitline = line.split(':')
			offsets.append(int(splitline[-1]))

	print offsets

	with open('clade1comp.bin', 'rb') as patch, open('9205_fota_image.mbn', 'r+b') as file:
		file.seek(offsets[0])
		file.write(patch.read())

	# with open('clade2comp.bin', 'rb') as patch, open('9205_fota_image.mbn', 'r+b') as file:
		# file.seek(offsets[1])
		# file.write(patch.read())

	# This option is for using the included clade_extpl.bin file and overwriting the patching image
	# In this case we have already extraded the dynamic CLADE compressed data from the original final 
	# image. The next option will test overwriting the dynamic image using the clade1comp data.

	# with open('clade_extpl.bin', 'rb') as patch, open('9205_fota_image_paging.bin', 'r+b') as file:
		# file.seek(offsets[3])
		# file.write(patch.read())

	# We can replace the previous test with this one, which obtains the dynamic CLADE data from the
	# standalone CLADE data calculated during the FOTA process. We look up both offsets, and copy
	# the dynamic CLADE data and overwrite the custom dynamic FOTA image to create the final version.
	with open('clade1comp.bin', 'rb') as patch, open('9205_fota_image_paging.bin', 'r+b') as file:
		patch.seek(offsets[4])
		file.seek(offsets[3])
		file.write(patch.read(offsets[5]))

	print 'patched 9205_fota_image.mbn and 9205_fota_image_paging.bin using the offsets and compressed data sections'

if __name__ == '__main__':
	sys.exit(main(sys.argv))