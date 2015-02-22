#!/usr/bin/env python

# Script to turn the lights on and off
#
# Expects on or off and a pram that's it.
#
# It then send the right codes to the Arduino
#
#
import serial
import sys

if len(sys.argv) != 2:
  print "Expects argument of on or off"
  sys.exit(0)

DEV='/dev/ttyUSB0'

ser = serial.Serial(DEV, 9600, timeout=2)
print ser.readline()

if sys.argv[1] == 'on':
  ser.write('RF  A2on\n')
  ser.write('RF  B2on\n')
  print ser.readline()
elif sys.argv[1] == 'off':
  ser.write('RF  A2off\n')
  ser.write('RF  B2off\n')
else:
  print "Expects on or off as argument"

ser.close()




