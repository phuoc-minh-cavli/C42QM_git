#!/bin/usr/python

#Sample script to generate configuration
#csd\csd.conf is the config file containing extactly 64bytes of info

conf = open("csd\csd.conf","w")
i = 0

#First Byte
conf.write(chr(1))

#Remaing Byte
while i < 63:
  conf.write(chr(0))
  i += 1

conf.close()
  