
import os
import sys


def main(args):

	with open('offsets.txt', 'r') as infile:
		offsets = []
		for line in infile:
			line = line.strip()
			splitline = line.split(':')
			offsets.append(int(splitline[-1]))

	# print offsets

	with open('clade1comp.bin', 'rb') as patch, open('9205_fota_image.mbn', 'r+b') as file:
		file.seek(offsets[0])
		file.write(patch.read())

	with open('clade2comp.bin', 'rb') as patch, open('9205_fota_image.mbn', 'r+b') as file:
		file.seek(offsets[1])
		file.write(patch.read())



	print 'patched 9205_fota_image.mbn and 9205_fota_image_paging.bin using the offsets and compressed data sections'

if __name__ == '__main__':
	sys.exit(main(sys.argv))