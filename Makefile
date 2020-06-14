ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)

#ROOTCFLAGS    = $(shell /usr/bin/root-config --cflags)
#ROOTLIBS      = $(shell /usr/bin/root-config --libs)
#ROOTGLIBS     = $(shell /usr/bin/root-config --glibs)

CXX           = g++
CXXFLAGS      = -g -Wall -fPIC -Wno-deprecated

NGLIBS         = $(ROOTGLIBS) 
NGLIBS        += -lMinuit

CXXFLAGS      += $(ROOTCFLAGS)
CXX           += -I./	
LIBS           = $(ROOTLIBS) 

GLIBS          = $(filter-out -lNew, $(NGLIBS))

CXX	      += -I./lib/
OUTLIB	      = ./lib/
.SUFFIXES: .C
.PREFIXES: ./lib/

#----------------------------------------------------#

all: convertUSBWC2root convertUSBWC2rootBin

convertUSBWC2root: convertUSBWC2root.cc
	$(CXX) $(CXXFLAGS) -o convertUSBWC2root  $(GLIBS) $<

convertUSBWC2rootBin: convertUSBWC2rootBin.cc
	$(CXX) $(CXXFLAGS) -o convertUSBWC2rootBin  $(GLIBS) $<


#polarSim: polarSim.cc
#        g++ -o polarSim polarSim.cc `root-config --cflags --glibs`

clean:
	rm -f convertUSBWC2root
	rm -f convertUSBWC2rootBin
	rm *~
