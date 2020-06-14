#!/bin/bash

#./convertUSBWC2root
#input arguments: 
#             [1] : name of a text file containing the list of USBWC files to be converted to ROOT
#             [2] : path of the directory containing the input USBWC files
#             [3] : name of the output ROOT file

make clean; make convertUSBWC2root; 
./convertUSBWC2root dataFileListUSBWC.txt ./ out.root
