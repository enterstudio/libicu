#**********************************************************************
#* Copyright (C) 1999-2000, International Business Machines Corporation
#* and others.  All Rights Reserved.
#**********************************************************************
# nmake file for creating data files on win32
# invoke with
# nmake /f makedata.mak icup=<path_to_icu_instalation> [Debug|Release]
#
#	12/10/1999	weiv	Created

#If no config, we default to debug
!IF "$(CFG)" == ""
CFG=Debug
!MESSAGE No configuration specified. Defaulting to common - Win32 Debug.
!ENDIF

#Here we test if a valid configuration is given
!IF "$(CFG)" != "Release" && "$(CFG)" != "release" && "$(CFG)" != "Debug" && "$(CFG)" != "debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "makedata.mak" CFG="Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Release"
!MESSAGE "Debug"
!MESSAGE
!ERROR An invalid configuration is specified.
!ENDIF

#Let's see if user has given us a path to ICU
#This could be found according to the path to makefile, but for now it is this way
!IF "$(ICUP)"==""
!ERROR Can't find path!
!ENDIF
!MESSAGE icu path is $(ICUP)
RESNAME=uconvmsg
RESDIR=.  #$(ICUP)\..\icuapps\uconv\$(RESNAME)
RESFILES=resfiles.mk
ICUDATA=$(ICUP)\data

DLL_OUTPUT=.\$(CFG)
# set the following to 'static' or 'dll' depending
PKGMODE=static


ICD=$(ICUDATA)^\
DATA_PATH=$(ICUP)\data^\
ICUTOOLS=$(ICUP)\bin

# We have to prepare params for pkgdata - to help it find the tools
!IF "$(CFG)" == "Debug" || "$(CFG)" == "debug"
PKGOPT=D:$(ICUP)
!ELSE
PKGOPT=R:$(ICUP)
!ENDIF

# This appears in original Microsofts makefiles
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

PATH = $(PATH);$(ICUP)\bin

# Suffixes for data files
.SUFFIXES : .ucm .cnv .dll .dat .res .txt .c

# We're including a list of ucm files. There are two lists, one is essential 'ucmfiles.mk' and
# the other is optional 'ucmlocal.mk'
!IF EXISTS("$(RESFILES)")
!INCLUDE "$(RESFILES)"
!ELSE
!ERROR ERROR: cannot find "$(RESFILES)"
!ENDIF
RB_FILES = $(RESSRC:.txt=.res)

# This target should build all the data files
!IF "$(PKGMODE)" == "dll"
OUTPUT = "$(DLL_OUTPUT)\$(RESNAME).dll"
!ELSE
OUTPUT = "$(DLL_OUTPUT)\$(RESNAME).lib"
!ENDIF

ALL : GODATA  $(OUTPUT) GOBACK #$(RESNAME).dat
	@echo All targets are up to date (mode $(PKGMODE))


# invoke pkgdata - static
"$(DLL_OUTPUT)\$(RESNAME).lib" :  $(RB_FILES) $(RESFILES)
	@echo Building $(RESNAME).lib
	@"$(ICUTOOLS)\pkgdata" -v -m static -c -p $(RESNAME) -O "$(PKGOPT)" -d "$(DLL_OUTPUT)" -s "$(RESDIR)" <<pkgdatain.txt
$(RB_FILES:.res =.res
)
<<KEEP

# utility to send us to the right dir
GODATA :
#	cd "$(RESDIR)"

# utility to get us back to the right dir
GOBACK :
#	cd "$(RESDIR)\.."

# This is to remove all the data files
CLEAN :
	@cd "$(RESDIR)"
	-@erase "*.cnv"
	-@erase "*.res"f
	-@erase "cnvalias*.*"
	-@erase "icudata.*"
	-@erase "*.obj"
	-@erase "base*.*"
	@cd "$(ICUTOOLS)"

# Inference rule for creating resource bundles
.txt.res:
	@echo Making Resource Bundle files
	"$(ICUTOOLS)\genrb" -s$(@D) -d$(@D) $(?F)


$(RESSRC) : {"$(ICUTOOLS)"}genrb.exe
