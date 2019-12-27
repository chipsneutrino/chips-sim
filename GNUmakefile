# --------------------------------------------------------------
# GNUmakefile for WCSim
# --------------------------------------------------------------

G4DEBUG = 1

name := WCSim
G4TARGET := $(name)
G4EXLIB := true

CXX			= g++
CXXFLAGS    = -g -Wall -fPIC -O3
LDFLAGS     = -g -O3

INCDIR 		= ./include
SRCDIR 		= ./src
BINDIR 		= ./bin

ROOTCFLAGS := $(shell root-config --cflags) -DUSE_ROOT -fPIC
ROOTLDFLAGS:= $(shell root-config --ldflags)
ROOTLIBS   := $(shell root-config --libs) -lEG
ROOTGLIBS  := $(shell root-config --glibs)

CXXFLAGS   += $(ROOTCFLAGS)
LDFLAGS    += $(ROOTLDFLAGS)
LIBS        = $(ROOTLIBS) $(SYSLIBS)
GLIBS       = $(ROOTGLIBS) $(SYSLIBS)

CPPFLAGS  += -I$(ROOTSYS)/include $(ROOTCFLAGS)
EXTRALIBS += $(ROOTLIBS) $(ROOTGLIBS) -L$(G4LIB)/$(G4SYSTEM) -L

.PHONY: all
all: rootcint lib bin shared libWCSim.a evDisp geoHelp

LIBNAME := WCSim
ROOTSO := libWCSim.so
ROOTDICT := $(SRCDIR)/WCSimRootDict.cc

# This is the list of all the ROOT-based classes we need to worry about.
# Assumes that each class has src/*.cc, include/*.hh and tmp/*.o files.
ROOTCLASS := WCSimRootEvent WCSimRootGeom WCSimPmtInfo WCSimCHIPSPMT WCSimSK1pePMT WCSimTOTPMT WCSimPMTManager WCSimPMTConfig WCSimLCManager WCSimLCConfig WCSimEvDisplay WCSimTruthSummary

# Create the ROOTINC list from the class list, remembering to also add the LinkDef file
ROOTINC = $(ROOTCLASS:%=$(INCDIR)/%.hh)
ROOTINC += $(INCDIR)/WCSimRootLinkDef.hh
# For dictionary generation, want a version without the include directory.
ROOTINCNODIR = $(ROOTCLASS:%=%.hh)
ROOTINCNODIR += WCSimRootLinkDef.hh
# Now for the ROOTSRC list
ROOTSRC = $(ROOTCLASS:%=$(SRCDIR)/%.cc)
# Finally, the ROOTOBJ list, remembering to add on the RootDict file
G4PATH = $(G4WORKDIR)/tmp/$(G4SYSTEM)/$(G4TARGET)
ROOTOBJS = $(ROOTCLASS:%=$(G4PATH)/%.o)
ROOTOBJS += $(G4PATH)/WCSimRootDict.o

shared: $(ROOTDICT) $(ROOTSRC) $(ROOTINC) $(ROOTOBJS)
	@mkdir -p lib
	$(CXX) -shared -O $(ROOTOBJS) -o $(ROOTSO) $(ROOTLIBS) -O3

libWCSim.a: $(ROOTOBJS)
	$(RM) $@
	ar clq $@ $(ROOTOBJS)

$(ROOTDICT): $(ROOTSRC) $(ROOTINC)
	rootcint -f $(ROOTDICT) -c -I$(shell root-config --incdir) $(ROOTINC)

rootcint: $(ROOTDICT)

evDisp:
	@mkdir -p bin 
	$(CXX) -I$(INCDIR) -I$(shell root-config --incdir) -L./ -o $(BINDIR)/evDisplay evDisplay.cc $(ROOTDICT) -lWCSim -lEG $(CXXFLAGS) $(GLIBS) -O3

geoHelp:
	@mkdir -p bin
	$(CXX) -I$(INCDIR) -I$(shell root-config --incdir) -L./ $(LIBS) -o $(BINDIR)/geometryHelper geometryHelper.cc src/WCSimGeometryHelper.cc $(CXXFLAGS) $(GLIBS) -O3

include $(G4INSTALL)/config/binmake.gmk

