#!/bin/bash

make clean; make convertUSBWC2rootBin;

#./convertUSBWC2rootBin
# ERROR ---> in input arguments 
#   [1]   :  File containing the list of input data files
#   [2]   :  Path of the directory containing these input files
#   [3]   :  Name of the output ROOT file

./convertUSBWC2rootBin dataFileListUSBWCBin.txt ./ outFomBin.root
